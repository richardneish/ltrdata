Module Program_Linq2SQL

  Sub Main(args As String())

    Dim ConnString = String.Empty
    If args.Length >= 1 Then
      ConnString = args(0)
    End If

    Dim Comment = String.Empty
    If args.Length >= 2 Then
      Comment = args(1)
    End If

    Dim DataConnection As LinqToSql.Stats

    Try
      DataConnection = New LinqToSql.Stats(ConnString)

    Catch Ex As Exception
      Console.WriteLine("Error opening data table: " & Ex.Message)
      Return

    End Try

    Try
      Dim DataRow As New LinqToSql.Tptest
      DataRow.Log_time = Date.Now
      DataRow.Comment = Comment

      Do
        Dim Line = Console.In.ReadLine()
        If Line Is Nothing Then
          Exit Do
        End If

        If Line.StartsWith("Best UDP Send Rate") Then
          DataRow.Udp_tx = Val(Line.Split(" "c)(4)) * FactorFromUnitsPerSec(Line.Split(" "c)(5))
        ElseIf Line.StartsWith("Best UDP Recv Rate") Then
          DataRow.Udp_rx = Val(Line.Split(" "c)(4)) * FactorFromUnitsPerSec(Line.Split(" "c)(5))
        ElseIf Line.StartsWith("Best TCP Send Rate") Then
          DataRow.Tcp_tx = Val(Line.Split(" "c)(4)) * FactorFromUnitsPerSec(Line.Split(" "c)(5))
        ElseIf Line.StartsWith("Best TCP Recv Rate") Then
          DataRow.Tcp_rx = Val(Line.Split(" "c)(4)) * FactorFromUnitsPerSec(Line.Split(" "c)(5))
        End If
      Loop

      DataConnection.Tptest.InsertOnSubmit(DataRow)
      DataConnection.SubmitChanges(ConflictMode.FailOnFirstConflict)

    Catch Ex As Exception
      Console.Write("Error inserting data: " & Ex.Message)

    Finally
      DataConnection.Dispose()

    End Try
  End Sub

  Public Function Val(obj As Object) As Double
    Return Double.Parse(obj.ToString(), NumberFormatInfo.InvariantInfo)
  End Function

  Public Function FactorFromUnitsPerSec(ByVal UPSString As String) As Double
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
