VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.2#0"; "COMDLG32.OCX"
Begin VB.Form frmImExport 
   BorderStyle     =   1  'å≈íË(é¿ê¸)
   Caption         =   "Import/Export Data"
   ClientHeight    =   3195
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   4680
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   3195
   ScaleWidth      =   4680
   StartUpPosition =   1  'µ∞≈∞ Ã´∞—ÇÃíÜâõ
   Begin VB.CommandButton cmdCancel 
      Cancel          =   -1  'True
      Caption         =   "Cancel"
      Default         =   -1  'True
      Height          =   645
      Left            =   1890
      TabIndex        =   2
      Top             =   2490
      Width           =   1215
   End
   Begin VB.CommandButton cmdExport 
      Caption         =   "Export CSV file"
      Height          =   705
      Left            =   1260
      TabIndex        =   1
      Top             =   1320
      Width           =   2295
   End
   Begin VB.CommandButton cmdImport 
      Caption         =   "Import CSV file"
      Height          =   705
      Left            =   1260
      TabIndex        =   0
      Top             =   300
      Width           =   2295
   End
   Begin MSComDlg.CommonDialog FileSelectDialog 
      Left            =   180
      Top             =   240
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   393216
   End
End
Attribute VB_Name = "frmImExport"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

'Config.dat
Dim lang_data(0 To LANG_SIZE - 1) As Byte
Dim chime_data(0 To CHIME_SIZE - 1) As Byte
Dim version_data(0 To VERSION_SIZE - 1) As Byte
Dim voice_data(0 To VOICE_SIZE - 1) As Byte
'announce.dat
Dim door_data(0 To DOOR_SIZE - 1) As Byte
Dim emg_data(0 To ONE_EMGMA_SIZE - 1) As Byte
Dim spc_data(0 To ONE_SPCMA_SIZE - 1) As Byte
Dim ownversion_data(0 To OWN_VERSION_SIZE - 1) As Byte

'station.dat
Dim route_data(0 To ROUTE_SIZE - 1) As Byte

Dim sdata(0 To SEQUENCE_SIZE - 1) As Byte

Dim mcpdata(0 To 5) As Byte

Dim selroute As Integer
Dim getLineNo As Long

Public Sub DoModal()

    Me.Show vbModal

End Sub

Private Sub cmdCancel_Click()
    
    Unload Me

End Sub

Private Function Export_CSV(ByVal fno As Integer, ByVal msg As String, ByVal continue As Boolean) As Boolean
    On Error Resume Next

    If continue = True Then
        Print #fno, ",";
    Else
        Print #fno, ""
    End If

    Print #fno, Chr$(&H22) & msg & Chr$(&H22);

    If Err <> 0 Then
        Close fno
        Err.Clear
        Exit Function
    End If

    Export_CSV = True
End Function

Private Function Export_LangSelect(ByVal fno As Integer, ByVal data As Byte, ByVal continue As Boolean) As Boolean

        If data And &H80 Then   'EN
            If data And &H40 Then   'CN
                If data And &H20 Then   'MA
                    If Export_CSV(fno, "E-C-M", continue) = False Then
                        Exit Function
                    End If
                Else
                    If Export_CSV(fno, "EN-CN", continue) = False Then
                        Exit Function
                    End If
                End If
            Else
                If data And &H20 Then   'MA
                    If Export_CSV(fno, "EN-MA", continue) = False Then
                        Exit Function
                    End If
                Else
                    If Export_CSV(fno, "EN", continue) = False Then
                        Exit Function
                    End If
                End If
            End If
        Else
            If data And &H40 Then   'CN
                If data And &H20 Then   'MA
                    If Export_CSV(fno, "CN-MA", continue) = False Then
                        Exit Function
                    End If
                Else
                    If Export_CSV(fno, "CN", continue) = False Then
                        Exit Function
                    End If
                End If
            Else
                If data And &H20 Then   'MA
                    If Export_CSV(fno, "MA", continue) = False Then
                        Exit Function
                    End If
                Else
                    If Export_CSV(fno, "", continue) = False Then
                        Exit Function
                    End If
                End If
            End If
        End If

    Export_LangSelect = True
End Function

Private Function Export_Lang(ByVal fno As Integer) As Boolean
    Dim fno2 As Integer

    On Error Resume Next

    fno2 = FreeFile
    If befDateMode = False Then
        Open App.Path & "\CONFIG.DAT" For Binary As fno2
    Else
        Open App.Path & "\CONFIG.BEF" For Binary As fno2
    End If
    Seek #fno2, TOP_LANG
    Get fno2, , lang_data
    Close fno2
    
    If Export_CSV(fno, "Language", False) = False Then
        Exit Function
    End If

    If Export_LangSelect(fno, lang_data(0), True) = False Then
        Exit Function
    End If

    If Err <> 0 Then
        Err.Clear
        Exit Function
    End If

    Export_Lang = True
End Function

Private Function Export_Version(ByVal fno As Integer) As Boolean
    Dim fno2 As Integer
    Dim msg As String
    
    On Error Resume Next

    fno2 = FreeFile
    If befDateMode = False Then
        Open App.Path & "\ANNOUNCE.DAT" For Binary As fno2
    Else
        Open App.Path & "\ANNOUNCE.BEF" For Binary As fno2
    End If
    Seek #fno2, TOP_DVAU_VERSION
    Get fno2, , ownversion_data
    Close fno2

'data
    If Export_CSV(fno, "Version", False) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "DVAU", False) = False Then
        Exit Function
    End If
    msg = "'" + Right$("0000" + Hex$(ownversion_data(0) * 256 + ownversion_data(1)), 4)
    If Export_CSV(fno, msg, True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "MCP", False) = False Then
        Exit Function
    End If
    msg = "'" + Right$("0000" + Hex$(ownversion_data(2) * 256 + ownversion_data(3)), 4)
    If Export_CSV(fno, msg, True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "PAA", False) = False Then
        Exit Function
    End If
    msg = "'" + Right$("0000" + Hex$(ownversion_data(4) * 256 + ownversion_data(5)), 4)
    If Export_CSV(fno, msg, True) = False Then
        Exit Function
    End If
    
    If Err <> 0 Then
        Err.Clear
        Exit Function
    End If

    Export_Version = True
End Function

Private Function Export_Voice(ByVal fno As Integer) As Boolean
    Dim fno2 As Integer
    Dim msg As String
    Dim i As Integer

    On Error Resume Next

    fno2 = FreeFile
    If befDateMode = False Then
        Open App.Path & "\CONFIG.DAT" For Binary As fno2
    Else
        Open App.Path & "\CONFIG.BEF" For Binary As fno2
    End If
    Seek #fno2, TOP_VOICE
    Get fno2, , voice_data
    Close fno2

'title
    If Export_CSV(fno, "Initial Voice Level", False) = False Then
        Exit Function
    End If
    For i = 0 To 7
        msg = "PAA" & str$(i + 1)
        If i = 0 Then
            If Export_CSV(fno, msg, False) = False Then
                Exit Function
            End If
        Else
            If Export_CSV(fno, msg, True) = False Then
                Exit Function
            End If
        End If
    Next i
    
'data
    For i = 0 To 7
        If voice_data(i) = 0 Then
            msg = "0dB"
        Else
            msg = "-" & Trim$(voice_data(i)) & "dB"
        End If
        If i = 0 Then
            If Export_CSV(fno, msg, False) = False Then
                Exit Function
            End If
        Else
            If Export_CSV(fno, msg, True) = False Then
                Exit Function
            End If
        End If
    Next i
    
    If Err <> 0 Then
        Err.Clear
        Exit Function
    End If

    Export_Voice = True
End Function

Private Function Export_Chime(ByVal fno As Integer) As Boolean
    Dim fno2 As Integer
    Dim msg As String
    
    On Error Resume Next

    fno2 = FreeFile
    If befDateMode = False Then
        Open App.Path & "\CONFIG.DAT" For Binary As fno2
    Else
        Open App.Path & "\CONFIG.BEF" For Binary As fno2
    End If
    Seek #fno2, TOP_CHIME
    Get fno2, , chime_data
    Close fno2

'Title
    If Export_CSV(fno, "", False) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Wait Time", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Announce Code", True) = False Then
        Exit Function
    End If
    
'data
    If Export_CSV(fno, "Chime", False) = False Then
        Exit Function
    End If
    msg = Trim$(CInt(chime_data(0) * 256 + CInt(chime_data(1))))
    If Export_CSV(fno, msg, True) = False Then
        Exit Function
    End If

        Select Case CInt(chime_data(2) And &H3) * 256 + CInt(chime_data(3))
        Case 500
            msg = "2.0 Sec"
        Case 501
            msg = "2.5 Sec"
        Case 502
            msg = "3.0 Sec"
        Case 503
            msg = "3.5 Sec"
        Case 504
            msg = "4.0 Sec"
        Case 505
            msg = "4.5 Sec"
        Case 506
            msg = "5.0 Sec"
        Case 507
            msg = "5.5 Sec"
        Case 508
            msg = "6.0 Sec"
        Case Else
            msg = "2.0 Sec"
        End Select
    If Export_CSV(fno, msg, True) = False Then
        Exit Function
    End If

    If Export_CSV(fno, "", False) = False Then
        Exit Function
    End If
    
    If Err <> 0 Then
        Err.Clear
        Exit Function
    End If

    Export_Chime = True
End Function

Private Function Export_Door(ByVal fno As Integer) As Boolean
    Dim fno2 As Integer
    Dim msg As String
    Dim i As Integer
    Dim j As Integer
    
    On Error Resume Next

    fno2 = FreeFile
    If befDateMode = False Then
        Open App.Path & "\ANNOUNCE.DAT" For Binary As fno2
    Else
        Open App.Path & "\ANNOUNCE.BEF" For Binary As fno2
    End If
    Seek #fno2, TOP_DOOR
    Get fno2, , door_data
    Close fno2

'Title
    If Export_CSV(fno, "Door Open/Close Announce", False) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", False) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Sequence1", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Sequence2", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Sequence3", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Sequence4", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Sequence5", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Sequence6", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", False) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Wait Time", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Announce Code", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Language", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Wait Time", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Announce Code", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Language", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Wait Time", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Announce Code", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Language", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Wait Time", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Announce Code", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Language", True) = False Then
        Exit Function
    End If
    
'data
    For i = 0 To 3
        Select Case i
        Case 0
            If Export_CSV(fno, "Door A Open", False) = False Then
                Exit Function
            End If
        Case 1
            If Export_CSV(fno, "Door A Close", False) = False Then
                Exit Function
            End If
        Case 2
            If Export_CSV(fno, "Door B Open", False) = False Then
                Exit Function
            End If
        Case 3
            If Export_CSV(fno, "Door B Close", False) = False Then
                Exit Function
            End If
        End Select

        For j = 0 To 5
            msg = Trim$(CInt(door_data(i * ONE_DOOR_SIZE + j * 4 + 0) * 256 + CInt(door_data(i * ONE_DOOR_SIZE + j * 4 + 1))))
            If Export_CSV(fno, msg, True) = False Then
                Exit Function
            End If
            msg = Trim$(CInt((door_data(i * ONE_DOOR_SIZE + j * 4 + 2) And &H3) * 256 + CInt(door_data(i * ONE_DOOR_SIZE + j * 4 + 3))))
            If Export_CSV(fno, msg, True) = False Then
                Exit Function
            End If
            If Export_LangSelect(fno, door_data(i * ONE_DOOR_SIZE + j * 4 + 2), True) = False Then
                Exit Function
            End If
        Next j
    Next i

    If Export_CSV(fno, "", False) = False Then
        Exit Function
    End If
    
    If Err <> 0 Then
        Err.Clear
        Exit Function
    End If

    Export_Door = True
End Function

Private Function Export_emg(ByVal fno As Integer) As Boolean
    Dim fno2 As Integer
    Dim msg As String
    Dim i As Integer
    Dim j As Integer
    
    On Error Resume Next

'Title
    If Export_CSV(fno, "Emergency Announce", False) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", False) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "EIDS Code", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "MCP Line1", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "MCP Line2", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Sequence1", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Sequence2", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Sequence3", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Sequence4", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Sequence5", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Sequence6", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", False) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Wait Time", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Announce Code", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Language", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Wait Time", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Announce Code", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Language", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Wait Time", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Announce Code", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Language", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Wait Time", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Announce Code", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Language", True) = False Then
        Exit Function
    End If
    
'data
    For i = 0 To 31
        fno2 = FreeFile
        If befDateMode = False Then
            Open App.Path & "\ANNOUNCE.DAT" For Binary As fno2
        Else
            Open App.Path & "\ANNOUNCE.BEF" For Binary As fno2
        End If
        Seek #fno2, TOP_EMGMA + i * ONE_EMGMA_SIZE
        Get fno2, , emg_data
        Close fno2

        msg = "EMG" & Trim$(i + 1)
        If Export_CSV(fno, msg, False) = False Then
            Exit Function
        End If
        msg = "'" + Right$("0" + Hex$(emg_data(0)), 2)
        If Export_CSV(fno, msg, True) = False Then
            Exit Function
        End If
        For j = 0 To 5
            mcpdata(j) = emg_data(1 + j)
        Next j
        msg = "'" + getMCPString(mcpdata)
        If Export_CSV(fno, msg, True) = False Then
            Exit Function
        End If
        For j = 0 To 5
            mcpdata(j) = emg_data(7 + j)
        Next j
        msg = "'" + getMCPString(mcpdata)
        If Export_CSV(fno, msg, True) = False Then
            Exit Function
        End If
        
        For j = 0 To 5
            msg = Trim$(CInt(emg_data(1 + 12 + j * 4 + 0) * 256 + CInt(emg_data(1 + 12 + j * 4 + 1))))
            If Export_CSV(fno, msg, True) = False Then
                Exit Function
            End If
            msg = Trim$(CInt((emg_data(1 + 12 + j * 4 + 2) And &H3) * 256 + CInt(emg_data(1 + 12 + j * 4 + 3))))
            If Export_CSV(fno, msg, True) = False Then
                Exit Function
            End If
            If Export_LangSelect(fno, emg_data(1 + 12 + j * 4 + 2), True) = False Then
                Exit Function
            End If
        Next j
    Next i

    If Err <> 0 Then
        Err.Clear
        Exit Function
    End If

    Export_emg = True
End Function

Private Function Export_spc(ByVal fno As Integer) As Boolean
    Dim fno2 As Integer
    Dim msg As String
    Dim i As Integer
    Dim j As Integer
    
    On Error Resume Next

'Title
    If Export_CSV(fno, "Special Announce", False) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", False) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "EIDS Code", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "MCP Line1", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "MCP Line2", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Sequence1", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Sequence2", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Sequence3", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Sequence4", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Sequence5", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Sequence6", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", False) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Wait Time", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Announce Code", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Language", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Wait Time", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Announce Code", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Language", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Wait Time", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Announce Code", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Language", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Wait Time", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Announce Code", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Language", True) = False Then
        Exit Function
    End If
    
'data
    For i = 0 To 63
        fno2 = FreeFile
        If befDateMode = False Then
            Open App.Path & "\ANNOUNCE.DAT" For Binary As fno2
        Else
            Open App.Path & "\ANNOUNCE.BEF" For Binary As fno2
        End If
        Seek #fno2, TOP_SPCMA + i * ONE_SPCMA_SIZE
        Get fno2, , spc_data
        Close fno2

        msg = "SPC" & Trim$(i + 1)
        If Export_CSV(fno, msg, False) = False Then
            Exit Function
        End If
        msg = "'" + Right$("0" + Hex$(spc_data(0)), 2)
        If Export_CSV(fno, msg, True) = False Then
            Exit Function
        End If
        For j = 0 To 5
            mcpdata(j) = spc_data(1 + j)
        Next j
        msg = "'" + getMCPString(mcpdata)
        If Export_CSV(fno, msg, True) = False Then
            Exit Function
        End If
        For j = 0 To 5
            mcpdata(j) = spc_data(7 + j)
        Next j
        msg = "'" + getMCPString(mcpdata)
        If Export_CSV(fno, msg, True) = False Then
            Exit Function
        End If

        For j = 0 To 5
            msg = Trim$(CInt(spc_data(1 + 12 + j * 4 + 0) * 256 + CInt(spc_data(1 + 12 + j * 4 + 1))))
            If Export_CSV(fno, msg, True) = False Then
                Exit Function
            End If
            msg = Trim$(CInt((spc_data(1 + 12 + j * 4 + 2) And &H3) * 256 + CInt(spc_data(1 + 12 + j * 4 + 3))))
            If Export_CSV(fno, msg, True) = False Then
                Exit Function
            End If
            If Export_LangSelect(fno, spc_data(1 + 12 + j * 4 + 2), True) = False Then
                Exit Function
            End If
        Next j
    Next i

    If Err <> 0 Then
        Err.Clear
        Exit Function
    End If

    Export_spc = True
End Function

Private Function Export_Config(ByVal filename As String) As Boolean
    Dim fno As Integer

    On Error Resume Next

    fno = FreeFile
    Open filename For Output As fno
    If Err <> 0 Then Exit Function

    If Export_Lang(fno) = False Then Exit Function
    If Export_Version(fno) = False Then Exit Function
    If Export_Voice(fno) = False Then Exit Function
    If Export_Chime(fno) = False Then Exit Function
    
    If Export_Door(fno) = False Then Exit Function
    If Export_emg(fno) = False Then Exit Function
    If Export_spc(fno) = False Then Exit Function
    
    Close fno

    Export_Config = True
    
End Function

Private Function Export_station(ByVal srcname As String, ByVal fno As Integer) As Boolean
    Dim fno2 As Integer
    Dim routeno As Integer
    Dim msg As String
    Dim i As Integer
    Dim j As Integer

    On Error Resume Next

    fno2 = FreeFile
    Open App.Path & "\" & srcname For Binary As fno2
    Get #fno2, , route_data
    Close fno2

    If Export_CSV(fno, "", False) = False Then
        Exit Function
    End If
'Title
    If Export_CSV(fno, "", False) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "EIDS Code", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "MCP Line", True) = False Then
        Exit Function
    End If
    
    msg = "Route Code" & Right$("00" + Hex$(route_data(0)), 2) & " (" & GetRouteName(route_data(0)) & ")"
    If Export_CSV(fno, msg, False) = False Then
        Exit Function
    End If
    msg = "'" + Right$("0" + Hex$(route_data(1)), 2)
    If Export_CSV(fno, msg, True) = False Then
        Exit Function
    End If
    For j = 0 To 5
        mcpdata(j) = route_data(2 + j)
    Next j
    msg = "'" + getMCPString(mcpdata)
    If Export_CSV(fno, msg, True) = False Then
        Exit Function
    End If
    
    If Export_CSV(fno, "", False) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Sequence1", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Sequence2", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Sequence3", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Sequence4", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Sequence5", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Sequence6", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Station", False) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "EIDS Code", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "MCP Line", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Line No", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Station No", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Wait Time", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Announce Code", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Language", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Wait Time", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Announce Code", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Language", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Wait Time", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Announce Code", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Language", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Wait Time", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Announce Code", True) = False Then
        Exit Function
    End If
    If Export_CSV(fno, "Language", True) = False Then
        Exit Function
    End If
    
'data

    For i = 0 To 63
        If route_data(8 + i * STATION_SIZE + 0) = 0 Then Exit For
        
        msg = "Station Code" & Right$("00" + Hex$(route_data(8 + i * STATION_SIZE + 0)), 2) & " (" & GetStationName(CInt(Hex$(route_data(i * STATION_SIZE + 8)))) & ")"
        If Export_CSV(fno, msg, False) = False Then
            Exit Function
        End If
        
        If route_data(8 + i * STATION_SIZE + 9) And &H80 Then
            msg = "Arriving"
        ElseIf route_data(8 + i * STATION_SIZE + 9) And &H40 Then
            msg = "Arrived"
            If route_data(8 + i * STATION_SIZE + 9) And &H20 Then
            msg = "Extra"
            End If
        Else
            msg = "Leaving"
        End If
        If Export_CSV(fno, msg, True) = False Then
            Exit Function
        End If
        
        msg = "'" + Right$("0" + Hex$(route_data(8 + i * STATION_SIZE + 1)), 2)
        If Export_CSV(fno, msg, True) = False Then
            Exit Function
        End If
        For j = 0 To 5
            mcpdata(j) = route_data(8 + i * STATION_SIZE + 2 + j)
        Next j
        msg = "'" + getMCPString(mcpdata)
        If Export_CSV(fno, msg, True) = False Then
            Exit Function
        End If

        msg = Hex$((route_data(8 + i * STATION_SIZE + 8) And &HE0) \ 32) & " (" & GetLineName(route_data(0), (route_data(8 + i * STATION_SIZE + 8) And &HE0) \ 32) & ")"
        If Export_CSV(fno, msg, True) = False Then
            Exit Function
        End If
        msg = "'" + Right$("00" + Hex$((route_data(8 + i * STATION_SIZE + 8) And &H1F)), 2) & " (" & GetStationNumberName(route_data(0), (route_data(8 + i * STATION_SIZE + 8) And &H1F)) & ")"
        If Export_CSV(fno, msg, True) = False Then
            Exit Function
        End If
        
        For j = 0 To 5
            msg = Trim$(CInt(route_data(8 + i * STATION_SIZE + 10 + j * 4 + 0) * 256 + CInt(route_data(8 + i * STATION_SIZE + 10 + j * 4 + 1))))
            If Export_CSV(fno, msg, True) = False Then
                Exit Function
            End If
            msg = Trim$(CInt((route_data(8 + i * STATION_SIZE + 10 + j * 4 + 2) And &H3) * 256 + CInt(route_data(8 + i * STATION_SIZE + 10 + j * 4 + 3))))
            If Export_CSV(fno, msg, True) = False Then
                Exit Function
            End If
            If Export_LangSelect(fno, route_data(8 + i * STATION_SIZE + 10 + j * 4 + 2), True) = False Then
                Exit Function
            End If
        Next j
    Next i
    
    Export_station = True
    
End Function

Private Function Export_Route(ByVal csvname As String) As Boolean
    Dim fno As Integer
    Dim fno2 As Integer
    Dim routeno As Integer
    Dim routeName As String
    Dim fname As String
    

    On Error Resume Next

    fno = FreeFile
    Open csvname For Output As fno
    If Err <> 0 Then Exit Function

    If befDateMode = False Then
        fname = Dir(App.Path & "\ROUTE*.DAT")
    Else
        fname = Dir(App.Path & "\ROUTE*.BEF")
    End If
    Do While fname <> ""
        routeName = SplitPath(fname, True)
        routeno = "&H" + Right$(routeName, 2)

        If Export_station(fname, fno) = False Then Exit Function
        fname = Dir
    Loop
 
    Close fno

    Export_Route = True
    
End Function

Private Sub cmdExport_Click()
    On Error Resume Next

    If Dir(App.Path & "\CONFIG.CSV") <> "" Then
        If MsgBox("Already CSV Files  OverWrite OK?", vbOKCancel) = vbCancel Then
            Exit Sub
        End If
    End If

    If Export_Config(App.Path & "\CONFIG.CSV") = False Then
        MsgBox "File Not Created", vbExclamation
        Exit Sub
    End If

    If Export_Route(App.Path & "\ROUTE.CSV") = False Then
        MsgBox "File Not Created", vbExclamation
        Exit Sub
    End If
End Sub



Private Function Import_String(ByVal data As String, ByVal no As Integer) As String
    Dim i As Integer
    Dim foundno As Integer
    Dim qfound As Boolean
    Dim sdata As String
    
    qfound = False
    sdata = ""
    For i = 1 To Len(data)
        If Mid$(data, i, 1) = Chr$(&H22) Then
            If qfound = False Then
                qfound = True
                sdata = ""
            Else
                qfound = False
            End If
        ElseIf Mid$(data, i, 1) = "," Then
            If qfound = False Then
                foundno = foundno + 1
                If foundno = no Then
                    If Mid$(sdata, 1, 1) = "'" Then
                        Import_String = Mid$(sdata, 2)
                    Else
                        Import_String = sdata
                    End If
                    Exit Function
                End If
                sdata = ""
            Else
                sdata = sdata & Mid$(data, i, 1)
            End If
        Else
            sdata = sdata & Mid$(data, i, 1)
        End If
    Next i
    foundno = foundno + 1
    If foundno = no Then
        If Mid$(sdata, 1, 1) = "'" Then
            Import_String = Mid$(sdata, 2)
        Else
            Import_String = sdata
        End If
        Exit Function
    End If

    Import_String = ""
End Function

Private Function Import_LangSelect(ByRef data As Byte, ByVal sdata As String) As Boolean

        data = data And &H1F
    
        Select Case sdata
        Case "E-C-M"
            data = data Or &H80 Or &H40 Or &H20
        Case "EN-CN"
            data = data Or &H80 Or &H40
        Case "EN-MA"
            data = data Or &H80 Or &H20
        Case "EN"
            data = data Or &H80
        Case "CN-MA"
            data = data Or &H40 Or &H20
        Case "CN"
            data = data Or &H40
        Case "MA"
            data = data Or &H20
        Case ""
            data = data Or &H0
        Case Else
            Exit Function
        End Select

    Import_LangSelect = True

End Function

Private Function Import_Language(ByVal data As String, ByVal fno As Integer) As Boolean
    Dim sdata As String
    Dim fno2 As Integer

    On Error Resume Next

    fno2 = FreeFile
    If befDateMode = False Then
        Open App.Path & "\CONFIG.DAT" For Binary As fno2
    Else
        Open App.Path & "\CONFIG.BEF" For Binary As fno2
    End If
    Seek #fno2, TOP_LANG
    Get fno2, , lang_data
    Close fno2

    sdata = Import_String(data, 2)
    If Import_LangSelect(lang_data(0), sdata) = False Then
        Exit Function
    End If

    fno2 = FreeFile
    If befDateMode = False Then
        Open App.Path & "\CONFIG.DAT" For Binary As fno2
    Else
        Open App.Path & "\CONFIG.BEF" For Binary As fno2
    End If
    Seek #fno2, TOP_LANG
    Put fno2, , lang_data
    Close fno2

    If Err <> 0 Then Exit Function

    Import_Language = True
End Function

Private Function Import_VersionDVAU(ByVal data As String, ByVal fno As Integer) As Boolean
    Dim sdata As String
    Dim fno2 As Integer
    Dim bdata As Integer

    On Error Resume Next

    fno2 = FreeFile
    If befDateMode = False Then
        Open App.Path & "\CONFIG.DAT" For Binary As fno2
    Else
        Open App.Path & "\CONFIG.BEF" For Binary As fno2
    End If
    Seek #fno2, TOP_VERSION
    Get fno2, , version_data
    Close fno2

    sdata = Import_String(data, 2)
    
    bdata = Val("&H" + sdata)
    version_data(0) = bdata \ 256
    version_data(1) = bdata Mod 256
    
    fno2 = FreeFile
    If befDateMode = False Then
        Open App.Path & "\CONFIG.DAT" For Binary As fno2
    Else
        Open App.Path & "\CONFIG.BEF" For Binary As fno2
    End If
    Seek #fno2, TOP_VERSION
    Put fno2, , version_data
    Close fno2

    fno2 = FreeFile
    If befDateMode = False Then
        Open App.Path & "\ANNOUNCE.DAT" For Binary As fno2
    Else
        Open App.Path & "\ANNOUNCE.BEF" For Binary As fno2
    End If
    Seek #fno2, TOP_DVAU_VERSION
    Get fno2, , ownversion_data
    Close fno2

    ownversion_data(0) = bdata \ 256
    ownversion_data(1) = bdata Mod 256

    fno2 = FreeFile
    If befDateMode = False Then
        Open App.Path & "\ANNOUNCE.DAT" For Binary As fno2
    Else
        Open App.Path & "\ANNOUNCE.BEF" For Binary As fno2
    End If
    Seek #fno2, TOP_DVAU_VERSION
    Put fno2, , ownversion_data
    Close fno2

    If Err <> 0 Then Exit Function

    Import_VersionDVAU = True
End Function

Private Function Import_VersionMCP(ByVal data As String, ByVal fno As Integer) As Boolean
    Dim sdata As String
    Dim fno2 As Integer
    Dim bdata As Integer

    On Error Resume Next

    sdata = Import_String(data, 2)
    
    fno2 = FreeFile
    If befDateMode = False Then
        Open App.Path & "\ANNOUNCE.DAT" For Binary As fno2
    Else
        Open App.Path & "\ANNOUNCE.BEF" For Binary As fno2
    End If
    Seek #fno2, TOP_DVAU_VERSION
    Get fno2, , ownversion_data
    Close fno2

    bdata = Val("&H" + sdata)
    ownversion_data(2) = bdata \ 256
    ownversion_data(3) = bdata Mod 256

    fno2 = FreeFile
    If befDateMode = False Then
        Open App.Path & "\ANNOUNCE.DAT" For Binary As fno2
    Else
        Open App.Path & "\ANNOUNCE.BEF" For Binary As fno2
    End If
    Seek #fno2, TOP_DVAU_VERSION
    Put fno2, , ownversion_data
    Close fno2

    If Err <> 0 Then Exit Function

    Import_VersionMCP = True
End Function

Private Function Import_VersionPAA(ByVal data As String, ByVal fno As Integer) As Boolean
    Dim sdata As String
    Dim fno2 As Integer
    Dim bdata As Integer

    On Error Resume Next

    sdata = Import_String(data, 2)
    
    fno2 = FreeFile
    If befDateMode = False Then
        Open App.Path & "\ANNOUNCE.DAT" For Binary As fno2
    Else
        Open App.Path & "\ANNOUNCE.BEF" For Binary As fno2
    End If
    Seek #fno2, TOP_DVAU_VERSION
    Get fno2, , ownversion_data
    Close fno2

    bdata = Val("&H" + sdata)
    ownversion_data(4) = bdata \ 256
    ownversion_data(5) = bdata Mod 256

    fno2 = FreeFile
    If befDateMode = False Then
        Open App.Path & "\ANNOUNCE.DAT" For Binary As fno2
    Else
        Open App.Path & "\ANNOUNCE.BEF" For Binary As fno2
    End If
    Seek #fno2, TOP_DVAU_VERSION
    Put fno2, , ownversion_data
    Close fno2

    If Err <> 0 Then Exit Function

    Import_VersionPAA = True
End Function

Private Function Import_Voice(ByVal data As String, ByVal fno As Integer) As Boolean
    Dim sdata As String
    Dim fno2 As Integer
    Dim bdata As Integer
    Dim i As Integer

    On Error Resume Next

    fno2 = FreeFile
    If befDateMode = False Then
        Open App.Path & "\CONFIG.DAT" For Binary As fno2
    Else
        Open App.Path & "\CONFIG.BEF" For Binary As fno2
    End If
    Seek #fno2, TOP_VOICE
    Get fno2, , voice_data
    Close fno2

    For i = 0 To 7
        sdata = Import_String(data, i + 1)
        If sdata = "0dB" Then
            bdata = 0
        Else
            sdata = GetTokenValue(sdata, 1, "dB")
            bdata = Val(Mid$(sdata, 2))
        End If
    
        voice_data(i) = bdata
    Next i
    
    fno2 = FreeFile
    If befDateMode = False Then
        Open App.Path & "\CONFIG.DAT" For Binary As fno2
    Else
        Open App.Path & "\CONFIG.BEF" For Binary As fno2
    End If
    Seek #fno2, TOP_VOICE
    Put fno2, , voice_data
    Close fno2

    If Err <> 0 Then Exit Function

    Import_Voice = True
End Function

Private Function Import_Chime(ByVal data As String, ByVal fno As Integer) As Boolean
    Dim sdata As String
    Dim fno2 As Integer
    Dim bdata As Integer
    Dim i As Integer

    On Error Resume Next

    fno2 = FreeFile
    If befDateMode = False Then
        Open App.Path & "\CONFIG.DAT" For Binary As fno2
    Else
        Open App.Path & "\CONFIG.BEF" For Binary As fno2
    End If
    Seek #fno2, TOP_CHIME
    Get fno2, , chime_data
    Close fno2

    sdata = Import_String(data, 2)
    chime_data(0) = Val(sdata) \ 256
    chime_data(1) = Val(sdata) Mod 256
    sdata = Import_String(data, 3)
    Select Case sdata
    Case "2.0 Sec"
        bdata = 500
    Case "2.5 Sec"
        bdata = 501
    Case "3.0 Sec"
        bdata = 502
    Case "3.5 Sec"
        bdata = 503
    Case "4.0 Sec"
        bdata = 504
    Case "4.5 Sec"
        bdata = 505
    Case "5.0 Sec"
        bdata = 506
    Case "5.5 Sec"
        bdata = 507
    Case "6.0 Sec"
        bdata = 508
    Case Else
        Exit Function
    End Select
    chime_data(2) = (bdata \ 256) Or &HE0
    chime_data(3) = bdata Mod 256
        
    fno2 = FreeFile
    If befDateMode = False Then
        Open App.Path & "\CONFIG.DAT" For Binary As fno2
    Else
        Open App.Path & "\CONFIG.BEF" For Binary As fno2
    End If
    Seek #fno2, TOP_CHIME
    Put fno2, , chime_data
    Close fno2

    If Err <> 0 Then Exit Function

    Import_Chime = True
End Function

Private Function Import_Door(ByVal data As String, ByVal fno As Integer) As Boolean
    Dim sdata As String
    Dim fno2 As Integer
    Dim bdata As Integer
    Dim i As Integer
    Dim j As Integer

    On Error Resume Next

    sdata = Import_String(data, 1)
    Select Case sdata
    Case "Door A Open"
        i = 0
    Case "Door A Close"
        i = 1
    Case "Door B Open"
        i = 2
    Case "Door B Close"
        i = 3
    Case Else
        Exit Function
    End Select
        
    fno2 = FreeFile
    If befDateMode = False Then
        Open App.Path & "\ANNOUNCE.DAT" For Binary As fno2
    Else
        Open App.Path & "\ANNOUNCE.BEF" For Binary As fno2
    End If
    Seek #fno2, TOP_DOOR
    Get fno2, , door_data
    Close fno2

    For j = 0 To 5
        sdata = Import_String(data, 2 + 0 + j * 3)
        door_data(i * ONE_DOOR_SIZE + j * 4 + 0) = Val(sdata) \ 256
        door_data(i * ONE_DOOR_SIZE + j * 4 + 1) = Val(sdata) Mod 256
        sdata = Import_String(data, 2 + 1 + j * 3)
        door_data(i * ONE_DOOR_SIZE + j * 4 + 2) = Val(sdata) \ 256
        door_data(i * ONE_DOOR_SIZE + j * 4 + 3) = Val(sdata) Mod 256
        sdata = Import_String(data, 2 + 2 + j * 3)
        If Import_LangSelect(door_data(i * ONE_DOOR_SIZE + j * 4 + 2), sdata) = False Then
            Exit Function
        End If
    Next j
        
        
    fno2 = FreeFile
    If befDateMode = False Then
        Open App.Path & "\ANNOUNCE.DAT" For Binary As fno2
    Else
        Open App.Path & "\ANNOUNCE.BEF" For Binary As fno2
    End If
    Seek #fno2, TOP_DOOR
    Put fno2, , door_data
    Close fno2

    If Err <> 0 Then Exit Function

    Import_Door = True
End Function

Private Function Import_EMG(ByVal data As String, ByVal fno As Integer) As Boolean
    Dim sdata As String
    Dim fno2 As Integer
    Dim bdata As Integer
    Dim i As Integer
    Dim j As Integer

    On Error Resume Next

    sdata = Import_String(data, 1)
    i = Val(Mid$(sdata, 4))
    If i < 1 Or i > 32 Then Exit Function
    i = i - 1

    fno2 = FreeFile
    If befDateMode = False Then
        Open App.Path & "\ANNOUNCE.DAT" For Binary As fno2
    Else
        Open App.Path & "\ANNOUNCE.BEF" For Binary As fno2
    End If
    Seek #fno2, TOP_EMGMA + ONE_EMGMA_SIZE * i
    Get fno2, , emg_data
    Close fno2

    sdata = Import_String(data, 2)
    emg_data(0) = Val("&H" + sdata)
    sdata = Import_String(data, 3)
    emg_data(1) = Asc(Mid$(sdata & "      ", 1, 1))
    emg_data(2) = Asc(Mid$(sdata & "      ", 2, 1))
    emg_data(3) = Asc(Mid$(sdata & "      ", 3, 1))
    emg_data(4) = Asc(Mid$(sdata & "      ", 4, 1))
    emg_data(5) = Asc(Mid$(sdata & "      ", 5, 1))
    emg_data(6) = Asc(Mid$(sdata & "      ", 6, 1))
    sdata = Import_String(data, 4)
    emg_data(7) = Asc(Mid$(sdata & "      ", 1, 1))
    emg_data(8) = Asc(Mid$(sdata & "      ", 2, 1))
    emg_data(9) = Asc(Mid$(sdata & "      ", 3, 1))
    emg_data(10) = Asc(Mid$(sdata & "      ", 4, 1))
    emg_data(11) = Asc(Mid$(sdata & "      ", 5, 1))
    emg_data(12) = Asc(Mid$(sdata & "      ", 6, 1))

    For j = 0 To 5
        sdata = Import_String(data, 5 + 0 + j * 3)
        emg_data(13 + j * 4 + 0) = Val(sdata) \ 256
        emg_data(13 + j * 4 + 1) = Val(sdata) Mod 256
        sdata = Import_String(data, 5 + 1 + j * 3)
        emg_data(13 + j * 4 + 2) = Val(sdata) \ 256
        emg_data(13 + j * 4 + 3) = Val(sdata) Mod 256
        sdata = Import_String(data, 5 + 2 + j * 3)
        If Import_LangSelect(emg_data(13 + j * 4 + 2), sdata) = False Then
            Exit Function
        End If
    Next j
        
    fno2 = FreeFile
    If befDateMode = False Then
        Open App.Path & "\ANNOUNCE.DAT" For Binary As fno2
    Else
        Open App.Path & "\ANNOUNCE.BEF" For Binary As fno2
    End If
    Seek #fno2, TOP_EMGMA + ONE_EMGMA_SIZE * i
    Put fno2, , emg_data
    Close fno2

    If Err <> 0 Then Exit Function

    Import_EMG = True
End Function

Private Function Import_SPC(ByVal data As String, ByVal fno As Integer) As Boolean
    Dim sdata As String
    Dim fno2 As Integer
    Dim bdata As Integer
    Dim i As Integer
    Dim j As Integer

    On Error Resume Next

    sdata = Import_String(data, 1)
    i = Val(Mid$(sdata, 4))
    If i < 1 Or i > 64 Then Exit Function
    i = i - 1

    fno2 = FreeFile
    If befDateMode = False Then
        Open App.Path & "\ANNOUNCE.DAT" For Binary As fno2
    Else
        Open App.Path & "\ANNOUNCE.BEF" For Binary As fno2
    End If
    Seek #fno2, TOP_SPCMA + ONE_SPCMA_SIZE * i
    Get fno2, , spc_data
    Close fno2

    sdata = Import_String(data, 2)
    spc_data(0) = Val("&H" + sdata)
    sdata = Import_String(data, 3)
    spc_data(1) = Asc(Mid$(sdata & "      ", 1, 1))
    spc_data(2) = Asc(Mid$(sdata & "      ", 2, 1))
    spc_data(3) = Asc(Mid$(sdata & "      ", 3, 1))
    spc_data(4) = Asc(Mid$(sdata & "      ", 4, 1))
    spc_data(5) = Asc(Mid$(sdata & "      ", 5, 1))
    spc_data(6) = Asc(Mid$(sdata & "      ", 6, 1))
    sdata = Import_String(data, 4)
    spc_data(7) = Asc(Mid$(sdata & "      ", 1, 1))
    spc_data(8) = Asc(Mid$(sdata & "      ", 2, 1))
    spc_data(9) = Asc(Mid$(sdata & "      ", 3, 1))
    spc_data(10) = Asc(Mid$(sdata & "      ", 4, 1))
    spc_data(11) = Asc(Mid$(sdata & "      ", 5, 1))
    spc_data(12) = Asc(Mid$(sdata & "      ", 6, 1))

    For j = 0 To 5
        sdata = Import_String(data, 5 + 0 + j * 3)
        spc_data(13 + j * 4 + 0) = Val(sdata) \ 256
        spc_data(13 + j * 4 + 1) = Val(sdata) Mod 256
        sdata = Import_String(data, 5 + 1 + j * 3)
        spc_data(13 + j * 4 + 2) = Val(sdata) \ 256
        spc_data(13 + j * 4 + 3) = Val(sdata) Mod 256
        sdata = Import_String(data, 5 + 2 + j * 3)
        If Import_LangSelect(spc_data(13 + j * 4 + 2), sdata) = False Then
            Exit Function
        End If
    Next j
        
    fno2 = FreeFile
    If befDateMode = False Then
        Open App.Path & "\ANNOUNCE.DAT" For Binary As fno2
    Else
        Open App.Path & "\ANNOUNCE.BEF" For Binary As fno2
    End If
    Seek #fno2, TOP_SPCMA + ONE_SPCMA_SIZE * i
    Put fno2, , spc_data
    Close fno2

    If Err <> 0 Then Exit Function

    Import_SPC = True
End Function

Private Function Import_Name(ByVal data As String) As String
    Dim i As Integer
    Dim sdata As String
    Dim found As Boolean

    sdata = ""
    found = False
    For i = 1 To Len(data)
        If found = False Then
            If Mid$(data, i, 1) = "(" Then
                found = True
            End If
        Else
            If Mid$(data, i, 1) = ")" Then
                Import_Name = sdata
                Exit Function
            End If
            sdata = sdata + Mid$(data, i, 1)
        End If
    Next i

    Import_Name = ""
End Function

Private Function Import_Route(ByVal data As String, ByVal fno As Integer) As Boolean
    Dim sdata As String
    Dim fno2 As Integer
    Dim bdata As Integer
    Dim i As Integer
    Dim j As Integer
    Dim routeName As String
    Dim fnoS As Integer
    Dim fnoD As Integer
    Dim buff As String

    On Error Resume Next

    sdata = Import_String(data, 1)
    i = Val("&H" + Mid$(sdata, 11, 2))
    If i < 1 Or i > 255 Then Exit Function

    selroute = i
    routeName = Import_Name(sdata)

    fno2 = FreeFile
    If befDateMode = False Then
        Open App.Path & "\ROUTE" & Right$("00" + Hex$(selroute), 2) & ".DAT" For Binary As fno2
    Else
        Open App.Path & "\ROUTE" & Right$("00" + Hex$(selroute), 2) & ".BEF" For Binary As fno2
    End If
    Get fno2, , route_data
    Close fno2

    For i = 0 To ROUTE_SIZE - 1
        route_data(i) = 0
    Next i

    route_data(0) = selroute

    sdata = Import_String(data, 2)
    route_data(1) = Val("&H" + sdata)
    sdata = Import_String(data, 3)
    route_data(2) = Asc(Mid$(sdata & "      ", 1, 1))
    route_data(3) = Asc(Mid$(sdata & "      ", 2, 1))
    route_data(4) = Asc(Mid$(sdata & "      ", 3, 1))
    route_data(5) = Asc(Mid$(sdata & "      ", 4, 1))
    route_data(6) = Asc(Mid$(sdata & "      ", 5, 1))
    route_data(7) = Asc(Mid$(sdata & "      ", 6, 1))
        
    fno2 = FreeFile
    If befDateMode = False Then
        Open App.Path & "\ROUTE" & Right$("00" + Hex$(selroute), 2) & ".DAT" For Binary As fno2
    Else
        Open App.Path & "\ROUTE" & Right$("00" + Hex$(selroute), 2) & ".BEF" For Binary As fno2
    End If
    Put fno2, , route_data
    Close fno2

    If Err <> 0 Then Exit Function
    
    Call SetRouteName(selroute, routeName)

    Err.Clear

    Import_Route = True
End Function

Private Function Import_Station(ByVal data As String, ByVal fno As Integer) As Boolean
    Dim sdata As String
    Dim fno2 As Integer
    Dim bdata As Integer
    Dim i As Integer
    Dim j As Integer
    Dim fnoS As Integer
    Dim fnoD As Integer
    Dim buff As String
    Dim StationName As String
    Dim lineNo As Integer
    Dim stationNumber As Integer
    Dim stationNumberName As String
    Dim LineNumberName As String

    On Error Resume Next

    fno2 = FreeFile
    If befDateMode = False Then
        Open App.Path & "\ROUTE" & Right$("00" + Hex$(selroute), 2) & ".DAT" For Binary As fno2
    Else
        Open App.Path & "\ROUTE" & Right$("00" + Hex$(selroute), 2) & ".BEF" For Binary As fno2
    End If
    Get fno2, , route_data
    Close fno2

    For i = 0 To 63
        If route_data(8 + i * STATION_SIZE + 0) = 0 Then
            Exit For
        End If
    Next i

    If i > 63 Then Exit Function

    sdata = Import_String(data, 1)
    route_data(8 + i * STATION_SIZE + 0) = Val("&H" + Mid$(sdata, 13, 2))
    StationName = Import_Name(sdata)

    sdata = Import_String(data, 2)
    If sdata = "Leaving" Then
    ElseIf sdata = "Arriving" Then
        route_data(8 + i * STATION_SIZE + 9) = &H80
    ElseIf sdata = "Arrived" Then
        route_data(8 + i * STATION_SIZE + 9) = &H40
    ElseIf sdata = "Extra" Then
        route_data(8 + i * STATION_SIZE + 9) = &H60
    Else
        Exit Function
    End If
    
    sdata = Import_String(data, 3)
    route_data(8 + i * STATION_SIZE + 1) = Val("&H" + sdata)
    sdata = Import_String(data, 4)
    route_data(8 + i * STATION_SIZE + 2) = Asc(Mid$(sdata & "      ", 1, 1))
    route_data(8 + i * STATION_SIZE + 3) = Asc(Mid$(sdata & "      ", 2, 1))
    route_data(8 + i * STATION_SIZE + 4) = Asc(Mid$(sdata & "      ", 3, 1))
    route_data(8 + i * STATION_SIZE + 5) = Asc(Mid$(sdata & "      ", 4, 1))
    route_data(8 + i * STATION_SIZE + 6) = Asc(Mid$(sdata & "      ", 5, 1))
    route_data(8 + i * STATION_SIZE + 7) = Asc(Mid$(sdata & "      ", 6, 1))

'Line No
    sdata = Import_String(data, 5)
    lineNo = Val(Mid$(sdata, 1, 1))
    LineNumberName = Import_Name(sdata)
'Station No
    sdata = Import_String(data, 6)
    stationNumber = Val("&H" + Mid$(sdata, 1, 2))
    stationNumberName = Import_Name(sdata)
    
    route_data(8 + i * STATION_SIZE + 8) = lineNo * 32 + stationNumber
    
    For j = 0 To 5
        sdata = Import_String(data, 7 + 0 + j * 3)
        route_data(8 + i * STATION_SIZE + 10 + j * 4 + 0) = Val(sdata) \ 256
        route_data(8 + i * STATION_SIZE + 10 + j * 4 + 1) = Val(sdata) Mod 256
        sdata = Import_String(data, 7 + 1 + j * 3)
        route_data(8 + i * STATION_SIZE + 10 + j * 4 + 2) = Val(sdata) \ 256
        route_data(8 + i * STATION_SIZE + 10 + j * 4 + 3) = Val(sdata) Mod 256
        sdata = Import_String(data, 7 + 2 + j * 3)
        If Import_LangSelect(route_data(8 + i * STATION_SIZE + 10 + j * 4 + 2), sdata) = False Then
            Exit Function
        End If
    Next j
        
    fno2 = FreeFile
    If befDateMode = False Then
        Open App.Path & "\ROUTE" & Right$("00" + Hex$(selroute), 2) & ".DAT" For Binary As fno2
    Else
        Open App.Path & "\ROUTE" & Right$("00" + Hex$(selroute), 2) & ".BEF" For Binary As fno2
    End If
    Put fno2, , route_data
    Close fno2

    If Err <> 0 Then Exit Function

    Call SetStationName(CInt(Hex$(route_data(8 + i * STATION_SIZE + 0))), StationName)

    Call SetLineName(selroute, lineNo, LineNumberName)

    Call SetStationNumberName(selroute, stationNumber, stationNumberName)

    Err.Clear

    Import_Station = True
End Function

Private Sub Import_ErrorMessage()
    
    MsgBox "Error Found Line No.=" & getLineNo, vbExclamation

End Sub

Private Sub cmdImport_Click()
    Dim fno As Integer
    Dim data As String
    Dim i As Integer

    On Error Resume Next

    FileSelectDialog.Flags = cdlOFNHideReadOnly Or cdlOFNPathMustExist Or cdlOFNReadOnly
    FileSelectDialog.CancelError = True
    FileSelectDialog.DefaultExt = "CSV"
    FileSelectDialog.Filter = "CSV|*.CSV"
    FileSelectDialog.ShowOpen
    
    If Err <> 0 Then Exit Sub

    fno = FreeFile
    Open FileSelectDialog.filename For Input As fno
    getLineNo = 0
    Do While EOF(fno) = False
        Line Input #fno, data
        getLineNo = getLineNo + 1
        If Import_String(data, 1) = "Language" Then
            If Import_Language(data, fno) = False Then
                Import_ErrorMessage
                Exit Do
            End If
        End If
        If Import_String(data, 1) = "Version" Then
            Line Input #fno, data
            getLineNo = getLineNo + 1
            If Import_VersionDVAU(data, fno) = False Then
                Import_ErrorMessage
                Exit Do
            End If
            Line Input #fno, data
            getLineNo = getLineNo + 1
            If Import_VersionMCP(data, fno) = False Then
                Import_ErrorMessage
                Exit Do
            End If
            Line Input #fno, data
            getLineNo = getLineNo + 1
            If Import_VersionPAA(data, fno) = False Then
                Import_ErrorMessage
                Exit Do
            End If
        End If
        If Import_String(data, 1) = "Initial Voice Level" Then
            Line Input #fno, data
            getLineNo = getLineNo + 1
            Line Input #fno, data
            getLineNo = getLineNo + 1
            If Import_Voice(data, fno) = False Then
                Import_ErrorMessage
                Exit Do
            End If
        End If
        If Import_String(data, 1) = "Chime" Then
            If Import_Chime(data, fno) = False Then
                Import_ErrorMessage
                Exit Do
            End If
        End If
        If Import_String(data, 1) = "Door A Open" Then
            If Import_Door(data, fno) = False Then
                Import_ErrorMessage
                Exit Do
            End If
        End If
        If Import_String(data, 1) = "Door A Close" Then
            If Import_Door(data, fno) = False Then
                Import_ErrorMessage
                Exit Do
            End If
        End If
        If Import_String(data, 1) = "Door B Open" Then
            If Import_Door(data, fno) = False Then
                Import_ErrorMessage
                Exit Do
            End If
        End If
        If Import_String(data, 1) = "Door B Close" Then
            If Import_Door(data, fno) = False Then
                Import_ErrorMessage
                Exit Do
            End If
        End If
        For i = 0 To 31
            If Import_String(data, 1) = "EMG" & Trim$(i + 1) Then
                If Import_EMG(data, fno) = False Then
                    Import_ErrorMessage
                    Exit Do
                End If
            End If
        Next i
        For i = 0 To 63
            If Import_String(data, 1) = "SPC" & Trim$(i + 1) Then
                If Import_SPC(data, fno) = False Then
                    Import_ErrorMessage
                    Exit Do
                End If
            End If
        Next i
        If Mid$(Import_String(data, 1), 1, 10) = "Route Code" Then
            If Import_Route(data, fno) = False Then
                Import_ErrorMessage
                Exit Do
            End If
        End If
        If Mid$(Import_String(data, 1), 1, 12) = "Station Code" Then
            If Import_Station(data, fno) = False Then
                Import_ErrorMessage
                Exit Do
            End If
        End If
    Loop
    Close fno
End Sub
