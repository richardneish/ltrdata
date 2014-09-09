Public Class Dataviewer

  Private BindingManager As BindingManagerBase
  Private DataAdapter As Odbc.OdbcDataAdapter
  Private WithEvents SourceTable As DataTable

  Public Sub SetSourceTable(DataTable As DataTable)
    If SourceTable IsNot Nothing Then
      SourceTable.Dispose()
    End If
    SourceTable = Nothing
    DataGridView.DataSource = DataTable
    SourceTable = DataTable
  End Sub

  Private Sub DataTable_RowChanged(ByVal sender As Object, ByVal e As EventArgs) _
    Handles _
      SourceTable.RowChanged,
      SourceTable.RowDeleted,
      SourceTable.TableNewRow

    Try
      DataAdapter.Update(SourceTable)
    Catch Ex As Exception
      MsgBox(Ex.Message, MsgBoxStyle.Exclamation, "Error updating data source")
    End Try
  End Sub

  Protected Overrides Sub OnClosing(ByVal e As System.ComponentModel.CancelEventArgs)
    MyBase.OnClosing(e)

    If BindingManager IsNot Nothing Then
      BindingManager.EndCurrentEdit()
    End If
  End Sub

  Private Sub btnLoad_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnLoad.Click
    If BindingManager IsNot Nothing Then
      BindingManager.EndCurrentEdit()
      BindingManager = Nothing
    End If

    SetSourceTable(Nothing)

    If DataAdapter IsNot Nothing Then
      DataAdapter.SelectCommand.Connection.Dispose()
      DataAdapter.Dispose()
      DataAdapter = Nothing
    End If

    Dim Connection As New Odbc.OdbcConnection
    Dim NewSourceTable As New DataTable
    Try
      Connection.ConnectionString = tbDatasource.Text
      Connection.ConnectionTimeout = 20
      Connection.Open()

      Dim Command As New Odbc.OdbcCommand(tbTable.Text, Connection)
      Command.CommandTimeout = 45

      DataAdapter = New Odbc.OdbcDataAdapter(Command)

      With New Odbc.OdbcCommandBuilder(DataAdapter)
      End With

      DataAdapter.Fill(NewSourceTable)

      If NewSourceTable.Columns.Count = 0 Then
        NewSourceTable = Nothing
        DataAdapter = Nothing
        BindingManager = Nothing

        Connection.Dispose()

        MsgBox("Command executed successfully.", MsgBoxStyle.Information)

        Exit Sub
      End If

    Catch Ex As Exception
      NewSourceTable = Nothing
      DataAdapter = Nothing
      BindingManager = Nothing

      Connection.Dispose()

      MsgBox(Ex.Message, MsgBoxStyle.Exclamation, "Error connecting to data source")

      Exit Sub
    End Try

    With DataGridView
      For Each DataGridViewColumn In .Columns
        .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.DisplayedCells
      Next
    End With

    BindingManager = BindingContext(NewSourceTable)

    SetSourceTable(NewSourceTable)

  End Sub

  Private Sub TextBox_LostFocus(ByVal sender As Object, ByVal e As System.EventArgs) Handles tbDatasource.LostFocus, tbTable.LostFocus
    With DirectCast(sender, TextBox)
      .SelectionStart = 0
      .SelectionLength = .TextLength
    End With
  End Sub

  Protected Overrides Sub OnShown(ByVal e As EventArgs)
    MyBase.OnShown(e)

    tbTable.Select()
  End Sub

  Private Sub DataGridView_DataError(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewDataErrorEventArgs) Handles DataGridView.DataError
    If TypeOf e.Exception Is FormatException Then
      MsgBox(e.Exception.Message, MsgBoxStyle.Exclamation, "Data format error")
    End If
  End Sub

  Private Sub btnAbout_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnAbout.Click
    AboutBox.ShowDialog(Me)
  End Sub
End Class
