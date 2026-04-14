VERSION 5.00
Begin VB.Form frmEMG 
   BorderStyle     =   1  'å≈íË(é¿ê¸)
   Caption         =   "Emergency Announce"
   ClientHeight    =   2205
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   4425
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
   MinButton       =   0   'False
   ScaleHeight     =   2205
   ScaleWidth      =   4425
   StartUpPosition =   1  'µ∞≈∞ Ã´∞—ÇÃíÜâõ
   Begin VB.Frame Frame2 
      Caption         =   "MCP Display Message"
      Height          =   1275
      Left            =   2070
      TabIndex        =   5
      Top             =   150
      Width           =   2295
      Begin VB.TextBox txtMCPDisplay 
         Height          =   345
         Index           =   1
         Left            =   300
         MaxLength       =   6
         TabIndex        =   7
         Text            =   "Text1"
         Top             =   690
         Width           =   1665
      End
      Begin VB.TextBox txtMCPDisplay 
         Height          =   345
         Index           =   0
         Left            =   300
         MaxLength       =   6
         TabIndex        =   6
         Text            =   "Text1"
         Top             =   330
         Width           =   1665
      End
   End
   Begin VB.CommandButton cmdCancel 
      Cancel          =   -1  'True
      Caption         =   "Cancel"
      Default         =   -1  'True
      Height          =   400
      Left            =   2310
      TabIndex        =   4
      Top             =   1650
      Width           =   1215
   End
   Begin VB.CommandButton cmdOK 
      Caption         =   "O K"
      Height          =   400
      Left            =   960
      TabIndex        =   3
      Top             =   1650
      Width           =   1215
   End
   Begin VB.Frame Frame1 
      Caption         =   "EIDS Display No."
      Height          =   735
      Left            =   60
      TabIndex        =   0
      Top             =   150
      Width           =   1995
      Begin VB.ComboBox selectEIDS 
         Height          =   345
         Left            =   360
         TabIndex        =   1
         Text            =   "00"
         Top             =   330
         Width           =   615
      End
   End
   Begin VB.CommandButton cmdAnnounce 
      Caption         =   "Announce Sequence"
      Height          =   495
      Left            =   60
      TabIndex        =   2
      Top             =   930
      Width           =   2025
   End
End
Attribute VB_Name = "frmEMG"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Dim bUpdate As Boolean

Dim selno As Integer

Dim bdata(0 To ONE_EMGMA_SIZE - 1) As Byte
Dim sdata(0 To SEQUENCE_SIZE - 1) As Byte

Sub DoModal(ByVal no As Integer)
    Dim fno As Integer

    On Error Resume Next

    selno = no

    fno = FreeFile
    If befDateMode = False Then
        Open App.Path & "\ANNOUNCE.DAT" For Binary As fno
    Else
        Open App.Path & "\ANNOUNCE.BEF" For Binary As fno
    End If
    Seek #fno, TOP_EMGMA + selno * ONE_EMGMA_SIZE
    Get fno, , bdata
    Close fno

    If bdata(0) = 0 Then
        bdata(0) = &H21
    End If
    Me.selectEIDS.Text = Right$("0" + Hex$(bdata(0)), 2)
    Me.txtMCPDisplay(0).Text = Chr$(bdata(1)) & Chr$(bdata(2)) & Chr$(bdata(3)) & Chr$(bdata(4)) & Chr$(bdata(5)) & Chr$(bdata(6))
    Me.txtMCPDisplay(1).Text = Chr$(bdata(7)) & Chr$(bdata(8)) & Chr$(bdata(9)) & Chr$(bdata(10)) & Chr$(bdata(11)) & Chr$(bdata(12))

    Me.Show vbModal

    If bUpdate = True Then
        bdata(0) = CInt("&H" & selectEIDS.Text)
        bdata(1) = Asc(Mid$(Me.txtMCPDisplay(0) & "      ", 1, 1))
        bdata(2) = Asc(Mid$(Me.txtMCPDisplay(0) & "      ", 2, 1))
        bdata(3) = Asc(Mid$(Me.txtMCPDisplay(0) & "      ", 3, 1))
        bdata(4) = Asc(Mid$(Me.txtMCPDisplay(0) & "      ", 4, 1))
        bdata(5) = Asc(Mid$(Me.txtMCPDisplay(0) & "      ", 5, 1))
        bdata(6) = Asc(Mid$(Me.txtMCPDisplay(0) & "      ", 6, 1))
        bdata(7) = Asc(Mid$(Me.txtMCPDisplay(1) & "      ", 1, 1))
        bdata(8) = Asc(Mid$(Me.txtMCPDisplay(1) & "      ", 2, 1))
        bdata(9) = Asc(Mid$(Me.txtMCPDisplay(1) & "      ", 3, 1))
        bdata(10) = Asc(Mid$(Me.txtMCPDisplay(1) & "      ", 4, 1))
        bdata(11) = Asc(Mid$(Me.txtMCPDisplay(1) & "      ", 5, 1))
        bdata(12) = Asc(Mid$(Me.txtMCPDisplay(1) & "      ", 6, 1))
        fno = FreeFile
        If befDateMode = False Then
            Open App.Path & "\ANNOUNCE.DAT" For Binary As fno
        Else
            Open App.Path & "\ANNOUNCE.BEF" For Binary As fno
        End If
        Seek #fno, TOP_EMGMA + selno * ONE_EMGMA_SIZE
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
        sdata(i) = bdata(i + 13)
    Next i
    
    If frmSequence.DoModal(sdata, "Emergency " & Trim$(selno + 1) & " Announce Code Define", SEQUENCE_MAX) = False Then Exit Sub

    For i = 0 To SEQUENCE_SIZE - 1
        bdata(i + 13) = sdata(i)
    Next i
    
End Sub

Private Sub Form_Load()
    Dim i As Integer

    selectEIDS.Clear
    For i = 1 To 95
        selectEIDS.AddItem Right$("0" + Hex$(i + &H20), 2)
    Next i
    selectEIDS.Text = "21"

End Sub

Private Sub selectEIDS_LostFocus()
    On Error Resume Next

    If CInt("&H" & selectEIDS.Text) < &H21 Then
        selectEIDS.Text = "21"
    End If
    If CInt("&H" & selectEIDS.Text) > &H7F Then
        selectEIDS.Text = "7F"
    End If
    
End Sub
