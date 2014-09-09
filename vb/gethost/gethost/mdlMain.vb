Imports System.Net.Dns

Module mdlMain

    Sub Main()
        Dim HostEntry As IPHostEntry

        If My.Application.CommandLineArgs.Count = 0 Then
            Try
                HostEntry = GetHostByName(GetHostName())
                PrintEntry(HostEntry)
            Catch Ex As Exception
                Console.Error.WriteLine(Ex.Message)
                Exit Sub
            End Try
        Else
            For Each Srv As String In My.Application.CommandLineArgs
                Try
                    HostEntry = GetHostByAddress(Srv)
                Catch
                End Try

                If HostEntry Is Nothing Then
                    Try
                        HostEntry = GetHostByName(Srv)
                    Catch Ex As Exception
                        Console.Error.WriteLine(Ex.Message)
                        Continue For
                    End Try
                End If

                PrintEntry(HostEntry)
            Next
        End If
    End Sub

    Sub PrintEntry(ByVal HostEntry As IPHostEntry)
        Console.Out.WriteLine("Host:" & vbTab & HostEntry.HostName)

        If HostEntry.Aliases.Length Then
            Console.Out.Write("Alias:")
            For Each AliasName As String In HostEntry.Aliases
                Console.Out.Write(vbTab & AliasName)
            Next
            Console.Out.Write(vbNewLine)
        End If

        If HostEntry.AddressList.Length Then
            Console.Out.Write("IP:")
            For Each IPAddress As IPAddress In HostEntry.AddressList
                Console.Out.Write(vbTab & IPAddress.ToString())
            Next
            Console.Out.Write(vbNewLine)
        End If

        Console.Out.Write(vbNewLine)
    End Sub
End Module
