VERSION 5.00
Begin VB.Form frmChime 
   BorderStyle     =   1  'å≈íË(é¿ê¸)
   Caption         =   "Chime Code"
   ClientHeight    =   2580
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   3165
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
   ScaleHeight     =   2580
   ScaleWidth      =   3165
   StartUpPosition =   1  'µ∞≈∞ Ã´∞—ÇÃíÜâõ
   Begin VB.CommandButton Command1 
      Cancel          =   -1  'True
      Caption         =   "Cancel"
      Default         =   -1  'True
      Height          =   645
      Left            =   1800
      TabIndex        =   7
      Top             =   1800
      Width           =   1215
   End
   Begin VB.CommandButton cmdOK 
      Caption         =   "O K"
      Height          =   645
      Left            =   120
      TabIndex        =   3
      Top             =   1800
      Width           =   1215
   End
   Begin VB.Frame Frame1 
      Caption         =   "Chime"
      Height          =   1395
      Index           =   0
      Left            =   90
      TabIndex        =   0
      Top             =   150
      Width           =   2955
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
         ItemData        =   "frmChime.frx":0000
         Left            =   1050
         List            =   "frmChime.frx":0002
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
         TabIndex        =   6
         Top             =   630
         Width           =   825
      End
      Begin VB.Label Label2 
         Caption         =   "x100mS"
         Height          =   255
         Index           =   0
         Left            =   2190
         TabIndex        =   5
         Top             =   270
         Width           =   690
      End
      Begin VB.Label Label1 
         Caption         =   "Wait Time"
         Height          =   255
         Index           =   0
         Left            =   120
         TabIndex        =   4
         Top             =   270
         Width           =   765
      End
   End
End
Attribute VB_Name = "frmChime"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Dim bUpdate As Boolean

Public Function DoModal(ByRef bdata() As Byte, ByVal Msg As String) As Boolean
    Dim i As Integer

    On Error Resume Next

    Me.Caption = Msg
    i = 0
        
        Me.waitTime(i).Text = Trim$(CInt(bdata(i * 4 + 0) * 256 + CInt(bdata(i * 4 + 1))))
        Select Case CInt(bdata(i * 4 + 2) And &H3) * 256 + CInt(bdata(i * 4 + 3))
        Case 500
            Me.AnnounceCode(i).Text = "2.0 Sec"
        Case 501
            Me.AnnounceCode(i).Text = "2.5 Sec"
        Case 502
            Me.AnnounceCode(i).Text = "3.0 Sec"
        Case 503
            Me.AnnounceCode(i).Text = "3.5 Sec"
        Case 504
            Me.AnnounceCode(i).Text = "4.0 Sec"
        Case 505
            Me.AnnounceCode(i).Text = "4.5 Sec"
        Case 506
            Me.AnnounceCode(i).Text = "5.0 Sec"
        Case 507
            Me.AnnounceCode(i).Text = "5.5 Sec"
        Case 508
            Me.AnnounceCode(i).Text = "6.0 Sec"
        Case Else
            Me.AnnounceCode(i).Text = "2.0 Sec"
        End Select

    Me.Show vbModal

    DoModal = bUpdate
    If bUpdate Then
        i = 0
        
            bdata(i * 4 + 0) = CByte(CInt(Me.waitTime(i).Text) \ 256)
            bdata(i * 4 + 1) = CByte(CInt(Me.waitTime(i).Text) Mod 256)
            
            Select Case Me.AnnounceCode(i).Text
            Case "2.0 Sec"
                bdata(i * 4 + 2) = CByte(500 \ 256)
                bdata(i * 4 + 3) = CByte(500 Mod 256)
            Case "2.5 Sec"
                bdata(i * 4 + 2) = CByte(501 \ 256)
                bdata(i * 4 + 3) = CByte(501 Mod 256)
            Case "3.0 Sec"
                bdata(i * 4 + 2) = CByte(502 \ 256)
                bdata(i * 4 + 3) = CByte(502 Mod 256)
            Case "3.5 Sec"
                bdata(i * 4 + 2) = CByte(503 \ 256)
                bdata(i * 4 + 3) = CByte(503 Mod 256)
            Case "4.0 Sec"
                bdata(i * 4 + 2) = CByte(504 \ 256)
                bdata(i * 4 + 3) = CByte(504 Mod 256)
            Case "4.5 Sec"
                bdata(i * 4 + 2) = CByte(505 \ 256)
                bdata(i * 4 + 3) = CByte(505 Mod 256)
            Case "5.0 Sec"
                bdata(i * 4 + 2) = CByte(506 \ 256)
                bdata(i * 4 + 3) = CByte(506 Mod 256)
            Case "5.5 Sec"
                bdata(i * 4 + 2) = CByte(507 \ 256)
                bdata(i * 4 + 3) = CByte(507 Mod 256)
            Case "6.0 Sec"
                bdata(i * 4 + 2) = CByte(508 \ 256)
                bdata(i * 4 + 3) = CByte(508 Mod 256)
            Case Else
                bdata(i * 4 + 2) = CByte(500 \ 256)
                bdata(i * 4 + 3) = CByte(500 Mod 256)
            End Select
            
            bdata(i * 4 + 2) = bdata(i * 4 + 2) Or &H80
            bdata(i * 4 + 2) = bdata(i * 4 + 2) Or &H40
            bdata(i * 4 + 2) = bdata(i * 4 + 2) Or &H20
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

    i = 0
    waitTime(i).Clear
    For j = 0 To 300
        waitTime(i).AddItem Trim$(j)
    Next j
    AnnounceCode(i).Clear
    AnnounceCode(i).AddItem "2.0 Sec"
    AnnounceCode(i).AddItem "2.5 Sec"
    AnnounceCode(i).AddItem "3.0 Sec"
    AnnounceCode(i).AddItem "3.5 Sec"
    AnnounceCode(i).AddItem "4.0 Sec"
    AnnounceCode(i).AddItem "4.5 Sec"
    AnnounceCode(i).AddItem "5.0 Sec"
    AnnounceCode(i).AddItem "5.5 Sec"
    AnnounceCode(i).AddItem "6.0 Sec"

End Sub

