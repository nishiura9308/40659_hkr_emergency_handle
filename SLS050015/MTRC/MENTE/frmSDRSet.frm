VERSION 5.00
Begin VB.Form frmSDRSet 
   BorderStyle     =   1  'å≈íË(é¿ê¸)
   Caption         =   "TMS SET"
   ClientHeight    =   8520
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   8220
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
   MDIChild        =   -1  'True
   MinButton       =   0   'False
   ScaleHeight     =   8520
   ScaleWidth      =   8220
   Begin VB.Timer CheckTImer 
      Interval        =   2000
      Left            =   6930
      Top             =   810
   End
   Begin VB.CommandButton cmdClose 
      Caption         =   "Close"
      Height          =   555
      Left            =   7080
      TabIndex        =   0
      Top             =   7920
      Width           =   1065
   End
   Begin VB.Frame SDRFrame 
      Caption         =   "TMS SDR Data Reqeust "
      Height          =   7905
      Left            =   120
      TabIndex        =   1
      Top             =   120
      Width           =   8055
      Begin VB.CheckBox chkSEND 
         Caption         =   "Send Enable"
         Height          =   405
         Left            =   5460
         Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
         TabIndex        =   93
         Top             =   240
         Width           =   1575
      End
      Begin VB.Frame Frame2 
         Caption         =   "TEST"
         Height          =   765
         Left            =   150
         TabIndex        =   39
         Top             =   240
         Width           =   2985
         Begin VB.CheckBox chkCAT2 
            Caption         =   "CAT2"
            Height          =   345
            Left            =   1650
            Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
            TabIndex        =   96
            Top             =   270
            Width           =   645
         End
         Begin VB.CheckBox chkEXEC 
            Caption         =   "EXEC"
            Height          =   345
            Left            =   180
            Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
            TabIndex        =   41
            Top             =   270
            Width           =   645
         End
         Begin VB.CheckBox chkCAT1 
            Caption         =   "CAT1"
            Height          =   345
            Left            =   960
            Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
            TabIndex        =   42
            Top             =   270
            Width           =   645
         End
         Begin VB.TextBox testItemNo 
            Height          =   345
            Left            =   2430
            TabIndex        =   40
            Text            =   "01"
            Top             =   270
            Width           =   375
         End
      End
      Begin VB.Frame Frame3 
         Caption         =   "ActiveFlag"
         Height          =   1905
         Left            =   3360
         TabIndex        =   33
         Top             =   1020
         Width           =   3735
         Begin VB.CheckBox chkCARTYPE 
            Caption         =   "CarType"
            Height          =   345
            Left            =   210
            Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
            TabIndex        =   52
            Top             =   1050
            Width           =   915
         End
         Begin VB.CheckBox chkCARNO 
            Caption         =   "CarNo"
            Height          =   345
            Left            =   210
            Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
            TabIndex        =   51
            Top             =   690
            Width           =   615
         End
         Begin VB.CheckBox chkADJ 
            Caption         =   "ADJ"
            Height          =   345
            Left            =   840
            Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
            TabIndex        =   50
            Top             =   300
            Width           =   615
         End
         Begin VB.CheckBox chkTIME 
            Caption         =   "TIME"
            Height          =   345
            Left            =   210
            Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
            TabIndex        =   49
            Top             =   300
            Width           =   615
         End
         Begin VB.TextBox timeData 
            Height          =   315
            Left            =   1620
            Locked          =   -1  'True
            TabIndex        =   37
            Text            =   "00/08/10 10:00:00"
            Top             =   300
            Width           =   1965
         End
         Begin VB.TextBox carNo 
            Height          =   315
            Left            =   1620
            Locked          =   -1  'True
            TabIndex        =   36
            Text            =   "000"
            Top             =   690
            Width           =   465
         End
         Begin VB.TextBox carType 
            Height          =   315
            Left            =   1620
            Locked          =   -1  'True
            TabIndex        =   35
            Text            =   "0"
            Top             =   1080
            Width           =   465
         End
         Begin VB.TextBox couplingUnit 
            Height          =   330
            Left            =   1620
            Locked          =   -1  'True
            TabIndex        =   34
            Text            =   "0"
            Top             =   1470
            Width           =   435
         End
         Begin VB.Label Label12 
            Caption         =   "CouplingUit"
            Height          =   225
            Left            =   240
            TabIndex        =   38
            Top             =   1530
            Width           =   825
         End
      End
      Begin VB.Frame Frame4 
         Caption         =   "Station Data"
         Height          =   1935
         Left            =   150
         TabIndex        =   21
         Top             =   1020
         Width           =   3165
         Begin VB.TextBox stationNumber 
            Height          =   315
            Left            =   720
            TabIndex        =   27
            Text            =   "01"
            Top             =   480
            Width           =   435
         End
         Begin VB.TextBox LineNumber 
            Height          =   315
            Left            =   120
            TabIndex        =   26
            Text            =   "1"
            Top             =   480
            Width           =   435
         End
         Begin VB.TextBox nextStationName 
            Height          =   315
            Left            =   1440
            TabIndex        =   25
            Text            =   "01"
            Top             =   480
            Width           =   435
         End
         Begin VB.TextBox destinationCode 
            Height          =   315
            Left            =   1980
            TabIndex        =   24
            Text            =   "01"
            Top             =   510
            Width           =   435
         End
         Begin VB.TextBox routeCode 
            Height          =   315
            Left            =   2520
            TabIndex        =   23
            Text            =   "01"
            Top             =   510
            Width           =   435
         End
         Begin VB.Frame Frame7 
            Caption         =   "Broadcast Active"
            Height          =   915
            Left            =   90
            TabIndex        =   22
            Top             =   900
            Width           =   2925
            Begin VB.CheckBox chkOFF 
               Caption         =   "OFF"
               Height          =   345
               Left            =   1440
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   45
               Top             =   360
               Width           =   555
            End
            Begin VB.CheckBox chkPA 
               Caption         =   "BR"
               Height          =   345
               Left            =   780
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   44
               Top             =   360
               Width           =   555
            End
            Begin VB.CheckBox chkBR 
               Caption         =   "ATC"
               Height          =   345
               Left            =   150
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   43
               Top             =   360
               Width           =   555
            End
            Begin VB.TextBox specialCode 
               Height          =   315
               Left            =   2310
               TabIndex        =   94
               Text            =   "00"
               Top             =   450
               Width           =   435
            End
            Begin VB.Label Label6 
               Caption         =   "Special"
               Height          =   285
               Left            =   2130
               TabIndex        =   95
               Top             =   180
               Width           =   735
            End
         End
         Begin VB.Label Label1 
            Caption         =   "Line"
            Height          =   285
            Left            =   120
            TabIndex        =   32
            Top             =   240
            Width           =   555
         End
         Begin VB.Label Label2 
            Caption         =   "Station"
            Height          =   285
            Left            =   690
            TabIndex        =   31
            Top             =   240
            Width           =   555
         End
         Begin VB.Label Label3 
            Caption         =   "Next"
            Height          =   285
            Left            =   1410
            TabIndex        =   30
            Top             =   270
            Width           =   555
         End
         Begin VB.Label Label4 
            Caption         =   "Dest."
            Height          =   285
            Left            =   1950
            TabIndex        =   29
            Top             =   270
            Width           =   555
         End
         Begin VB.Label Label5 
            Caption         =   "Route"
            Height          =   285
            Left            =   2490
            TabIndex        =   28
            Top             =   270
            Width           =   555
         End
      End
      Begin VB.Frame Frame5 
         Caption         =   "Car Info"
         Height          =   3675
         Left            =   150
         TabIndex        =   3
         Top             =   3880
         Width           =   7750
         Begin VB.Frame FrameReset 
            Caption         =   "EAH Reset(Physical layout)"
            Height          =   3315
            Index           =   1
            Left            =   4350
            TabIndex        =   97
            Top             =   210
            Width           =   3315
            Begin VB.CheckBox chkEAHR 
               Caption         =   "V3"
               Height          =   345
               Index           =   2
               Left            =   1320
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   137
               Top             =   270
               Width           =   615
            End
            Begin VB.CheckBox chkEAHR 
               Caption         =   "V4"
               Height          =   345
               Index           =   3
               Left            =   1950
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   136
               Top             =   270
               Width           =   615
            End
            Begin VB.CheckBox chkEAHR 
               Caption         =   "V5"
               Height          =   345
               Index           =   4
               Left            =   2580
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   135
               Top             =   270
               Width           =   615
            End
            Begin VB.CheckBox chkEAHR 
               Caption         =   "Z1"
               Height          =   345
               Index           =   5
               Left            =   60
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   134
               Top             =   630
               Width           =   615
            End
            Begin VB.CheckBox chkEAHR 
               Caption         =   "Z2"
               Height          =   345
               Index           =   6
               Left            =   690
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   133
               Top             =   630
               Width           =   615
            End
            Begin VB.CheckBox chkEAHR 
               Caption         =   "Z3"
               Height          =   345
               Index           =   7
               Left            =   1320
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   132
               Top             =   630
               Width           =   615
            End
            Begin VB.CheckBox chkEAHR 
               Caption         =   "Z4"
               Height          =   345
               Index           =   8
               Left            =   1950
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   131
               Top             =   630
               Width           =   615
            End
            Begin VB.CheckBox chkEAHR 
               Caption         =   "Z5"
               Height          =   345
               Index           =   9
               Left            =   2580
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   130
               Top             =   630
               Width           =   615
            End
            Begin VB.CheckBox chkEAHR 
               Caption         =   "X1"
               Height          =   345
               Index           =   10
               Left            =   60
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   129
               Top             =   990
               Width           =   615
            End
            Begin VB.CheckBox chkEAHR 
               Caption         =   "X2"
               Height          =   345
               Index           =   11
               Left            =   690
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   128
               Top             =   990
               Width           =   615
            End
            Begin VB.CheckBox chkEAHR 
               Caption         =   "X3"
               Height          =   345
               Index           =   12
               Left            =   1320
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   127
               Top             =   990
               Width           =   615
            End
            Begin VB.CheckBox chkEAHR 
               Caption         =   "X4"
               Height          =   345
               Index           =   13
               Left            =   1950
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   126
               Top             =   990
               Width           =   615
            End
            Begin VB.CheckBox chkEAHR 
               Caption         =   "X5"
               Height          =   345
               Index           =   14
               Left            =   2580
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   125
               Top             =   990
               Width           =   615
            End
            Begin VB.CheckBox chkEAHR 
               Caption         =   "W1"
               Height          =   345
               Index           =   15
               Left            =   60
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   124
               Top             =   1350
               Width           =   615
            End
            Begin VB.CheckBox chkEAHR 
               Caption         =   "W2"
               Height          =   345
               Index           =   16
               Left            =   690
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   123
               Top             =   1350
               Width           =   615
            End
            Begin VB.CheckBox chkEAHR 
               Caption         =   "W3"
               Height          =   345
               Index           =   17
               Left            =   1320
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   122
               Top             =   1350
               Width           =   615
            End
            Begin VB.CheckBox chkEAHR 
               Caption         =   "W4"
               Height          =   345
               Index           =   18
               Left            =   1950
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   121
               Top             =   1350
               Width           =   615
            End
            Begin VB.CheckBox chkEAHR 
               Caption         =   "W5"
               Height          =   345
               Index           =   19
               Left            =   2580
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   120
               Top             =   1350
               Width           =   615
            End
            Begin VB.CheckBox chkEAHR 
               Caption         =   "Y1"
               Height          =   345
               Index           =   20
               Left            =   60
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   119
               Top             =   1710
               Width           =   615
            End
            Begin VB.CheckBox chkEAHR 
               Caption         =   "Y2"
               Height          =   345
               Index           =   21
               Left            =   690
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   118
               Top             =   1710
               Width           =   615
            End
            Begin VB.CheckBox chkEAHR 
               Caption         =   "Y3"
               Height          =   345
               Index           =   22
               Left            =   1320
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   117
               Top             =   1710
               Width           =   615
            End
            Begin VB.CheckBox chkEAHR 
               Caption         =   "Y4"
               Height          =   345
               Index           =   23
               Left            =   1950
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   116
               Top             =   1680
               Width           =   615
            End
            Begin VB.CheckBox chkEAHR 
               Caption         =   "Y5"
               Height          =   345
               Index           =   24
               Left            =   2580
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   115
               Top             =   1710
               Width           =   615
            End
            Begin VB.CheckBox chkEAHR 
               Caption         =   "X1"
               Height          =   345
               Index           =   25
               Left            =   60
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   114
               Top             =   2070
               Width           =   615
            End
            Begin VB.CheckBox chkEAHR 
               Caption         =   "X2"
               Height          =   345
               Index           =   26
               Left            =   690
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   113
               Top             =   2070
               Width           =   615
            End
            Begin VB.CheckBox chkEAHR 
               Caption         =   "X3"
               Height          =   345
               Index           =   27
               Left            =   1320
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   112
               Top             =   2070
               Width           =   615
            End
            Begin VB.CheckBox chkEAHR 
               Caption         =   "X4"
               Height          =   345
               Index           =   28
               Left            =   1950
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   111
               Top             =   2040
               Width           =   615
            End
            Begin VB.CheckBox chkEAHR 
               Caption         =   "X5"
               Height          =   345
               Index           =   29
               Left            =   2580
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   110
               Top             =   2070
               Width           =   615
            End
            Begin VB.CheckBox chkEAHR 
               Caption         =   "Z1"
               Height          =   345
               Index           =   30
               Left            =   60
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   109
               Top             =   2430
               Width           =   615
            End
            Begin VB.CheckBox chkEAHR 
               Caption         =   "Z2"
               Height          =   345
               Index           =   31
               Left            =   690
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   108
               Top             =   2430
               Width           =   615
            End
            Begin VB.CheckBox chkEAHR 
               Caption         =   "Z3"
               Height          =   345
               Index           =   32
               Left            =   1320
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   107
               Top             =   2430
               Width           =   615
            End
            Begin VB.CheckBox chkEAHR 
               Caption         =   "Z4"
               Height          =   345
               Index           =   33
               Left            =   1950
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   106
               Top             =   2430
               Width           =   615
            End
            Begin VB.CheckBox chkEAHR 
               Caption         =   "Z5"
               Height          =   345
               Index           =   34
               Left            =   2580
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   105
               Top             =   2430
               Width           =   615
            End
            Begin VB.CheckBox chkEAHR 
               Caption         =   "V1"
               Height          =   345
               Index           =   35
               Left            =   60
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   104
               Top             =   2790
               Width           =   615
            End
            Begin VB.CheckBox chkEAHR 
               Caption         =   "V2"
               Height          =   345
               Index           =   36
               Left            =   690
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   103
               Top             =   2790
               Width           =   615
            End
            Begin VB.CheckBox chkEAHR 
               Caption         =   "V3"
               Height          =   345
               Index           =   37
               Left            =   1320
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   102
               Top             =   2790
               Width           =   615
            End
            Begin VB.CheckBox chkEAHR 
               Caption         =   "V4"
               Height          =   345
               Index           =   38
               Left            =   1950
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   101
               Top             =   2790
               Width           =   615
            End
            Begin VB.CheckBox chkEAHR 
               Caption         =   "V5"
               Height          =   345
               Index           =   39
               Left            =   2580
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   100
               Top             =   2790
               Width           =   615
            End
            Begin VB.CheckBox chkEAHR 
               Caption         =   "V1"
               Height          =   345
               Index           =   0
               Left            =   60
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   99
               Top             =   270
               Width           =   615
            End
            Begin VB.CheckBox chkEAHR 
               Caption         =   "V2"
               Height          =   345
               Index           =   1
               Left            =   690
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   98
               Top             =   270
               Width           =   615
            End
         End
         Begin VB.TextBox CarNo2 
            Height          =   330
            Index           =   0
            Left            =   120
            TabIndex        =   20
            Text            =   "000"
            Top             =   540
            Width           =   385
         End
         Begin VB.Frame FrameTalkback 
            Caption         =   "EAH Talkback(Physical layout)"
            Height          =   3315
            Index           =   0
            Left            =   1010
            TabIndex        =   19
            Top             =   210
            Width           =   3315
            Begin VB.CheckBox chkEAH 
               Caption         =   "V2"
               Height          =   345
               Index           =   1
               Left            =   690
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   54
               Top             =   270
               Width           =   615
            End
            Begin VB.CheckBox chkEAH 
               Caption         =   "V1"
               Height          =   345
               Index           =   0
               Left            =   60
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   53
               Top             =   270
               Width           =   615
            End
            Begin VB.CheckBox chkEAH 
               Caption         =   "V5"
               Height          =   345
               Index           =   39
               Left            =   2580
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   92
               Top             =   2790
               Width           =   615
            End
            Begin VB.CheckBox chkEAH 
               Caption         =   "V4"
               Height          =   345
               Index           =   38
               Left            =   1950
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   91
               Top             =   2790
               Width           =   615
            End
            Begin VB.CheckBox chkEAH 
               Caption         =   "V3"
               Height          =   345
               Index           =   37
               Left            =   1320
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   90
               Top             =   2790
               Width           =   615
            End
            Begin VB.CheckBox chkEAH 
               Caption         =   "V2"
               Height          =   345
               Index           =   36
               Left            =   690
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   89
               Top             =   2790
               Width           =   615
            End
            Begin VB.CheckBox chkEAH 
               Caption         =   "V1"
               Height          =   345
               Index           =   35
               Left            =   60
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   88
               Top             =   2790
               Width           =   615
            End
            Begin VB.CheckBox chkEAH 
               Caption         =   "Z5"
               Height          =   345
               Index           =   34
               Left            =   2580
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   87
               Top             =   2430
               Width           =   615
            End
            Begin VB.CheckBox chkEAH 
               Caption         =   "Z4"
               Height          =   345
               Index           =   33
               Left            =   1950
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   86
               Top             =   2430
               Width           =   615
            End
            Begin VB.CheckBox chkEAH 
               Caption         =   "Z3"
               Height          =   345
               Index           =   32
               Left            =   1320
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   85
               Top             =   2430
               Width           =   615
            End
            Begin VB.CheckBox chkEAH 
               Caption         =   "Z2"
               Height          =   345
               Index           =   31
               Left            =   690
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   84
               Top             =   2430
               Width           =   615
            End
            Begin VB.CheckBox chkEAH 
               Caption         =   "Z1"
               Height          =   345
               Index           =   30
               Left            =   60
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   83
               Top             =   2430
               Width           =   615
            End
            Begin VB.CheckBox chkEAH 
               Caption         =   "X5"
               Height          =   345
               Index           =   29
               Left            =   2580
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   82
               Top             =   2070
               Width           =   615
            End
            Begin VB.CheckBox chkEAH 
               Caption         =   "X4"
               Height          =   345
               Index           =   28
               Left            =   1950
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   81
               Top             =   2070
               Width           =   615
            End
            Begin VB.CheckBox chkEAH 
               Caption         =   "X3"
               Height          =   345
               Index           =   27
               Left            =   1320
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   80
               Top             =   2070
               Width           =   615
            End
            Begin VB.CheckBox chkEAH 
               Caption         =   "X2"
               Height          =   345
               Index           =   26
               Left            =   690
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   79
               Top             =   2070
               Width           =   615
            End
            Begin VB.CheckBox chkEAH 
               Caption         =   "X1"
               Height          =   345
               Index           =   25
               Left            =   60
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   78
               Top             =   2070
               Width           =   615
            End
            Begin VB.CheckBox chkEAH 
               Caption         =   "Y5"
               Height          =   345
               Index           =   24
               Left            =   2580
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   77
               Top             =   1710
               Width           =   615
            End
            Begin VB.CheckBox chkEAH 
               Caption         =   "Y4"
               Height          =   345
               Index           =   23
               Left            =   1950
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   76
               Top             =   1710
               Width           =   615
            End
            Begin VB.CheckBox chkEAH 
               Caption         =   "Y3"
               Height          =   345
               Index           =   22
               Left            =   1320
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   75
               Top             =   1710
               Width           =   615
            End
            Begin VB.CheckBox chkEAH 
               Caption         =   "Y2"
               Height          =   345
               Index           =   21
               Left            =   690
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   74
               Top             =   1710
               Width           =   615
            End
            Begin VB.CheckBox chkEAH 
               Caption         =   "Y1"
               Height          =   345
               Index           =   20
               Left            =   60
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   73
               Top             =   1710
               Width           =   615
            End
            Begin VB.CheckBox chkEAH 
               Caption         =   "W5"
               Height          =   345
               Index           =   19
               Left            =   2580
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   72
               Top             =   1350
               Width           =   615
            End
            Begin VB.CheckBox chkEAH 
               Caption         =   "W4"
               Height          =   345
               Index           =   18
               Left            =   1950
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   71
               Top             =   1350
               Width           =   615
            End
            Begin VB.CheckBox chkEAH 
               Caption         =   "W3"
               Height          =   345
               Index           =   17
               Left            =   1320
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   70
               Top             =   1350
               Width           =   615
            End
            Begin VB.CheckBox chkEAH 
               Caption         =   "W2"
               Height          =   345
               Index           =   16
               Left            =   690
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   69
               Top             =   1350
               Width           =   615
            End
            Begin VB.CheckBox chkEAH 
               Caption         =   "W1"
               Height          =   345
               Index           =   15
               Left            =   60
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   68
               Top             =   1350
               Width           =   615
            End
            Begin VB.CheckBox chkEAH 
               Caption         =   "X5"
               Height          =   345
               Index           =   14
               Left            =   2580
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   67
               Top             =   990
               Width           =   615
            End
            Begin VB.CheckBox chkEAH 
               Caption         =   "X4"
               Height          =   345
               Index           =   13
               Left            =   1950
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   66
               Top             =   990
               Width           =   615
            End
            Begin VB.CheckBox chkEAH 
               Caption         =   "X3"
               Height          =   345
               Index           =   12
               Left            =   1320
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   65
               Top             =   990
               Width           =   615
            End
            Begin VB.CheckBox chkEAH 
               Caption         =   "X2"
               Height          =   345
               Index           =   11
               Left            =   690
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   64
               Top             =   990
               Width           =   615
            End
            Begin VB.CheckBox chkEAH 
               Caption         =   "X1"
               Height          =   345
               Index           =   10
               Left            =   60
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   63
               Top             =   990
               Width           =   615
            End
            Begin VB.CheckBox chkEAH 
               Caption         =   "Z5"
               Height          =   345
               Index           =   9
               Left            =   2580
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   62
               Top             =   630
               Width           =   615
            End
            Begin VB.CheckBox chkEAH 
               Caption         =   "Z4"
               Height          =   345
               Index           =   8
               Left            =   1950
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   61
               Top             =   630
               Width           =   615
            End
            Begin VB.CheckBox chkEAH 
               Caption         =   "Z3"
               Height          =   345
               Index           =   7
               Left            =   1320
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   60
               Top             =   630
               Width           =   615
            End
            Begin VB.CheckBox chkEAH 
               Caption         =   "Z2"
               Height          =   345
               Index           =   6
               Left            =   690
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   59
               Top             =   630
               Width           =   615
            End
            Begin VB.CheckBox chkEAH 
               Caption         =   "Z1"
               Height          =   345
               Index           =   5
               Left            =   60
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   58
               Top             =   630
               Width           =   615
            End
            Begin VB.CheckBox chkEAH 
               Caption         =   "V5"
               Height          =   345
               Index           =   4
               Left            =   2580
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   57
               Top             =   270
               Width           =   615
            End
            Begin VB.CheckBox chkEAH 
               Caption         =   "V4"
               Height          =   345
               Index           =   3
               Left            =   1950
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   56
               Top             =   270
               Width           =   615
            End
            Begin VB.CheckBox chkEAH 
               Caption         =   "V3"
               Height          =   345
               Index           =   2
               Left            =   1320
               Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
               TabIndex        =   55
               Top             =   270
               Width           =   615
            End
         End
         Begin VB.TextBox CarNo2 
            Height          =   330
            Index           =   1
            Left            =   120
            TabIndex        =   18
            Text            =   "000"
            Top             =   885
            Width           =   385
         End
         Begin VB.TextBox CarNo2 
            Height          =   330
            Index           =   2
            Left            =   120
            TabIndex        =   17
            Text            =   "000"
            Top             =   1215
            Width           =   385
         End
         Begin VB.TextBox CarNo2 
            Height          =   330
            Index           =   3
            Left            =   120
            TabIndex        =   16
            Text            =   "000"
            Top             =   1560
            Width           =   385
         End
         Begin VB.TextBox CarNo2 
            Height          =   330
            Index           =   4
            Left            =   120
            TabIndex        =   15
            Text            =   "000"
            Top             =   1905
            Width           =   385
         End
         Begin VB.TextBox CarNo2 
            Height          =   330
            Index           =   5
            Left            =   120
            TabIndex        =   14
            Text            =   "000"
            Top             =   2235
            Width           =   385
         End
         Begin VB.TextBox CarNo2 
            Height          =   330
            Index           =   6
            Left            =   120
            TabIndex        =   13
            Text            =   "000"
            Top             =   2580
            Width           =   385
         End
         Begin VB.TextBox CarNo2 
            Height          =   330
            Index           =   7
            Left            =   120
            TabIndex        =   12
            Text            =   "000"
            Top             =   2940
            Width           =   385
         End
         Begin VB.TextBox carType2 
            Height          =   330
            Index           =   0
            Left            =   550
            TabIndex        =   11
            Text            =   "0"
            Top             =   540
            Width           =   385
         End
         Begin VB.TextBox carType2 
            Height          =   330
            Index           =   1
            Left            =   550
            TabIndex        =   10
            Text            =   "0"
            Top             =   885
            Width           =   385
         End
         Begin VB.TextBox carType2 
            Height          =   330
            Index           =   2
            Left            =   550
            TabIndex        =   9
            Text            =   "0"
            Top             =   1215
            Width           =   385
         End
         Begin VB.TextBox carType2 
            Height          =   330
            Index           =   3
            Left            =   550
            TabIndex        =   8
            Text            =   "0"
            Top             =   1560
            Width           =   385
         End
         Begin VB.TextBox carType2 
            Height          =   330
            Index           =   4
            Left            =   550
            TabIndex        =   7
            Text            =   "0"
            Top             =   1905
            Width           =   385
         End
         Begin VB.TextBox carType2 
            Height          =   330
            Index           =   5
            Left            =   550
            TabIndex        =   6
            Text            =   "0"
            Top             =   2235
            Width           =   385
         End
         Begin VB.TextBox carType2 
            Height          =   330
            Index           =   6
            Left            =   550
            TabIndex        =   5
            Text            =   "0"
            Top             =   2580
            Width           =   385
         End
         Begin VB.TextBox carType2 
            Height          =   330
            Index           =   7
            Left            =   550
            TabIndex        =   4
            Text            =   "0"
            Top             =   2940
            Width           =   385
         End
      End
      Begin VB.Frame Frame8 
         Caption         =   "Open Active"
         Height          =   915
         Left            =   150
         TabIndex        =   2
         Top             =   2950
         Width           =   2385
         Begin VB.CheckBox chkDOOR 
            Caption         =   "DOOR"
            Height          =   345
            Left            =   1590
            Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
            TabIndex        =   48
            Top             =   360
            Width           =   615
         End
         Begin VB.CheckBox chkRIGHT 
            Caption         =   "RIGHT"
            Height          =   345
            Left            =   810
            Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
            TabIndex        =   47
            Top             =   360
            Width           =   675
         End
         Begin VB.CheckBox chkLEFT 
            Caption         =   "LEFT"
            Height          =   345
            Left            =   90
            Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
            TabIndex        =   46
            Top             =   360
            Width           =   615
         End
      End
   End
End
Attribute VB_Name = "frmSDRSet"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Dim g_send_sdr As String
Dim phase As Integer

Public Sub DoModeless()

    If MDIMain.MenteCOMM.OnLine = False Then
        MsgBox "Not Online"
'        Exit Sub
    End If

    MDIMain.MenteCOMM.SDRMonitor True
    MDIMain.MenteCOMM.STARTRequest

    frmSDR.Show vbModeless
    frmSDRSet.Show vbModeless

End Sub

Private Sub CheckTimer_Timer()
    Dim i As Integer

    CheckTimer.Enabled = False

    Select Case phase
    
    Case 0
        MDIMain.MenteCOMM.SDRMonitor False
        If MDIMain.MenteCOMM.STARTRequest = True Then
            phase = phase + 1
        End If
        CheckTimer.Interval = 200
        CheckTimer.Enabled = True
        Exit Sub

    Case 1
        g_send_sdr = "SDR1"
        If chkEXEC.Value = 1 Then
            g_send_sdr = g_send_sdr + "EXEC"
        Else
            g_send_sdr = g_send_sdr + "    "
        End If
        If chkCAT1.Value = 1 Then
            g_send_sdr = g_send_sdr + "CAT1"
        Else
            g_send_sdr = g_send_sdr + "    "
        End If
        If chkCAT2.Value = 1 Then
            g_send_sdr = g_send_sdr + "CAT2"
        Else
            g_send_sdr = g_send_sdr + "    "
        End If
        g_send_sdr = g_send_sdr + Right$("00" + testItemNo.Text, 2)
    
    Case 2
        g_send_sdr = "SDR3"
        g_send_sdr = g_send_sdr & Right$(String$(17, "0") + timeData.Text, 17)

    Case 3
        g_send_sdr = "SDR2"
        If chkTIME.Value = 1 Then
            g_send_sdr = g_send_sdr + "TIME"
        Else
            g_send_sdr = g_send_sdr + "    "
        End If
        If chkADJ.Value = 1 Then
            g_send_sdr = g_send_sdr + "ADJ "
        Else
            g_send_sdr = g_send_sdr + "    "
        End If
        If chkCARNO.Value = 1 Then
            g_send_sdr = g_send_sdr + "CARNO"
        Else
            g_send_sdr = g_send_sdr + "     "
        End If
        If chkCARTYPE.Value = 1 Then
            g_send_sdr = g_send_sdr + "CARTYPE"
        Else
            g_send_sdr = g_send_sdr + "       "
        End If
        
    Case 4
        g_send_sdr = "SDR4"
        g_send_sdr = g_send_sdr & Right$(String$(3, "0") + carNo.Text, 3)
        g_send_sdr = g_send_sdr & Right$(String$(1, "0") + carType.Text, 1)

    Case 5
        g_send_sdr = "SDR5"
        g_send_sdr = g_send_sdr & Right$(String$(1, "0") + couplingUnit.Text, 1)
    
    Case 6
        g_send_sdr = "SDR6"
        g_send_sdr = g_send_sdr & Right$(String$(2, "0") + stationNumber.Text, 2)
        g_send_sdr = g_send_sdr & Right$(String$(1, "0") + LineNumber.Text, 1)

    Case 7
        g_send_sdr = "SDR7"
        g_send_sdr = g_send_sdr & Right$(String$(2, "0") + nextStationName.Text, 2)
        g_send_sdr = g_send_sdr & Right$(String$(2, "0") + destinationCode.Text, 2)
        g_send_sdr = g_send_sdr & Right$(String$(2, "0") + routeCode.Text, 2)
        g_send_sdr = g_send_sdr & Right$(String$(2, "0") + specialCode.Text, 2)

    Case 8
        g_send_sdr = "SDR8"
        For i = 0 To 39
            If chkEAH(i).Value = 1 Then
                g_send_sdr = g_send_sdr & Left$(chkEAH(i).Caption + Space$(4), 4)
            Else
                g_send_sdr = g_send_sdr & Space$(4)
            End If
        Next i
        
    Case 9
        g_send_sdr = "SDR9"
        If chkBR.Value = 1 Then
            g_send_sdr = g_send_sdr + "PA"
        Else
            g_send_sdr = g_send_sdr + "  "
        End If
        If chkPA.Value = 1 Then
            g_send_sdr = g_send_sdr + "DB"
        Else
            g_send_sdr = g_send_sdr + "  "
        End If

    Case 10
        g_send_sdr = "SDRA"
        If chkLEFT.Value = 1 Then
            g_send_sdr = g_send_sdr + "L"
        Else
            g_send_sdr = g_send_sdr + " "
        End If
        If chkRIGHT.Value = 1 Then
            g_send_sdr = g_send_sdr + "R"
        Else
            g_send_sdr = g_send_sdr + " "
        End If
        If chkDOOR.Value = 1 Then
            g_send_sdr = g_send_sdr + "DOOR"
        Else
            g_send_sdr = g_send_sdr + "    "
        End If
        If chkOFF.Value = 1 Then
            g_send_sdr = g_send_sdr + "OFF"
        Else
            g_send_sdr = g_send_sdr + "   "
        End If
    
    Case 11
        g_send_sdr = "SDRB"
        For i = 0 To 7
            g_send_sdr = g_send_sdr & Right$(String$(3, "0") + CarNo2(i).Text, 3)
            g_send_sdr = g_send_sdr & Right$(String$(1, "0") + carType2(i).Text, 1)
        Next i
        
    Case 12
        g_send_sdr = "SDRC"
        For i = 0 To 39
            If chkEAHR(i).Value = 1 Then
                g_send_sdr = g_send_sdr & Left$(chkEAHR(i).Caption + Space$(4), 4)
            Else
                g_send_sdr = g_send_sdr & Space$(4)
            End If
        Next i
        

    Case 13
        If MDIMain.MenteCOMM.SDRRequest = True Then
            phase = phase + 1
        End If
        CheckTimer.Interval = 1000
        CheckTimer.Enabled = True
        Exit Sub

    Case 14
        If MDIMain.MenteCOMM.SDRequest = True Then
            phase = phase + 1
        End If
        CheckTimer.Enabled = True
        Exit Sub

    Case Else
        MDIMain.MenteCOMM.SDRMonitor True
        CheckTimer.Interval = 2000
        CheckTimer.Enabled = True
        phase = 0
        Exit Sub
    End Select

    If MDIMain.MenteCOMM.SetRequest(g_send_sdr) = True Then
        phase = phase + 1
    End If
    CheckTimer.Enabled = True

End Sub

Private Sub chkSEND_Click()

    If chkSEND.Value = 0 Then
        CheckTimer.Enabled = False
        MDIMain.MenteCOMM.SDRMonitor True
    Else
        CheckTimer.Enabled = True
        MDIMain.MenteCOMM.SDRMonitor False
    End If

End Sub

Private Sub cmdClose_Click()
    
    Unload Me

End Sub

Private Sub Form_Load()

    timeData.Text = Format$(Now, "dd/mm/yy hh:mm:ss")

    CheckTimer.Enabled = False

End Sub

Private Sub Form_Unload(Cancel As Integer)

    MDIMain.MenteCOMM.OffLine
    Unload frmSDR

End Sub

