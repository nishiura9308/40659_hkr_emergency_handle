VERSION 5.00
Begin VB.Form frmLogin 
   BorderStyle     =   3  '固定ﾀﾞｲｱﾛｸﾞ
   Caption         =   "Administrator Check"
   ClientHeight    =   1170
   ClientLeft      =   2835
   ClientTop       =   3480
   ClientWidth     =   3750
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   691.275
   ScaleMode       =   0  'ﾕｰｻﾞｰ
   ScaleWidth      =   3521.047
   ShowInTaskbar   =   0   'False
   StartUpPosition =   1  'ｵｰﾅｰ ﾌｫｰﾑの中央
   Begin VB.CommandButton cmdOK 
      Caption         =   "OK"
      Default         =   -1  'True
      Height          =   390
      Left            =   510
      TabIndex        =   2
      Top             =   630
      Width           =   1140
   End
   Begin VB.CommandButton cmdCancel 
      Cancel          =   -1  'True
      Caption         =   "Cancel"
      Height          =   390
      Left            =   2100
      TabIndex        =   3
      Top             =   630
      Width           =   1140
   End
   Begin VB.TextBox txtPassword 
      Height          =   345
      IMEMode         =   3  'ｵﾌ固定
      Left            =   1290
      PasswordChar    =   "*"
      TabIndex        =   1
      Top             =   105
      Width           =   2325
   End
   Begin VB.Label lblLabels 
      Caption         =   "Password"
      Height          =   270
      Index           =   1
      Left            =   105
      TabIndex        =   0
      Top             =   120
      Width           =   1080
   End
End
Attribute VB_Name = "frmLogin"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Public LoginSucceeded As Boolean
Public sPassword As String

Private Sub cmdCancel_Click()
    'ﾛｸﾞｵﾝに失敗したことを示すために、
    'ｸﾞﾛｰﾊﾞﾙ変数を Flase に設定します。
    LoginSucceeded = False
    Me.Hide
End Sub

Private Sub cmdOK_Click()
    sPassword = GetSetting(App.Title, "SETTING", "PASSWORD", "password")
    
    '正しいﾊﾟｽﾜｰﾄﾞかどうかを調べます。
    If txtPassword = sPassword Then
        'ここにﾊﾟｽﾜｰﾄﾞが一致したことを呼び
        '出し元に通知するｺｰﾄﾞを記述します。
        'ここでは簡単にｸﾞﾛｰﾊﾞﾙ変数を True に設定します。
        LoginSucceeded = True
        txtPassword = ""
        Me.Hide
    Else
        MsgBox "Password Incorrect. Re-Enter!", , "Password"
        txtPassword.SetFocus
        SendKeys "{Home}+{End}"
    End If
End Sub

