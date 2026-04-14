VERSION 5.00
Begin VB.Form frmRouteDownload 
   BorderStyle     =   1  'å≈íË(é¿ê¸)
   Caption         =   "Route Download"
   ClientHeight    =   4365
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   4350
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
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MDIChild        =   -1  'True
   MinButton       =   0   'False
   ScaleHeight     =   4365
   ScaleWidth      =   4350
   Begin VB.CommandButton cmdSelectALL 
      Caption         =   "SelectAll"
      Height          =   525
      Left            =   1350
      TabIndex        =   3
      Top             =   3000
      Width           =   1425
   End
   Begin VB.Timer downTimer 
      Enabled         =   0   'False
      Interval        =   1000
      Left            =   120
      Top             =   2970
   End
   Begin VB.CommandButton cmdDownload 
      Caption         =   "Download"
      Height          =   525
      Left            =   420
      TabIndex        =   2
      Top             =   3630
      Width           =   1425
   End
   Begin VB.CommandButton cmdCancel 
      Cancel          =   -1  'True
      Caption         =   "Cancel"
      Height          =   525
      Left            =   2400
      TabIndex        =   0
      Top             =   3600
      Width           =   1425
   End
   Begin VB.ListBox selectRoute 
      Height          =   2760
      Left            =   180
      MultiSelect     =   1  'ïWèÄ
      TabIndex        =   1
      Top             =   120
      Width           =   3885
   End
End
Attribute VB_Name = "frmRouteDownload"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Dim selAll As Boolean

Private Sub cmdCancel_Click()

    Unload Me

End Sub

Private Sub cmdDownload_Click()
    
    If MsgBox("Route DATA Download OK?", vbOKCancel) = vbOK Then
        downTimer.Enabled = True
    End If

End Sub

Private Sub cmdSelectALL_Click()
    Dim i As Integer

    For i = 0 To 199
        If i < selectRoute.ListCount Then
            If selAll = True Then
                selectRoute.Selected(i) = False
            Else
                selectRoute.Selected(i) = True
            End If
        End If
    Next i

    If selAll = True Then
        selAll = False
    Else
        selAll = True
    End If
End Sub

Private Sub downTimer_Timer()
    Dim i As Integer
    Dim befFound As Boolean

    cmdDownload.Enabled = False
    cmdSelectALL.Enabled = False
    cmdCancel.Enabled = False

    befFound = False
    For i = 0 To 199
        If i < selectRoute.ListCount Then
            If selectRoute.List(i) = "--- Before ---" Then
                befFound = True
            ElseIf selectRoute.Selected(i) = True Then
                If befFound = True Then
                    If MDIMain.MenteCOMM.DownConfRequest1(App.Path & "\ROUTE" & Mid$(selectRoute.List(i), 8, 2) & ".BEF") = True Then
                        selectRoute.Selected(i) = False
                    End If
                    Exit Sub
                Else
                    If MDIMain.MenteCOMM.DownConfRequest1(App.Path & "\ROUTE" & Mid$(selectRoute.List(i), 8, 2) & ".DAT") = True Then
                        selectRoute.Selected(i) = False
                    End If
                    Exit Sub
                End If
            End If
        End If
    Next i

    frmDownload.ReconfSet
    Unload Me

'    If MDIMain.MenteCOMM.RECONFIGRequest() = True Then
'        Unload Me
'    End If

End Sub

Private Sub Form_Load()
    Dim i As Integer
    Dim fname As String
    Dim fno As Integer

    On Error Resume Next
    
    selectRoute.Clear
    For i = 0 To 99
        fname = App.Path & "\ROUTE" & Right$("0" + Hex$(i), 2) & ".DAT"
        fno = FreeFile
        Err.Clear
        Open fname For Input As fno
        Close fno
        If Err = 0 Then
            selectRoute.AddItem " Route " & Right$("0" + Hex$(i), 2) & "  (" & GetRouteName(i) & ")"
        End If
    Next i

    If Now < befDateConfig Then
        selectRoute.AddItem "--- Before ---"
        For i = 0 To 99
            fname = App.Path & "\ROUTE" & Right$("0" + Hex$(i), 2) & ".BEF"
            fno = FreeFile
            Err.Clear
            Open fname For Input As fno
            Close fno
            If Err = 0 Then
                selectRoute.AddItem " Route " & Right$("0" + Hex$(i), 2) & "  (" & GetRouteName(i) & ")"
            End If
        Next i
    End If

    downBusy = True
    selAll = False
End Sub

Private Sub Form_Unload(Cancel As Integer)

    downBusy = False

End Sub
