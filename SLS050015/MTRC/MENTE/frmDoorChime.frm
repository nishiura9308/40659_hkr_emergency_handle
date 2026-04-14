VERSION 5.00
Begin VB.Form frmDoorChime 
   BorderStyle     =   1  'å≈íË(é¿ê¸)
   Caption         =   "Door Chime"
   ClientHeight    =   1605
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   4680
   LinkTopic       =   "Form2"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   1605
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
      TabIndex        =   1
      Top             =   1020
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
      TabIndex        =   2
      Top             =   1020
      Width           =   1215
   End
   Begin VB.CommandButton cmdChime 
      Caption         =   "Chime"
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
      Left            =   690
      TabIndex        =   0
      Top             =   270
      Width           =   3225
   End
End
Attribute VB_Name = "frmDoorChime"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Dim bUpdate As Boolean

Dim bdata(0 To CHIME_SIZE - 1) As Byte
Dim sdata(0 To SEQUENCE_SIZE - 1) As Byte

Public Sub DoModal()
    Dim fno As Integer

    fno = FreeFile
    If befDateMode = False Then
        Open App.Path & "\CONFIG.DAT" For Binary As fno
    Else
        Open App.Path & "\CONFIG.BEF" For Binary As fno
    End If
    Seek #fno, TOP_CHIME
    Get fno, , bdata
    Close fno
    
    Me.Show vbModal

    If bUpdate = True Then
        fno = FreeFile
        If befDateMode = False Then
            Open App.Path & "\CONFIG.DAT" For Binary As fno
        Else
            Open App.Path & "\CONFIG.BEF" For Binary As fno
        End If
        Seek #fno, TOP_CHIME
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

Private Sub cmdChime_Click()
    Dim i As Integer

    For i = 0 To SEQUENCE_SIZE - 1
        sdata(i) = 0
    Next i
    
    For i = 0 To CHIME_SIZE - 1
        sdata(i) = bdata(i)
    Next i
    
'    If frmSequence.DoModal(sdata, "Chime Code Define", 1) = False Then Exit Sub
    If frmChime.DoModal(sdata, "Chime Code Define") = False Then Exit Sub

    For i = 0 To CHIME_SIZE - 1
        bdata(i) = sdata(i)
    Next i
    
End Sub

