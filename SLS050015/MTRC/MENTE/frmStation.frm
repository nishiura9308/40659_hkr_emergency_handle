VERSION 5.00
Begin VB.Form frmStation 
   BorderStyle     =   1  'å≈íË(é¿ê¸)
   Caption         =   "Station Announce"
   ClientHeight    =   3735
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   7080
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
   ScaleHeight     =   3735
   ScaleWidth      =   7080
   StartUpPosition =   1  'µ∞≈∞ Ã´∞—ÇÃíÜâõ
   Begin VB.CheckBox chkArrived 
      Caption         =   "Arrived"
      Height          =   375
      Left            =   3135
      Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
      TabIndex        =   3
      Top             =   2385
      Width           =   1275
   End
   Begin VB.Frame Distance 
      Caption         =   "Broadcasted by Line,Station No."
      Height          =   855
      Left            =   3000
      TabIndex        =   15
      Top             =   2055
      Width           =   3645
      Begin VB.CheckBox chkExtramsg 
         Caption         =   "extra message"
         Height          =   360
         Left            =   1800
         TabIndex        =   16
         Top             =   330
         Width           =   1710
      End
   End
   Begin VB.TextBox StationName 
      Height          =   375
      Left            =   0
      TabIndex        =   14
      Text            =   "Route = xx  Station = xx"
      Top             =   -30
      Width           =   6975
   End
   Begin VB.Frame Frame4 
      Caption         =   "Station Number"
      Height          =   795
      Left            =   90
      TabIndex        =   13
      Top             =   2700
      Width           =   2745
      Begin VB.TextBox txtStationNumber 
         Height          =   345
         Left            =   930
         TabIndex        =   7
         Top             =   240
         Width           =   1665
      End
      Begin VB.ComboBox selectStationNumber 
         Height          =   345
         Left            =   150
         TabIndex        =   6
         Text            =   "00"
         Top             =   300
         Width           =   705
      End
   End
   Begin VB.Frame Frame3 
      Caption         =   "Line Number"
      Height          =   795
      Left            =   90
      TabIndex        =   12
      Top             =   1800
      Width           =   2745
      Begin VB.TextBox txtLineNo 
         Height          =   345
         Left            =   930
         TabIndex        =   5
         Top             =   300
         Width           =   1665
      End
      Begin VB.ComboBox selectLineNo 
         Height          =   345
         Left            =   150
         TabIndex        =   4
         Text            =   "00"
         Top             =   300
         Width           =   705
      End
   End
   Begin VB.CommandButton cmdAnnounce 
      Caption         =   "Announce Sequence"
      Height          =   495
      Left            =   3030
      TabIndex        =   8
      Top             =   1320
      Width           =   2685
   End
   Begin VB.Frame Frame2 
      Caption         =   "MCP Display Message"
      Height          =   735
      Left            =   1830
      TabIndex        =   11
      Top             =   480
      Width           =   2265
      Begin VB.TextBox txtMCPDisplay 
         Height          =   345
         Left            =   210
         MaxLength       =   6
         TabIndex        =   2
         Top             =   270
         Width           =   1665
      End
   End
   Begin VB.CommandButton Command1 
      Cancel          =   -1  'True
      Caption         =   "Cancel"
      Height          =   400
      Left            =   4440
      TabIndex        =   10
      Top             =   3090
      Width           =   1215
   End
   Begin VB.CommandButton cmdOK 
      Caption         =   "O K"
      Default         =   -1  'True
      Height          =   400
      Left            =   3120
      TabIndex        =   9
      Top             =   3090
      Width           =   1215
   End
   Begin VB.Frame Frame1 
      Caption         =   "EIDS Display No."
      Height          =   735
      Left            =   90
      TabIndex        =   0
      Top             =   450
      Width           =   1665
      Begin VB.ComboBox selectEIDS 
         Height          =   345
         Left            =   360
         TabIndex        =   1
         Text            =   "00"
         Top             =   330
         Width           =   615
      End
   End
End
Attribute VB_Name = "frmStation"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Dim bUpdate As Boolean

Dim selRouteNo As Integer
Dim selno As Integer

Dim bdata(0 To STATION_SIZE - 1) As Byte
Dim sdata(0 To SEQUENCE_SIZE - 1) As Byte

Function DoModal(ByVal routeno As Integer, ByVal no As Integer, ByRef rdata() As Byte, ByVal msg As String) As Boolean
    Dim fno As Integer
    Dim i As Integer

    selRouteNo = routeno
    selno = no

    Me.StationName.Text = msg

    For i = 0 To STATION_SIZE - 1
        bdata(i) = rdata(i)
    Next i

'    If bdata(1) = 0 Then
'        bdata(1) = &H20
'    End If

    Me.selectEIDS.Text = Right$("0" + Hex$(bdata(1)), 2)
    Me.txtMCPDisplay.Text = Chr$(bdata(2)) & Chr$(bdata(3)) & Chr$(bdata(4)) & Chr$(bdata(5)) & Chr$(bdata(6)) & Chr$(bdata(7))
    For i = 0 To 7
        If ((bdata(8) And &HE0) \ 32) = i Then
            Me.selectLineNo.ListIndex = i
            Exit For
        End If
    Next i
    For i = 0 To 31
        If (bdata(8) And &H1F) = i Then
            Me.selectStationNumber.ListIndex = i
            Exit For
        End If
    Next i
    If bdata(9) And &H40 Then
        chkArrived.Value = 1
        If bdata(9) And &H20 Then
            chkExtramsg.Value = 1
        End If
    End If

    Me.Show vbModal

    DoModal = bUpdate
    If bUpdate = True Then
        bdata(1) = CByte("&H" & Me.selectEIDS.Text)
        bdata(2) = Asc(Mid$(Me.txtMCPDisplay & "      ", 1, 1))
        bdata(3) = Asc(Mid$(Me.txtMCPDisplay & "      ", 2, 1))
        bdata(4) = Asc(Mid$(Me.txtMCPDisplay & "      ", 3, 1))
        bdata(5) = Asc(Mid$(Me.txtMCPDisplay & "      ", 4, 1))
        bdata(6) = Asc(Mid$(Me.txtMCPDisplay & "      ", 5, 1))
        bdata(7) = Asc(Mid$(Me.txtMCPDisplay & "      ", 6, 1))
        bdata(8) = CByte(Me.selectLineNo.Text) * 32 + CByte("&H" + Me.selectStationNumber.Text)
        
        If chkArrived.Value = 1 Then
            bdata(9) = bdata(9) Or &H40
        Else
            bdata(9) = bdata(9) And &HB0
        End If

        If chkExtramsg.Value = 1 Then
            bdata(9) = bdata(9) Or &H20
        Else
            bdata(9) = bdata(9) And &HD0
        End If

        For i = 0 To STATION_SIZE - 1
            rdata(i) = bdata(i)
        Next i
    End If

    Unload Me

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

    selectEIDS.Clear
    selectEIDS.AddItem "00"
    For i = 0 To 95
        selectEIDS.AddItem Right$("0" + Hex$(i + &H20), 2)
    Next i
    selectEIDS.Text = "20"

    selectLineNo.Clear
    For i = 0 To 7
        selectLineNo.AddItem Trim$(i)
    Next i
    selectLineNo.Text = "0"

    selectStationNumber.Clear
    For i = 0 To &H1F
        selectStationNumber.AddItem Right$("0" + Hex$(i), 2)
    Next i
    selectStationNumber.Text = "00"

    txtStationNumber.Text = GetStationName(CInt(selectStationNumber.Text))
End Sub


Private Sub cmdAnnounce_Click()
    Dim i As Integer
    
    For i = 0 To SEQUENCE_SIZE - 1
        sdata(i) = bdata(1 + 1 + 6 + 1 + 1 + i)
    Next i
    If frmSequence.DoModal(sdata, Me.StationName.Text & " Code Define", SEQUENCE_MAX) = True Then
        For i = 0 To SEQUENCE_SIZE - 1
            bdata(1 + 1 + 6 + 1 + 1 + i) = sdata(i)
        Next i
    End If

End Sub


Private Sub selectLineNo_Click()

    txtLineNo.Text = GetLineName(selRouteNo, CInt(selectLineNo.Text))

End Sub

Private Sub selectLineNo_LostFocus()
    On Error Resume Next

    If CInt(selectLineNo.Text) < 0 Then
        selectLineNo.Text = "0"
    End If
    If CInt(selectLineNo.Text) > 7 Then
        selectLineNo.Text = "7"
    End If
    txtLineNo.Text = GetLineName(selRouteNo, CInt(selectLineNo.Text))
End Sub

Private Sub selectStationNumber_Click()

    txtStationNumber.Text = GetStationNumberName(selRouteNo, CInt("&H" + selectStationNumber.Text))

End Sub

Private Sub selectStationNumber_LostFocus()

    On Error Resume Next

    If CInt("&H" + selectStationNumber.Text) < 0 Then
        selectStationNumber.Text = "00"
    End If
    If CInt("&H" + selectStationNumber.Text) > &H1F Then
        selectStationNumber.Text = "1F"
    End If
    txtStationNumber.Text = GetStationNumberName(selRouteNo, CInt("&H" + selectStationNumber.Text))
End Sub

Private Sub txtLineNo_LostFocus()
    
    On Error Resume Next
    If txtLineNo.Text <> GetLineName(selRouteNo, CInt(selectLineNo.Text)) Then
        If MsgBox("Change Line Name OK?", vbOKCancel) = vbOK Then
            Call SetLineName(selRouteNo, CInt(selectLineNo.Text), txtLineNo.Text)
        End If
    End If

End Sub

Private Sub txtStationNumber_LostFocus()

    On Error Resume Next

    If txtStationNumber.Text <> GetStationNumberName(selRouteNo, CInt("&H" + selectStationNumber.Text)) Then
        If MsgBox("Change Station Number Name OK?", vbOKCancel) = vbOK Then
            Call SetStationNumberName(selRouteNo, CInt("&H" + selectStationNumber.Text), txtStationNumber.Text)
        End If
    End If

End Sub

Private Sub selectEIDS_LostFocus()

    If CInt("&h" & selectEIDS.Text) < &H20 Then
        If CInt("&h" & selectEIDS.Text) > 0 Then
            selectEIDS.Text = "20"
        End If
    End If
    If CInt("&h" & selectEIDS.Text) > &H7F Then
        selectEIDS.Text = "7F"
    End If

End Sub


