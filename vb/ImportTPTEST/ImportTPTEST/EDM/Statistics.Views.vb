'------------------------------------------------------------------------------
' <auto-generated>
'     This code was generated by a tool.
'     Runtime Version:2.0.50727.3625
'
'     Changes to this file may cause incorrect behavior and will be lost if
'     the code is regenerated.
' </auto-generated>
'------------------------------------------------------------------------------

Option Strict Off
Option Explicit On

<Assembly: System.Data.Mapping.EntityViewGenerationAttribute(GetType(Edm_EntityMappingGeneratedViews.ViewsForBaseEntitySets4B0214C7D13DC775A8A70400EFB110C7))> 

Namespace Edm_EntityMappingGeneratedViews
    
    '''<Summary>
    '''The type contains views for EntitySets and AssociationSets that were generated at design time.
    '''</Summary>
    Public NotInheritable Class ViewsForBaseEntitySets4B0214C7D13DC775A8A70400EFB110C7
        Inherits System.Data.Mapping.EntityViewContainer
        
        '''<Summary>
        '''The constructor stores the views for the extents and also the hash values generated based on the metadata and mapping closure and views
        '''</Summary>
        Public Sub New()
            MyBase.New
            Me.EdmEntityContainerName = "StatisticsContext"
            Me.StoreEntityContainerName = "EDMStoreContainer"
            Me.HashOverMappingClosure = "1669d1db84f3372ad3370f5f76e7bc19"
            Me.HashOverAllExtentViews = "d49059cc5ef6a3205fcb258d56c2f8fc"
            Me.ViewCount = 4
        End Sub
        
        'The method returns the view for the index given.
        Protected Overrides Function GetViewAt(ByVal index As Integer) As System.Collections.Generic.KeyValuePair(Of String, String)
            If (index = 0) Then
                'return view for EDMStoreContainer.dtproperties
                Return New System.Collections.Generic.KeyValuePair(Of String, String)("EDMStoreContainer.dtproperties", ""&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"    SELECT VALUE -- Constructing dtproperties"&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"        [EDM.Store].dtproperties"& _ 
                        "(T1.dtproperties_id, T1.dtproperties_objectid, T1.dtproperties_property, T1.dtpr"& _ 
                        "operties_value, T1.dtproperties_uvalue, T1.dtproperties_lvalue, T1.dtproperties_"& _ 
                        "version)"&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"    FROM ("&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"        SELECT "&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"            T.id AS dtproperties_id, "&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"  "& _ 
                        "          T.objectid AS dtproperties_objectid, "&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"            T.property AS dtpro"& _ 
                        "perties_property, "&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"            T.[value] AS dtproperties_value, "&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"            T"& _ 
                        ".uvalue AS dtproperties_uvalue, "&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"            T.lvalue AS dtproperties_lvalue, "&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"            T.version AS dtproperties_version, "&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"            True AS _from0"&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"  "& _ 
                        "      FROM StatisticsContext.dtproperties AS T"&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"    ) AS T1")
            Else
                If (index = 1) Then
                    'return view for StatisticsContext.dtproperties
                    Return New System.Collections.Generic.KeyValuePair(Of String, String)("StatisticsContext.dtproperties", ""&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"    SELECT VALUE -- Constructing dtproperties"&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"        EDM.dtproperties(T1.dtpr"& _ 
                            "operties_id, T1.dtproperties_objectid, T1.dtproperties_property, T1.dtproperties"& _ 
                            "_value, T1.dtproperties_uvalue, T1.dtproperties_lvalue, T1.dtproperties_version)"& _ 
                            ""&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"    FROM ("&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"        SELECT "&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"            T.id AS dtproperties_id, "&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"          "& _ 
                            "  T.objectid AS dtproperties_objectid, "&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"            T.property AS dtproperties_"& _ 
                            "property, "&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"            T.[value] AS dtproperties_value, "&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"            T.uvalue "& _ 
                            "AS dtproperties_uvalue, "&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"            T.lvalue AS dtproperties_lvalue, "&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"       "& _ 
                            "     T.version AS dtproperties_version, "&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"            True AS _from0"&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"        FR"& _ 
                            "OM EDMStoreContainer.dtproperties AS T"&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"    ) AS T1")
                Else
                    If (index = 2) Then
                        'return view for EDMStoreContainer.tptest
                        Return New System.Collections.Generic.KeyValuePair(Of String, String)("EDMStoreContainer.tptest", ""&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"    SELECT VALUE -- Constructing tptest"&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"        [EDM.Store].tptest(T1.[tptest."& _ 
                                "log_time], T1.[tptest.udp_tx], T1.[tptest.udp_rx], T1.[tptest.tcp_tx], T1.[tptes"& _ 
                                "t.tcp_rx], T1.tptest_comment)"&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"    FROM ("&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"        SELECT "&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"            T.log_ti"& _ 
                                "me AS [tptest.log_time], "&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"            T.udp_tx AS [tptest.udp_tx], "&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"          "& _ 
                                "  T.udp_rx AS [tptest.udp_rx], "&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"            T.tcp_tx AS [tptest.tcp_tx], "&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"    "& _ 
                                "        T.tcp_rx AS [tptest.tcp_rx], "&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"            T.comment AS tptest_comment, "& _ 
                                ""&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"            True AS _from0"&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"        FROM StatisticsContext.tptest AS T"&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"    ) "& _ 
                                "AS T1")
                    Else
                        If (index = 3) Then
                            'return view for StatisticsContext.tptest
                            Return New System.Collections.Generic.KeyValuePair(Of String, String)("StatisticsContext.tptest", ""&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"    SELECT VALUE -- Constructing tptest"&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"        EDM.tptest(T1.[tptest.log_time"& _ 
                                    "], T1.[tptest.udp_tx], T1.[tptest.udp_rx], T1.[tptest.tcp_tx], T1.[tptest.tcp_rx"& _ 
                                    "], T1.tptest_comment)"&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"    FROM ("&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"        SELECT "&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"            T.log_time AS [t"& _ 
                                    "ptest.log_time], "&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"            T.udp_tx AS [tptest.udp_tx], "&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"            T.udp_"& _ 
                                    "rx AS [tptest.udp_rx], "&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"            T.tcp_tx AS [tptest.tcp_tx], "&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"            "& _ 
                                    "T.tcp_rx AS [tptest.tcp_rx], "&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"            T.comment AS tptest_comment, "&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"      "& _ 
                                    "      True AS _from0"&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"        FROM EDMStoreContainer.tptest AS T"&Global.Microsoft.VisualBasic.ChrW(13)&Global.Microsoft.VisualBasic.ChrW(10)&"    ) AS T1")
                        End If
                    End If
                End If
            End If
            Throw New System.IndexOutOfRangeException
        End Function
    End Class
End Namespace
