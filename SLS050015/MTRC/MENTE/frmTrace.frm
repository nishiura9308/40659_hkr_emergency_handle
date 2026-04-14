VERSION 5.00
Begin VB.Form frmTrace 
   BorderStyle     =   1  'ŒÅ’è(ŽÀü)
   Caption         =   "Trace Info."
   ClientHeight    =   6525
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   6990
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
   ScaleHeight     =   6525
   ScaleWidth      =   6990
   Begin VB.Timer Timer1 
      Interval        =   1000
      Left            =   270
      Top             =   6030
   End
   Begin VB.CommandButton cmdOK 
      Caption         =   "Cancel"
      Height          =   375
      Left            =   2790
      TabIndex        =   1
      Top             =   6000
      Width           =   1215
   End
   Begin VB.TextBox txtTraceData 
      Height          =   5745
      Left            =   0
      MultiLine       =   -1  'True
      ScrollBars      =   2  '‚’¼
      TabIndex        =   0
      Top             =   0
      Width           =   6885
   End
End
Attribute VB_Name = "frmTrace"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Public Sub DoModeless()

    If MDIMain.MenteCOMM.OnLine = False Then
        MsgBox "Not Online"
        Exit Sub
    End If

    If MDIMain.MenteCOMM.TRACERequest = True Then
        Me.Show vbModeless
    End If

End Sub

Private Sub cmdOK_Click()
    
    If cmdOK.Caption = "CANCEL" Then
        MDIMain.MenteCOMM.AbortFlag = True

        cmdOK.Caption = "O K"
        Exit Sub
    End If

    Unload Me

End Sub

Private Sub Form_Load()

    cmdOK.Caption = "CANCEL"

End Sub

Private Sub Form_Unload(Cancel As Integer)
    On Error Resume Next

    MDIMain.MenteCOMM.OffLine

End Sub

Private Sub Timer1_Timer()

    On Error Resume Next

    If Len(Me.txtTraceData.Text) > 3000 Then
        Me.txtTraceData.Text = Mid$(Me.txtTraceData.Text, 2000)
        frmTrace.txtTraceData.SelStart = Len(frmTrace.txtTraceData.Text)
    End If

    
End Sub
