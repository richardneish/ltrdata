Imports System.Text
Imports LTRLib.Compiler
Imports System.Security
Imports System.Security.Permissions
Imports System.IO

Public Class ScriptControl

    Protected ReadOnly AddedSourceCode As String

    Protected Delegate Function ExpressionMethodDelegate(X As Double, Y As Double) As Double?

    Protected ExpressionMethod As ExpressionMethodDelegate

    Protected ReadOnly SubDomain As AppDomain

    Protected m_Expression As String

    Public Sub New(Language As CodeCompiler.CompilerLanguage, SourceCode As String)
        Me.Language = Language
        AddedSourceCode = SourceCode

        Dim domsetup As New AppDomainSetup With {
            .ApplicationBase = AppDomain.CurrentDomain.SetupInformation.ApplicationBase,
            .DisallowApplicationBaseProbing = True,
            .DisallowBindingRedirects = True,
            .DisallowCodeDownload = True,
            .DisallowPublisherPolicy = True
        }

        Dim perm As New PermissionSet(PermissionState.None)
        perm.AddPermission(New SecurityPermission(SecurityPermissionFlag.Execution))

        SubDomain = AppDomain.CreateDomain("MathSubDomain", Nothing, domsetup, perm)

    End Sub

    Protected ReadOnly Language As CodeCompiler.CompilerLanguage

    Public Property Expression As String
        Get
            Return m_Expression
        End Get
        Set(value As String)
            If value = m_Expression Then
                Return
            End If

            m_Expression = Nothing
            ExpressionMethod = Nothing

            Dim SourceCode As New StringBuilder
            SourceCode.AppendLine("Imports System.Math")
            SourceCode.AppendLine("Imports MathExtensions")
            SourceCode.AppendLine()
            SourceCode.AppendLine("Public Class SourceCode")
            SourceCode.AppendLine()
            SourceCode.AppendLine("  Public Shared Function EvaluateExpression(X As Double, Y As Double) As System.Nullable(Of Double)")
            SourceCode.AppendLine()
            SourceCode.AppendLine("    Try")
            SourceCode.AppendLine("      Return " & value)
            SourceCode.AppendLine()
            SourceCode.AppendLine("    Catch")
            SourceCode.AppendLine("      Return Nothing")
            SourceCode.AppendLine()
            SourceCode.AppendLine("    End Try")
            SourceCode.AppendLine()
            SourceCode.AppendLine("  End Function")
            SourceCode.AppendLine()
            SourceCode.AppendLine("End Class")

            Dim CompilerResults = CodeCompiler.CompileSourceCodeToMemory({AddedSourceCode, SourceCode.ToString()}, Language, CodeCompiler.CompilerVersion.v2_0, Nothing)
            If CompilerResults.Output.Count > 1 Then
                Dim CompilerOutput As New StringBuilder
                For Each Row In CompilerResults.Output
                    CompilerOutput.AppendLine(Row)
                Next

                FormCompilerOutput.Text = "Compile errors"
                FormCompilerOutput.TextBox.Text = CompilerOutput.ToString()
                FormCompilerOutput.TextBox.SelectionStart = FormCompilerOutput.TextBox.TextLength
                FormCompilerOutput.TextBox.ScrollToCaret()
                FormCompilerOutput.ShowDialog()
                Throw New Exception(CompilerResults.Errors(0).ErrorText)
            End If

            Dim IL =
                CompilerResults.
                CompiledAssembly.
                GetType("SourceCode").
                GetMethod("EvaluateExpression",
                          Reflection.BindingFlags.Public Or Reflection.BindingFlags.Static,
                          Nothing,
                          {GetType(Double), GetType(Double)},
                          Nothing).
                      GetMethodBody().
                      GetILAsByteArray()

            Dim method =
                SubDomain.
                DefineDynamicAssembly(CompilerResults.CompiledAssembly.GetName(), Reflection.Emit.AssemblyBuilderAccess.Run).
                DefineDynamicModule("SourceCode").
                DefineType("SourceCode").
                DefineMethod("EvaluateExpression",
                             Reflection.MethodAttributes.Public Or Reflection.MethodAttributes.Static, GetType(Double?), {GetType(Double), GetType(Double)})

            method.CreateMethodBody(IL, IL.Length)

            ExpressionMethod =
              DirectCast([Delegate].CreateDelegate(GetType(ExpressionMethodDelegate),
                                                   method), 
                                                         ExpressionMethodDelegate)

            m_Expression = value
        End Set
    End Property

    Public Function Eval(X As Double, Y As Double) As Double?
        If ExpressionMethod Is Nothing Then
            Return Nothing
        End If

        Return ExpressionMethod(X, Y)
    End Function

End Class
