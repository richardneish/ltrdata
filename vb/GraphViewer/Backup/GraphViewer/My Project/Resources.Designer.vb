﻿'------------------------------------------------------------------------------
' <auto-generated>
'     This code was generated by a tool.
'     Runtime Version:2.0.50727.1433
'
'     Changes to this file may cause incorrect behavior and will be lost if
'     the code is regenerated.
' </auto-generated>
'------------------------------------------------------------------------------

Option Strict On
Option Explicit On

Imports System

Namespace My.Resources
    
    'This class was auto-generated by the StronglyTypedResourceBuilder
    'class via a tool like ResGen or Visual Studio.
    'To add or remove a member, edit your .ResX file then rerun ResGen
    'with the /str option, or rebuild your VS project.
    '''<summary>
    '''  A strongly-typed resource class, for looking up localized strings, etc.
    '''</summary>
    <Global.System.CodeDom.Compiler.GeneratedCodeAttribute("System.Resources.Tools.StronglyTypedResourceBuilder", "2.0.0.0"),  _
     Global.System.Diagnostics.DebuggerNonUserCodeAttribute(),  _
     Global.System.Runtime.CompilerServices.CompilerGeneratedAttribute(),  _
     Global.Microsoft.VisualBasic.HideModuleNameAttribute()>  _
    Friend Module Resources
        
        Private resourceMan As Global.System.Resources.ResourceManager
        
        Private resourceCulture As Global.System.Globalization.CultureInfo
        
        '''<summary>
        '''  Returns the cached ResourceManager instance used by this class.
        '''</summary>
        <Global.System.ComponentModel.EditorBrowsableAttribute(Global.System.ComponentModel.EditorBrowsableState.Advanced)>  _
        Friend ReadOnly Property ResourceManager() As Global.System.Resources.ResourceManager
            Get
                If Object.ReferenceEquals(resourceMan, Nothing) Then
                    Dim temp As Global.System.Resources.ResourceManager = New Global.System.Resources.ResourceManager("GraphViewer.Resources", GetType(Resources).Assembly)
                    resourceMan = temp
                End If
                Return resourceMan
            End Get
        End Property
        
        '''<summary>
        '''  Overrides the current thread's CurrentUICulture property for all
        '''  resource lookups using this strongly typed resource class.
        '''</summary>
        <Global.System.ComponentModel.EditorBrowsableAttribute(Global.System.ComponentModel.EditorBrowsableState.Advanced)>  _
        Friend Property Culture() As Global.System.Globalization.CultureInfo
            Get
                Return resourceCulture
            End Get
            Set
                resourceCulture = value
            End Set
        End Property
        
        '''<summary>
        '''  Looks up a localized string similar to Option Explicit
        '''
        '''Const e = 2.71828182845905
        '''Const pi = 3.14159265358979
        '''
        '''Public Function Sec(X)
        '''	Sec = 1 / Cos(X)
        '''End Function
        '''
        '''Public Function Cosec(X)
        '''	Cosec = 1 / Sin(X)
        '''End Function
        '''
        '''Public Function Cotan(X)
        '''	Cotan = 1 / Tan(X)
        '''End Function
        '''
        '''Public Function Arcsin(X)
        '''	Arcsin = Atn(X / Sqr(-X * X + 1))
        '''End Function
        '''
        '''Public Function Arccos(X)
        '''	Arccos = Atn(-X / Sqr(-X * X + 1)) + 2 * Atn(1)
        '''End Function
        '''
        '''Public Function Arctan(X)
        '''	Arctan = Atn(X)
        '''End Function
        '''
        '''Public Function  [rest of string was truncated]&quot;;.
        '''</summary>
        Friend ReadOnly Property Functions() As String
            Get
                Return ResourceManager.GetString("Functions", resourceCulture)
            End Get
        End Property
    End Module
End Namespace