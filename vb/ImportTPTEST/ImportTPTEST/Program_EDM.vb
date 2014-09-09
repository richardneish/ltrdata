Module Program

    Sub Main(args As String())

        Dim ConnString = String.Empty
        If args.Length >= 1 Then
            ConnString = args(0)
        End If

        Dim Comment = String.Empty
        If args.Length >= 2 Then
            Comment = args(1)
        End If

        Dim EntityBuilder As New EntityConnectionStringBuilder With {
          .Provider = "System.Data.SqlClient",
          .ProviderConnectionString = ConnString,
          .Metadata = "res://*/ImportTPTEST.Statistics.csdl|res://*/ImportTPTEST.Statistics.ssdl|res://*/ImportTPTEST.Statistics.msl"
        }

        Dim DataConnection As EDM.StatisticsContext

        Try
            DataConnection = New EDM.StatisticsContext(EntityBuilder.ToString())

        Catch Ex As Exception
            Console.WriteLine("Error opening data table: " & Ex.GetBaseException().Message)
            Return

        End Try

        Try
            Dim DataRow As New EDM.tptest
            DataRow.log_time = Date.Now
            DataRow.comment = Comment

            Do
                Dim Line = Console.In.ReadLine()
                If Line Is Nothing Then
                    Exit Do
                End If

                If Line.StartsWith("Best UDP Send Rate") Then
                    DataRow.udp_tx = Val(Line.Split(" "c)(4)) * FactorFromUnitsPerSec(Line.Split(" "c)(5))
                ElseIf Line.StartsWith("Best UDP Recv Rate") Then
                    DataRow.udp_rx = Val(Line.Split(" "c)(4)) * FactorFromUnitsPerSec(Line.Split(" "c)(5))
                ElseIf Line.StartsWith("Best TCP Send Rate") Then
                    DataRow.tcp_tx = Val(Line.Split(" "c)(4)) * FactorFromUnitsPerSec(Line.Split(" "c)(5))
                ElseIf Line.StartsWith("Best TCP Recv Rate") Then
                    DataRow.tcp_rx = Val(Line.Split(" "c)(4)) * FactorFromUnitsPerSec(Line.Split(" "c)(5))
                End If
            Loop

            DataConnection.AddTotptest(DataRow)
            DataConnection.SaveChanges(True)

        Catch Ex As Exception
            Console.Write("Error inserting data: " & Ex.GetBaseException().Message)

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
