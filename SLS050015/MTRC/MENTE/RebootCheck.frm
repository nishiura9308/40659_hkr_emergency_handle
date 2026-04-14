VERSION 5.00
Begin VB.Form RebootCheck 
   BorderStyle     =   3  'ŒÅ’èÀÞ²±Û¸Þ
   Caption         =   "Reboot Check"
   ClientHeight    =   1095
   ClientLeft      =   2760
   ClientTop       =   3750
   ClientWidth     =   3735
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   1095
   ScaleWidth      =   3735
   ShowInTaskbar   =   0   'False
   StartUpPosition =   1  'µ°Å° Ì«°Ñ‚Ì’†‰›
   Begin VB.CommandButton CancelButton 
      Caption         =   "·¬Ý¾Ù"
      Height          =   375
      Left            =   2250
      TabIndex        =   1
      Top             =   570
      Width           =   1215
   End
   Begin VB.CommandButton OKButton 
      Caption         =   "OK"
      Height          =   375
      Left            =   2250
      TabIndex        =   0
      Top             =   60
      Width           =   1215
   End
   Begin VB.Label Label1 
      Caption         =   "Reboot NOW?"
      BeginProperty Font 
         Name            =   "Courier New"
         Size            =   9.75
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   300
      TabIndex        =   2
      Top             =   300
      Width           =   1815
   End
End
Attribute VB_Name = "RebootCheck"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False

Option Explicit

Public RebootConf As Boolean

Private Sub CancelButton_Click()

    RebootConf = False
    Me.Hide

End Sub

Private Sub OKButton_Click()

    RebootConf = True
    Me.Hide

End Sub
