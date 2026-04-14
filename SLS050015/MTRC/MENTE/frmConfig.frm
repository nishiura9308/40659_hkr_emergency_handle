VERSION 5.00
Begin VB.Form frmConfig 
   BorderStyle     =   1  'å≈íË(é¿ê¸)
   Caption         =   "Configuration Select"
   ClientHeight    =   4920
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   5655
   BeginProperty Font 
      Name            =   "Courier New"
      Size            =   9
      Charset         =   0
      Weight          =   400
      Underline       =   0   'False
      Italic          =   0   'False
      Strikethrough   =   0   'False
   EndProperty
   LinkTopic       =   "Form2"
   MaxButton       =   0   'False
   MDIChild        =   -1  'True
   MinButton       =   0   'False
   ScaleHeight     =   4920
   ScaleWidth      =   5655
   Begin VB.OptionButton befData 
      Caption         =   "Before"
      Height          =   375
      Index           =   1
      Left            =   1200
      Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
      TabIndex        =   27
      Top             =   4440
      Width           =   975
   End
   Begin VB.OptionButton befData 
      Caption         =   "After"
      Height          =   375
      Index           =   0
      Left            =   120
      Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
      TabIndex        =   26
      Top             =   4440
      Width           =   975
   End
   Begin VB.Frame framePassword 
      Caption         =   "Setting Password"
      Height          =   705
      Left            =   0
      TabIndex        =   24
      Top             =   3510
      Width           =   2445
      Begin VB.TextBox txtPassword 
         Height          =   345
         IMEMode         =   3  'µÃå≈íË
         Left            =   30
         PasswordChar    =   "*"
         TabIndex        =   25
         Top             =   270
         Width           =   2325
      End
   End
   Begin VB.CommandButton cmdImExport 
      Caption         =   "Import/Export"
      Height          =   465
      Left            =   3840
      TabIndex        =   23
      Top             =   4350
      Width           =   1695
   End
   Begin VB.Frame Frame1 
      Caption         =   "Door Announce/Chime"
      Height          =   1995
      Left            =   0
      TabIndex        =   19
      Top             =   30
      Width           =   2475
      Begin VB.CommandButton cmdDoorChime 
         Caption         =   "Door Chime"
         Height          =   375
         Left            =   210
         TabIndex        =   22
         Top             =   300
         Width           =   2000
      End
      Begin VB.CommandButton cmdDoor 
         Caption         =   "Door A Close"
         Height          =   375
         Index           =   1
         Left            =   210
         TabIndex        =   21
         Top             =   870
         Width           =   2000
      End
      Begin VB.CommandButton cmdDoor 
         Caption         =   "Door B Close"
         Height          =   375
         Index           =   3
         Left            =   210
         TabIndex        =   20
         Top             =   1350
         Width           =   2000
      End
   End
   Begin VB.Frame Frame6 
      Caption         =   "Language/Version"
      Height          =   1425
      Left            =   0
      TabIndex        =   16
      Top             =   2070
      Width           =   2445
      Begin VB.CommandButton cmdVersion 
         Caption         =   "Version"
         Height          =   375
         Left            =   360
         TabIndex        =   18
         Top             =   840
         Width           =   1605
      End
      Begin VB.CommandButton cmdLanguage 
         Caption         =   "Language"
         Height          =   375
         Left            =   360
         TabIndex        =   17
         Top             =   330
         Width           =   1605
      End
   End
   Begin VB.Frame Frame5 
      Caption         =   "Initial Voice Level"
      Height          =   705
      Left            =   2460
      TabIndex        =   15
      Top             =   3510
      Width           =   3045
      Begin VB.CommandButton cmdVoice 
         Caption         =   "Select"
         Height          =   375
         Left            =   1410
         TabIndex        =   7
         Top             =   270
         Width           =   1425
      End
   End
   Begin VB.Frame Frame4 
      Caption         =   "Special Announce"
      Height          =   915
      Left            =   2490
      TabIndex        =   12
      Top             =   1110
      Width           =   3015
      Begin VB.ComboBox selectSPCNo 
         Height          =   345
         Left            =   630
         TabIndex        =   2
         Text            =   "00"
         Top             =   330
         Width           =   615
      End
      Begin VB.CommandButton cmdSPCAnnounce 
         Caption         =   "Select"
         Height          =   495
         Left            =   1410
         TabIndex        =   3
         Top             =   270
         Width           =   1425
      End
      Begin VB.Label Label2 
         Caption         =   "No."
         Height          =   345
         Left            =   300
         TabIndex        =   13
         Top             =   390
         Width           =   435
      End
   End
   Begin VB.CommandButton cmdDownload 
      Caption         =   "O K"
      Default         =   -1  'True
      Height          =   525
      Left            =   2340
      TabIndex        =   8
      Top             =   4320
      Width           =   1425
   End
   Begin VB.Frame Frame3 
      Caption         =   "Route Station Announce"
      Height          =   1425
      Left            =   2460
      TabIndex        =   10
      Top             =   2070
      Width           =   3045
      Begin VB.TextBox txtRouteName 
         Height          =   315
         Left            =   180
         TabIndex        =   6
         Top             =   720
         Width           =   1515
      End
      Begin VB.ComboBox selectRouteNo 
         Height          =   345
         Left            =   600
         TabIndex        =   4
         Text            =   "00"
         Top             =   300
         Width           =   615
      End
      Begin VB.CommandButton cmdRoute 
         Caption         =   "Select"
         Height          =   375
         Left            =   1410
         TabIndex        =   5
         Top             =   270
         Width           =   1455
      End
      Begin VB.Label Label3 
         Caption         =   "No."
         Height          =   345
         Left            =   270
         TabIndex        =   14
         Top             =   360
         Width           =   435
      End
   End
   Begin VB.Frame Frame2 
      Caption         =   "Emergency Announce"
      Height          =   915
      Left            =   2490
      TabIndex        =   9
      Top             =   120
      Width           =   2985
      Begin VB.CommandButton cmdEMGAnnounce 
         Caption         =   "Select"
         Height          =   495
         Left            =   1410
         TabIndex        =   1
         Top             =   270
         Width           =   1425
      End
      Begin VB.ComboBox selectEMGNo 
         Height          =   345
         Left            =   630
         TabIndex        =   0
         Text            =   "00"
         Top             =   330
         Width           =   615
      End
      Begin VB.Label Label1 
         Caption         =   "No."
         Height          =   345
         Left            =   300
         TabIndex        =   11
         Top             =   390
         Width           =   435
      End
   End
End
Attribute VB_Name = "frmConfig"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Dim datedata(0 To 7) As Byte

Private Sub befData_Click(Index As Integer)

    On Error Resume Next
    
    If Index = 0 Then
        befData(0).Value = True
        befData(1).Value = False
        befDateMode = False
    Else
        befData(0).Value = False
        befData(1).Value = True
        befDateMode = True
    End If

End Sub

Private Sub cmdDoor_Click(Index As Integer)

    frmDoorSelect.DoModal Index
    
End Sub

Private Sub cmdDoorChime_Click()

    frmDoorChime.DoModal
    
End Sub

Private Sub cmdDownload_Click()

    Unload Me

End Sub

Private Sub cmdEMGAnnounce_Click()

    If CInt(selectEMGNo.Text) < 1 Or CInt(selectEMGNo.Text) > 32 Then Exit Sub

    frmEMG.DoModal (CInt(selectEMGNo.Text) - 1)

End Sub

Private Sub cmdImExport_Click()

    frmImExport.DoModal

End Sub

Private Sub cmdLanguage_Click()

    frmLanguage.DoModal

End Sub

Private Sub cmdSPCAnnounce_Click()

    If CInt(selectSPCNo.Text) < 1 Or CInt(selectSPCNo.Text) > 64 Then Exit Sub

    frmSPC.DoModal (CInt(selectSPCNo.Text) - 1)

End Sub

Private Sub cmdRoute_Click()
    
    frmRouteSelect.DoModal CInt("&H" + selectRouteNo.Text)

End Sub

Private Sub cmdVersion_Click()
    
    frmVersion.DoModal

End Sub

Private Sub cmdVoice_Click()

    frmVoiceLevel.DoModal

End Sub

Private Sub Form_Load()
    Dim i As Integer
    Dim fd As Integer

    On Error Resume Next

    selectEMGNo.Clear
    For i = 1 To 32
        selectEMGNo.AddItem Right$("0" + Trim$(i), 2)
    Next i
    selectEMGNo.Text = "01"
    
    selectSPCNo.Clear
    For i = 1 To 64
        selectSPCNo.AddItem Right$("0" + Trim$(i), 2)
    Next i
    selectSPCNo.Text = "01"
    
    selectRouteNo.Clear
    For i = 1 To 255
        selectRouteNo.AddItem Right$("0" + Hex$(i), 2)
    Next i
    selectRouteNo.Text = "01"
    txtRouteName.Text = GetRouteName(CInt("&H" + selectRouteNo.Text))
        
    frmLogin.Show vbModal
    If frmLogin.LoginSucceeded = True Then
        cmdDoorChime.Enabled = True
        cmdDoor(1).Enabled = True
        cmdDoor(3).Enabled = True
        cmdLanguage.Enabled = True
        cmdVersion.Enabled = True
        framePassword.Visible = True
        txtPassword.Text = frmLogin.sPassword
    Else
        cmdDoorChime.Enabled = False
        cmdDoor(1).Enabled = False
        cmdDoor(3).Enabled = False
        cmdLanguage.Enabled = False
        cmdVersion.Enabled = False
        framePassword.Visible = False
    End If
    
    If Now >= befDateConfig Then
        befData(0).Visible = False
        befData(1).Visible = False
        befDateMode = False
    Else
        befData(0).Visible = True
        befData(1).Visible = True
        befData(0).Enabled = True
        befData(1).Enabled = True
        befData(0).Value = False
        befData(1).Value = True
        befDateMode = True

        datedata(0) = CByte(Format$(befDateConfig, "yy"))
        datedata(1) = CByte(Format$(befDateConfig, "mm"))
        datedata(2) = CByte(Format$(befDateConfig, "dd"))
        datedata(3) = CByte(Format$(befDateConfig, "hh"))
        datedata(4) = CByte(Format$(befDateConfig, "nn"))
        datedata(5) = 0
        datedata(6) = 0
        datedata(7) = 0
        
        fd = FreeFile
        Open App.Path & "\SWITCH.DAT" For Binary As fd
        Put fd, , datedata
        Close fd
    End If

End Sub

Private Sub selectRouteNo_Click()

    txtRouteName.Text = GetRouteName(CInt("&H" + selectRouteNo.Text))

End Sub

Private Sub selectRouteNo_LostFocus()

    On Error Resume Next

    If CInt("&H" + selectRouteNo.Text) < &H1 Then
        selectRouteNo.Text = "01"
    End If
    If CInt("&H" + selectRouteNo.Text) > &HFF Then
        selectRouteNo.Text = "FF"
    End If
    txtRouteName.Text = GetRouteName(CInt("&H" + selectRouteNo.Text))
    
End Sub

Private Sub selectEMGNo_LostFocus()

    On Error Resume Next

    If CInt(selectEMGNo.Text) < 0 Then
        selectEMGNo.Text = "1"
    End If
    If CInt(selectEMGNo.Text) > 32 Then
        selectEMGNo.Text = "32"
    End If
    
End Sub

Private Sub selectSPCNo_LostFocus()

    On Error Resume Next

    If CInt(selectSPCNo.Text) < 0 Then
        selectSPCNo.Text = "1"
    End If
    If CInt(selectSPCNo.Text) > 64 Then
        selectSPCNo.Text = "64"
    End If
    
End Sub


Private Sub txtPassword_LostFocus()

    If frmLogin.sPassword <> txtPassword Then
        If MsgBox("Change Password OK?", vbOKCancel Or vbExclamation) = vbOK Then
            frmLogin.sPassword = txtPassword
            SaveSetting App.Title, "SETTING", "PASSWORD", txtPassword
        Else
            txtPassword = frmLogin.sPassword
        End If
    End If

End Sub

Private Sub txtRouteName_LostFocus()
    If GetRouteName(CInt("&H" + selectRouteNo.Text)) <> txtRouteName.Text Then
        If MsgBox("Change Routename OK?", vbOKCancel) = vbOK Then
            Call SetRouteName(CInt("&H" + selectRouteNo.Text), txtRouteName.Text)
        End If
    End If

End Sub
