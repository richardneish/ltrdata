Public Class Dataviewer

    Private BindingManager As BindingManagerBase
    Private DataAdapter As Odbc.OdbcDataAdapter

    ' Those event handlers cannot have the "Handles" clause directly since we cannot let them handle
    ' these events while the DataTable is filled by the DataAdapter
    Private Sub DataTable_DataRowChanged(ByVal sender As Object, ByVal e As DataRowChangeEventArgs)
        Try
            DataAdapter.Update(DataGridView.DataSource)
        Catch Ex As Exception
            MsgBox(Ex.Message, MsgBoxStyle.Exclamation, "Error updating data source")
        End Try
    End Sub

    Private Sub DataTable_TableRowInserted(ByVal sender As Object, ByVal e As DataTableNewRowEventArgs)
        Try
            DataAdapter.Update(DataGridView.DataSource)
        Catch Ex As Exception
            MsgBox(Ex.Message, MsgBoxStyle.Exclamation, "Error inserting new record")
        End Try
    End Sub

    Private Sub Form_FormClosing(ByVal sender As Object, ByVal e As System.Windows.Forms.FormClosingEventArgs) Handles Me.FormClosing
        If BindingManager IsNot Nothing Then
            BindingManager.EndCurrentEdit()
        End If
    End Sub

    Private Sub Button_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles btnLoad.Click
        If BindingManager IsNot Nothing Then
            BindingManager.EndCurrentEdit()
            BindingManager = Nothing
        End If

        If DataGridView.DataSource IsNot Nothing Then
            CType(DataGridView.DataSource, DataTable).Dispose()
        End If
        DataGridView.DataSource = New DataTable

        If DataAdapter IsNot Nothing Then
            DataAdapter.SelectCommand.Connection.Dispose()
            DataAdapter.Dispose()
            DataAdapter = Nothing
        End If

        Dim Connection As New Odbc.OdbcConnection
        Try
            Connection.ConnectionString = tbDatasource.Text
            Connection.ConnectionTimeout = 30
            Connection.Open()

            Dim Command As New Odbc.OdbcCommand(tbTable.Text, Connection)
            Command.CommandTimeout = 45

            DataAdapter = New Odbc.OdbcDataAdapter(Command)

            With New Odbc.OdbcCommandBuilder(DataAdapter)
            End With

            DataAdapter.Fill(DataGridView.DataSource)

            If CType(DataGridView.DataSource, DataTable).Columns.Count = 0 Then
                DataGridView.DataSource = Nothing
                DataAdapter = Nothing
                BindingManager = Nothing

                Connection.Dispose()

                MsgBox("Command executed successfully.", MsgBoxStyle.Information)

                Exit Sub
            End If

        Catch Ex As Exception
            DataGridView.DataSource = Nothing
            DataAdapter = Nothing
            BindingManager = Nothing

            Connection.Dispose()

            MsgBox(Ex.Message, MsgBoxStyle.Exclamation, "Error connecting to data source")

            Exit Sub
        End Try

        With DataGridView
            For Each DataGridViewColumn As DataGridViewColumn In .Columns
                .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.DisplayedCells
            Next
        End With

        BindingManager = BindingContext(DataGridView.DataSource)

        With CType(DataGridView.DataSource, DataTable)
            AddHandler .RowChanged, AddressOf DataTable_DataRowChanged
            AddHandler .RowDeleted, AddressOf DataTable_DataRowChanged
            AddHandler .TableNewRow, AddressOf DataTable_TableRowInserted
        End With
    End Sub

    Private Sub TextBox_LostFocus(ByVal sender As Object, ByVal e As System.EventArgs) Handles tbDatasource.LostFocus, tbTable.LostFocus
        With DirectCast(sender, TextBox)
            .SelectionStart = 0
            .SelectionLength = .TextLength
        End With
    End Sub

    Private Sub frmMain_Shown(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Shown
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
