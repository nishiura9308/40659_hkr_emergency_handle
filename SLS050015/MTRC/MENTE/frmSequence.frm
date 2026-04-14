VERSION 5.00
Begin VB.Form frmSequence 
   BorderStyle     =   1  'å≈íË(é¿ê¸)
   Caption         =   "Announce Code"
   ClientHeight    =   4440
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   9090
   ControlBox      =   0   'False
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
   ScaleHeight     =   4440
   ScaleWidth      =   9090
   StartUpPosition =   1  'µ∞≈∞ Ã´∞—ÇÃíÜâõ
   Begin VB.CommandButton Command1 
      Cancel          =   -1  'True
      Caption         =   "Cancel"
      Default         =   -1  'True
      Height          =   645
      Left            =   4800
      TabIndex        =   55
      Top             =   3630
      Width           =   1215
   End
   Begin VB.CommandButton cmdOK 
      Caption         =   "O K"
      Height          =   645
      Left            =   2580
      TabIndex        =   31
      Top             =   3630
      Width           =   1215
   End
   Begin VB.Frame Frame1 
      Caption         =   "Sequence6"
      Height          =   1455
      Index           =   5
      Left            =   6060
      TabIndex        =   51
      Top             =   1830
      Width           =   2925
      Begin VB.CheckBox chkLang 
         Caption         =   "MA"
         Height          =   285
         Index           =   17
         Left            =   1740
         Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
         TabIndex        =   30
         Top             =   1080
         Width           =   615
      End
      Begin VB.CheckBox chkLang 
         Caption         =   "CN"
         Height          =   285
         Index           =   16
         Left            =   990
         Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
         TabIndex        =   29
         Top             =   1080
         Width           =   615
      End
      Begin VB.CheckBox chkLang 
         Caption         =   "EN"
         Height          =   285
         Index           =   15
         Left            =   270
         Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
         TabIndex        =   28
         Top             =   1080
         Width           =   615
      End
      Begin VB.ComboBox AnnounceCode 
         Height          =   345
         Index           =   5
         Left            =   1020
         TabIndex        =   27
         Text            =   "000"
         Top             =   690
         Width           =   1095
      End
      Begin VB.ComboBox waitTime 
         Height          =   345
         Index           =   5
         ItemData        =   "frmSequence.frx":0000
         Left            =   1020
         List            =   "frmSequence.frx":0002
         TabIndex        =   26
         Text            =   "0"
         Top             =   210
         Width           =   1095
      End
      Begin VB.Label Label3 
         Caption         =   "Announce Code"
         Height          =   495
         Index           =   5
         Left            =   120
         TabIndex        =   54
         Top             =   630
         Width           =   885
      End
      Begin VB.Label Label2 
         Caption         =   "x100mS"
         Height          =   255
         Index           =   5
         Left            =   2190
         TabIndex        =   53
         Top             =   240
         Width           =   660
      End
      Begin VB.Label Label1 
         Caption         =   "Wait Time"
         Height          =   255
         Index           =   5
         Left            =   120
         TabIndex        =   52
         Top             =   240
         Width           =   765
      End
   End
   Begin VB.Frame Frame1 
      Caption         =   "Sequence5"
      Height          =   1485
      Index           =   4
      Left            =   3090
      TabIndex        =   47
      Top             =   1800
      Width           =   2925
      Begin VB.CheckBox chkLang 
         Caption         =   "MA"
         Height          =   285
         Index           =   14
         Left            =   1710
         Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
         TabIndex        =   25
         Top             =   1080
         Width           =   615
      End
      Begin VB.CheckBox chkLang 
         Caption         =   "CN"
         Height          =   285
         Index           =   13
         Left            =   960
         Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
         TabIndex        =   24
         Top             =   1080
         Width           =   615
      End
      Begin VB.CheckBox chkLang 
         Caption         =   "EN"
         Height          =   285
         Index           =   12
         Left            =   240
         Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
         TabIndex        =   23
         Top             =   1080
         Width           =   615
      End
      Begin VB.ComboBox AnnounceCode 
         Height          =   345
         Index           =   4
         Left            =   1020
         TabIndex        =   22
         Text            =   "000"
         Top             =   690
         Width           =   1095
      End
      Begin VB.ComboBox waitTime 
         Height          =   345
         Index           =   4
         ItemData        =   "frmSequence.frx":0004
         Left            =   1020
         List            =   "frmSequence.frx":0006
         TabIndex        =   21
         Text            =   "0"
         Top             =   210
         Width           =   1095
      End
      Begin VB.Label Label3 
         Caption         =   "Announce Code"
         Height          =   495
         Index           =   4
         Left            =   120
         TabIndex        =   50
         Top             =   630
         Width           =   915
      End
      Begin VB.Label Label2 
         Caption         =   "x100mS"
         Height          =   255
         Index           =   4
         Left            =   2190
         TabIndex        =   49
         Top             =   240
         Width           =   660
      End
      Begin VB.Label Label1 
         Caption         =   "Wait Time"
         Height          =   255
         Index           =   4
         Left            =   120
         TabIndex        =   48
         Top             =   240
         Width           =   765
      End
   End
   Begin VB.Frame Frame1 
      Caption         =   "Sequence4"
      Height          =   1485
      Index           =   3
      Left            =   90
      TabIndex        =   43
      Top             =   1770
      Width           =   2925
      Begin VB.CheckBox chkLang 
         Caption         =   "MA"
         Height          =   285
         Index           =   11
         Left            =   1710
         Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
         TabIndex        =   20
         Top             =   1110
         Width           =   615
      End
      Begin VB.CheckBox chkLang 
         Caption         =   "CN"
         Height          =   285
         Index           =   10
         Left            =   960
         Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
         TabIndex        =   19
         Top             =   1110
         Width           =   615
      End
      Begin VB.CheckBox chkLang 
         Caption         =   "EN"
         Height          =   285
         Index           =   9
         Left            =   240
         Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
         TabIndex        =   18
         Top             =   1110
         Width           =   615
      End
      Begin VB.ComboBox AnnounceCode 
         Height          =   345
         Index           =   3
         Left            =   1020
         TabIndex        =   17
         Text            =   "000"
         Top             =   690
         Width           =   1095
      End
      Begin VB.ComboBox waitTime 
         Height          =   345
         Index           =   3
         ItemData        =   "frmSequence.frx":0008
         Left            =   1020
         List            =   "frmSequence.frx":000A
         TabIndex        =   16
         Text            =   "0"
         Top             =   210
         Width           =   1095
      End
      Begin VB.Label Label3 
         Caption         =   "Announce Code"
         Height          =   495
         Index           =   3
         Left            =   120
         TabIndex        =   46
         Top             =   630
         Width           =   885
      End
      Begin VB.Label Label2 
         Caption         =   "x100mS"
         Height          =   255
         Index           =   3
         Left            =   2190
         TabIndex        =   45
         Top             =   240
         Width           =   690
      End
      Begin VB.Label Label1 
         Caption         =   "Wait Time"
         Height          =   255
         Index           =   3
         Left            =   120
         TabIndex        =   44
         Top             =   240
         Width           =   765
      End
   End
   Begin VB.Frame Frame1 
      Caption         =   "Sequence3"
      Height          =   1575
      Index           =   2
      Left            =   6060
      TabIndex        =   39
      Top             =   150
      Width           =   2925
      Begin VB.CheckBox chkLang 
         Caption         =   "MA"
         Height          =   285
         Index           =   8
         Left            =   1710
         Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
         TabIndex        =   15
         Top             =   1110
         Width           =   615
      End
      Begin VB.CheckBox chkLang 
         Caption         =   "CN"
         Height          =   285
         Index           =   7
         Left            =   960
         Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
         TabIndex        =   14
         Top             =   1110
         Width           =   615
      End
      Begin VB.CheckBox chkLang 
         Caption         =   "EN"
         Height          =   285
         Index           =   6
         Left            =   240
         Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
         TabIndex        =   13
         Top             =   1110
         Width           =   615
      End
      Begin VB.ComboBox AnnounceCode 
         Height          =   345
         Index           =   2
         Left            =   1050
         TabIndex        =   12
         Text            =   "000"
         Top             =   690
         Width           =   1095
      End
      Begin VB.ComboBox waitTime 
         Height          =   345
         Index           =   2
         ItemData        =   "frmSequence.frx":000C
         Left            =   1050
         List            =   "frmSequence.frx":000E
         TabIndex        =   11
         Text            =   "0"
         Top             =   210
         Width           =   1095
      End
      Begin VB.Label Label3 
         Caption         =   "Announce Code"
         Height          =   495
         Index           =   2
         Left            =   120
         TabIndex        =   42
         Top             =   630
         Width           =   855
      End
      Begin VB.Label Label2 
         Caption         =   "x100mS"
         Height          =   255
         Index           =   2
         Left            =   2190
         TabIndex        =   41
         Top             =   240
         Width           =   690
      End
      Begin VB.Label Label1 
         Caption         =   "Wait Time"
         Height          =   255
         Index           =   2
         Left            =   120
         TabIndex        =   40
         Top             =   240
         Width           =   765
      End
   End
   Begin VB.Frame Frame1 
      Caption         =   "Sequence2"
      Height          =   1575
      Index           =   1
      Left            =   3090
      TabIndex        =   35
      Top             =   150
      Width           =   2925
      Begin VB.CheckBox chkLang 
         Caption         =   "MA"
         Height          =   285
         Index           =   5
         Left            =   1710
         Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
         TabIndex        =   10
         Top             =   1110
         Width           =   615
      End
      Begin VB.CheckBox chkLang 
         Caption         =   "CN"
         Height          =   285
         Index           =   4
         Left            =   960
         Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
         TabIndex        =   9
         Top             =   1110
         Width           =   615
      End
      Begin VB.CheckBox chkLang 
         Caption         =   "EN"
         Height          =   285
         Index           =   3
         Left            =   240
         Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
         TabIndex        =   8
         Top             =   1110
         Width           =   615
      End
      Begin VB.ComboBox waitTime 
         Height          =   345
         Index           =   1
         ItemData        =   "frmSequence.frx":0010
         Left            =   1050
         List            =   "frmSequence.frx":0012
         TabIndex        =   6
         Text            =   "0"
         Top             =   210
         Width           =   1095
      End
      Begin VB.ComboBox AnnounceCode 
         Height          =   345
         Index           =   1
         Left            =   1050
         TabIndex        =   7
         Text            =   "000"
         Top             =   690
         Width           =   1095
      End
      Begin VB.Label Label1 
         Caption         =   "Wait Time"
         Height          =   255
         Index           =   1
         Left            =   120
         TabIndex        =   38
         Top             =   240
         Width           =   765
      End
      Begin VB.Label Label2 
         Caption         =   "x100mS"
         Height          =   255
         Index           =   1
         Left            =   2190
         TabIndex        =   37
         Top             =   240
         Width           =   690
      End
      Begin VB.Label Label3 
         Caption         =   "Announce Code"
         Height          =   495
         Index           =   1
         Left            =   120
         TabIndex        =   36
         Top             =   630
         Width           =   855
      End
   End
   Begin VB.Frame Frame1 
      Caption         =   "Sequence1"
      Height          =   1575
      Index           =   0
      Left            =   90
      TabIndex        =   0
      Top             =   150
      Width           =   2955
      Begin VB.CheckBox chkLang 
         Caption         =   "MA"
         Height          =   285
         Index           =   2
         Left            =   1740
         Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
         TabIndex        =   5
         Top             =   1140
         Width           =   615
      End
      Begin VB.CheckBox chkLang 
         Caption         =   "CN"
         Height          =   285
         Index           =   1
         Left            =   990
         Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
         TabIndex        =   4
         Top             =   1140
         Width           =   615
      End
      Begin VB.CheckBox chkLang 
         Caption         =   "EN"
         Height          =   285
         Index           =   0
         Left            =   270
         Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
         TabIndex        =   3
         Top             =   1140
         Width           =   615
      End
      Begin VB.ComboBox AnnounceCode 
         Height          =   345
         Index           =   0
         Left            =   1050
         TabIndex        =   2
         Text            =   "000"
         Top             =   720
         Width           =   1095
      End
      Begin VB.ComboBox waitTime 
         Height          =   345
         Index           =   0
         ItemData        =   "frmSequence.frx":0014
         Left            =   1050
         List            =   "frmSequence.frx":0016
         TabIndex        =   1
         Text            =   "0"
         Top             =   240
         Width           =   1095
      End
      Begin VB.Label Label3 
         Caption         =   "Announce Code"
         Height          =   495
         Index           =   0
         Left            =   120
         TabIndex        =   34
         Top             =   630
         Width           =   825
      End
      Begin VB.Label Label2 
         Caption         =   "x100mS"
         Height          =   255
         Index           =   0
         Left            =   2190
         TabIndex        =   33
         Top             =   270
         Width           =   690
      End
      Begin VB.Label Label1 
         Caption         =   "Wait Time"
         Height          =   255
         Index           =   0
         Left            =   120
         TabIndex        =   32
         Top             =   270
         Width           =   765
      End
   End
End
Attribute VB_Name = "frmSequence"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Dim bUpdate As Boolean

Public Function DoModal(ByRef bdata() As Byte, ByVal msg As String, ByVal seqsize As Integer) As Boolean
    Dim i As Integer

    On Error Resume Next

    Me.Caption = msg
    For i = 0 To SEQUENCE_MAX - 1
        Me.waitTime(i).Text = Trim$(CInt(bdata(i * 4 + 0) * 256 + CInt(bdata(i * 4 + 1))))
        Me.AnnounceCode(i).Text = Trim$(CInt(bdata(i * 4 + 2) And &H3) * 256 + CInt(bdata(i * 4 + 3)))
        If bdata(i * 4 + 2) And &H80 Then
            chkLang(i * 3 + 0).Value = 1
        Else
            chkLang(i * 3 + 0).Value = 0
        End If
        If bdata(i * 4 + 2) And &H40 Then
            chkLang(i * 3 + 1).Value = 1
        Else
            chkLang(i * 3 + 1).Value = 0
        End If
        If bdata(i * 4 + 2) And &H20 Then
            chkLang(i * 3 + 2).Value = 1
        Else
            chkLang(i * 3 + 2).Value = 0
        End If

        If i < seqsize Then
            Frame1(i).Visible = True
        Else
            Frame1(i).Visible = False
        End If
        If seqsize = 1 Then
            chkLang(i * 3 + 0).Value = 1
            chkLang(i * 3 + 1).Value = 1
            chkLang(i * 3 + 2).Value = 1
        End If
    Next i

    Me.Show vbModal

    DoModal = bUpdate
    If bUpdate Then
        For i = 0 To SEQUENCE_MAX - 1
            bdata(i * 4 + 0) = CByte(CInt(Me.waitTime(i).Text) \ 256)
            bdata(i * 4 + 1) = CByte(CInt(Me.waitTime(i).Text) Mod 256)
            bdata(i * 4 + 2) = CByte(CInt(Me.AnnounceCode(i).Text) \ 256)
            bdata(i * 4 + 3) = CByte(CInt(Me.AnnounceCode(i).Text) Mod 256)
            If chkLang(i * 3 + 0).Value = 1 Then
                bdata(i * 4 + 2) = bdata(i * 4 + 2) Or &H80
            End If
            If chkLang(i * 3 + 1).Value = 1 Then
                bdata(i * 4 + 2) = bdata(i * 4 + 2) Or &H40
            End If
            If chkLang(i * 3 + 2).Value = 1 Then
                bdata(i * 4 + 2) = bdata(i * 4 + 2) Or &H20
            End If
        Next i
    End If

End Function

Private Sub cmdOK_Click()

    bUpdate = True
    Me.Hide

End Sub

Private Sub Command1_Click()

    bUpdate = False
    Me.Hide

End Sub

Private Sub Form_Load()
    Dim i As Integer
    Dim j As Long

    For i = 0 To SEQUENCE_MAX - 1
        waitTime(i).Clear
        For j = 0 To 300
            waitTime(i).AddItem Trim$(j)
        Next j
        AnnounceCode(i).Clear
        For j = 0 To 511
            AnnounceCode(i).AddItem Trim$(j)
        Next j
        For j = 0 To 2
            chkLang(i * 3 + j).Value = 0
        Next j
    Next i

End Sub

