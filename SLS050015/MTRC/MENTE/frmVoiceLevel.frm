VERSION 5.00
Begin VB.Form frmVoiceLevel 
   BorderStyle     =   1  'å≈íË(é¿ê¸)
   Caption         =   "Initial Voice Level"
   ClientHeight    =   3585
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   2985
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
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   3585
   ScaleWidth      =   2985
   StartUpPosition =   1  'µ∞≈∞ Ã´∞—ÇÃíÜâõ
   Begin VB.CommandButton cmdOK 
      Caption         =   "O K"
      Height          =   400
      Left            =   120
      TabIndex        =   9
      Top             =   2970
      Width           =   1215
   End
   Begin VB.CommandButton cmdCancel 
      Cancel          =   -1  'True
      Caption         =   "Cancel"
      Default         =   -1  'True
      Height          =   400
      Left            =   1500
      TabIndex        =   10
      Top             =   2970
      Width           =   1215
   End
   Begin VB.ComboBox selectVoiceLevel 
      Height          =   345
      Index           =   7
      Left            =   1290
      TabIndex        =   8
      Text            =   "-6dB"
      Top             =   2400
      Width           =   855
   End
   Begin VB.ComboBox selectVoiceLevel 
      Height          =   345
      Index           =   6
      Left            =   1290
      TabIndex        =   7
      Text            =   "-6dB"
      Top             =   2100
      Width           =   855
   End
   Begin VB.ComboBox selectVoiceLevel 
      Height          =   345
      Index           =   5
      Left            =   1290
      TabIndex        =   6
      Text            =   "-6dB"
      Top             =   1800
      Width           =   855
   End
   Begin VB.ComboBox selectVoiceLevel 
      Height          =   345
      Index           =   4
      Left            =   1290
      TabIndex        =   5
      Text            =   "-6dB"
      Top             =   1500
      Width           =   855
   End
   Begin VB.ComboBox selectVoiceLevel 
      Height          =   345
      Index           =   3
      Left            =   1290
      TabIndex        =   4
      Text            =   "-6dB"
      Top             =   1200
      Width           =   855
   End
   Begin VB.ComboBox selectVoiceLevel 
      Height          =   345
      Index           =   2
      Left            =   1290
      TabIndex        =   3
      Text            =   "-6dB"
      Top             =   900
      Width           =   855
   End
   Begin VB.ComboBox selectVoiceLevel 
      Height          =   345
      Index           =   1
      Left            =   1290
      TabIndex        =   2
      Text            =   "-6dB"
      Top             =   600
      Width           =   855
   End
   Begin VB.ComboBox selectVoiceLevel 
      Height          =   345
      Index           =   0
      Left            =   1290
      TabIndex        =   1
      Text            =   "-6dB"
      Top             =   300
      Width           =   855
   End
   Begin VB.Label Label1 
      Caption         =   "PAA8"
      Height          =   255
      Index           =   7
      Left            =   795
      TabIndex        =   17
      Top             =   2430
      Width           =   525
   End
   Begin VB.Label Label1 
      Caption         =   "PAA7"
      Height          =   255
      Index           =   6
      Left            =   795
      TabIndex        =   16
      Top             =   2130
      Width           =   525
   End
   Begin VB.Label Label1 
      Caption         =   "PAA6"
      Height          =   255
      Index           =   5
      Left            =   795
      TabIndex        =   15
      Top             =   1830
      Width           =   525
   End
   Begin VB.Label Label1 
      Caption         =   "PAA5"
      Height          =   255
      Index           =   4
      Left            =   795
      TabIndex        =   14
      Top             =   1530
      Width           =   525
   End
   Begin VB.Label Label1 
      Caption         =   "PAA4"
      Height          =   255
      Index           =   3
      Left            =   795
      TabIndex        =   13
      Top             =   1230
      Width           =   525
   End
   Begin VB.Label Label1 
      Caption         =   "PAA3"
      Height          =   255
      Index           =   2
      Left            =   795
      TabIndex        =   12
      Top             =   930
      Width           =   525
   End
   Begin VB.Label Label1 
      Caption         =   "PAA2"
      Height          =   255
      Index           =   1
      Left            =   795
      TabIndex        =   11
      Top             =   630
      Width           =   525
   End
   Begin VB.Label Label1 
      Caption         =   "PAA1"
      Height          =   255
      Index           =   0
      Left            =   795
      TabIndex        =   0
      Top             =   330
      Width           =   525
   End
End
Attribute VB_Name = "frmVoiceLevel"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Dim bUpdate As Boolean

Dim bdata(0 To VOICE_SIZE - 1) As Byte

Public Sub DoModal()
    Dim fno As Integer
    Dim i As Integer
    Dim j As Integer

    fno = FreeFile
    If befDateMode = False Then
        Open App.Path & "\CONFIG.DAT" For Binary As fno
    Else
        Open App.Path & "\CONFIG.BEF" For Binary As fno
    End If
    Seek #fno, TOP_VOICE
    Get fno, , bdata
    Close fno

    For i = 0 To 7
        Me.selectVoiceLevel(i).Text = Me.selectVoiceLevel(i).List(bdata(i))
    Next i
    Me.Show vbModal

    If bUpdate = True Then
        For i = 0 To 7
            For j = 0 To Me.selectVoiceLevel(i).ListCount - 1
                If Me.selectVoiceLevel(i).Text = Me.selectVoiceLevel(i).List(j) Then
                    bdata(i) = j
                End If
            Next j
        Next i
                
        fno = FreeFile
        If befDateMode = False Then
            Open App.Path & "\CONFIG.DAT" For Binary As fno
        Else
            Open App.Path & "\CONFIG.BEF" For Binary As fno
        End If
        Seek #fno, TOP_VOICE
        Put fno, , bdata
        Close fno
    End If

    Unload Me

End Sub

Private Sub cmdCancel_Click()

    bUpdate = False
    Me.Hide
    
End Sub

Private Sub cmdOK_Click()
    
    bUpdate = True
    Me.Hide

End Sub


Private Sub Form_Load()
    Dim i As Integer
    Dim j As Integer

    For i = 0 To 7
        selectVoiceLevel(i).Clear
        For j = 0 To 78
            If j = 0 Then
                selectVoiceLevel(i).AddItem "0dB"
            Else
                selectVoiceLevel(i).AddItem "-" & Trim$(j) & "dB"
            End If
        Next j
        selectVoiceLevel(i).Text = "0dB"
    Next i

End Sub
