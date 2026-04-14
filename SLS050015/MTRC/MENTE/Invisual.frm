VERSION 5.00
Object = "{648A5603-2C6E-101B-82B6-000000000014}#1.1#0"; "MSCOMM32.OCX"
Begin VB.Form InvisualForm 
   Caption         =   "Invisual"
   ClientHeight    =   3195
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   4680
   LinkTopic       =   "Form1"
   ScaleHeight     =   3195
   ScaleWidth      =   4680
   StartUpPosition =   3  'Windows ‚ÌŠù’è’l
   Visible         =   0   'False
   Begin VB.Timer SimTimer 
      Enabled         =   0   'False
      Interval        =   500
      Left            =   3570
      Top             =   1110
   End
   Begin VB.Timer TimeOutTimer 
      Interval        =   2000
      Left            =   2550
      Top             =   270
   End
   Begin VB.Timer CheckTimer 
      Interval        =   2000
      Left            =   1500
      Top             =   300
   End
   Begin MSCommLib.MSComm mscMSComm 
      Left            =   480
      Top             =   270
      _ExtentX        =   1005
      _ExtentY        =   1005
      _Version        =   393216
      DTREnable       =   -1  'True
      RThreshold      =   1
      RTSEnable       =   -1  'True
      InputMode       =   1
   End
End
Attribute VB_Name = "InvisualForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
