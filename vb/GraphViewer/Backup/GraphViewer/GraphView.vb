Public Class GraphView
  ' This is the object that provides formula evaluation functions
  Private ScriptControl As New ScriptControl

  ' This is the drawing surface instance for the form
  Private ScreenSurface As New Surface

  Private Sub NumericTextBox_KeyPress(ByVal sender As Object, ByVal e As KeyPressEventArgs) Handles tbXmin.KeyPress, tbXmax.KeyPress, tbYmin.KeyPress, tbYmax.KeyPress
    If (Not Char.IsDigit(e.KeyChar)) AndAlso e.KeyChar <> vbBack AndAlso e.KeyChar <> "." AndAlso Not (e.KeyChar = "-" AndAlso DirectCast(sender, TextBox).SelectionStart = 0 AndAlso Not DirectCast(sender, TextBox).Text.StartsWith("-")) Then
      e.Handled = True
    End If
  End Sub

  Private Sub cmbExpression_KeyPress(ByVal sender As Object, ByVal e As KeyPressEventArgs) Handles cmbExpression.KeyPress
    If e.KeyChar = vbCr Then
      e.Handled = True

      cmbExpression.DroppedDown = False

      If cmbExpression.Text <> "" AndAlso Not cmbExpression.Items.Contains(cmbExpression.Text) Then
        cmbExpression.Items.Insert(0, cmbExpression.Text)
      End If

      RedrawGraphs()
    End If
  End Sub

  Public Sub RedrawGraphs()
    If Not Visible Then
      Exit Sub
    End If

    If ClearSurfaceBeforeDrawingToolStripMenuItem.Checked Then
      ScreenSurface.Clear()
    End If

    Try
      ScreenSurface.Refresh(cmbExpression.Text, ScriptControl)
    Catch Ex As Exception
      MsgBox(Ex.Message, MsgBoxStyle.Exclamation)
    End Try

    pbSurface.Refresh()
  End Sub

  Private Sub TextBox_Leave(ByVal sender As Object, ByVal e As EventArgs) Handles tbXmin.Leave, tbXmax.Leave, tbYmin.Leave, tbYmax.Leave, cmbExpression.Leave
    With sender
      .SelectionStart = 0
      .SelectionLength = .Text.Length
    End With
  End Sub

  Private Sub pbSurface_Layout(ByVal sender As Object, ByVal e As LayoutEventArgs) Handles pbSurface.Layout
    With ScreenSurface
      .Area.Size = CType(sender, PictureBox).ClientRectangle.Size

      .Clear()
    End With

    RedrawGraphs()
  End Sub

  Private Sub tbXmin_TextChanged(ByVal sender As Object, ByVal e As EventArgs) Handles tbXmin.TextChanged
    Try
      ScreenSurface.Xmin = Val(tbXmin.Text)
    Catch
      ScreenSurface.Xmin = 0
    End Try
  End Sub

  Private Sub tbXmax_TextChanged(ByVal sender As Object, ByVal e As EventArgs) Handles tbXmax.TextChanged
    Try
      ScreenSurface.Xmax = Val(tbXmax.Text)
    Catch
      ScreenSurface.Xmax = 0
    End Try
  End Sub

  Private Sub tbYmin_TextChanged(ByVal sender As Object, ByVal e As EventArgs) Handles tbYmin.TextChanged
    Try
      ScreenSurface.Ymin = Val(tbYmin.Text)
    Catch
      ScreenSurface.Ymin = 0
    End Try
  End Sub

  Private Sub tbYmax_TextChanged(ByVal sender As Object, ByVal e As EventArgs) Handles tbYmax.TextChanged
    Try
      ScreenSurface.Ymax = Val(tbYmax.Text)
    Catch
      ScreenSurface.Ymax = 0
    End Try
  End Sub

  Private Sub ExitToolStripMenuItem_Click(ByVal sender As Object, ByVal e As EventArgs) Handles ExitToolStripMenuItem.Click
    Close()
  End Sub

  Protected Overrides Sub OnFormClosed(ByVal e As FormClosedEventArgs)
    My.Settings.DrawDerivative = DrawCalculatedderivativeGraphToolStripMenuItem.Checked
    My.Settings.DrawAntiderivative = DrawCalculatedantiderivativeGraphToolStripMenuItem.Checked
    My.Settings.ClearBeforeRedraw = ClearSurfaceBeforeDrawingToolStripMenuItem.Checked
  End Sub

  Protected Overrides Sub OnLoad(ByVal e As EventArgs)
    ' Initialize the script control for using the VBScript engine.
    ScriptControl.Language = "VBScript"
    ' Add the supplementary math function library in Functions.vbs
    ScriptControl.AddCode(My.Resources.ResourceManager.GetString("Functions"))

    PrintDocument.DefaultPageSettings.Landscape = True

    DrawCalculatedderivativeGraphToolStripMenuItem.Checked = My.Settings.DrawDerivative
    DrawCalculatedantiderivativeGraphToolStripMenuItem.Checked = My.Settings.DrawAntiderivative
    ClearSurfaceBeforeDrawingToolStripMenuItem.Checked = My.Settings.ClearBeforeRedraw
  End Sub

  Protected Overrides Sub OnShown(ByVal e As EventArgs)
    If cmbExpression.Text <> "" Then
      cmbExpression.Items.Add(cmbExpression.Text)
    End If

    RedrawGraphs()
  End Sub

  Private Sub ClearDrawingSurfaceToolStripMenuItem_Click(ByVal sender As Object, ByVal e As EventArgs) Handles ClearDrawingSurfaceToolStripMenuItem.Click
    ScreenSurface.Clear()
    RedrawGraphs()
  End Sub

  Private Sub RedrawGraphToolStripMenuItem_Click(ByVal sender As Object, ByVal e As EventArgs) Handles RedrawGraphToolStripMenuItem.Click, DrawCalculatedderivativeGraphToolStripMenuItem.CheckedChanged, DrawCalculatedantiderivativeGraphToolStripMenuItem.CheckedChanged
    RedrawGraphs()
  End Sub

  Private Sub SaveAsPictureToolStripMenuItem_Click(ByVal sender As Object, ByVal e As EventArgs) Handles SaveAsPictureToolStripMenuItem.Click
    Dim SelectedImageFormat As ImageFormat

    Try
      With SaveFileDialog
        If .ShowDialog(Me) = Forms.DialogResult.Cancel Then
          Exit Sub
        End If

        Select Case SaveFileDialog.FilterIndex
          Case 2
            SelectedImageFormat = ImageFormat.Gif
          Case 3
            SelectedImageFormat = ImageFormat.Jpeg
          Case 4
            SelectedImageFormat = ImageFormat.Png
          Case 5
            SelectedImageFormat = ImageFormat.Tiff
          Case Else
            SelectedImageFormat = ImageFormat.Bmp
        End Select
      End With

      Dim Bitmap As New Bitmap(pbSurface.Width, pbSurface.Height)
      pbSurface.DrawToBitmap(Bitmap, pbSurface.ClientRectangle)
      Bitmap.Save(SaveFileDialog.FileName, SelectedImageFormat)
    Catch Ex As Exception
      MsgBox(Ex.Message, MsgBoxStyle.Exclamation)
    End Try
  End Sub

  Private Sub pbSurface_Paint(ByVal sender As Object, ByVal e As PaintEventArgs) Handles pbSurface.Paint
    Try
      e.Graphics.Clear(Color.DarkBlue)

      With ScreenSurface
        .DrawAxis(e.Graphics, Pens.DarkRed)

        If DrawCalculatedderivativeGraphToolStripMenuItem.Checked Then
          .DrawDerivative(e.Graphics, Pens.DarkGreen)
        End If
        If DrawCalculatedantiderivativeGraphToolStripMenuItem.Checked Then
          .DrawIntegral(e.Graphics, Pens.Blue)
        End If
        .DrawGraph(e.Graphics, Pens.Yellow)
      End With
    Catch
    End Try
  End Sub

  Private Sub PrintDocument_PrintPage(ByVal sender As Object, ByVal e As Drawing.Printing.PrintPageEventArgs) Handles PrintDocument.PrintPage
    With New Surface
      .Xmin = ScreenSurface.Xmin
      .Xmax = ScreenSurface.Xmax
      .Ymin = ScreenSurface.Ymin
      .Ymax = ScreenSurface.Ymax

      .Area = e.MarginBounds

      .Refresh(cmbExpression.Text, ScriptControl)

      .DrawAxis(e.Graphics, Pens.Orange)

      If DrawCalculatedderivativeGraphToolStripMenuItem.Checked Then
        .DrawDerivative(e.Graphics, Pens.LightGray)
      End If
      If DrawCalculatedantiderivativeGraphToolStripMenuItem.Checked Then
        .DrawIntegral(e.Graphics, Pens.DarkGray)
      End If
      .DrawGraph(e.Graphics, Pens.DarkRed)
    End With

    Dim fntTimes As New Font("Times", 12)

    Dim sftRightAligned As New StringFormat
    sftRightAligned.LineAlignment = StringAlignment.Center
    sftRightAligned.Alignment = StringAlignment.Far

    Dim sftLeftAligned As New StringFormat
    sftLeftAligned.LineAlignment = StringAlignment.Center
    sftLeftAligned.Alignment = StringAlignment.Near

    Dim sftTopAligned As New StringFormat
    sftTopAligned.LineAlignment = StringAlignment.Near
    sftTopAligned.Alignment = StringAlignment.Center

    Dim sftBottomAligned As New StringFormat
    sftBottomAligned.LineAlignment = StringAlignment.Far
    sftBottomAligned.Alignment = StringAlignment.Center

    With e.Graphics
      .DrawString("y = " & cmbExpression.Text, fntTimes, Brushes.DarkRed, New Rectangle(e.MarginBounds.Left, e.PageBounds.Top, e.MarginBounds.Width, e.MarginBounds.Top), sftLeftAligned)

      .DrawString(tbXmin.Text, fntTimes, Brushes.Black, New Rectangle(e.PageBounds.Left, e.MarginBounds.Top, e.MarginBounds.Left, tbYmax.Text * 2 * e.MarginBounds.Height / (tbYmax.Text - tbYmin.Text)), sftRightAligned)
      .DrawString(tbXmax.Text, fntTimes, Brushes.Black, New Rectangle(e.MarginBounds.Right, e.MarginBounds.Top, e.PageBounds.Right - e.MarginBounds.Right, tbYmax.Text * 2 * e.MarginBounds.Height / (tbYmax.Text - tbYmin.Text)), sftLeftAligned)

      .DrawString(tbYmin.Text, fntTimes, Brushes.Black, New Rectangle(e.MarginBounds.Left, e.MarginBounds.Bottom, tbXmin.Text * -2 * e.MarginBounds.Width / (tbXmax.Text - tbXmin.Text), e.PageBounds.Bottom - e.MarginBounds.Bottom), sftTopAligned)
      .DrawString(tbYmax.Text, fntTimes, Brushes.Black, New Rectangle(e.MarginBounds.Left, e.PageBounds.Top, tbXmin.Text * -2 * e.MarginBounds.Width / (tbXmax.Text - tbXmin.Text), e.MarginBounds.Top), sftBottomAligned)
    End With
  End Sub

  Private Sub PrintToolStripMenuItem_Click(ByVal sender As Object, ByVal e As EventArgs) Handles PrintToolStripMenuItem.Click
    If PrintDialog.ShowDialog() = Forms.DialogResult.OK Then
      PrintDocument.Print()
    End If
  End Sub

  Private Sub PrintPreviewToolStripMenuItem_Click(ByVal sender As Object, ByVal e As EventArgs) Handles PrintPreviewToolStripMenuItem.Click
    PrintPreviewDialog.ShowDialog(Me)
  End Sub

  Private Sub PrintPreviewDialog_Load(ByVal sender As Object, ByVal e As EventArgs) Handles PrintPreviewDialog.Load
    With PrintPreviewDialog
      .Left = Left + 10
      .Top = Top + 40
      .Width = ScreenSurface.Area.Width - 20
      .Height = ScreenSurface.Area.Height
    End With
  End Sub

  Private Sub PrintSetupToolStripMenuItem_Click(ByVal sender As Object, ByVal e As EventArgs) Handles PrintSetupToolStripMenuItem.Click
    PageSetupDialog.ShowDialog(Me)
  End Sub

  Private Sub AboutToolStripMenuItem_Click(ByVal sender As Object, ByVal e As EventArgs) Handles AboutToolStripMenuItem.Click
    AboutBox.ShowDialog(Me)
  End Sub
End Class