VERSION 5.00
Begin VB.Form frmDEBUG 
   Caption         =   "DEBUG"
   ClientHeight    =   3195
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   4680
   BeginProperty Font 
      Name            =   "Courier New"
      Size            =   9
      Charset         =   0
      Weight          =   400
      Underline       =   0   'False
      Italic          =   0   'False
      Strikethrough   =   0   'False
   EndProperty
   LinkTopic       =   "Form1"
   ScaleHeight     =   3195
   ScaleWidth      =   4680
   StartUpPosition =   3  'Windows ‚ÌŠù’è’l
   Begin VB.Timer Timer1 
      Interval        =   1000
      Left            =   60
      Top             =   0
   End
   Begin VB.TextBox txtDEBUG 
      Height          =   3135
      Left            =   30
      MultiLine       =   -1  'True
      ScrollBars      =   2  '‚’¼
      TabIndex        =   0
      Text            =   "frmDEBUG.frx":0000
      Top             =   30
      Width           =   4605
   End
End
Attribute VB_Name = "frmDEBUG"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Public Sub SendMessage(ByVal msg As String)
    txtDEBUG.SelText = Time$ & " S:" & msg & vbCrLf
    txtDEBUG.SelStart = Len(txtDEBUG.Text)
End Sub

Public Sub RecvMessage(ByVal msg As String)
    txtDEBUG.SelText = Time$ & " R:" & msg & vbCrLf
    txtDEBUG.SelStart = Len(txtDEBUG.Text)
End Sub

Public Sub ActionMessage(ByVal msg As String)
    txtDEBUG.SelText = Time$ & " A:" & msg & vbCrLf
    txtDEBUG.SelStart = Len(txtDEBUG.Text)
End Sub

Private Sub Form_Load()

    On Error Resume Next

    txtDEBUG.Text = ""

'    Kill App.Path & "\COMM.LOG"

End Sub

Private Sub Form_Resize()

    On Error Resume Next

    txtDEBUG.Left = 0
    txtDEBUG.Top = 0
    txtDEBUG.Width = Me.Width - 100
    txtDEBUG.Height = Me.Height - 400

End Sub

Private Sub Form_Unload(Cancel As Integer)
    Dim fno As Integer

    If GetSetting(App.Title, "SETTING", "COMMLog", "0") = "0" Then Exit Sub
    
    fno = FreeFile
    Open App.Path & "\COMM.LOG" For Append As #fno
    Print #fno, txtDEBUG.Text;
    Close #fno

End Sub

Private Sub Timer1_Timer()
    Dim fno As Integer
    
    If Len(txtDEBUG.Text) > 3000 Then
        If GetSetting(App.Title, "SETTING", "COMMLog", "0") <> "0" Then
            fno = FreeFile
            Open App.Path & "\COMM.LOG" For Append As #fno
            Print #fno, Mid$(txtDEBUG.Text, 1, 1000);
            Close #fno
        End If
        txtDEBUG.Text = Mid$(txtDEBUG.Text, 1001)
        txtDEBUG.SelStart = Len(txtDEBUG.Text)
    End If

End Sub
