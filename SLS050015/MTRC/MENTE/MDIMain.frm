VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.2#0"; "COMDLG32.OCX"
Begin VB.MDIForm MDIMain 
   BackColor       =   &H8000000C&
   Caption         =   "Maintenance Terminal"
   ClientHeight    =   6150
   ClientLeft      =   165
   ClientTop       =   855
   ClientWidth     =   9570
   LinkTopic       =   "MDIForm1"
   StartUpPosition =   3  'Windows ÇÃä˘íËíl
   Begin MSComctlLib.StatusBar ProcessStatus 
      Align           =   2  'â∫ëµÇ¶
      Height          =   285
      Left            =   0
      TabIndex        =   1
      Top             =   5865
      Width           =   9570
      _ExtentX        =   16880
      _ExtentY        =   503
      _Version        =   393216
      BeginProperty Panels {8E3867A5-8586-11D1-B16A-00C0F0283628} 
         NumPanels       =   6
         BeginProperty Panel1 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
            AutoSize        =   1
            Object.Width           =   3149
            Text            =   "Connect"
            TextSave        =   "Connect"
         EndProperty
         BeginProperty Panel2 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
            AutoSize        =   1
            Object.Width           =   3149
            Text            =   "Action"
            TextSave        =   "Action"
         EndProperty
         BeginProperty Panel3 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
            AutoSize        =   1
            Object.Width           =   3149
            Text            =   "Filename"
            TextSave        =   "Filename"
         EndProperty
         BeginProperty Panel4 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
            AutoSize        =   1
            Object.Width           =   3149
            Text            =   "Progress"
            TextSave        =   "Progress"
         EndProperty
         BeginProperty Panel5 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
            Style           =   6
            Object.Width           =   1764
            MinWidth        =   1764
            TextSave        =   "2025/08/25"
         EndProperty
         BeginProperty Panel6 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
            Style           =   5
            Object.Width           =   1764
            MinWidth        =   1764
            TextSave        =   "11:33"
         EndProperty
      EndProperty
   End
   Begin VB.Timer UpdateTimer 
      Interval        =   5000
      Left            =   1260
      Top             =   960
   End
   Begin MSComDlg.CommonDialog FileSelectDialog 
      Left            =   360
      Top             =   990
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   393216
   End
   Begin MSComctlLib.Toolbar MainToolbar 
      Align           =   1  'è„ëµÇ¶
      Height          =   615
      Left            =   0
      TabIndex        =   0
      Top             =   0
      Width           =   9570
      _ExtentX        =   16880
      _ExtentY        =   1085
      ButtonWidth     =   1905
      ButtonHeight    =   926
      Appearance      =   1
      _Version        =   393216
      BeginProperty Buttons {66833FE8-8583-11D1-B16A-00C0F0283628} 
         NumButtons      =   6
         BeginProperty Button1 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Caption         =   "TMS Monitor"
            Key             =   "TMS"
         EndProperty
         BeginProperty Button2 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Caption         =   "TRACE"
            Key             =   "TRACE"
         EndProperty
         BeginProperty Button3 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Style           =   4
            Object.Width           =   2000
            MixedState      =   -1  'True
         EndProperty
         BeginProperty Button4 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Caption         =   "CONFIG"
            Key             =   "CONFIG"
         EndProperty
         BeginProperty Button5 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Style           =   4
            Object.Width           =   1000
         EndProperty
         BeginProperty Button6 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Caption         =   "DOWNLOAD"
            Key             =   "DOWNLOAD"
         EndProperty
      EndProperty
   End
   Begin VB.Menu mnuFILE 
      Caption         =   "FILE(&F)"
      Begin VB.Menu nmuDOWNLOAD 
         Caption         =   "DOWNLOAD(&D)"
      End
      Begin VB.Menu nmuEXIT 
         Caption         =   "Exit(&X)"
      End
   End
   Begin VB.Menu mnuEDIT 
      Caption         =   "EDIT(&E)"
      Begin VB.Menu mnuCONFIG 
         Caption         =   "CONFIG(&C)"
      End
   End
   Begin VB.Menu nmuDISPLAY 
      Caption         =   "Display(&D)"
      Begin VB.Menu mnuTMS 
         Caption         =   "TMS Monitor(&M)"
      End
      Begin VB.Menu mnuTRACE 
         Caption         =   "TRACE(&T)"
      End
      Begin VB.Menu mnuDEBUG 
         Caption         =   "DebugWindow(&D)"
      End
   End
   Begin VB.Menu mnuHELP 
      Caption         =   "Help(&H)"
      Begin VB.Menu mnuAbout 
         Caption         =   "About(&A)"
      End
   End
End
Attribute VB_Name = "MDIMain"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit


Dim bdata(0 To 127) As Byte

Dim phase As Integer
Dim rBuff As String

Dim bef_Active As Boolean

Public WithEvents MenteCOMM As clsMenteComm
Attribute MenteCOMM.VB_VarHelpID = -1


Public Function selectDirectory() As Boolean
    Dim fnamework As String
    
    On Error Resume Next

    FileSelectDialog.DefaultExt = ".DAT"
    FileSelectDialog.DialogTitle = "DVA Data/Program Select"
    FileSelectDialog.Filter = "DATA File|*.DAT|Prog File|*.MOT"
    FileSelectDialog.CancelError = True
    FileSelectDialog.Flags = cdlOFNFileMustExist Or cdlOFNHideReadOnly
    FileSelectDialog.filename = "CONFIG.DAT"
    If DataDir = "" Then DataDir = App.Path
    FileSelectDialog.InitDir = DataDir
    FileSelectDialog.ShowOpen

    If Err <> 0 Then Exit Function

    fnamework = SplitPath(FileSelectDialog.filename, False, DataDir)

    If Dir(DataDir & "\CONFIG.DAT") = "" Then
        MsgBox "Illegal Folder"
        Exit Function
    End If
    If Dir(DataDir & "\ANNOUNCE.DAT") = "" Then
        MsgBox "Illegal Folder"
        Exit Function
    End If
    If Dir(DataDir & "\ROUTE*.DAT") = "" Then
        MsgBox "Illegal Folder"
        Exit Function
    End If
    selectDirectory = True

End Function


Private Sub MenteCOMM_CommAction(ByVal msg As String)

    ProcessStatus.Panels(2).Text = Left$(msg + Space$(20), 20)
    frmDEBUG.ActionMessage ProcessStatus.Panels(2)

End Sub

Private Sub menteComm_DebugSend(ByVal str As String)

    frmDEBUG.SendMessage str

End Sub

Private Sub menteComm_DebugRecv(ByVal str As String)

    frmDEBUG.RecvMessage str

End Sub

Private Sub MainToolbar_ButtonClick(ByVal Button As MSComctlLib.Button)

    Select Case Button.Key
    Case "TMS"
        frmSDR.DoModeless
        MenteCOMM.SDRRequest
    Case "TRACE"
        frmTrace.DoModeless
    Case "CONFIG"
        frmConfig.Show
    Case "DOWNLOAD"
        frmDownload.DoModeless
    End Select

End Sub

Private Sub MDIForm_Load()

    menteMode = True

    Set MenteCOMM = New clsMenteComm
    
    ProcessStatus.Panels(1).Text = "DisConnect"      'Connect
    ProcessStatus.Panels(2).Text = "          "       'Action
    ProcessStatus.Panels(3).Text = "          "       'Filename
    ProcessStatus.Panels(4).Text = "          "       'Progress
    
End Sub

Private Sub MDIForm_QueryUnload(Cancel As Integer, UnloadMode As Integer)

    If MsgBox("Maintenance PC End OK?", vbOKCancel) = vbCancel Then
        Cancel = 1
    End If

    Unload frmDEBUG
End Sub

Private Sub MDIForm_Unload(Cancel As Integer)

    Unload frmConfig
    Unload frmSequence
    Unload frmSPC
    Unload frmEMG
    Unload frmStation
    Unload frmDoorSelect
    Unload frmRouteSelect

    Set MenteCOMM = Nothing

    End
End Sub

Private Sub menteComm_RecvDownLoad(ByVal filename As String, ByVal nowSize As Long, ByVal allSize As Long, ByVal bOK As Boolean)

    ProcessStatus.Panels(2).Text = "Downloading..."
    ProcessStatus.Panels(3).Text = filename
    If allSize > 0 Then
        ProcessStatus.Panels(4).Text = nowSize & " / " & allSize & " (" & Format$(nowSize / allSize, "0.0%") & ")"
    Else
        ProcessStatus.Panels(4).Text = nowSize & " / " & allSize
    End If

End Sub

Private Sub MenteCOMM_RecvOK(ByVal bOK As Boolean)

    If bOK = True Then
        frmDEBUG.ActionMessage "Receive OK"
    Else
        frmDEBUG.ActionMessage "Receive NG!!"
    End If
    
End Sub

Private Sub menteComm_RecvSD(ByVal str As String)
        Dim i As Integer
    
        Select Case Mid$(str, 1, 3)
        Case "SD1"
            frmSDR.testEXECResponce.Text = Mid$(str, 4, 4)
            frmSDR.testCatResponce(0).Text = Mid$(str, 8, 4)
            frmSDR.testCatResponce(1).Text = Mid$(str, 12, 4)
            frmSDR.testItemNoResponce.Text = Mid$(str, 18, 2)
            frmSDR.testOKNG.Text = Mid$(str, 20, 2)
            For i = 0 To 7
                frmSDR.testNG(i).Text = Mid$(str, 22 + i * 3, 3)
            Next i
            Exit Sub
        Case "SD2"
            frmSDR.textVersion.Text = Mid$(str, 4, 4)
            frmSDR.txtCRC.Text = Mid$(str, 8, 4)
            Exit Sub
        Case "SD3"
            For i = 0 To 39
                frmSDR.EAHActive(i).Text = Mid$(str, 4 + i * 4, 4)
            Next i
            Exit Sub
        Case "SD4"
            For i = 0 To 11
                frmSDR.comError(i).Text = Mid$(str, 4 + i * 4, 4)
            Next i
        Case "SD5"
            frmSDR.modeRespoince(0).Text = Mid$(str, 4, 6)
            frmSDR.modeRespoince(1).Text = Mid$(str, 10, 7)
            frmSDR.modeRespoince(2).Text = Mid$(str, 17, 9)
            frmSDR.modeRespoince(3).Text = Mid$(str, 26, 7)
            frmSDR.modeRespoince(4).Text = Mid$(str, 33, 4)
            frmSDR.modeRespoince(5).Text = Mid$(str, 37, 4)
            frmSDR.modeRespoince(6).Text = Mid$(str, 41, 4)
        End Select

End Sub

Private Sub menteComm_RecvSDR(ByVal str As String)
        Dim i As Integer

        Select Case Mid$(str, 1, 4)
        Case "SDR1"
            frmSDR.testExecute.Text = Mid$(str, 5, 4)
            frmSDR.testCat(0).Text = Mid$(str, 9, 4)
            frmSDR.testCat(1).Text = Mid$(str, 13, 4)
            frmSDR.testItemNo.Text = Mid$(str, 17, 2)
            Exit Sub
        Case "SDR2"
            frmSDR.timeValid.Text = Mid$(str, 5, 4)
            frmSDR.TimeADJ.Text = Mid$(str, 9, 4)
            frmSDR.carNoValid = Mid$(str, 13, 5)
            frmSDR.carTypeValid.Text = Mid$(str, 18, 7)
            Exit Sub
        Case "SDR3"
            frmSDR.timeData.Text = Mid$(str, 5, 17)
            Exit Sub
        Case "SDR4"
            frmSDR.carNo.Text = Mid$(str, 5, 3)
            frmSDR.carType.Text = Mid$(str, 8, 1)
            Exit Sub
        Case "SDR5"
            frmSDR.couplingUnit.Text = Mid$(str, 5, 1)
            Exit Sub
        Case "SDR6"
            frmSDR.stationNumber.Text = Mid$(str, 5, 2)
            frmSDR.LineNumber.Text = Mid$(str, 7, 1)
            Exit Sub
        Case "SDR7"
            frmSDR.nextStationName.Text = Mid$(str, 5, 2)
            frmSDR.destinationCode.Text = Mid$(str, 7, 2)
            frmSDR.routeCode.Text = Mid$(str, 9, 2)
            frmSDR.specialCode.Text = Mid$(str, 11, 2)
            Exit Sub
        Case "SDR8"
            For i = 0 To 39
                frmSDR.EAH(i).Text = Mid$(str, 5 + i * 4, 4)
            Next i
            Exit Sub
        Case "SDR9"
            If Mid$(str, 5, 2) = "PA" Then
                frmSDR.broadcast.Text = "ATC"
            Else
                frmSDR.broadcast.Text = ""
            End If
            If Mid$(str, 7, 2) = "DB" Then
                frmSDR.pa.Text = "BR"
            Else
                frmSDR.pa.Text = ""
            End If
            Exit Sub
        Case "SDRA"
            frmSDR.DoorLeft.Text = Mid$(str, 5, 1)
            frmSDR.DoorRight.Text = Mid$(str, 6, 1)
            frmSDR.doorEffective.Text = Mid$(str, 7, 4)
            frmSDR.broadcastoff.Text = Mid$(str, 11, 3)
            Exit Sub
        Case "SDRB"
            For i = 0 To 7
                frmSDR.CarNo2(i).Text = Mid$(str, 5 + i * 4, 3)
                frmSDR.carType2(i).Text = Mid$(str, 5 + i * 4 + 3, 1)
            Next i
            Exit Sub
        Case "SDRC"
            For i = 0 To 39
                frmSDR.EAHR(i).Text = Mid$(str, 5 + i * 4, 4)
            Next i
            Exit Sub
            
        End Select
End Sub

Private Sub MenteCOMM_RecvTBL(ByVal str As String)
        Dim i As Integer

        Select Case Mid$(str, 1, 4)
        Case "TRB1"
            If Mid$(str, 5, 6) = Space$(6) Then
                frmSDR.Trouble(0).BackColor = &H80000005
                frmSDR.Trouble(0).Enabled = True
            Else
                frmSDR.Trouble(0).BackColor = &HFF&
                frmSDR.Trouble(0).Enabled = False
            End If
            If Mid$(str, 11, 6) = Space$(6) Then
                frmSDR.Trouble(1).BackColor = &H80000005
                frmSDR.Trouble(1).Enabled = True
            Else
                frmSDR.Trouble(1).BackColor = &HFF&
                 frmSDR.Trouble(1).Enabled = False
           End If
            If Mid$(str, 17, 6) = Space$(6) Then
                frmSDR.Trouble(2).BackColor = &H80000005
                frmSDR.Trouble(2).Enabled = True
            Else
                frmSDR.Trouble(2).BackColor = &HFF&
                frmSDR.Trouble(2).Enabled = False
            End If
            Exit Sub
        Case "TRB2"
            If Mid$(str, 5, 3) = "PAA" Then
                For i = 0 To 6
                    If Mid$(str, 12 + i * 4, 1) = " " Then
                        frmSDR.Trouble(4 + i).BackColor = &H80000005
                        frmSDR.Trouble(4 + i).Enabled = True
                    Else
                        frmSDR.Trouble(4 + i).BackColor = &HFF&
                        frmSDR.Trouble(4 + i).Enabled = False
                    End If
                Next i
            Else
                For i = 0 To 6
                    frmSDR.Trouble(4 + i).BackColor = &H80000005
                    frmSDR.Trouble(4 + i).Enabled = False
                Next i
            End If
            Exit Sub
        Case "TRB3"
            If Mid$(str, 5, 3) = "PAA" Then
                For i = 0 To 6
                    If Mid$(str, 12 + i * 4, 1) = " " Then
                        frmSDR.Trouble(11 + i).BackColor = &H80000005
                        frmSDR.Trouble(11 + i).Enabled = True
                    Else
                        frmSDR.Trouble(11 + i).BackColor = &HFF&
                        frmSDR.Trouble(11 + i).Enabled = False
                    End If
                Next i
            Else
                For i = 0 To 6
                    frmSDR.Trouble(11 + i).BackColor = &H80000005
                    frmSDR.Trouble(11 + i).Enabled = False
                Next i
            End If
            Exit Sub
        Case "TRB4"
            If Mid$(str, 5, 3) = "PAA" Then
                For i = 0 To 6
                    If Mid$(str, 12 + i * 4, 1) = " " Then
                        frmSDR.Trouble(18 + i).BackColor = &H80000005
                        frmSDR.Trouble(18 + i).Enabled = True
                    Else
                        frmSDR.Trouble(18 + i).BackColor = &HFF&
                        frmSDR.Trouble(18 + i).Enabled = False
                    End If
                Next i
            Else
                For i = 0 To 6
                    frmSDR.Trouble(18 + i).BackColor = &H80000005
                    frmSDR.Trouble(18 + i).Enabled = False
                Next i
            End If
            Exit Sub
        Case "TRB5"
            If Mid$(str, 5, 3) = "PAA" Then
                For i = 0 To 6
                    If Mid$(str, 12 + i * 4, 1) = " " Then
                        frmSDR.Trouble(25 + i).BackColor = &H80000005
                        frmSDR.Trouble(25 + i).Enabled = True
                    Else
                        frmSDR.Trouble(25 + i).BackColor = &HFF&
                        frmSDR.Trouble(25 + i).Enabled = False
                    End If
                Next i
            Else
                For i = 0 To 6
                    frmSDR.Trouble(25 + i).BackColor = &H80000005
                    frmSDR.Trouble(25 + i).Enabled = False
                Next i
            End If
            Exit Sub
        Case "TRB6"
            If Mid$(str, 5, 3) = "PAA" Then
                For i = 0 To 6
                    If Mid$(str, 12 + i * 4, 1) = " " Then
                        frmSDR.Trouble(32 + i).BackColor = &H80000005
                        frmSDR.Trouble(32 + i).Enabled = True
                    Else
                        frmSDR.Trouble(32 + i).BackColor = &HFF&
                        frmSDR.Trouble(32 + i).Enabled = False
                    End If
                Next i
            Else
                For i = 0 To 6
                    frmSDR.Trouble(32 + i).BackColor = &H80000005
                    frmSDR.Trouble(32 + i).Enabled = False
                Next i
            End If
            Exit Sub
        Case "TRB7"
            If Mid$(str, 5, 3) = "PAA" Then
                For i = 0 To 6
                    If Mid$(str, 12 + i * 4, 1) = " " Then
                        frmSDR.Trouble(39 + i).BackColor = &H80000005
                        frmSDR.Trouble(39 + i).Enabled = True
                    Else
                        frmSDR.Trouble(39 + i).BackColor = &HFF&
                        frmSDR.Trouble(39 + i).Enabled = False
                    End If
                Next i
            Else
                For i = 0 To 6
                    frmSDR.Trouble(39 + i).BackColor = &H80000005
                    frmSDR.Trouble(39 + i).Enabled = False
                Next i
            End If
            Exit Sub
        Case "TRB8"
            If Mid$(str, 5, 3) = "PAA" Then
                For i = 0 To 6
                    If Mid$(str, 12 + i * 4, 1) = " " Then
                        frmSDR.Trouble(46 + i).BackColor = &H80000005
                        frmSDR.Trouble(46 + i).Enabled = True
                    Else
                        frmSDR.Trouble(46 + i).BackColor = &HFF&
                        frmSDR.Trouble(46 + i).Enabled = False
                    End If
                Next i
            Else
                For i = 0 To 6
                    frmSDR.Trouble(46 + i).BackColor = &H80000005
                    frmSDR.Trouble(46 + i).Enabled = False
                Next i
            End If
            Exit Sub
        Case "TRB9"
            If Mid$(str, 5, 3) = "PAA" Then
                For i = 0 To 6
                    If Mid$(str, 12 + i * 4, 1) = " " Then
                        frmSDR.Trouble(53 + i).BackColor = &H80000005
                        frmSDR.Trouble(53 + i).Enabled = True
                    Else
                        frmSDR.Trouble(53 + i).BackColor = &HFF&
                        frmSDR.Trouble(53 + i).Enabled = False
                    End If
                Next i
            Else
                For i = 0 To 6
                    frmSDR.Trouble(53 + i).BackColor = &H80000005
                    frmSDR.Trouble(53 + i).Enabled = False
                Next i
            End If
            Exit Sub
        End Select
End Sub

Private Sub menteComm_RecvTrace(ByVal str As String)
    Dim fno As Integer

    fno = FreeFile
    Open App.Path & "\TRACE" & Format$(Now, "yymmddhhnn") & ".TXT" For Append As fno
    Print #fno, str
    Close #fno
    
    frmTrace.txtTraceData.SelText = str & vbCrLf
    frmTrace.txtTraceData.SelStart = Len(frmTrace.txtTraceData.Text)

End Sub

Private Sub mnuAbout_Click()

    frmAbout.Show
    
End Sub

Private Sub mnuCONFIG_Click()

    frmConfig.Show

End Sub

Private Sub mnuDEBUG_Click()

    frmDEBUG.Show

End Sub

Private Sub mnuTMS_Click()

    frmSDR.DoModeless

End Sub

Private Sub mnuTRACE_Click()

    frmTrace.DoModeless

End Sub

Private Sub nmuDOWNLOAD_Click()
        
    frmDownload.DoModeless

End Sub

Private Sub nmuEXIT_Click()

    Unload Me

End Sub

Private Sub UpdateTimer_Timer()

    If MenteCOMM.OnlineStatus = 0 Then
        ProcessStatus.Panels(1).Text = "DisConnect"       'Connect
    Else
        ProcessStatus.Panels(1).Text = "Connect"       'Connect
    End If
    
End Sub
