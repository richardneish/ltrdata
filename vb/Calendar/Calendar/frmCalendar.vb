Option Strict Off

Imports System.Reflection

Class frmCalendar

    Public MaxDateViewed As Date

    Protected Overrides Sub OnLoad(e As EventArgs)

        Width = Calendar.Width + Width - ClientRectangle.Width
        Height = Calendar.Height + Height - ClientRectangle.Height
        MaximumSize = Size

        ExtendToDate(Date.Today.AddDays(1 - Date.Today.Day).AddMonths(13))

    End Sub

    Private Sub ExtendToDate(newDate As Date)

        If newDate <= MaxDateViewed Then
            Return
        End If

        Dim AppointmentDates = GetAppointmentDates(MaxDateViewed, newDate)
        AppointmentDates.ForEach(AddressOf Calendar.AddBoldedDate)

        Trace.WriteLine("New end date: " & newDate)
        MaxDateViewed = newDate

        Calendar.UpdateBoldedDates()

    End Sub

    Private Sub Calendar_DateChanged(sender As Object, e As DateRangeEventArgs) Handles Calendar.DateChanged

        ExtendToDate(e.End)

    End Sub

    Private Function GetAppointmentDates(MaxDateViewed As Date, newDate As Date) As List(Of Date)

        Dim LiveAuthClient As New LiveAuthClient("http://app.ltr-data.se/weekcal/")
        Dim LiveConnectClient As New LiveConnectClient(LiveAuthClient.Session)
        Dim operationResult As Object = Nothing
        LiveConnectClient.GetAsync("event.8c8ce076ca27823f.9690899aeeae4e97be18cdc75b644454.87bfc5dc263c403eacadd35b1e64bdf3", operationResult)
        Dim result = operationResult.Result
        MessageBox.Show(Me, "Event: " + result.name + ", Location: " + result.location)

        Return New List(Of Date)

    End Function

End Class
