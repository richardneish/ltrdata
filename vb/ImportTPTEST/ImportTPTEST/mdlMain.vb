Module mdlMain

  Sub Main()
    Dim ConnString = "DSN=Statistics"
    If My.Application.CommandLineArgs.Count >= 1 Then
      ConnString = My.Application.CommandLineArgs(0)
    End If

    Dim Table = "tptest"
    If My.Application.CommandLineArgs.Count >= 2 Then
      Table = My.Application.CommandLineArgs(1).Replace(" ", "_")
    End If

    Dim Comment = ""
    If My.Application.CommandLineArgs.Count >= 3 Then
      Comment = My.Application.CommandLineArgs(2)
    End If

    Dim DataAdapter As Odbc.OdbcDataAdapter
    Dim DataTable As New DataTable

    Try
      DataAdapter = New Odbc.OdbcDataAdapter("SELECT * FROM " & Table, ConnString)

      With New Odbc.OdbcCommandBuilder(DataAdapter)
      End With

      DataAdapter.FillSchema(DataTable, SchemaType.Source)

    Catch Ex As Exception
      MsgBox(Ex.Message, MsgBoxStyle.Exclamation, "Error opening data table")
      Exit Sub

    End Try

    Try
      Dim DataRow = DataTable.NewRow()
      DataRow!log_time = Now()
      DataRow!comment = Comment

      Do
        Dim Line = Console.In.ReadLine()
        If Line Is Nothing Then
          Exit Do
        End If

        If Line.StartsWith("Best UDP Send Rate") Then
          DataRow!udp_tx = Val(Line.Split(" "c)(4)) * FactorFromUnitsPerSec(Line.Split(" "c)(5))
        ElseIf Line.StartsWith("Best UDP Recv Rate") Then
          DataRow!udp_rx = Val(Line.Split(" "c)(4)) * FactorFromUnitsPerSec(Line.Split(" "c)(5))
        ElseIf Line.StartsWith("Best TCP Send Rate") Then
          DataRow!tcp_tx = Val(Line.Split(" "c)(4)) * FactorFromUnitsPerSec(Line.Split(" "c)(5))
        ElseIf Line.StartsWith("Best TCP Recv Rate") Then
          DataRow!tcp_rx = Val(Line.Split(" "c)(4)) * FactorFromUnitsPerSec(Line.Split(" "c)(5))
        End If
      Loop

      DataTable.Rows.Add(DataRow)
      DataAdapter.Update(DataTable)

    Catch Ex As Exception
      MsgBox(Ex.Message, MsgBoxStyle.Exclamation, "Error inserting data")

    End Try
  End Sub

  Public Function FactorFromUnitsPerSec(ByVal UPSString As String) As Single
    Select Case UPSString
      Case "Gbit/s"
        Return 1000
      Case "Mbit/s"
        Return 1
      Case "kbit/s"
        Return 0.001
      Case Else
        Return 0.000001
    End Select
  End Function
End Module
