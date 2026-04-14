VERSION 5.00
Begin VB.Form frmRouteSelect 
   BorderStyle     =   1  'å≈íË(é¿ê¸)
   Caption         =   "Route Code"
   ClientHeight    =   1740
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   3735
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
   ScaleHeight     =   1740
   ScaleWidth      =   3735
   StartUpPosition =   1  'µ∞≈∞ Ã´∞—ÇÃíÜâõ
   Begin VB.CommandButton cmdCancel 
      Cancel          =   -1  'True
      Caption         =   "Cancel"
      Height          =   375
      Left            =   1890
      TabIndex        =   4
      Top             =   1110
      Width           =   1455
   End
   Begin VB.TextBox txtRouteName 
      Height          =   315
      Left            =   30
      Locked          =   -1  'True
      TabIndex        =   2
      Top             =   510
      Width           =   1515
   End
   Begin VB.ComboBox selectRouteNo 
      Height          =   345
      Left            =   990
      TabIndex        =   1
      Text            =   "00"
      Top             =   60
      Width           =   615
   End
   Begin VB.CommandButton cmdRoute 
      Caption         =   "Select"
      Height          =   375
      Left            =   330
      TabIndex        =   0
      Top             =   1110
      Width           =   1455
   End
   Begin VB.Label Label3 
      Caption         =   "No."
      Height          =   345
      Left            =   540
      TabIndex        =   3
      Top             =   90
      Width           =   435
   End
End
Attribute VB_Name = "frmRouteSelect"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Dim selno As Integer

Public Function DoModal() As Integer

    Me.Show vbModal

    DoModal = selno


    Unload Me
End Function

Private Sub cmdCancel_Click()

    selno = 0
    Me.Hide

End Sub

Private Sub cmdRoute_Click()

    selno = CInt("&H" + selectRouteNo.Text)
    Me.Hide

End Sub

Private Sub Form_Load()
    Dim i As Integer

    selectRouteNo.Clear
    For i = 1 To 255
        selectRouteNo.AddItem Right$("0" + Hex$(i), 2)
    Next i
    selectRouteNo.Text = "01"
    txtRouteName.Text = GetRouteName(CInt("&H" + selectRouteNo.Text))

End Sub

Private Sub selectRouteNo_Click()

    txtRouteName.Text = GetRouteName(CInt("&H" + selectRouteNo.Text))

End Sub

Private Sub selectRouteNo_LostFocus()

    On Error Resume Next

    If CInt("&H" + selectRouteNo.Text) < &H1 Then
        selectRouteNo.Text = "01"
    End If
    If CInt("&H" + selectRouteNo.Text) > &HFF Then
        selectRouteNo.Text = "FF"
    End If
    txtRouteName.Text = GetRouteName(CInt("&H" + selectRouteNo.Text))
    
End Sub

