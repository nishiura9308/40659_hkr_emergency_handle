VERSION 5.00
Begin VB.Form frmDoorSelect 
   BorderStyle     =   1  'å≈íË(é¿ê¸)
   Caption         =   "Door Announce"
   ClientHeight    =   1860
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   4680
   LinkTopic       =   "Form2"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   1860
   ScaleWidth      =   4680
   StartUpPosition =   1  'µ∞≈∞ Ã´∞—ÇÃíÜâõ
   Begin VB.CommandButton cmdOK 
      Caption         =   "O K"
      BeginProperty Font 
         Name            =   "Courier New"
         Size            =   9
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   400
      Left            =   1020
      TabIndex        =   2
      Top             =   1320
      Width           =   1215
   End
   Begin VB.CommandButton cmdCancel 
      Cancel          =   -1  'True
      Caption         =   "Cancel"
      Default         =   -1  'True
      BeginProperty Font 
         Name            =   "Courier New"
         Size            =   9
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   400
      Left            =   2400
      TabIndex        =   3
      Top             =   1320
      Width           =   1215
   End
   Begin VB.CommandButton cmdAnnounce 
      Caption         =   "Announce"
      BeginProperty Font 
         Name            =   "Courier New"
         Size            =   9
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   555
      Left            =   720
      TabIndex        =   1
      Top             =   600
      Width           =   3225
   End
   Begin VB.TextBox txtTitle 
      Alignment       =   2  'íÜâõëµÇ¶
      BeginProperty Font 
         Name            =   "Courier New"
         Size            =   9
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   315
      Left            =   120
      Locked          =   -1  'True
      TabIndex        =   0
      Text            =   "Door A Open"
      Top             =   60
      Width           =   4365
   End
End
Attribute VB_Name = "frmDoorSelect"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Dim bUpdate As Boolean

Dim selno As Integer

Dim bdata(0 To ONE_DOOR_SIZE - 1) As Byte
Dim sdata(0 To SEQUENCE_SIZE - 1) As Byte

Public Sub DoModal(ByVal no As Integer)
    Dim fno As Integer

    selno = no
    
    fno = FreeFile
    If befDateMode = False Then
        Open App.Path & "\ANNOUNCE.DAT" For Binary As fno
    Else
        Open App.Path & "\ANNOUNCE.BEF" For Binary As fno
    End If
    Seek #fno, TOP_DOOR + selno * ONE_DOOR_SIZE
    Get fno, , bdata
    Close fno
    
    Select Case selno
    Case 0
        Me.txtTitle.Text = "DoorA Open Announce"
    Case 1
        Me.txtTitle.Text = "DoorA Close Announce"
    Case 2
        Me.txtTitle.Text = "DoorB Open Announce"
    Case 3
        Me.txtTitle.Text = "DoorB Close Announce"
    End Select

    Me.Show vbModal

    If bUpdate = True Then
        fno = FreeFile
        If befDateMode = False Then
            Open App.Path & "\ANNOUNCE.DAT" For Binary As fno
        Else
            Open App.Path & "\ANNOUNCE.BEF" For Binary As fno
        End If
        Seek #fno, TOP_DOOR + selno * ONE_DOOR_SIZE
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

Private Sub cmdAnnounce_Click()
    Dim i As Integer

    For i = 0 To SEQUENCE_SIZE - 1
        sdata(i) = bdata(i)
    Next i
    
    If frmSequence.DoModal(sdata, Me.txtTitle & " Code Define", SEQUENCE_MAX) = False Then Exit Sub

    For i = 0 To SEQUENCE_SIZE - 1
        bdata(i) = sdata(i)
    Next i
    
End Sub

