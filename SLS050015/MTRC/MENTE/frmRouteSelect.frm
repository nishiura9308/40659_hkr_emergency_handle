VERSION 5.00
Begin VB.Form frmRouteSelect 
   BorderStyle     =   1  'å≈íË(é¿ê¸)
   Caption         =   "Route Code"
   ClientHeight    =   4755
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   8670
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
   ScaleHeight     =   4755
   ScaleWidth      =   8670
   StartUpPosition =   1  'µ∞≈∞ Ã´∞—ÇÃíÜâõ
   Begin VB.Frame Frame3 
      Caption         =   "Station Name"
      Height          =   2805
      Left            =   180
      TabIndex        =   12
      Top             =   990
      Width           =   8355
      Begin VB.CommandButton cmdStation 
         Caption         =   "Station Data Set"
         Height          =   405
         Left            =   900
         TabIndex        =   4
         Top             =   2130
         Width           =   2325
      End
      Begin VB.CommandButton cmdDelete 
         Caption         =   "Delete Station"
         Height          =   495
         Left            =   6180
         TabIndex        =   8
         Top             =   1620
         Width           =   1995
      End
      Begin VB.Frame Frame4 
         Caption         =   "Add Station Name"
         Height          =   1185
         Left            =   4890
         TabIndex        =   13
         Top             =   330
         Width           =   3285
         Begin VB.CommandButton cmdINS 
            Caption         =   "Insert Station"
            Height          =   525
            Left            =   1710
            TabIndex        =   14
            Top             =   600
            Width           =   1335
         End
         Begin VB.TextBox txtStationName 
            Height          =   315
            Left            =   120
            TabIndex        =   6
            Top             =   600
            Width           =   1515
         End
         Begin VB.CommandButton cmdADD 
            Caption         =   "Add Station"
            Height          =   345
            Left            =   1710
            TabIndex        =   7
            Top             =   240
            Width           =   1335
         End
         Begin VB.ComboBox selectStation 
            Height          =   345
            Left            =   150
            TabIndex        =   5
            Text            =   "00"
            Top             =   240
            Width           =   795
         End
      End
      Begin VB.ListBox selectedStation 
         Height          =   1635
         ItemData        =   "frmRouteSelect.frx":0000
         Left            =   90
         List            =   "frmRouteSelect.frx":0002
         TabIndex        =   3
         Top             =   330
         Width           =   4755
      End
   End
   Begin VB.Frame Frame2 
      Caption         =   "MCP Display Message"
      Height          =   735
      Left            =   1800
      TabIndex        =   11
      Top             =   120
      Width           =   2115
      Begin VB.TextBox txtMCPDisplay 
         Height          =   345
         Left            =   210
         MaxLength       =   6
         TabIndex        =   2
         Text            =   "Text1"
         Top             =   270
         Width           =   1665
      End
   End
   Begin VB.CommandButton cmdCancel 
      Cancel          =   -1  'True
      Caption         =   "Cancel"
      Height          =   400
      Left            =   5550
      TabIndex        =   10
      Top             =   4050
      Width           =   1215
   End
   Begin VB.CommandButton cmdOK 
      Caption         =   "O K"
      Default         =   -1  'True
      Height          =   400
      Left            =   1770
      TabIndex        =   9
      Top             =   4020
      Width           =   1215
   End
   Begin VB.Frame Frame1 
      Caption         =   "EIDS Display No."
      Height          =   735
      Left            =   150
      TabIndex        =   0
      Top             =   90
      Width           =   1575
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
Attribute VB_Name = "frmRouteSelect"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Dim bUpdate As Boolean

Dim bdata(0 To ROUTE_SIZE - 1) As Byte
Dim rdata(0 To STATION_SIZE - 1) As Byte

Dim selno As Integer

Sub DoModal(ByVal no As Integer)
    Dim fno As Integer
    Dim i As Integer
    Dim fname As String

    On Error Resume Next

    selno = no

    If befDateMode = False Then
        fname = App.Path & "\ROUTE" & Right$("0" + Hex$(selno), 2) & ".DAT"
    Else
        fname = App.Path & "\ROUTE" & Right$("0" + Hex$(selno), 2) & ".BEF"
    End If
    fno = FreeFile
    Open fname For Binary As fno
    Get #fno, , bdata
    Close fno
    If FileLen(fname) = 0 Then
        Kill fname
        bdata(0) = 0
    End If

    If bdata(0) <> selno Then
        For i = 0 To ROUTE_SIZE - 1
            bdata(i) = 0
        Next i
    End If

    If bdata(1) = 0 Then
        bdata(1) = &H20
    End If

    Me.selectEIDS.Text = Right$("0" + Hex$(bdata(1)), 2)
    Me.txtMCPDisplay.Text = Chr$(bdata(2)) & Chr$(bdata(3)) & Chr$(bdata(4)) & Chr$(bdata(5)) & Chr$(bdata(6)) & Chr$(bdata(7))
    selectedStation.Clear
    For i = 0 To MAX_STATION - 1
        If bdata(i * STATION_SIZE + 8) = 0 Then Exit For
        If bdata(i * STATION_SIZE + 17) And &H80 Then
            selectedStation.AddItem Right$("0" + Trim$(CInt(Hex$(bdata(i * STATION_SIZE + 8)))), 2) & "  (Station Name : " & GetStationName(CInt(Hex$(bdata(i * STATION_SIZE + 8)))) & ")  Arriving"
        ElseIf bdata(i * STATION_SIZE + 17) And &H40 Then
            selectedStation.AddItem Right$("0" + Trim$(CInt(Hex$(bdata(i * STATION_SIZE + 8)))), 2) & "  (Station Name : " & GetStationName(CInt(Hex$(bdata(i * STATION_SIZE + 8)))) & ")  Arrived"
            If bdata(i * STATION_SIZE + 17) And &H20 Then
            selectedStation.List(i) = selectedStation.List(i) + " Extra"
            End If
        Else
            selectedStation.AddItem Right$("0" + Trim$(CInt(Hex$(bdata(i * STATION_SIZE + 8)))), 2) & "  (Station Name : " & GetStationName(CInt(Hex$(bdata(i * STATION_SIZE + 8)))) & ")  Leaving"
        End If
    Next i

    If selectedStation.ListCount <> 0 Then
        selectedStation.ListIndex = 0
    End If

    Me.Show vbModal

    If bUpdate = True Then
        If selectedStation.ListCount = 0 Then
            If IsFileExist(fname) = True Then
                Kill fname
            End If
            Exit Sub
        End If
    
        bdata(0) = selno
        bdata(1) = CByte("&h" & Me.selectEIDS.Text)
        bdata(2) = Asc(Mid$(Me.txtMCPDisplay & "      ", 1, 1))
        bdata(3) = Asc(Mid$(Me.txtMCPDisplay & "      ", 2, 1))
        bdata(4) = Asc(Mid$(Me.txtMCPDisplay & "      ", 3, 1))
        bdata(5) = Asc(Mid$(Me.txtMCPDisplay & "      ", 4, 1))
        bdata(6) = Asc(Mid$(Me.txtMCPDisplay & "      ", 5, 1))
        bdata(7) = Asc(Mid$(Me.txtMCPDisplay & "      ", 6, 1))

        For i = 0 To MAX_STATION - 1
            If i < selectedStation.ListCount Then
                bdata(i * STATION_SIZE + 8) = CByte("&h" & Mid$(selectedStation.List(i), 1, 2))
            Else
                bdata(i * STATION_SIZE + 8) = 0
            End If
        Next i

        fno = FreeFile
        Open fname For Binary As fno
        Put #fno, , bdata
        Close fno
    End If

End Sub


Private Sub cmdCancel_Click()

    bUpdate = False
    Me.Hide

End Sub

Private Sub cmdINS_Click()
    Dim i As Integer
    Dim j As Integer

    If selectedStation.ListCount = MAX_STATION Then
        MsgBox "Full Station Data"
        Exit Sub
    End If

    For i = 0 To selectedStation.ListCount - 1
        If selectedStation.Selected(i) = True Then
            selectedStation.AddItem Right$("0" + Trim$(CInt(selectStation.Text)), 2) & " (" & GetStationName(CInt(selectStation.Text)) & ")", i
            For j = selectedStation.ListCount * STATION_SIZE - 1 To i * STATION_SIZE Step -1
                bdata(j + STATION_SIZE) = bdata(j)
            Next j
            For j = 0 To STATION_SIZE - 1
                bdata(i * STATION_SIZE + j) = 0
            Next j
            Exit Sub
        End If
    Next i
    
End Sub

Private Sub cmdOK_Click()

    bUpdate = True
    Me.Hide

End Sub

Private Sub cmdADD_Click()
    Dim i As Integer
    Dim j As Integer

    If selectedStation.ListCount = MAX_STATION Then
        MsgBox "Full Station Data"
        Exit Sub
    End If

    selectedStation.AddItem Right$("0" + Trim$(CInt(selectStation.Text)), 2) & " (Station Name : " & GetStationName(CInt(selectStation.Text)) & ")"
    
End Sub

Private Sub cmdDelete_Click()
    Dim i As Integer
    Dim j As Integer

    For i = 0 To selectedStation.ListCount - 1
        If selectedStation.Selected(i) = True Then
            For j = (i + 1) * STATION_SIZE To selectedStation.ListCount * STATION_SIZE - 1
                bdata(j - STATION_SIZE) = bdata(j)
            Next j
            selectedStation.RemoveItem i
            Exit For
        End If
    Next i

End Sub

Private Sub cmdStation_Click()
    Dim i As Integer
    Dim j As Integer


    For i = 0 To selectedStation.ListCount - 1
        If selectedStation.Selected(i) = True Then
            selectStation.Text = Mid$(selectedStation.List(i), 1, 2)
            txtStationName.Text = GetStationName(CInt(selectStation.Text))
            bdata(STATION_SIZE * i + 8 + 0) = CByte("&H" & selectStation.Text)
            For j = 0 To STATION_SIZE - 1
                rdata(j) = bdata(STATION_SIZE * i + 8 + j)
            Next j
            If frmStation.DoModal(selno, i, rdata, "Route " & Right$("0" + Hex$(selno), 2) & " Station " & selectedStation.List(i)) = True Then
                For j = 0 To STATION_SIZE - 1
                    bdata(STATION_SIZE * i + 8 + j) = rdata(j)
                Next j
                If bdata(i * STATION_SIZE + 17) And &H80 Then
                    selectedStation.List(i) = Right$("0" + Trim$(CInt(Hex$(bdata(i * STATION_SIZE + 8)))), 2) & "  (Station Name : " & GetStationName(CInt(Hex$(bdata(i * STATION_SIZE + 8)))) & ")  Arriving"
                ElseIf bdata(i * STATION_SIZE + 17) And &H40 Then
                    selectedStation.List(i) = Right$("0" + Trim$(CInt(Hex$(bdata(i * STATION_SIZE + 8)))), 2) & "  (Station Name : " & GetStationName(CInt(Hex$(bdata(i * STATION_SIZE + 8)))) & ")  Arrived"
                    If bdata(i * STATION_SIZE + 17) And &H20 Then
                    selectedStation.List(i) = selectedStation.List(i) + " Extra"
                    End If
                Else
                    selectedStation.List(i) = Right$("0" + Trim$(CInt(Hex$(bdata(i * STATION_SIZE + 8)))), 2) & "  (Station Name : " & GetStationName(CInt(Hex$(bdata(i * STATION_SIZE + 8)))) & ")  Leaving"
                End If
            End If
            Exit For
        End If
    Next i


End Sub

Private Sub Form_Load()
    Dim i As Integer

    selectEIDS.Clear
    For i = 0 To 95
        selectEIDS.AddItem Right$("0" + Hex$(i + &H20), 2)
    Next i
    selectEIDS.Text = "20"

    selectStation.Clear
    For i = 1 To 99
        selectStation.AddItem Right$("0" + Trim$(i), 2)
    Next i
    selectStation.Text = "01"

    txtStationName.Text = GetStationName(CInt(selectStation.Text))

End Sub


Private Sub selectEIDS_LostFocus()

    If CInt("&h" & selectEIDS.Text) < &H20 Then
        selectEIDS.Text = "20"
    End If
    If CInt("&h" & selectEIDS.Text) > &H7F Then
        selectEIDS.Text = "7F"
    End If

End Sub

Private Sub selectStation_Click()
    
    txtStationName.Text = GetStationName(CInt(selectStation.Text))

End Sub

Private Sub selectStation_LostFocus()
    On Error Resume Next

    If CInt(selectStation.Text) < 0 Then
        selectStation.Text = "01"
    End If
    If CInt(selectStation.Text) > 99 Then
        selectStation.Text = "99"
    End If
    txtStationName.Text = GetStationName(CInt(selectStation.Text))
End Sub


Private Sub txtStationName_LostFocus()

    If txtStationName.Text <> GetStationName(CInt(selectStation.Text)) Then
        If MsgBox("Change StationName OK?", vbOKCancel) = vbOK Then
            Call SetStationName(CInt(selectStation.Text), txtStationName.Text)
        End If
    End If
    
End Sub
