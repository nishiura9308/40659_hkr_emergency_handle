VERSION 5.00
Begin VB.Form frmVersion 
   BorderStyle     =   1  'å≈íË(é¿ê¸)
   Caption         =   "Version"
   ClientHeight    =   2685
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   3120
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   2685
   ScaleWidth      =   3120
   Begin VB.CommandButton cmdCancel 
      Cancel          =   -1  'True
      Caption         =   "Cancel"
      Default         =   -1  'True
      Height          =   400
      Left            =   1620
      TabIndex        =   8
      Top             =   1980
      Width           =   1215
   End
   Begin VB.CommandButton cmdOK 
      Caption         =   "O K"
      Height          =   400
      Left            =   240
      TabIndex        =   7
      Top             =   1980
      Width           =   1215
   End
   Begin VB.Frame Frame1 
      Caption         =   "Version Control"
      Height          =   1545
      Left            =   30
      TabIndex        =   0
      Top             =   0
      Width           =   3045
      Begin VB.TextBox txtDVAU 
         Height          =   315
         Left            =   1080
         TabIndex        =   3
         Top             =   270
         Width           =   1605
      End
      Begin VB.TextBox txtMCP 
         Height          =   315
         Left            =   1080
         TabIndex        =   2
         Top             =   630
         Width           =   1605
      End
      Begin VB.TextBox txtPAA 
         Height          =   315
         Left            =   1080
         TabIndex        =   1
         Top             =   990
         Width           =   1605
      End
      Begin VB.Label DVAUVersion 
         Caption         =   "DVAU"
         Height          =   225
         Left            =   420
         TabIndex        =   6
         Top             =   330
         Width           =   645
      End
      Begin VB.Label Label1 
         Caption         =   "MCP"
         Height          =   225
         Left            =   420
         TabIndex        =   5
         Top             =   690
         Width           =   645
      End
      Begin VB.Label Label2 
         Caption         =   "PAA"
         Height          =   225
         Left            =   420
         TabIndex        =   4
         Top             =   1050
         Width           =   645
      End
   End
End
Attribute VB_Name = "frmVersion"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Dim bUpdate As Boolean

Dim bdata(0 To OWN_VERSION_SIZE - 1) As Byte
Dim bdata2(0 To VERSION_SIZE - 1) As Byte

Dim selno As Integer

Sub DoModal()
    Dim fno As Integer
    Dim data As Integer

    fno = FreeFile
    If befDateMode = False Then
        Open App.Path & "\ANNOUNCE.DAT" For Binary As fno
    Else
        Open App.Path & "\ANNOUNCE.BEF" For Binary As fno
    End If
    Seek #fno, TOP_DVAU_VERSION
    Get fno, , bdata
    Close fno

    txtDVAU.Text = Right$("0000" + Hex$(bdata(0) * 256 + bdata(1)), 4)
    txtMCP.Text = Right$("0000" + Hex$(bdata(2) * 256 + bdata(3)), 4)
    txtPAA.Text = Right$("0000" + Hex$(bdata(4) * 256 + bdata(5)), 4)
    
    Me.Show vbModal

    If bUpdate = True Then
        data = Val("&H" + txtDVAU.Text)
        bdata(0) = data \ 256
        bdata(1) = data Mod 256
        
        data = Val("&H" + txtMCP.Text)
        bdata(2) = data \ 256
        bdata(3) = data Mod 256
        
        data = Val("&H" + txtPAA.Text)
        bdata(4) = data \ 256
        bdata(5) = data Mod 256
        
        fno = FreeFile
        If befDateMode = False Then
            Open App.Path & "\ANNOUNCE.DAT" For Binary As fno
        Else
            Open App.Path & "\ANNOUNCE.BEF" For Binary As fno
        End If
        Seek #fno, TOP_DVAU_VERSION
        Put fno, , bdata
        Close fno
        
        bdata2(0) = bdata(0)
        bdata2(1) = bdata(1)
        
        fno = FreeFile
        If befDateMode = False Then
            Open App.Path & "\CONFIG.DAT" For Binary As fno
        Else
            Open App.Path & "\CONFIG.BEF" For Binary As fno
        End If
        Seek #fno, TOP_VERSION
        Put fno, , bdata2
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


