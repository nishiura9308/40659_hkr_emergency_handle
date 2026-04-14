VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.2#0"; "COMDLG32.OCX"
Begin VB.Form frmDownload 
   BorderStyle     =   1  'å≈íË(é¿ê¸)
   Caption         =   "Download Select"
   ClientHeight    =   2760
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   4395
   ControlBox      =   0   'False
   LinkTopic       =   "Form2"
   MaxButton       =   0   'False
   MDIChild        =   -1  'True
   MinButton       =   0   'False
   ScaleHeight     =   2760
   ScaleWidth      =   4395
   Begin VB.Timer CheckTimer 
      Interval        =   1000
      Left            =   300
      Top             =   1650
   End
   Begin VB.CommandButton cmdOK 
      Caption         =   "O K"
      Height          =   375
      Left            =   1590
      TabIndex        =   3
      Top             =   2250
      Width           =   1215
   End
   Begin VB.CommandButton cmdStation 
      Caption         =   "Station Data Download"
      BeginProperty Font 
         Name            =   "Courier New"
         Size            =   9
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   525
      Left            =   720
      TabIndex        =   1
      Top             =   840
      Width           =   3195
   End
   Begin VB.CommandButton cmdCONFIG 
      Caption         =   "Configuration Data Download"
      BeginProperty Font 
         Name            =   "Courier New"
         Size            =   9
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   585
      Left            =   720
      TabIndex        =   0
      Top             =   150
      Width           =   3195
   End
   Begin VB.CommandButton cmdProgram 
      Caption         =   "Program Update"
      BeginProperty Font 
         Name            =   "Courier New"
         Size            =   9
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   525
      Left            =   1080
      TabIndex        =   2
      Top             =   1530
      Width           =   2265
   End
   Begin MSComDlg.CommonDialog FileSelectDialog 
      Left            =   90
      Top             =   150
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   393216
   End
End
Attribute VB_Name = "frmDownload"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Dim configSet As Integer
Dim prog1name As String
Dim prog2name As String
Dim progset As Integer
Dim reconf As Boolean
Dim reboot As Boolean
Dim reconf2 As Boolean

Public Sub DoModeless()

    reconf = False
    reboot = False

    frmLogin.Show vbModal
    If MDIMain.MenteCOMM.OnLine = False Then
        MsgBox "Not Online"
        Exit Sub
    End If

    If frmLogin.LoginSucceeded = True Then
        cmdProgram.Visible = True
    Else
        cmdProgram.Visible = False
    End If

    Me.Show vbModeless

End Sub

Public Sub ReconfSet()
    reconf2 = True
End Sub

Private Sub CheckTimer_Timer()

    If MDIMain.MenteCOMM.RequestMode = 0 Then
        If progset = 1 Then
            If MDIMain.MenteCOMM.DownProgRequest(App.Path & "\DVAU2.MOT") = True Then
                cmdCONFIG.Enabled = False
                cmdStation.Enabled = False
                cmdProgram.Enabled = False
                cmdOK.Enabled = False

                progset = 2
            End If
            Exit Sub
        End If
        If progset = 2 Then
            RebootCheck.Show vbModal
            If RebootCheck.RebootConf = True Then
                If MDIMain.MenteCOMM.REBOOTRequest() = True Then
                    cmdCONFIG.Enabled = False
                    cmdStation.Enabled = False
                    cmdProgram.Enabled = False
                    cmdOK.Enabled = False
    
                    progset = 0
                    reboot = False
                End If
            Else
                progset = 0
                reboot = True
            End If
            Exit Sub
        End If
            
            
        If configSet = 1 Then
            If MDIMain.MenteCOMM.DownConfRequest1(App.Path & "\ANNOUNCE.DAT") = True Then
                cmdCONFIG.Enabled = False
                cmdStation.Enabled = False
                cmdProgram.Enabled = False
                cmdOK.Enabled = False

                If Now < befDateConfig Then
                    configSet = 2
                Else
                    configSet = 5
                End If
            End If
            Exit Sub
        End If
        If configSet = 2 Then
            If MDIMain.MenteCOMM.DownConfRequest1(App.Path & "\CONFIG.BEF") = True Then
                cmdCONFIG.Enabled = False
                cmdStation.Enabled = False
                cmdProgram.Enabled = False
                cmdOK.Enabled = False

                configSet = 3
            End If
            Exit Sub
        End If
        If configSet = 3 Then
            If MDIMain.MenteCOMM.DownConfRequest1(App.Path & "\ANNOUNCE.BEF") = True Then
                cmdCONFIG.Enabled = False
                cmdStation.Enabled = False
                cmdProgram.Enabled = False
                cmdOK.Enabled = False

                configSet = 4
            End If
            Exit Sub
        End If
        If configSet = 4 Then
            If MDIMain.MenteCOMM.DownConfRequest1(App.Path & "\SWITCH.DAT") = True Then
                cmdCONFIG.Enabled = False
                cmdStation.Enabled = False
                cmdProgram.Enabled = False
                cmdOK.Enabled = False

                configSet = 5
            End If
            Exit Sub
        End If
        If configSet = 5 Then
            RebootCheck.Show vbModal
            If RebootCheck.RebootConf = True Then
                If MDIMain.MenteCOMM.RECONFIGRequest() = True Then
                    cmdCONFIG.Enabled = False
                    cmdStation.Enabled = False
                    cmdProgram.Enabled = False
                    cmdOK.Enabled = False
    
                    configSet = 0
                    reconf = False
                End If
            Else
                configSet = 0
                reconf = True
            End If
            Exit Sub
        End If
            
        If downBusy = True Then
            cmdCONFIG.Enabled = False
            cmdStation.Enabled = False
            cmdProgram.Enabled = False
            cmdOK.Enabled = False
            Exit Sub
        ElseIf reconf2 = True Then
            reconf2 = False
            RebootCheck.Show vbModal
            If RebootCheck.RebootConf = True Then
                If MDIMain.MenteCOMM.RECONFIGRequest() = True Then
                    cmdCONFIG.Enabled = False
                    cmdStation.Enabled = False
                    cmdProgram.Enabled = False
                    cmdOK.Enabled = False
    
                    configSet = 0
                    reconf = False
                End If
            Else
                configSet = 0
                reconf = True
            End If
            Exit Sub
        End If
    
        cmdCONFIG.Enabled = True
        cmdStation.Enabled = True
        If frmLogin.LoginSucceeded = True Then
            Me.cmdProgram.Enabled = True
        Else
            Me.cmdProgram.Enabled = False
        End If
        cmdOK.Enabled = True
    Else
        cmdCONFIG.Enabled = False
        cmdStation.Enabled = False
        cmdProgram.Enabled = False
        cmdOK.Enabled = False
    End If

End Sub

Private Sub cmdCONFIG_Click()

    If MDIMain.MenteCOMM.DownConfRequest1(App.Path & "\CONFIG.DAT") = True Then
        cmdCONFIG.Enabled = False
        cmdStation.Enabled = False
        cmdProgram.Enabled = False

        configSet = 1
    End If

End Sub

Private Sub cmdOK_Click()

    If reconf = True Or reconf2 = True Then
        RebootCheck.Show vbModal
        If RebootCheck.RebootConf = True Then
            If MDIMain.MenteCOMM.RECONFIGRequest() = True Then
                cmdCONFIG.Enabled = False
                cmdStation.Enabled = False
                cmdProgram.Enabled = False
                cmdOK.Enabled = False

                configSet = 0
                reconf = False
            End If
        End If
        Exit Sub
    End If

    If reboot = True Then
        RebootCheck.Show vbModal
        If RebootCheck.RebootConf = True Then
            If MDIMain.MenteCOMM.REBOOTRequest() = True Then
                cmdCONFIG.Enabled = False
                cmdStation.Enabled = False
                cmdProgram.Enabled = False
                cmdOK.Enabled = False

                progset = 0
                reboot = False
            End If
        End If
        Exit Sub
    End If
    
    Unload Me

End Sub

Private Sub cmdStation_Click()

    frmRouteDownload.Show
    
End Sub

Private Sub cmdProgram_Click()

    If Dir(App.Path & "\DVAU1.MOT") = "" Then
        MsgBox "Illegal Folder"
        Exit Sub
    End If
    If Dir(App.Path & "\DVAU2.MOT") = "" Then
        MsgBox "Illegal Folder"
        Exit Sub
    End If
    If MDIMain.MenteCOMM.DownProgRequest(App.Path & "\DVAU1.MOT") = True Then
        cmdCONFIG.Enabled = False
        cmdStation.Enabled = False
        cmdProgram.Enabled = False
        progset = 1
    End If

End Sub

Private Sub Form_Unload(Cancel As Integer)

    MDIMain.MenteCOMM.OffLine
    progset = 0
    configSet = 0
End Sub

