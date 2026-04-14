VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.2#0"; "Comdlg32.ocx"
Begin VB.MDIForm MDIMain 
   BackColor       =   &H8000000C&
   Caption         =   "TMS Simulator"
   ClientHeight    =   5130
   ClientLeft      =   165
   ClientTop       =   855
   ClientWidth     =   10800
   LinkTopic       =   "MDIForm1"
   StartUpPosition =   3  'Windows ÇÃä˘íËíl
   Begin VB.Timer UpdateTimer 
      Interval        =   5000
      Left            =   1200
      Top             =   1260
   End
   Begin MSComctlLib.Toolbar MainToolbar 
      Align           =   1  'è„ëµÇ¶
      Height          =   615
      Left            =   0
      TabIndex        =   0
      Top             =   0
      Width           =   10800
      _ExtentX        =   19050
      _ExtentY        =   1085
      ButtonWidth     =   1429
      ButtonHeight    =   926
      Appearance      =   1
      _Version        =   393216
      BeginProperty Buttons {66833FE8-8583-11D1-B16A-00C0F0283628} 
         NumButtons      =   6
         BeginProperty Button1 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Caption         =   "AUTO"
            Key             =   "AUTO"
         EndProperty
         BeginProperty Button2 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Caption         =   "MANUAL"
            Key             =   "MANUAL"
         EndProperty
         BeginProperty Button3 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Style           =   4
            Object.Width           =   1000
         EndProperty
         BeginProperty Button4 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Caption         =   "LDR"
            Key             =   "LDR"
         EndProperty
         BeginProperty Button5 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Style           =   4
            Object.Width           =   2000
            MixedState      =   -1  'True
         EndProperty
         BeginProperty Button6 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Caption         =   "DEBUG"
            Key             =   "DEBUG"
         EndProperty
      EndProperty
   End
   Begin MSComDlg.CommonDialog FileSelectDialog 
      Left            =   270
      Top             =   1320
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   393216
   End
   Begin MSComctlLib.StatusBar ProcessStatus 
      Align           =   2  'â∫ëµÇ¶
      Height          =   285
      Left            =   0
      TabIndex        =   1
      Top             =   4845
      Width           =   10800
      _ExtentX        =   19050
      _ExtentY        =   503
      _Version        =   393216
      BeginProperty Panels {8E3867A5-8586-11D1-B16A-00C0F0283628} 
         NumPanels       =   6
         BeginProperty Panel1 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
            AutoSize        =   1
            Object.Width           =   3704
            Text            =   "Connect"
            TextSave        =   "Connect"
         EndProperty
         BeginProperty Panel2 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
            AutoSize        =   1
            Object.Width           =   3704
            Text            =   "Action"
            TextSave        =   "Action"
         EndProperty
         BeginProperty Panel3 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
            AutoSize        =   1
            Object.Width           =   3704
            Text            =   "Filename"
            TextSave        =   "Filename"
         EndProperty
         BeginProperty Panel4 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
            AutoSize        =   1
            Object.Width           =   3704
            Text            =   "Progress"
            TextSave        =   "Progress"
         EndProperty
         BeginProperty Panel5 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
            Style           =   6
            Object.Width           =   1764
            MinWidth        =   1764
            TextSave        =   "2001/01/01"
         EndProperty
         BeginProperty Panel6 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
            Style           =   5
            Object.Width           =   1764
            MinWidth        =   1764
            TextSave        =   "1:51"
         EndProperty
      EndProperty
   End
   Begin VB.Menu mnuFILE 
      Caption         =   "FILE(&F)"
      Begin VB.Menu AUTO 
         Caption         =   "AUTO(&A)"
      End
      Begin VB.Menu MANUAL 
         Caption         =   "MANUAL(&M)"
      End
      Begin VB.Menu nmuEXIT 
         Caption         =   "Exit(&X)"
      End
   End
   Begin VB.Menu nmuDISPLAY 
      Caption         =   "Display(&D)"
      Begin VB.Menu mnuTMS 
         Caption         =   "TMS SET(&M)"
      End
      Begin VB.Menu mnuDEBUG 
         Caption         =   "DebugWindow(&D)"
      End
   End
   Begin VB.Menu mnuHelp 
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

Public WithEvents MenteCOMM As clsMenteComm
Attribute MenteCOMM.VB_VarHelpID = -1

Dim bdata(0 To 15) As Byte

Public Function selectDirectory() As Boolean
    Dim fnamework As String
    
    On Error Resume Next

    If befDateMode = False Then
        FileSelectDialog.DefaultExt = ".DAT"
    Else
        FileSelectDialog.DefaultExt = ".BEF"
    End If
    FileSelectDialog.DialogTitle = "DVA Data/Program Select"
    If befDateMode = False Then
        FileSelectDialog.Filter = "DATA File|*.DAT|Prog File|*.MOT"
    Else
        FileSelectDialog.Filter = "DATA File|*.BEF|Prog File|*.MOT"
    End If
    FileSelectDialog.CancelError = True
    FileSelectDialog.Flags = cdlOFNFileMustExist Or cdlOFNHideReadOnly
    If befDateMode = False Then
        FileSelectDialog.filename = "CONFIG.DAT"
    Else
        FileSelectDialog.filename = "CONFIG.BEF"
    End If
    If DataDir = "" Then DataDir = App.Path
    FileSelectDialog.InitDir = DataDir
    FileSelectDialog.ShowOpen

    If Err <> 0 Then Exit Function

    fnamework = SplitPath(FileSelectDialog.filename, False, DataDir)

    If befDateMode = False Then
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
    Else
        If Dir(DataDir & "\CONFIG.BEF") = "" Then
            MsgBox "Illegal Folder"
            Exit Function
        End If
        If Dir(DataDir & "\ANNOUNCE.BEF") = "" Then
            MsgBox "Illegal Folder"
            Exit Function
        End If
        If Dir(DataDir & "\ROUTE*.BEF") = "" Then
            MsgBox "Illegal Folder"
            Exit Function
        End If
    End If
    selectDirectory = True

End Function


Private Sub AUTO_Click()

    frmAUTO.DoModeless

End Sub

Private Sub MANUAL_Click()

    frmSDRSet.DoModeless
    
End Sub

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
    Case "DEBUG"
        frmSDR.DoModeless
    Case "MANUAL"
        frmSDRSet.DoModeless
        MenteCOMM.SDRequest
    Case "AUTO"
        frmAUTO.DoModeless
        MenteCOMM.SDRequest
    
    Case "LDR"
        LDR_Command
    End Select

End Sub

Private Sub LDR_Command()
    Dim fno As Integer
    Dim i As Integer
    Dim str As String
    Dim befTime As Double

    If selectDirectory = False Then Exit Sub

    If MenteCOMM.OnLine = False Then Exit Sub

    fno = FreeFile

    If befDateMode = False Then
        Open DataDir & "\CONFIG.DAT" For Binary As #fno
    Else
        Open DataDir & "\CONFIG.BEF" For Binary As #fno
    End If
    Get #fno, , bdata
    Close #fno
    
    str = ""
    For i = 0 To 15
        str = str & Right$("00" + Hex$(bdata(i)), 2)
    Next

    befTime = Timer
    Do
        If MenteCOMM.LDRRequest(str) = True Then Exit Do
        DoEvents
    Loop While (Timer - befTime < 10#)

    MenteCOMM.OffLine

End Sub

Private Sub MenteCOMM_recvLD(ByVal str As String)

    ProcessStatus.Panels(2).Text = Left$(str + Space$(20), 20)

End Sub

Private Sub MDIForm_Load()

    menteMode = False

    Set MenteCOMM = New clsMenteComm
    
    ProcessStatus.Panels(1).Text = "DisConnect"      'Connect
    ProcessStatus.Panels(2).Text = "          "       'Action
    ProcessStatus.Panels(3).Text = "          "       'Filename
    ProcessStatus.Panels(4).Text = "          "       'Progress
    
End Sub

Private Sub MDIForm_QueryUnload(Cancel As Integer, UnloadMode As Integer)

    If MsgBox("Simulator End OK?", vbOKCancel) = vbCancel Then
        Cancel = 1
    End If

    Unload frmDEBUG
End Sub

Private Sub MDIForm_Unload(Cancel As Integer)

    Set MenteCOMM = Nothing

    End
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
            '    frmSDR.EAHActive(i).Text = Mid$(str, 5 + i * 4, 4)
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

Private Sub mnuAbout_Click()

    frmAbout.Show
    
End Sub

Private Sub mnuDEBUG_Click()

    frmDEBUG.Show

End Sub

Private Sub mnuTMS_Click()

    frmSDRSet.DoModeless

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


