VERSION 5.00
Begin VB.Form frmLanguage 
   BorderStyle     =   1  'å≈íË(é¿ê¸)
   Caption         =   "Select Language"
   ClientHeight    =   1605
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   3270
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   1605
   ScaleWidth      =   3270
   StartUpPosition =   1  'µ∞≈∞ Ã´∞—ÇÃíÜâõ
   Begin VB.Frame Frame2 
      Caption         =   "Language"
      Height          =   795
      Left            =   120
      TabIndex        =   2
      Top             =   90
      Width           =   3045
      Begin VB.ComboBox SelectLang 
         Height          =   300
         Left            =   270
         TabIndex        =   3
         Text            =   "000"
         Top             =   360
         Width           =   2385
      End
   End
   Begin VB.CommandButton cmdOK 
      Caption         =   "O K"
      Height          =   400
      Left            =   330
      TabIndex        =   1
      Top             =   1080
      Width           =   1215
   End
   Begin VB.CommandButton cmdCancel 
      Cancel          =   -1  'True
      Caption         =   "Cancel"
      Default         =   -1  'True
      Height          =   400
      Left            =   1710
      TabIndex        =   0
      Top             =   1080
      Width           =   1215
   End
End
Attribute VB_Name = "frmLanguage"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Dim bUpdate As Boolean

Dim bdata(0 To LANG_SIZE - 1) As Byte

Dim selno As Integer

Sub DoModal()
    Dim fno As Integer

    fno = FreeFile
    If befDateMode = False Then
        Open App.Path & "\CONFIG.DAT" For Binary As fno
    Else
        Open App.Path & "\CONFIG.BEF" For Binary As fno
    End If
    Seek #fno, TOP_LANG
    Get fno, , bdata
    Close fno

    If bdata(0) = 0 Then
        bdata(0) = &H80
    End If

    Me.SelectLang.Clear
    Me.SelectLang.AddItem "CN-MA"
    Me.SelectLang.AddItem "EN-CN"
    Me.SelectLang.AddItem "EN-MA"
    Me.SelectLang.AddItem "E-C-M"
    
    If bdata(0) And &H80 Then
        Me.SelectLang.ListIndex = 0
        If bdata(0) And &H40 Then
            Me.SelectLang.ListIndex = 1
            If bdata(0) And &H20 Then
                Me.SelectLang.ListIndex = 3
            End If
        ElseIf bdata(0) And &H20 Then
            Me.SelectLang.ListIndex = 2
        End If
    Else
        Me.SelectLang.ListIndex = 0
    End If
    
    Me.Show vbModal

    If bUpdate = True Then
        Select Case Me.SelectLang.ListIndex
        Case 0
            bdata(0) = &H60
        Case 1
            bdata(0) = &HC0
        Case 2
            bdata(0) = &HA0
        Case 3
            bdata(0) = &HE0
        End Select
        fno = FreeFile
        If befDateMode = False Then
            Open App.Path & "\CONFIG.DAT" For Binary As fno
        Else
            Open App.Path & "\CONFIG.BEF" For Binary As fno
        End If
        Seek #fno, TOP_LANG
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

