Public Class Surface
    ' Paths for the current graphs to be drawn when surface picturebox is painted
    Private GraphPath As New GraphicsPath
    Private DerivPath As New GraphicsPath
    Private IntegPath As New GraphicsPath

    ' Cache these values in variable for performance reasons
    Public Xmin As Double
    Public Xmax As Double
    Public Ymin As Double
    Public Ymax As Double

    Public Area As New Rectangle

    Public Sub Clear()
        GraphPath.Reset()
        DerivPath.Reset()
        IntegPath.Reset()
    End Sub

    Public Sub DrawAxis(ByVal Graphics As Graphics, ByVal Pen As Pen)
        With Graphics
            .DrawLine(Pen, VirtualToScreenX(Xmin), VirtualToScreenY(0), VirtualToScreenX(Xmax), VirtualToScreenY(0))
            .DrawLine(Pen, VirtualToScreenX(0), VirtualToScreenY(Ymin), VirtualToScreenX(0), VirtualToScreenY(Ymax))
        End With
    End Sub

    Public Sub DrawGraph(ByVal Graphics As Graphics, ByVal Pen As Pen)
        Graphics.DrawPath(Pen, GraphPath)
    End Sub

    Public Sub DrawDerivative(ByVal Graphics As Graphics, ByVal Pen As Pen)
        Graphics.DrawPath(Pen, DerivPath)
    End Sub

    Public Sub DrawIntegral(ByVal Graphics As Graphics, ByVal Pen As Pen)
        Graphics.DrawPath(Pen, IntegPath)
    End Sub

    Public Sub Refresh(ByVal Expression As String, ByVal ScriptControl As ScriptControl)
        ' The (to-be-calculated) Y values as init as DBNull. Later in code that is used
        ' as the value for "undefined Y".
        Dim X
        Dim Y = DBNull.Value
        Dim oldX = DBNull.Value
        Dim oldY = DBNull.Value

        Dim old_dYdX = DBNull.Value
        Dim old_FdX As Double = 0

        For Tracker As Integer = Area.Left To Area.Right
            X = ScreenToVirtualX(Tracker)
            Try
                With ScriptControl
                    .ExecuteStatement("X = " & Str(X) & " : Y = " & Str(IIf(IsDBNull(Y), 0, Y)))
                    Y = .Eval(Expression)
                End With
            Catch
                ' Set Y to DBNull in case no Y is defined for this X value
                Y = DBNull.Value
            End Try

            ' Only go into plotting in case both this Y and the last one was defined.
            ' In that case draw a line from the last point.
            If Not IsDBNull(Y) AndAlso Not IsDBNull(oldY) Then
                ' No line drawing in case curve passed +/- infinity (e.g. y=1/x)
                If (oldY > Ymin AndAlso oldY < Ymax) OrElse (Y > Ymin AndAlso Y < Ymax) Then
                    GraphPath.StartFigure()
                    Try
                        GraphPath.AddLine(Tracker - 1, VirtualToScreenY(oldY), Tracker, VirtualToScreenY(Y))
                    Catch
                    End Try
                    GraphPath.CloseFigure()
                End If

                ' Calcualte derivate by it's definition
                Dim dYdX = (Y - oldY) / (X - oldX)

                ' Only plot derivate curve if last derivate point was defined
                If Not IsDBNull(old_dYdX) Then
                    ' Same here, don't draw "through infinity"
                    If (old_dYdX > Ymin AndAlso old_dYdX < Ymax) OrElse (dYdX > Ymin AndAlso dYdX < Ymax) Then
                        DerivPath.StartFigure()
                        Try
                            DerivPath.AddLine(Tracker - 1, VirtualToScreenY(old_dYdX), Tracker, VirtualToScreenY(dYdX))
                        Catch
                        End Try
                        DerivPath.CloseFigure()
                    End If
                End If

                ' Calculate antiderivative
                Dim FdX As Double = old_FdX + Y * (X - oldX)
                If FdX < Ymin Then
                    FdX = Ymax
                ElseIf FdX > Ymax Then
                    FdX = Ymin
                ElseIf (old_FdX > Ymin AndAlso old_FdX < Ymax) OrElse (FdX > Ymin AndAlso FdX < Ymax) Then
                    IntegPath.StartFigure()
                    Try
                        IntegPath.AddLine(Tracker - 1, VirtualToScreenY(old_FdX), Tracker, VirtualToScreenY(FdX))
                    Catch
                    End Try
                    IntegPath.CloseFigure()
                End If

                old_dYdX = dYdX
                old_FdX = FdX
            Else
                old_FdX = 0
            End If

            oldX = X
            oldY = Y
        Next
    End Sub

    Public Function ScreenToVirtualX(ByVal X As Integer) As Double
        Return (X - Area.Left) * (Xmax - Xmin) / Area.Width + Xmin
    End Function

    Public Function ScreenToVirtualY(ByVal Y As Integer) As Double
        Return (Area.Height - Y + Area.Top) * (Ymax - Ymin) / Area.Height + Ymin
    End Function

    Public Function VirtualToScreenX(ByVal X As Double) As Integer
        Dim ScreenCoord As Integer = (X - Xmin) * Area.Width / (Xmax - Xmin)
        Select Case ScreenCoord
            Case Is < 0
                Return Area.Left - 1
            Case Is > Area.Width
                Return Area.Right + 1
            Case Else
                Return Area.Left + ScreenCoord
        End Select
    End Function

    Public Function VirtualToScreenY(ByVal Y As Double) As Integer
        Dim ScreenCoord As Integer = Area.Height - (Y - Ymin) * Area.Height / (Ymax - Ymin)
        Select Case ScreenCoord
            Case Is < 0
                Return Area.Top - 1
            Case Is > Area.Height
                Return Area.Bottom + 1
            Case Else
                Return Area.Top + ScreenCoord
        End Select
    End Function
End Class
