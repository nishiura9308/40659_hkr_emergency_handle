VERSION 5.00
Begin VB.Form frmSD 
   BorderStyle     =   1  'å≈íË(é¿ê¸)
   Caption         =   "SD Monitor"
   ClientHeight    =   5820
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   8715
   ControlBox      =   0   'False
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MDIChild        =   -1  'True
   MinButton       =   0   'False
   ScaleHeight     =   5820
   ScaleWidth      =   8715
   Begin VB.Frame SDFrame 
      Caption         =   "TMS SD Monitor"
      Height          =   5745
      Left            =   90
      TabIndex        =   0
      Top             =   60
      Width           =   8595
      Begin VB.Frame Frame11 
         Caption         =   "Car Info"
         Height          =   3675
         Left            =   3660
         TabIndex        =   38
         Top             =   1920
         Width           =   4755
         Begin VB.TextBox carTypeResponce 
            Height          =   330
            Index           =   7
            Left            =   720
            Locked          =   -1  'True
            TabIndex        =   95
            Text            =   "0"
            Top             =   2880
            Width           =   465
         End
         Begin VB.TextBox carTypeResponce 
            Height          =   330
            Index           =   6
            Left            =   720
            Locked          =   -1  'True
            TabIndex        =   94
            Text            =   "0"
            Top             =   2520
            Width           =   465
         End
         Begin VB.TextBox carTypeResponce 
            Height          =   330
            Index           =   5
            Left            =   720
            Locked          =   -1  'True
            TabIndex        =   93
            Text            =   "0"
            Top             =   2175
            Width           =   465
         End
         Begin VB.TextBox carTypeResponce 
            Height          =   330
            Index           =   4
            Left            =   720
            Locked          =   -1  'True
            TabIndex        =   92
            Text            =   "0"
            Top             =   1845
            Width           =   465
         End
         Begin VB.TextBox carTypeResponce 
            Height          =   330
            Index           =   3
            Left            =   720
            Locked          =   -1  'True
            TabIndex        =   91
            Text            =   "0"
            Top             =   1500
            Width           =   465
         End
         Begin VB.TextBox carTypeResponce 
            Height          =   330
            Index           =   2
            Left            =   720
            Locked          =   -1  'True
            TabIndex        =   90
            Text            =   "0"
            Top             =   1155
            Width           =   465
         End
         Begin VB.TextBox carTypeResponce 
            Height          =   330
            Index           =   1
            Left            =   720
            Locked          =   -1  'True
            TabIndex        =   89
            Text            =   "0"
            Top             =   825
            Width           =   465
         End
         Begin VB.TextBox carTypeResponce 
            Height          =   330
            Index           =   0
            Left            =   720
            Locked          =   -1  'True
            TabIndex        =   88
            Text            =   "0"
            Top             =   480
            Width           =   465
         End
         Begin VB.TextBox CarNoResponce 
            Height          =   330
            Index           =   7
            Left            =   150
            Locked          =   -1  'True
            TabIndex        =   87
            Text            =   "000"
            Top             =   2880
            Width           =   465
         End
         Begin VB.TextBox CarNoResponce 
            Height          =   330
            Index           =   6
            Left            =   150
            Locked          =   -1  'True
            TabIndex        =   86
            Text            =   "000"
            Top             =   2520
            Width           =   465
         End
         Begin VB.TextBox CarNoResponce 
            Height          =   330
            Index           =   5
            Left            =   150
            Locked          =   -1  'True
            TabIndex        =   85
            Text            =   "000"
            Top             =   2175
            Width           =   465
         End
         Begin VB.TextBox CarNoResponce 
            Height          =   330
            Index           =   4
            Left            =   150
            Locked          =   -1  'True
            TabIndex        =   84
            Text            =   "000"
            Top             =   1845
            Width           =   465
         End
         Begin VB.TextBox CarNoResponce 
            Height          =   330
            Index           =   3
            Left            =   150
            Locked          =   -1  'True
            TabIndex        =   83
            Text            =   "000"
            Top             =   1500
            Width           =   465
         End
         Begin VB.TextBox CarNoResponce 
            Height          =   330
            Index           =   2
            Left            =   150
            Locked          =   -1  'True
            TabIndex        =   82
            Text            =   "000"
            Top             =   1155
            Width           =   465
         End
         Begin VB.TextBox CarNoResponce 
            Height          =   330
            Index           =   1
            Left            =   150
            Locked          =   -1  'True
            TabIndex        =   81
            Text            =   "000"
            Top             =   825
            Width           =   465
         End
         Begin VB.Frame Frame12 
            Caption         =   "EAH Active(Physical layout)"
            Height          =   3315
            Left            =   1320
            TabIndex        =   40
            Top             =   180
            Width           =   3315
            Begin VB.TextBox EAHActive 
               Height          =   360
               Index           =   39
               Left            =   2540
               Locked          =   -1  'True
               TabIndex        =   80
               Text            =   "V5"
               Top             =   2715
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   38
               Left            =   1940
               Locked          =   -1  'True
               TabIndex        =   79
               Text            =   "V4"
               Top             =   2715
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   37
               Left            =   1340
               Locked          =   -1  'True
               TabIndex        =   78
               Text            =   "V3"
               Top             =   2715
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   36
               Left            =   740
               Locked          =   -1  'True
               TabIndex        =   77
               Text            =   "V2"
               Top             =   2715
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   35
               Left            =   120
               Locked          =   -1  'True
               TabIndex        =   76
               Text            =   "V1"
               Top             =   2715
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   34
               Left            =   2540
               Locked          =   -1  'True
               TabIndex        =   75
               Text            =   "Z5"
               Top             =   2385
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   33
               Left            =   1940
               Locked          =   -1  'True
               TabIndex        =   74
               Text            =   "Z4"
               Top             =   2385
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   32
               Left            =   1340
               Locked          =   -1  'True
               TabIndex        =   73
               Text            =   "Z3"
               Top             =   2385
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   31
               Left            =   740
               Locked          =   -1  'True
               TabIndex        =   72
               Text            =   "Z2"
               Top             =   2385
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   30
               Left            =   120
               Locked          =   -1  'True
               TabIndex        =   71
               Text            =   "Z1"
               Top             =   2385
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   29
               Left            =   2540
               Locked          =   -1  'True
               TabIndex        =   70
               Text            =   "X5"
               Top             =   2040
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   28
               Left            =   1940
               Locked          =   -1  'True
               TabIndex        =   69
               Text            =   "X4"
               Top             =   2040
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   27
               Left            =   1340
               Locked          =   -1  'True
               TabIndex        =   68
               Text            =   "X3"
               Top             =   2040
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   26
               Left            =   740
               Locked          =   -1  'True
               TabIndex        =   67
               Text            =   "X2"
               Top             =   2040
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   25
               Left            =   120
               Locked          =   -1  'True
               TabIndex        =   66
               Text            =   "X1"
               Top             =   2040
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   24
               Left            =   2540
               Locked          =   -1  'True
               TabIndex        =   65
               Text            =   "Y5"
               Top             =   1680
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   23
               Left            =   1940
               Locked          =   -1  'True
               TabIndex        =   64
               Text            =   "Y4"
               Top             =   1680
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   22
               Left            =   1340
               Locked          =   -1  'True
               TabIndex        =   63
               Text            =   "Y3"
               Top             =   1680
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   21
               Left            =   740
               Locked          =   -1  'True
               TabIndex        =   62
               Text            =   "Y2"
               Top             =   1680
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   20
               Left            =   120
               Locked          =   -1  'True
               TabIndex        =   61
               Text            =   "Y1"
               Top             =   1680
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   19
               Left            =   2540
               Locked          =   -1  'True
               TabIndex        =   60
               Text            =   "W5"
               Top             =   1335
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   18
               Left            =   1940
               Locked          =   -1  'True
               TabIndex        =   59
               Text            =   "W4"
               Top             =   1335
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   17
               Left            =   1340
               Locked          =   -1  'True
               TabIndex        =   58
               Text            =   "W3"
               Top             =   1335
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   16
               Left            =   740
               Locked          =   -1  'True
               TabIndex        =   57
               Text            =   "W2"
               Top             =   1335
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   15
               Left            =   120
               Locked          =   -1  'True
               TabIndex        =   56
               Text            =   "W1"
               Top             =   1335
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   14
               Left            =   2540
               Locked          =   -1  'True
               TabIndex        =   55
               Text            =   "X5"
               Top             =   975
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   13
               Left            =   1940
               Locked          =   -1  'True
               TabIndex        =   54
               Text            =   "X4"
               Top             =   975
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   12
               Left            =   1340
               Locked          =   -1  'True
               TabIndex        =   53
               Text            =   "X3"
               Top             =   975
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   11
               Left            =   740
               Locked          =   -1  'True
               TabIndex        =   52
               Text            =   "X2"
               Top             =   975
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   10
               Left            =   120
               Locked          =   -1  'True
               TabIndex        =   51
               Text            =   "X1"
               Top             =   975
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   9
               Left            =   2540
               Locked          =   -1  'True
               TabIndex        =   50
               Text            =   "Z5"
               Top             =   645
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   8
               Left            =   1940
               Locked          =   -1  'True
               TabIndex        =   49
               Text            =   "Z4"
               Top             =   645
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   7
               Left            =   1340
               Locked          =   -1  'True
               TabIndex        =   48
               Text            =   "Z3"
               Top             =   640
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   6
               Left            =   740
               Locked          =   -1  'True
               TabIndex        =   47
               Text            =   "Z2"
               Top             =   640
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   5
               Left            =   120
               Locked          =   -1  'True
               TabIndex        =   46
               Text            =   "Z1"
               Top             =   640
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   4
               Left            =   2540
               Locked          =   -1  'True
               TabIndex        =   45
               Text            =   "V5"
               Top             =   300
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   3
               Left            =   1940
               Locked          =   -1  'True
               TabIndex        =   44
               Text            =   "V4"
               Top             =   300
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   2
               Left            =   1340
               Locked          =   -1  'True
               TabIndex        =   43
               Text            =   "V3"
               Top             =   300
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   1
               Left            =   740
               Locked          =   -1  'True
               TabIndex        =   42
               Text            =   "V2"
               Top             =   300
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   0
               Left            =   120
               Locked          =   -1  'True
               TabIndex        =   41
               Text            =   "V1"
               Top             =   300
               Width           =   600
            End
         End
         Begin VB.TextBox CarNoResponce 
            Height          =   330
            Index           =   0
            Left            =   150
            Locked          =   -1  'True
            TabIndex        =   39
            Text            =   "000"
            Top             =   480
            Width           =   465
         End
      End
      Begin VB.Frame Frame13 
         Caption         =   "MODE"
         Height          =   1455
         Left            =   1110
         TabIndex        =   30
         Top             =   1830
         Width           =   2475
         Begin VB.TextBox modeRespoince 
            Height          =   315
            Index           =   0
            Left            =   210
            Locked          =   -1  'True
            TabIndex        =   37
            Text            =   "MANUAL"
            Top             =   270
            Width           =   825
         End
         Begin VB.TextBox modeRespoince 
            Height          =   315
            Index           =   1
            Left            =   1080
            Locked          =   -1  'True
            TabIndex        =   36
            Text            =   "CAB/CAB"
            Top             =   270
            Width           =   1035
         End
         Begin VB.TextBox modeRespoince 
            Height          =   315
            Index           =   2
            Left            =   210
            Locked          =   -1  'True
            TabIndex        =   35
            Text            =   "EMERGENCY"
            Top             =   600
            Width           =   1185
         End
         Begin VB.TextBox modeRespoince 
            Height          =   315
            Index           =   3
            Left            =   1410
            Locked          =   -1  'True
            TabIndex        =   34
            Text            =   "SPECIAL"
            Top             =   600
            Width           =   885
         End
         Begin VB.TextBox modeRespoince 
            Height          =   315
            Index           =   4
            Left            =   210
            Locked          =   -1  'True
            TabIndex        =   33
            Text            =   "AUTO"
            Top             =   960
            Width           =   555
         End
         Begin VB.TextBox modeRespoince 
            Height          =   315
            Index           =   5
            Left            =   990
            Locked          =   -1  'True
            TabIndex        =   32
            Text            =   "DOOR"
            Top             =   990
            Width           =   645
         End
         Begin VB.TextBox modeRespoince 
            Height          =   315
            Index           =   6
            Left            =   1650
            Locked          =   -1  'True
            TabIndex        =   31
            Text            =   "CHIME"
            Top             =   990
            Width           =   645
         End
      End
      Begin VB.Frame Frame15 
         Caption         =   "Comm. Error"
         Height          =   1575
         Left            =   4380
         TabIndex        =   17
         Top             =   240
         Width           =   2625
         Begin VB.TextBox comError 
            Height          =   315
            Index           =   0
            Left            =   210
            Locked          =   -1  'True
            TabIndex        =   29
            Text            =   "DVA1"
            Top             =   300
            Width           =   555
         End
         Begin VB.TextBox comError 
            Height          =   315
            Index           =   1
            Left            =   780
            Locked          =   -1  'True
            TabIndex        =   28
            Text            =   "DVA2"
            Top             =   300
            Width           =   555
         End
         Begin VB.TextBox comError 
            Height          =   315
            Index           =   2
            Left            =   1380
            Locked          =   -1  'True
            TabIndex        =   27
            Text            =   "MCP1"
            Top             =   300
            Width           =   555
         End
         Begin VB.TextBox comError 
            Height          =   315
            Index           =   3
            Left            =   1920
            Locked          =   -1  'True
            TabIndex        =   26
            Text            =   "MCP2"
            Top             =   300
            Width           =   555
         End
         Begin VB.TextBox comError 
            Height          =   315
            Index           =   4
            Left            =   210
            Locked          =   -1  'True
            TabIndex        =   25
            Text            =   "PAA1"
            Top             =   660
            Width           =   555
         End
         Begin VB.TextBox comError 
            Height          =   315
            Index           =   5
            Left            =   780
            Locked          =   -1  'True
            TabIndex        =   24
            Text            =   "PAA2"
            Top             =   660
            Width           =   555
         End
         Begin VB.TextBox comError 
            Height          =   315
            Index           =   6
            Left            =   1350
            Locked          =   -1  'True
            TabIndex        =   23
            Text            =   "PAA3"
            Top             =   660
            Width           =   555
         End
         Begin VB.TextBox comError 
            Height          =   315
            Index           =   7
            Left            =   1920
            Locked          =   -1  'True
            TabIndex        =   22
            Text            =   "PAA4"
            Top             =   660
            Width           =   555
         End
         Begin VB.TextBox comError 
            Height          =   315
            Index           =   8
            Left            =   210
            Locked          =   -1  'True
            TabIndex        =   21
            Text            =   "PAA5"
            Top             =   990
            Width           =   555
         End
         Begin VB.TextBox comError 
            Height          =   315
            Index           =   9
            Left            =   780
            Locked          =   -1  'True
            TabIndex        =   20
            Text            =   "PAA6"
            Top             =   990
            Width           =   555
         End
         Begin VB.TextBox comError 
            Height          =   315
            Index           =   10
            Left            =   1350
            Locked          =   -1  'True
            TabIndex        =   19
            Text            =   "PAA7"
            Top             =   990
            Width           =   555
         End
         Begin VB.TextBox comError 
            Height          =   315
            Index           =   11
            Left            =   1920
            Locked          =   -1  'True
            TabIndex        =   18
            Text            =   "PAA8"
            Top             =   990
            Width           =   555
         End
      End
      Begin VB.Frame Frame16 
         Caption         =   "TEST"
         Height          =   1455
         Left            =   150
         TabIndex        =   3
         Top             =   240
         Width           =   4125
         Begin VB.TextBox testItemNoResponce 
            Height          =   345
            Left            =   2430
            Locked          =   -1  'True
            TabIndex        =   16
            Text            =   "01"
            Top             =   240
            Width           =   375
         End
         Begin VB.TextBox testCatResponce 
            Height          =   315
            Index           =   0
            Left            =   1470
            Locked          =   -1  'True
            TabIndex        =   15
            Text            =   "CAT1"
            Top             =   240
            Width           =   615
         End
         Begin VB.TextBox testCatResponce 
            Height          =   315
            Index           =   1
            Left            =   810
            Locked          =   -1  'True
            TabIndex        =   14
            Text            =   "CAT2"
            Top             =   240
            Width           =   615
         End
         Begin VB.TextBox testEXECResponce 
            Height          =   315
            Left            =   150
            Locked          =   -1  'True
            TabIndex        =   13
            Text            =   "INIT"
            Top             =   240
            Width           =   615
         End
         Begin VB.TextBox testOKNG 
            Height          =   345
            Left            =   150
            Locked          =   -1  'True
            TabIndex        =   12
            Text            =   "01"
            Top             =   600
            Width           =   375
         End
         Begin VB.TextBox testNG 
            Height          =   345
            Index           =   0
            Left            =   150
            Locked          =   -1  'True
            TabIndex        =   11
            Text            =   "NG1"
            Top             =   990
            Width           =   500
         End
         Begin VB.TextBox testNG 
            Height          =   345
            Index           =   1
            Left            =   630
            Locked          =   -1  'True
            TabIndex        =   10
            Text            =   "NG2"
            Top             =   990
            Width           =   500
         End
         Begin VB.TextBox testNG 
            Height          =   345
            Index           =   2
            Left            =   1110
            Locked          =   -1  'True
            TabIndex        =   9
            Text            =   "NG3"
            Top             =   990
            Width           =   500
         End
         Begin VB.TextBox testNG 
            Height          =   345
            Index           =   3
            Left            =   1590
            Locked          =   -1  'True
            TabIndex        =   8
            Text            =   "NG4"
            Top             =   990
            Width           =   500
         End
         Begin VB.TextBox testNG 
            Height          =   345
            Index           =   4
            Left            =   2070
            Locked          =   -1  'True
            TabIndex        =   7
            Text            =   "NG5"
            Top             =   990
            Width           =   500
         End
         Begin VB.TextBox testNG 
            Height          =   345
            Index           =   5
            Left            =   2550
            Locked          =   -1  'True
            TabIndex        =   6
            Text            =   "NG6"
            Top             =   990
            Width           =   500
         End
         Begin VB.TextBox testNG 
            Height          =   345
            Index           =   6
            Left            =   3030
            Locked          =   -1  'True
            TabIndex        =   5
            Text            =   "NG7"
            Top             =   990
            Width           =   500
         End
         Begin VB.TextBox testNG 
            Height          =   345
            Index           =   7
            Left            =   3510
            Locked          =   -1  'True
            TabIndex        =   4
            Text            =   "NG8"
            Top             =   990
            Width           =   500
         End
      End
      Begin VB.Frame Frame17 
         Caption         =   "Version"
         Height          =   795
         Left            =   150
         TabIndex        =   1
         Top             =   1830
         Width           =   885
         Begin VB.TextBox textVersion 
            Height          =   345
            Left            =   120
            Locked          =   -1  'True
            TabIndex        =   2
            Text            =   "0100"
            Top             =   270
            Width           =   585
         End
      End
   End
End
Attribute VB_Name = "frmSD"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

