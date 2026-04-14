VERSION 5.00
Begin VB.Form frmSDR 
   BorderStyle     =   1  'å≈íË(é¿ê¸)
   Caption         =   "TMS SDR monitor"
   ClientHeight    =   8595
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   8295
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
   ScaleHeight     =   8595
   ScaleWidth      =   8295
   Begin VB.CommandButton cmdClose 
      Caption         =   "Close"
      Height          =   555
      Left            =   7200
      TabIndex        =   97
      Top             =   8040
      Width           =   1065
   End
   Begin VB.Frame TBLFrame 
      Caption         =   "Trouble Information"
      Height          =   3915
      Left            =   240
      TabIndex        =   184
      Top             =   120
      Width           =   7995
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   0
         Left            =   720
         Locked          =   -1  'True
         TabIndex        =   188
         Text            =   "24VSIG"
         Top             =   450
         Width           =   990
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   1
         Left            =   1740
         Locked          =   -1  'True
         TabIndex        =   187
         Text            =   "SIGNAL"
         Top             =   450
         Width           =   990
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   2
         Left            =   2760
         Locked          =   -1  'True
         TabIndex        =   244
         Text            =   "EV"
         Top             =   450
         Width           =   990
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   4
         Left            =   720
         Locked          =   -1  'True
         TabIndex        =   186
         Text            =   "AMP1"
         Top             =   1020
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   5
         Left            =   1560
         Locked          =   -1  'True
         TabIndex        =   185
         Text            =   "AMP2"
         Top             =   1020
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   6
         Left            =   2400
         Locked          =   -1  'True
         TabIndex        =   190
         Text            =   "SP1"
         Top             =   1020
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   7
         Left            =   3240
         Locked          =   -1  'True
         TabIndex        =   191
         Text            =   "SP2"
         Top             =   1020
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   8
         Left            =   4110
         Locked          =   -1  'True
         TabIndex        =   192
         Text            =   "SP3"
         Top             =   1020
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   9
         Left            =   4980
         Locked          =   -1  'True
         TabIndex        =   193
         Text            =   "SP4"
         Top             =   1020
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   10
         Left            =   5850
         Locked          =   -1  'True
         TabIndex        =   194
         Text            =   "SP5"
         Top             =   1020
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   11
         Left            =   1560
         Locked          =   -1  'True
         TabIndex        =   195
         Text            =   "AMP2"
         Top             =   1350
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   12
         Left            =   720
         Locked          =   -1  'True
         TabIndex        =   196
         Text            =   "AMP1"
         Top             =   1350
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   13
         Left            =   2400
         Locked          =   -1  'True
         TabIndex        =   197
         Text            =   "SP1"
         Top             =   1350
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   14
         Left            =   3240
         Locked          =   -1  'True
         TabIndex        =   198
         Text            =   "SP2"
         Top             =   1350
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   15
         Left            =   4110
         Locked          =   -1  'True
         TabIndex        =   199
         Text            =   "SP3"
         Top             =   1350
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   16
         Left            =   4980
         Locked          =   -1  'True
         TabIndex        =   200
         Text            =   "SP4"
         Top             =   1350
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   17
         Left            =   5850
         Locked          =   -1  'True
         TabIndex        =   201
         Text            =   "SP5"
         Top             =   1350
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   18
         Left            =   720
         Locked          =   -1  'True
         TabIndex        =   202
         Text            =   "AMP1"
         Top             =   1680
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   19
         Left            =   1560
         Locked          =   -1  'True
         TabIndex        =   203
         Text            =   "AMP2"
         Top             =   1680
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   20
         Left            =   2400
         Locked          =   -1  'True
         TabIndex        =   204
         Text            =   "SP1"
         Top             =   1680
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   21
         Left            =   3240
         Locked          =   -1  'True
         TabIndex        =   205
         Text            =   "SP2"
         Top             =   1680
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   22
         Left            =   4110
         Locked          =   -1  'True
         TabIndex        =   206
         Text            =   "SP3"
         Top             =   1680
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   23
         Left            =   4980
         Locked          =   -1  'True
         TabIndex        =   207
         Text            =   "SP4"
         Top             =   1680
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   24
         Left            =   5850
         Locked          =   -1  'True
         TabIndex        =   208
         Text            =   "SP5"
         Top             =   1680
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   59
         Left            =   5850
         Locked          =   -1  'True
         TabIndex        =   243
         Text            =   "SP5"
         Top             =   3330
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   58
         Left            =   4980
         Locked          =   -1  'True
         TabIndex        =   242
         Text            =   "SP4"
         Top             =   3330
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   57
         Left            =   4110
         Locked          =   -1  'True
         TabIndex        =   241
         Text            =   "SP3"
         Top             =   3330
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   56
         Left            =   3240
         Locked          =   -1  'True
         TabIndex        =   240
         Text            =   "SP2"
         Top             =   3330
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   55
         Left            =   2400
         Locked          =   -1  'True
         TabIndex        =   239
         Text            =   "SP1"
         Top             =   3330
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   54
         Left            =   1560
         Locked          =   -1  'True
         TabIndex        =   238
         Text            =   "AMP2"
         Top             =   3330
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   53
         Left            =   720
         Locked          =   -1  'True
         TabIndex        =   237
         Text            =   "AMP1"
         Top             =   3330
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   52
         Left            =   5850
         Locked          =   -1  'True
         TabIndex        =   236
         Text            =   "SP5"
         Top             =   3000
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   51
         Left            =   4980
         Locked          =   -1  'True
         TabIndex        =   235
         Text            =   "SP4"
         Top             =   3000
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   50
         Left            =   4110
         Locked          =   -1  'True
         TabIndex        =   234
         Text            =   "SP3"
         Top             =   3000
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   49
         Left            =   3240
         Locked          =   -1  'True
         TabIndex        =   233
         Text            =   "SP2"
         Top             =   3000
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   48
         Left            =   2400
         Locked          =   -1  'True
         TabIndex        =   232
         Text            =   "SP1"
         Top             =   3000
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   47
         Left            =   1560
         Locked          =   -1  'True
         TabIndex        =   231
         Text            =   "AMP2"
         Top             =   3000
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   46
         Left            =   720
         Locked          =   -1  'True
         TabIndex        =   230
         Text            =   "AMP1"
         Top             =   3000
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   45
         Left            =   5850
         Locked          =   -1  'True
         TabIndex        =   229
         Text            =   "SP5"
         Top             =   2670
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   44
         Left            =   4980
         Locked          =   -1  'True
         TabIndex        =   228
         Text            =   "SP4"
         Top             =   2670
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   43
         Left            =   4110
         Locked          =   -1  'True
         TabIndex        =   227
         Text            =   "SP3"
         Top             =   2670
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   42
         Left            =   3240
         Locked          =   -1  'True
         TabIndex        =   226
         Text            =   "SP2"
         Top             =   2670
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   41
         Left            =   2400
         Locked          =   -1  'True
         TabIndex        =   225
         Text            =   "SP1"
         Top             =   2670
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   40
         Left            =   1560
         Locked          =   -1  'True
         TabIndex        =   224
         Text            =   "AMP2"
         Top             =   2670
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   39
         Left            =   720
         Locked          =   -1  'True
         TabIndex        =   223
         Text            =   "AMP1"
         Top             =   2670
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   38
         Left            =   5850
         Locked          =   -1  'True
         TabIndex        =   222
         Text            =   "SP5"
         Top             =   2340
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   37
         Left            =   4980
         Locked          =   -1  'True
         TabIndex        =   221
         Text            =   "SP4"
         Top             =   2340
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   36
         Left            =   4110
         Locked          =   -1  'True
         TabIndex        =   220
         Text            =   "SP3"
         Top             =   2340
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   35
         Left            =   3240
         Locked          =   -1  'True
         TabIndex        =   219
         Text            =   "SP2"
         Top             =   2340
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   34
         Left            =   2400
         Locked          =   -1  'True
         TabIndex        =   218
         Text            =   "SP1"
         Top             =   2340
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   33
         Left            =   1560
         Locked          =   -1  'True
         TabIndex        =   217
         Text            =   "AMP2"
         Top             =   2340
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   32
         Left            =   720
         Locked          =   -1  'True
         TabIndex        =   216
         Text            =   "AMP1"
         Top             =   2340
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   31
         Left            =   5850
         Locked          =   -1  'True
         TabIndex        =   215
         Text            =   "SP5"
         Top             =   2010
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   30
         Left            =   4980
         Locked          =   -1  'True
         TabIndex        =   214
         Text            =   "SP4"
         Top             =   2010
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   29
         Left            =   4110
         Locked          =   -1  'True
         TabIndex        =   213
         Text            =   "SP3"
         Top             =   2010
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   28
         Left            =   3240
         Locked          =   -1  'True
         TabIndex        =   212
         Text            =   "SP2"
         Top             =   2010
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   27
         Left            =   2400
         Locked          =   -1  'True
         TabIndex        =   211
         Text            =   "SP1"
         Top             =   2010
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   26
         Left            =   1560
         Locked          =   -1  'True
         TabIndex        =   210
         Text            =   "AMP2"
         Top             =   2010
         Width           =   850
      End
      Begin VB.TextBox Trouble 
         Enabled         =   0   'False
         Height          =   330
         Index           =   25
         Left            =   720
         Locked          =   -1  'True
         TabIndex        =   209
         Text            =   "AMP1"
         Top             =   2010
         Width           =   850
      End
      Begin VB.CommandButton Command1 
         Caption         =   "SDR Monitor"
         Height          =   555
         Left            =   6360
         TabIndex        =   189
         Top             =   360
         Width           =   1455
      End
      Begin VB.Label Label14 
         Caption         =   "DVAU"
         Height          =   345
         Index           =   8
         Left            =   150
         TabIndex        =   253
         Top             =   480
         Width           =   645
      End
      Begin VB.Label Label14 
         Caption         =   "PAA8"
         Height          =   345
         Index           =   7
         Left            =   120
         TabIndex        =   252
         Top             =   3330
         Width           =   645
      End
      Begin VB.Label Label14 
         Caption         =   "PAA7"
         Height          =   345
         Index           =   6
         Left            =   120
         TabIndex        =   251
         Top             =   3000
         Width           =   645
      End
      Begin VB.Label Label14 
         Caption         =   "PAA6"
         Height          =   345
         Index           =   5
         Left            =   120
         TabIndex        =   250
         Top             =   2670
         Width           =   645
      End
      Begin VB.Label Label14 
         Caption         =   "PAA5"
         Height          =   345
         Index           =   4
         Left            =   120
         TabIndex        =   249
         Top             =   2340
         Width           =   645
      End
      Begin VB.Label Label14 
         Caption         =   "PAA4"
         Height          =   345
         Index           =   3
         Left            =   120
         TabIndex        =   248
         Top             =   2010
         Width           =   645
      End
      Begin VB.Label Label14 
         Caption         =   "PAA3"
         Height          =   345
         Index           =   2
         Left            =   120
         TabIndex        =   247
         Top             =   1710
         Width           =   645
      End
      Begin VB.Label Label14 
         Caption         =   "PAA2"
         Height          =   345
         Index           =   1
         Left            =   120
         TabIndex        =   246
         Top             =   1380
         Width           =   645
      End
      Begin VB.Label Label14 
         Caption         =   "PAA1"
         Height          =   345
         Index           =   0
         Left            =   120
         TabIndex        =   245
         Top             =   1050
         Width           =   645
      End
   End
   Begin VB.Frame SDFrame 
      Caption         =   "TMS SD Monitor"
      Height          =   5775
      Left            =   240
      TabIndex        =   98
      Top             =   120
      Width           =   7995
      Begin VB.Frame Frame17 
         Caption         =   "Version"
         Height          =   1335
         Left            =   150
         TabIndex        =   159
         Top             =   1830
         Width           =   885
         Begin VB.TextBox txtCRC 
            Height          =   315
            Left            =   120
            Locked          =   -1  'True
            TabIndex        =   254
            Text            =   "0000"
            Top             =   870
            Width           =   645
         End
         Begin VB.TextBox textVersion 
            Height          =   345
            Left            =   120
            Locked          =   -1  'True
            TabIndex        =   160
            Text            =   "0100"
            Top             =   270
            Width           =   585
         End
         Begin VB.Label Label15 
            Caption         =   "CRC"
            Height          =   285
            Left            =   150
            TabIndex        =   255
            Top             =   660
            Width           =   495
         End
      End
      Begin VB.Frame Frame16 
         Caption         =   "TEST"
         Height          =   1455
         Left            =   120
         TabIndex        =   145
         Top             =   240
         Width           =   3495
         Begin VB.TextBox testNG 
            Height          =   345
            Index           =   7
            Left            =   2880
            Locked          =   -1  'True
            TabIndex        =   158
            Text            =   "NG8"
            Top             =   990
            Width           =   500
         End
         Begin VB.TextBox testNG 
            Height          =   345
            Index           =   6
            Left            =   2490
            Locked          =   -1  'True
            TabIndex        =   157
            Text            =   "NG7"
            Top             =   990
            Width           =   500
         End
         Begin VB.TextBox testNG 
            Height          =   345
            Index           =   5
            Left            =   2100
            Locked          =   -1  'True
            TabIndex        =   156
            Text            =   "NG6"
            Top             =   990
            Width           =   500
         End
         Begin VB.TextBox testNG 
            Height          =   345
            Index           =   4
            Left            =   1710
            Locked          =   -1  'True
            TabIndex        =   155
            Text            =   "NG5"
            Top             =   990
            Width           =   500
         End
         Begin VB.TextBox testNG 
            Height          =   345
            Index           =   3
            Left            =   1320
            Locked          =   -1  'True
            TabIndex        =   154
            Text            =   "NG4"
            Top             =   990
            Width           =   500
         End
         Begin VB.TextBox testNG 
            Height          =   345
            Index           =   2
            Left            =   930
            Locked          =   -1  'True
            TabIndex        =   153
            Text            =   "NG3"
            Top             =   990
            Width           =   500
         End
         Begin VB.TextBox testNG 
            Height          =   345
            Index           =   1
            Left            =   540
            Locked          =   -1  'True
            TabIndex        =   152
            Text            =   "NG2"
            Top             =   990
            Width           =   500
         End
         Begin VB.TextBox testNG 
            Height          =   345
            Index           =   0
            Left            =   150
            Locked          =   -1  'True
            TabIndex        =   151
            Text            =   "NG1"
            Top             =   990
            Width           =   500
         End
         Begin VB.TextBox testOKNG 
            Height          =   345
            Left            =   150
            Locked          =   -1  'True
            TabIndex        =   150
            Text            =   "01"
            Top             =   600
            Width           =   375
         End
         Begin VB.TextBox testEXECResponce 
            Height          =   315
            Left            =   150
            Locked          =   -1  'True
            TabIndex        =   149
            Text            =   "INIT"
            Top             =   240
            Width           =   615
         End
         Begin VB.TextBox testCatResponce 
            Height          =   315
            Index           =   1
            Left            =   810
            Locked          =   -1  'True
            TabIndex        =   148
            Text            =   "CAT2"
            Top             =   240
            Width           =   615
         End
         Begin VB.TextBox testCatResponce 
            Height          =   315
            Index           =   0
            Left            =   1470
            Locked          =   -1  'True
            TabIndex        =   147
            Text            =   "CAT1"
            Top             =   240
            Width           =   615
         End
         Begin VB.TextBox testItemNoResponce 
            Height          =   345
            Left            =   2430
            Locked          =   -1  'True
            TabIndex        =   146
            Text            =   "01"
            Top             =   240
            Width           =   375
         End
      End
      Begin VB.Frame Frame15 
         Caption         =   "Comm. Error"
         Height          =   1575
         Left            =   3690
         TabIndex        =   143
         Top             =   240
         Width           =   2625
         Begin VB.TextBox comError 
            Height          =   315
            Index           =   11
            Left            =   1920
            Locked          =   -1  'True
            TabIndex        =   171
            Text            =   "PAA8"
            Top             =   990
            Width           =   555
         End
         Begin VB.TextBox comError 
            Height          =   315
            Index           =   10
            Left            =   1350
            Locked          =   -1  'True
            TabIndex        =   170
            Text            =   "PAA7"
            Top             =   990
            Width           =   555
         End
         Begin VB.TextBox comError 
            Height          =   315
            Index           =   9
            Left            =   780
            Locked          =   -1  'True
            TabIndex        =   169
            Text            =   "PAA6"
            Top             =   990
            Width           =   555
         End
         Begin VB.TextBox comError 
            Height          =   315
            Index           =   8
            Left            =   210
            Locked          =   -1  'True
            TabIndex        =   168
            Text            =   "PAA5"
            Top             =   990
            Width           =   555
         End
         Begin VB.TextBox comError 
            Height          =   315
            Index           =   7
            Left            =   1920
            Locked          =   -1  'True
            TabIndex        =   167
            Text            =   "PAA4"
            Top             =   660
            Width           =   555
         End
         Begin VB.TextBox comError 
            Height          =   315
            Index           =   6
            Left            =   1350
            Locked          =   -1  'True
            TabIndex        =   166
            Text            =   "PAA3"
            Top             =   660
            Width           =   555
         End
         Begin VB.TextBox comError 
            Height          =   315
            Index           =   5
            Left            =   780
            Locked          =   -1  'True
            TabIndex        =   165
            Text            =   "PAA2"
            Top             =   660
            Width           =   555
         End
         Begin VB.TextBox comError 
            Height          =   315
            Index           =   4
            Left            =   210
            Locked          =   -1  'True
            TabIndex        =   164
            Text            =   "PAA1"
            Top             =   660
            Width           =   555
         End
         Begin VB.TextBox comError 
            Height          =   315
            Index           =   3
            Left            =   1920
            Locked          =   -1  'True
            TabIndex        =   163
            Text            =   "MCP2"
            Top             =   300
            Width           =   555
         End
         Begin VB.TextBox comError 
            Height          =   315
            Index           =   2
            Left            =   1380
            Locked          =   -1  'True
            TabIndex        =   162
            Text            =   "MCP1"
            Top             =   300
            Width           =   555
         End
         Begin VB.TextBox comError 
            Height          =   315
            Index           =   1
            Left            =   780
            Locked          =   -1  'True
            TabIndex        =   161
            Text            =   "DVA2"
            Top             =   300
            Width           =   555
         End
         Begin VB.TextBox comError 
            Height          =   315
            Index           =   0
            Left            =   210
            Locked          =   -1  'True
            TabIndex        =   144
            Text            =   "DVA1"
            Top             =   300
            Width           =   555
         End
      End
      Begin VB.Frame Frame13 
         Caption         =   "MODE"
         Height          =   1455
         Left            =   1110
         TabIndex        =   141
         Top             =   1830
         Width           =   2475
         Begin VB.TextBox modeRespoince 
            Height          =   315
            Index           =   6
            Left            =   1650
            Locked          =   -1  'True
            TabIndex        =   177
            Text            =   "CHIME"
            Top             =   990
            Width           =   645
         End
         Begin VB.TextBox modeRespoince 
            Height          =   315
            Index           =   5
            Left            =   1080
            Locked          =   -1  'True
            TabIndex        =   176
            Text            =   "DOOR"
            Top             =   990
            Width           =   555
         End
         Begin VB.TextBox modeRespoince 
            Height          =   315
            Index           =   4
            Left            =   210
            Locked          =   -1  'True
            TabIndex        =   175
            Text            =   "AUTO"
            Top             =   960
            Width           =   555
         End
         Begin VB.TextBox modeRespoince 
            Height          =   315
            Index           =   3
            Left            =   1380
            Locked          =   -1  'True
            TabIndex        =   174
            Text            =   "SPECIAL"
            Top             =   600
            Width           =   825
         End
         Begin VB.TextBox modeRespoince 
            Height          =   315
            Index           =   2
            Left            =   210
            Locked          =   -1  'True
            TabIndex        =   173
            Text            =   "EMERGENCY"
            Top             =   600
            Width           =   1125
         End
         Begin VB.TextBox modeRespoince 
            Height          =   315
            Index           =   1
            Left            =   1080
            Locked          =   -1  'True
            TabIndex        =   172
            Text            =   "CAB/CAB"
            Top             =   270
            Width           =   825
         End
         Begin VB.TextBox modeRespoince 
            Height          =   315
            Index           =   0
            Left            =   210
            Locked          =   -1  'True
            TabIndex        =   142
            Text            =   "MANUAL"
            Top             =   270
            Width           =   825
         End
      End
      Begin VB.Frame Frame11 
         Caption         =   "Car Info"
         Height          =   3675
         Left            =   3660
         TabIndex        =   99
         Top             =   1920
         Width           =   3615
         Begin VB.Frame Frame12 
            Caption         =   "EAH Active(Physical layout)"
            Height          =   3315
            Left            =   120
            TabIndex        =   100
            Top             =   210
            Width           =   3315
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   0
               Left            =   120
               Locked          =   -1  'True
               TabIndex        =   140
               Text            =   "V1"
               Top             =   300
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   1
               Left            =   740
               Locked          =   -1  'True
               TabIndex        =   139
               Text            =   "V2"
               Top             =   300
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   2
               Left            =   1340
               Locked          =   -1  'True
               TabIndex        =   138
               Text            =   "V3"
               Top             =   300
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   3
               Left            =   1940
               Locked          =   -1  'True
               TabIndex        =   137
               Text            =   "V4"
               Top             =   300
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   4
               Left            =   2540
               Locked          =   -1  'True
               TabIndex        =   136
               Text            =   "V5"
               Top             =   300
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   5
               Left            =   120
               Locked          =   -1  'True
               TabIndex        =   135
               Text            =   "Z1"
               Top             =   640
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   6
               Left            =   740
               Locked          =   -1  'True
               TabIndex        =   134
               Text            =   "Z2"
               Top             =   640
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   7
               Left            =   1340
               Locked          =   -1  'True
               TabIndex        =   133
               Text            =   "Z3"
               Top             =   640
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   8
               Left            =   1940
               Locked          =   -1  'True
               TabIndex        =   132
               Text            =   "Z4"
               Top             =   645
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   9
               Left            =   2520
               Locked          =   -1  'True
               TabIndex        =   131
               Text            =   "Z5"
               Top             =   645
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   10
               Left            =   120
               Locked          =   -1  'True
               TabIndex        =   130
               Text            =   "X1"
               Top             =   975
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   11
               Left            =   720
               Locked          =   -1  'True
               TabIndex        =   129
               Text            =   "X2"
               Top             =   975
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   12
               Left            =   1320
               Locked          =   -1  'True
               TabIndex        =   128
               Text            =   "X3"
               Top             =   975
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   13
               Left            =   1940
               Locked          =   -1  'True
               TabIndex        =   127
               Text            =   "X4"
               Top             =   975
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   14
               Left            =   2540
               Locked          =   -1  'True
               TabIndex        =   126
               Text            =   "X5"
               Top             =   975
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   15
               Left            =   120
               Locked          =   -1  'True
               TabIndex        =   125
               Text            =   "W1"
               Top             =   1335
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   16
               Left            =   740
               Locked          =   -1  'True
               TabIndex        =   124
               Text            =   "W2"
               Top             =   1335
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   17
               Left            =   1340
               Locked          =   -1  'True
               TabIndex        =   123
               Text            =   "W3"
               Top             =   1335
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   18
               Left            =   1920
               Locked          =   -1  'True
               TabIndex        =   122
               Text            =   "W4"
               Top             =   1335
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   19
               Left            =   2540
               Locked          =   -1  'True
               TabIndex        =   121
               Text            =   "W5"
               Top             =   1320
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   20
               Left            =   120
               Locked          =   -1  'True
               TabIndex        =   120
               Text            =   "Y1"
               Top             =   1680
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   21
               Left            =   740
               Locked          =   -1  'True
               TabIndex        =   119
               Text            =   "Y2"
               Top             =   1680
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   22
               Left            =   1340
               Locked          =   -1  'True
               TabIndex        =   118
               Text            =   "Y3"
               Top             =   1680
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   23
               Left            =   1940
               Locked          =   -1  'True
               TabIndex        =   117
               Text            =   "Y4"
               Top             =   1680
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   24
               Left            =   2520
               Locked          =   -1  'True
               TabIndex        =   116
               Text            =   "Y5"
               Top             =   1680
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   25
               Left            =   120
               Locked          =   -1  'True
               TabIndex        =   115
               Text            =   "X1"
               Top             =   2040
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   26
               Left            =   740
               Locked          =   -1  'True
               TabIndex        =   114
               Text            =   "X2"
               Top             =   2040
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   27
               Left            =   1340
               Locked          =   -1  'True
               TabIndex        =   113
               Text            =   "X3"
               Top             =   2040
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   28
               Left            =   1940
               Locked          =   -1  'True
               TabIndex        =   112
               Text            =   "X4"
               Top             =   2040
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   29
               Left            =   2540
               Locked          =   -1  'True
               TabIndex        =   111
               Text            =   "X5"
               Top             =   2040
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   30
               Left            =   120
               Locked          =   -1  'True
               TabIndex        =   110
               Text            =   "Z1"
               Top             =   2385
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   31
               Left            =   740
               Locked          =   -1  'True
               TabIndex        =   109
               Text            =   "Z2"
               Top             =   2385
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   32
               Left            =   1340
               Locked          =   -1  'True
               TabIndex        =   108
               Text            =   "Z3"
               Top             =   2385
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   33
               Left            =   1940
               Locked          =   -1  'True
               TabIndex        =   107
               Text            =   "Z4"
               Top             =   2385
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   34
               Left            =   2540
               Locked          =   -1  'True
               TabIndex        =   106
               Text            =   "Z5"
               Top             =   2385
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   35
               Left            =   120
               Locked          =   -1  'True
               TabIndex        =   105
               Text            =   "V1"
               Top             =   2715
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   36
               Left            =   740
               Locked          =   -1  'True
               TabIndex        =   104
               Text            =   "V2"
               Top             =   2715
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   37
               Left            =   1340
               Locked          =   -1  'True
               TabIndex        =   103
               Text            =   "V3"
               Top             =   2715
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   330
               Index           =   38
               Left            =   1940
               Locked          =   -1  'True
               TabIndex        =   102
               Text            =   "V4"
               Top             =   2715
               Width           =   600
            End
            Begin VB.TextBox EAHActive 
               Height          =   360
               Index           =   39
               Left            =   2540
               Locked          =   -1  'True
               TabIndex        =   101
               Text            =   "V5"
               Top             =   2715
               Width           =   600
            End
         End
      End
      Begin VB.CommandButton cmdSDRMonitor 
         Caption         =   "TBL Monitor"
         Height          =   555
         Left            =   6420
         TabIndex        =   2
         Top             =   210
         Width           =   1455
      End
   End
   Begin VB.Frame SDRFrame 
      Caption         =   "TMS SDR Monitor"
      Height          =   8025
      Left            =   120
      TabIndex        =   0
      Top             =   120
      Width           =   8115
      Begin VB.CommandButton cmdSD 
         Caption         =   "SD Monitor"
         Height          =   555
         Left            =   6320
         TabIndex        =   1
         Top             =   180
         Width           =   1545
      End
      Begin VB.Frame Frame8 
         Caption         =   "Open Active"
         Height          =   915
         Left            =   150
         TabIndex        =   85
         Top             =   3000
         Width           =   2805
         Begin VB.TextBox doorEffective 
            Height          =   330
            Left            =   1380
            Locked          =   -1  'True
            TabIndex        =   178
            Text            =   "DOOR"
            Top             =   480
            Width           =   645
         End
         Begin VB.TextBox DoorLeft 
            Height          =   315
            Left            =   180
            Locked          =   -1  'True
            TabIndex        =   87
            Text            =   "L"
            Top             =   450
            Width           =   435
         End
         Begin VB.TextBox DoorRight 
            Height          =   315
            Left            =   750
            Locked          =   -1  'True
            TabIndex        =   86
            Text            =   "R"
            Top             =   450
            Width           =   435
         End
         Begin VB.Label Label11 
            Caption         =   "Door Effect"
            Height          =   285
            Left            =   1380
            TabIndex        =   179
            Top             =   285
            Width           =   1035
         End
         Begin VB.Label Label9 
            Caption         =   "LEFT"
            Height          =   285
            Left            =   180
            TabIndex        =   89
            Top             =   270
            Width           =   405
         End
         Begin VB.Label Label10 
            Caption         =   "RIGHT"
            Height          =   285
            Left            =   720
            TabIndex        =   88
            Top             =   270
            Width           =   525
         End
      End
      Begin VB.Frame Frame5 
         Caption         =   "Car Info"
         Height          =   3675
         Left            =   150
         TabIndex        =   27
         Top             =   3960
         Width           =   7815
         Begin VB.Frame FrameReset 
            Caption         =   "EAH Reset(Physical layout)"
            Height          =   3315
            Index           =   1
            Left            =   4390
            TabIndex        =   256
            Top             =   210
            Width           =   3290
            Begin VB.TextBox EAHR 
               Height          =   330
               Index           =   0
               Left            =   120
               Locked          =   -1  'True
               TabIndex        =   296
               Text            =   "V1"
               Top             =   300
               Width           =   600
            End
            Begin VB.TextBox EAHR 
               Height          =   330
               Index           =   1
               Left            =   740
               Locked          =   -1  'True
               TabIndex        =   295
               Text            =   "V2"
               Top             =   300
               Width           =   600
            End
            Begin VB.TextBox EAHR 
               Height          =   330
               Index           =   2
               Left            =   1340
               Locked          =   -1  'True
               TabIndex        =   294
               Text            =   "V3"
               Top             =   300
               Width           =   600
            End
            Begin VB.TextBox EAHR 
               Height          =   330
               Index           =   3
               Left            =   1940
               Locked          =   -1  'True
               TabIndex        =   293
               Text            =   "V4"
               Top             =   300
               Width           =   600
            End
            Begin VB.TextBox EAHR 
               Height          =   330
               Index           =   4
               Left            =   2540
               Locked          =   -1  'True
               TabIndex        =   292
               Text            =   "V5"
               Top             =   300
               Width           =   600
            End
            Begin VB.TextBox EAHR 
               Height          =   330
               Index           =   5
               Left            =   120
               Locked          =   -1  'True
               TabIndex        =   291
               Text            =   "Z1"
               Top             =   640
               Width           =   600
            End
            Begin VB.TextBox EAHR 
               Height          =   330
               Index           =   6
               Left            =   740
               Locked          =   -1  'True
               TabIndex        =   290
               Text            =   "Z2"
               Top             =   640
               Width           =   600
            End
            Begin VB.TextBox EAHR 
               Height          =   330
               Index           =   7
               Left            =   1340
               Locked          =   -1  'True
               TabIndex        =   289
               Text            =   "Z3"
               Top             =   640
               Width           =   600
            End
            Begin VB.TextBox EAHR 
               Height          =   330
               Index           =   8
               Left            =   1940
               Locked          =   -1  'True
               TabIndex        =   288
               Text            =   "Z4"
               Top             =   645
               Width           =   600
            End
            Begin VB.TextBox EAHR 
               Height          =   330
               Index           =   9
               Left            =   2540
               Locked          =   -1  'True
               TabIndex        =   287
               Text            =   "Z5"
               Top             =   645
               Width           =   600
            End
            Begin VB.TextBox EAHR 
               Height          =   330
               Index           =   10
               Left            =   120
               Locked          =   -1  'True
               TabIndex        =   286
               Text            =   "X1"
               Top             =   975
               Width           =   600
            End
            Begin VB.TextBox EAHR 
               Height          =   330
               Index           =   11
               Left            =   740
               Locked          =   -1  'True
               TabIndex        =   285
               Text            =   "X2"
               Top             =   975
               Width           =   600
            End
            Begin VB.TextBox EAHR 
               Height          =   330
               Index           =   12
               Left            =   1340
               Locked          =   -1  'True
               TabIndex        =   284
               Text            =   "X3"
               Top             =   975
               Width           =   600
            End
            Begin VB.TextBox EAHR 
               Height          =   330
               Index           =   13
               Left            =   1940
               Locked          =   -1  'True
               TabIndex        =   283
               Text            =   "X4"
               Top             =   975
               Width           =   600
            End
            Begin VB.TextBox EAHR 
               Height          =   330
               Index           =   14
               Left            =   2540
               Locked          =   -1  'True
               TabIndex        =   282
               Text            =   "X5"
               Top             =   975
               Width           =   600
            End
            Begin VB.TextBox EAHR 
               Height          =   330
               Index           =   15
               Left            =   120
               Locked          =   -1  'True
               TabIndex        =   281
               Text            =   "W1"
               Top             =   1335
               Width           =   600
            End
            Begin VB.TextBox EAHR 
               Height          =   330
               Index           =   16
               Left            =   740
               Locked          =   -1  'True
               TabIndex        =   280
               Text            =   "W2"
               Top             =   1335
               Width           =   600
            End
            Begin VB.TextBox EAHR 
               Height          =   330
               Index           =   17
               Left            =   1340
               Locked          =   -1  'True
               TabIndex        =   279
               Text            =   "W3"
               Top             =   1335
               Width           =   600
            End
            Begin VB.TextBox EAHR 
               Height          =   330
               Index           =   18
               Left            =   1940
               Locked          =   -1  'True
               TabIndex        =   278
               Text            =   "W4"
               Top             =   1335
               Width           =   600
            End
            Begin VB.TextBox EAHR 
               Height          =   330
               Index           =   19
               Left            =   2540
               Locked          =   -1  'True
               TabIndex        =   277
               Text            =   "W5"
               Top             =   1335
               Width           =   600
            End
            Begin VB.TextBox EAHR 
               Height          =   330
               Index           =   20
               Left            =   120
               Locked          =   -1  'True
               TabIndex        =   276
               Text            =   "Y1"
               Top             =   1680
               Width           =   600
            End
            Begin VB.TextBox EAHR 
               Height          =   330
               Index           =   21
               Left            =   740
               Locked          =   -1  'True
               TabIndex        =   275
               Text            =   "Y2"
               Top             =   1680
               Width           =   600
            End
            Begin VB.TextBox EAHR 
               Height          =   330
               Index           =   22
               Left            =   1340
               Locked          =   -1  'True
               TabIndex        =   274
               Text            =   "Y3"
               Top             =   1680
               Width           =   600
            End
            Begin VB.TextBox EAHR 
               Height          =   330
               Index           =   23
               Left            =   1940
               Locked          =   -1  'True
               TabIndex        =   273
               Text            =   "Y4"
               Top             =   1680
               Width           =   600
            End
            Begin VB.TextBox EAHR 
               Height          =   330
               Index           =   24
               Left            =   2540
               Locked          =   -1  'True
               TabIndex        =   272
               Text            =   "Y5"
               Top             =   1680
               Width           =   600
            End
            Begin VB.TextBox EAHR 
               Height          =   330
               Index           =   25
               Left            =   120
               Locked          =   -1  'True
               TabIndex        =   271
               Text            =   "X1"
               Top             =   2040
               Width           =   600
            End
            Begin VB.TextBox EAHR 
               Height          =   330
               Index           =   26
               Left            =   740
               Locked          =   -1  'True
               TabIndex        =   270
               Text            =   "X2"
               Top             =   2040
               Width           =   600
            End
            Begin VB.TextBox EAHR 
               Height          =   330
               Index           =   27
               Left            =   1340
               Locked          =   -1  'True
               TabIndex        =   269
               Text            =   "X3"
               Top             =   2040
               Width           =   600
            End
            Begin VB.TextBox EAHR 
               Height          =   330
               Index           =   28
               Left            =   1940
               Locked          =   -1  'True
               TabIndex        =   268
               Text            =   "X4"
               Top             =   2040
               Width           =   600
            End
            Begin VB.TextBox EAHR 
               Height          =   330
               Index           =   29
               Left            =   2540
               Locked          =   -1  'True
               TabIndex        =   267
               Text            =   "X5"
               Top             =   2040
               Width           =   600
            End
            Begin VB.TextBox EAHR 
               Height          =   330
               Index           =   30
               Left            =   120
               Locked          =   -1  'True
               TabIndex        =   266
               Text            =   "Z1"
               Top             =   2385
               Width           =   600
            End
            Begin VB.TextBox EAHR 
               Height          =   330
               Index           =   31
               Left            =   740
               Locked          =   -1  'True
               TabIndex        =   265
               Text            =   "Z2"
               Top             =   2385
               Width           =   600
            End
            Begin VB.TextBox EAHR 
               Height          =   330
               Index           =   32
               Left            =   1340
               Locked          =   -1  'True
               TabIndex        =   264
               Text            =   "Z3"
               Top             =   2385
               Width           =   600
            End
            Begin VB.TextBox EAHR 
               Height          =   330
               Index           =   33
               Left            =   1940
               Locked          =   -1  'True
               TabIndex        =   263
               Text            =   "Z4"
               Top             =   2385
               Width           =   600
            End
            Begin VB.TextBox EAHR 
               Height          =   330
               Index           =   34
               Left            =   2540
               Locked          =   -1  'True
               TabIndex        =   262
               Text            =   "Z5"
               Top             =   2385
               Width           =   600
            End
            Begin VB.TextBox EAHR 
               Height          =   330
               Index           =   35
               Left            =   120
               Locked          =   -1  'True
               TabIndex        =   261
               Text            =   "V1"
               Top             =   2715
               Width           =   600
            End
            Begin VB.TextBox EAHR 
               Height          =   330
               Index           =   36
               Left            =   740
               Locked          =   -1  'True
               TabIndex        =   260
               Text            =   "V2"
               Top             =   2715
               Width           =   600
            End
            Begin VB.TextBox EAHR 
               Height          =   330
               Index           =   37
               Left            =   1340
               Locked          =   -1  'True
               TabIndex        =   259
               Text            =   "V3"
               Top             =   2715
               Width           =   600
            End
            Begin VB.TextBox EAHR 
               Height          =   330
               Index           =   38
               Left            =   1940
               Locked          =   -1  'True
               TabIndex        =   258
               Text            =   "V4"
               Top             =   2715
               Width           =   600
            End
            Begin VB.TextBox EAHR 
               Height          =   360
               Index           =   39
               Left            =   2540
               Locked          =   -1  'True
               TabIndex        =   257
               Text            =   "V5"
               Top             =   2715
               Width           =   600
            End
         End
         Begin VB.TextBox carType2 
            Height          =   330
            Index           =   7
            Left            =   550
            Locked          =   -1  'True
            TabIndex        =   84
            Text            =   "0"
            Top             =   2880
            Width           =   385
         End
         Begin VB.TextBox carType2 
            Height          =   330
            Index           =   6
            Left            =   550
            Locked          =   -1  'True
            TabIndex        =   83
            Text            =   "0"
            Top             =   2520
            Width           =   385
         End
         Begin VB.TextBox carType2 
            Height          =   330
            Index           =   5
            Left            =   550
            Locked          =   -1  'True
            TabIndex        =   82
            Text            =   "0"
            Top             =   2175
            Width           =   385
         End
         Begin VB.TextBox carType2 
            Height          =   330
            Index           =   4
            Left            =   550
            Locked          =   -1  'True
            TabIndex        =   81
            Text            =   "0"
            Top             =   1845
            Width           =   385
         End
         Begin VB.TextBox carType2 
            Height          =   330
            Index           =   3
            Left            =   550
            Locked          =   -1  'True
            TabIndex        =   80
            Text            =   "0"
            Top             =   1500
            Width           =   385
         End
         Begin VB.TextBox carType2 
            Height          =   330
            Index           =   2
            Left            =   550
            Locked          =   -1  'True
            TabIndex        =   79
            Text            =   "0"
            Top             =   1155
            Width           =   385
         End
         Begin VB.TextBox carType2 
            Height          =   330
            Index           =   1
            Left            =   550
            Locked          =   -1  'True
            TabIndex        =   78
            Text            =   "0"
            Top             =   825
            Width           =   385
         End
         Begin VB.TextBox carType2 
            Height          =   330
            Index           =   0
            Left            =   550
            Locked          =   -1  'True
            TabIndex        =   77
            Text            =   "0"
            Top             =   480
            Width           =   385
         End
         Begin VB.TextBox CarNo2 
            Height          =   330
            Index           =   7
            Left            =   120
            Locked          =   -1  'True
            TabIndex        =   76
            Text            =   "000"
            Top             =   2880
            Width           =   385
         End
         Begin VB.TextBox CarNo2 
            Height          =   330
            Index           =   6
            Left            =   120
            Locked          =   -1  'True
            TabIndex        =   75
            Text            =   "000"
            Top             =   2520
            Width           =   385
         End
         Begin VB.TextBox CarNo2 
            Height          =   330
            Index           =   5
            Left            =   120
            Locked          =   -1  'True
            TabIndex        =   74
            Text            =   "000"
            Top             =   2175
            Width           =   385
         End
         Begin VB.TextBox CarNo2 
            Height          =   330
            Index           =   4
            Left            =   120
            Locked          =   -1  'True
            TabIndex        =   73
            Text            =   "000"
            Top             =   1845
            Width           =   385
         End
         Begin VB.TextBox CarNo2 
            Height          =   330
            Index           =   3
            Left            =   120
            Locked          =   -1  'True
            TabIndex        =   72
            Text            =   "000"
            Top             =   1500
            Width           =   385
         End
         Begin VB.TextBox CarNo2 
            Height          =   330
            Index           =   2
            Left            =   120
            Locked          =   -1  'True
            TabIndex        =   71
            Text            =   "000"
            Top             =   1155
            Width           =   385
         End
         Begin VB.TextBox CarNo2 
            Height          =   330
            Index           =   1
            Left            =   120
            Locked          =   -1  'True
            TabIndex        =   70
            Text            =   "000"
            Top             =   825
            Width           =   385
         End
         Begin VB.Frame FrameTalkback 
            Caption         =   "EAH Talkback(Physical layout)"
            Height          =   3315
            Index           =   0
            Left            =   1040
            TabIndex        =   29
            Top             =   210
            Width           =   3290
            Begin VB.TextBox EAH 
               Height          =   360
               Index           =   39
               Left            =   2540
               Locked          =   -1  'True
               TabIndex        =   69
               Text            =   "V5"
               Top             =   2715
               Width           =   600
            End
            Begin VB.TextBox EAH 
               Height          =   330
               Index           =   38
               Left            =   1940
               Locked          =   -1  'True
               TabIndex        =   68
               Text            =   "V4"
               Top             =   2715
               Width           =   600
            End
            Begin VB.TextBox EAH 
               Height          =   330
               Index           =   37
               Left            =   1340
               Locked          =   -1  'True
               TabIndex        =   67
               Text            =   "V3"
               Top             =   2715
               Width           =   600
            End
            Begin VB.TextBox EAH 
               Height          =   330
               Index           =   36
               Left            =   740
               Locked          =   -1  'True
               TabIndex        =   66
               Text            =   "V2"
               Top             =   2715
               Width           =   600
            End
            Begin VB.TextBox EAH 
               Height          =   330
               Index           =   35
               Left            =   120
               Locked          =   -1  'True
               TabIndex        =   65
               Text            =   "V1"
               Top             =   2715
               Width           =   600
            End
            Begin VB.TextBox EAH 
               Height          =   330
               Index           =   34
               Left            =   2540
               Locked          =   -1  'True
               TabIndex        =   64
               Text            =   "Z5"
               Top             =   2385
               Width           =   600
            End
            Begin VB.TextBox EAH 
               Height          =   330
               Index           =   33
               Left            =   1940
               Locked          =   -1  'True
               TabIndex        =   63
               Text            =   "Z4"
               Top             =   2385
               Width           =   600
            End
            Begin VB.TextBox EAH 
               Height          =   330
               Index           =   32
               Left            =   1340
               Locked          =   -1  'True
               TabIndex        =   62
               Text            =   "Z3"
               Top             =   2385
               Width           =   600
            End
            Begin VB.TextBox EAH 
               Height          =   330
               Index           =   31
               Left            =   740
               Locked          =   -1  'True
               TabIndex        =   61
               Text            =   "Z2"
               Top             =   2385
               Width           =   600
            End
            Begin VB.TextBox EAH 
               Height          =   330
               Index           =   30
               Left            =   120
               Locked          =   -1  'True
               TabIndex        =   60
               Text            =   "Z1"
               Top             =   2385
               Width           =   600
            End
            Begin VB.TextBox EAH 
               Height          =   330
               Index           =   29
               Left            =   2540
               Locked          =   -1  'True
               TabIndex        =   59
               Text            =   "X5"
               Top             =   2040
               Width           =   600
            End
            Begin VB.TextBox EAH 
               Height          =   330
               Index           =   28
               Left            =   1940
               Locked          =   -1  'True
               TabIndex        =   58
               Text            =   "X4"
               Top             =   2040
               Width           =   600
            End
            Begin VB.TextBox EAH 
               Height          =   330
               Index           =   27
               Left            =   1340
               Locked          =   -1  'True
               TabIndex        =   57
               Text            =   "X3"
               Top             =   2040
               Width           =   600
            End
            Begin VB.TextBox EAH 
               Height          =   330
               Index           =   26
               Left            =   740
               Locked          =   -1  'True
               TabIndex        =   56
               Text            =   "X2"
               Top             =   2040
               Width           =   600
            End
            Begin VB.TextBox EAH 
               Height          =   330
               Index           =   25
               Left            =   120
               Locked          =   -1  'True
               TabIndex        =   55
               Text            =   "X1"
               Top             =   2040
               Width           =   600
            End
            Begin VB.TextBox EAH 
               Height          =   330
               Index           =   24
               Left            =   2540
               Locked          =   -1  'True
               TabIndex        =   54
               Text            =   "Y5"
               Top             =   1680
               Width           =   600
            End
            Begin VB.TextBox EAH 
               Height          =   330
               Index           =   23
               Left            =   1940
               Locked          =   -1  'True
               TabIndex        =   53
               Text            =   "Y4"
               Top             =   1680
               Width           =   600
            End
            Begin VB.TextBox EAH 
               Height          =   330
               Index           =   22
               Left            =   1340
               Locked          =   -1  'True
               TabIndex        =   52
               Text            =   "Y3"
               Top             =   1680
               Width           =   600
            End
            Begin VB.TextBox EAH 
               Height          =   330
               Index           =   21
               Left            =   740
               Locked          =   -1  'True
               TabIndex        =   51
               Text            =   "Y2"
               Top             =   1680
               Width           =   600
            End
            Begin VB.TextBox EAH 
               Height          =   330
               Index           =   20
               Left            =   120
               Locked          =   -1  'True
               TabIndex        =   50
               Text            =   "Y1"
               Top             =   1680
               Width           =   600
            End
            Begin VB.TextBox EAH 
               Height          =   330
               Index           =   19
               Left            =   2540
               Locked          =   -1  'True
               TabIndex        =   49
               Text            =   "W5"
               Top             =   1335
               Width           =   600
            End
            Begin VB.TextBox EAH 
               Height          =   330
               Index           =   18
               Left            =   1940
               Locked          =   -1  'True
               TabIndex        =   48
               Text            =   "W4"
               Top             =   1335
               Width           =   600
            End
            Begin VB.TextBox EAH 
               Height          =   330
               Index           =   17
               Left            =   1340
               Locked          =   -1  'True
               TabIndex        =   47
               Text            =   "W3"
               Top             =   1335
               Width           =   600
            End
            Begin VB.TextBox EAH 
               Height          =   330
               Index           =   16
               Left            =   740
               Locked          =   -1  'True
               TabIndex        =   46
               Text            =   "W2"
               Top             =   1335
               Width           =   600
            End
            Begin VB.TextBox EAH 
               Height          =   330
               Index           =   15
               Left            =   120
               Locked          =   -1  'True
               TabIndex        =   45
               Text            =   "W1"
               Top             =   1335
               Width           =   600
            End
            Begin VB.TextBox EAH 
               Height          =   330
               Index           =   14
               Left            =   2540
               Locked          =   -1  'True
               TabIndex        =   44
               Text            =   "X5"
               Top             =   975
               Width           =   600
            End
            Begin VB.TextBox EAH 
               Height          =   330
               Index           =   13
               Left            =   1940
               Locked          =   -1  'True
               TabIndex        =   43
               Text            =   "X4"
               Top             =   975
               Width           =   600
            End
            Begin VB.TextBox EAH 
               Height          =   330
               Index           =   12
               Left            =   1340
               Locked          =   -1  'True
               TabIndex        =   42
               Text            =   "X3"
               Top             =   975
               Width           =   600
            End
            Begin VB.TextBox EAH 
               Height          =   330
               Index           =   11
               Left            =   740
               Locked          =   -1  'True
               TabIndex        =   41
               Text            =   "X2"
               Top             =   975
               Width           =   600
            End
            Begin VB.TextBox EAH 
               Height          =   330
               Index           =   10
               Left            =   120
               Locked          =   -1  'True
               TabIndex        =   40
               Text            =   "X1"
               Top             =   975
               Width           =   600
            End
            Begin VB.TextBox EAH 
               Height          =   330
               Index           =   9
               Left            =   2540
               Locked          =   -1  'True
               TabIndex        =   39
               Text            =   "Z5"
               Top             =   645
               Width           =   600
            End
            Begin VB.TextBox EAH 
               Height          =   330
               Index           =   8
               Left            =   1940
               Locked          =   -1  'True
               TabIndex        =   38
               Text            =   "Z4"
               Top             =   645
               Width           =   600
            End
            Begin VB.TextBox EAH 
               Height          =   330
               Index           =   7
               Left            =   1340
               Locked          =   -1  'True
               TabIndex        =   37
               Text            =   "Z3"
               Top             =   640
               Width           =   600
            End
            Begin VB.TextBox EAH 
               Height          =   330
               Index           =   6
               Left            =   740
               Locked          =   -1  'True
               TabIndex        =   36
               Text            =   "Z2"
               Top             =   640
               Width           =   600
            End
            Begin VB.TextBox EAH 
               Height          =   330
               Index           =   5
               Left            =   120
               Locked          =   -1  'True
               TabIndex        =   35
               Text            =   "Z1"
               Top             =   640
               Width           =   600
            End
            Begin VB.TextBox EAH 
               Height          =   330
               Index           =   4
               Left            =   2540
               Locked          =   -1  'True
               TabIndex        =   34
               Text            =   "V5"
               Top             =   300
               Width           =   600
            End
            Begin VB.TextBox EAH 
               Height          =   330
               Index           =   3
               Left            =   1940
               Locked          =   -1  'True
               TabIndex        =   33
               Text            =   "V4"
               Top             =   300
               Width           =   600
            End
            Begin VB.TextBox EAH 
               Height          =   330
               Index           =   2
               Left            =   1340
               Locked          =   -1  'True
               TabIndex        =   32
               Text            =   "V3"
               Top             =   300
               Width           =   600
            End
            Begin VB.TextBox EAH 
               Height          =   330
               Index           =   1
               Left            =   740
               Locked          =   -1  'True
               TabIndex        =   31
               Text            =   "V2"
               Top             =   300
               Width           =   600
            End
            Begin VB.TextBox EAH 
               Height          =   330
               Index           =   0
               Left            =   120
               Locked          =   -1  'True
               TabIndex        =   30
               Text            =   "V1"
               Top             =   300
               Width           =   600
            End
         End
         Begin VB.TextBox CarNo2 
            Height          =   330
            Index           =   0
            Left            =   120
            Locked          =   -1  'True
            TabIndex        =   28
            Text            =   "000"
            Top             =   480
            Width           =   385
         End
      End
      Begin VB.Frame Frame4 
         Caption         =   "Station Data"
         Height          =   1935
         Left            =   150
         TabIndex        =   16
         Top             =   1020
         Width           =   3165
         Begin VB.Frame Frame7 
            Caption         =   "Broadcast Active"
            Height          =   915
            Left            =   90
            TabIndex        =   90
            Top             =   900
            Width           =   2925
            Begin VB.TextBox specialCode 
               Height          =   330
               Left            =   2250
               Locked          =   -1  'True
               TabIndex        =   182
               Text            =   "00"
               Top             =   405
               Width           =   435
            End
            Begin VB.TextBox broadcastoff 
               Height          =   330
               Left            =   1350
               Locked          =   -1  'True
               TabIndex        =   96
               Text            =   "OFF"
               Top             =   400
               Width           =   435
            End
            Begin VB.TextBox pa 
               Height          =   330
               Left            =   780
               Locked          =   -1  'True
               TabIndex        =   94
               Text            =   "PA"
               Top             =   400
               Width           =   435
            End
            Begin VB.TextBox broadcast 
               Height          =   330
               Left            =   180
               Locked          =   -1  'True
               TabIndex        =   93
               Text            =   "BR"
               Top             =   400
               Width           =   435
            End
            Begin VB.Label Label13 
               Caption         =   "Special"
               Height          =   285
               Left            =   2100
               TabIndex        =   183
               Top             =   150
               Width           =   735
            End
            Begin VB.Label Label8 
               Caption         =   "OFF"
               Height          =   285
               Left            =   1410
               TabIndex        =   95
               Top             =   200
               Width           =   405
            End
            Begin VB.Label Label7 
               Caption         =   "BR"
               Height          =   285
               Left            =   810
               TabIndex        =   92
               Top             =   200
               Width           =   405
            End
            Begin VB.Label Label6 
               Caption         =   "ATC"
               Height          =   285
               Left            =   180
               TabIndex        =   91
               Top             =   200
               Width           =   555
            End
         End
         Begin VB.TextBox routeCode 
            Height          =   315
            Left            =   2520
            Locked          =   -1  'True
            TabIndex        =   25
            Text            =   "01"
            Top             =   510
            Width           =   435
         End
         Begin VB.TextBox destinationCode 
            Height          =   315
            Left            =   1980
            Locked          =   -1  'True
            TabIndex        =   23
            Text            =   "01"
            Top             =   510
            Width           =   435
         End
         Begin VB.TextBox nextStationName 
            Height          =   315
            Left            =   1440
            Locked          =   -1  'True
            TabIndex        =   21
            Text            =   "01"
            Top             =   480
            Width           =   435
         End
         Begin VB.TextBox LineNumber 
            Height          =   315
            Left            =   120
            Locked          =   -1  'True
            TabIndex        =   18
            Text            =   "1"
            Top             =   480
            Width           =   435
         End
         Begin VB.TextBox stationNumber 
            Height          =   315
            Left            =   720
            Locked          =   -1  'True
            TabIndex        =   17
            Text            =   "01"
            Top             =   480
            Width           =   435
         End
         Begin VB.Label Label5 
            Caption         =   "Route"
            Height          =   285
            Left            =   2490
            TabIndex        =   26
            Top             =   270
            Width           =   555
         End
         Begin VB.Label Label4 
            Caption         =   "Dest."
            Height          =   285
            Left            =   1950
            TabIndex        =   24
            Top             =   270
            Width           =   555
         End
         Begin VB.Label Label3 
            Caption         =   "Next"
            Height          =   285
            Left            =   1410
            TabIndex        =   22
            Top             =   270
            Width           =   555
         End
         Begin VB.Label Label2 
            Caption         =   "Station"
            Height          =   285
            Left            =   690
            TabIndex        =   20
            Top             =   240
            Width           =   555
         End
         Begin VB.Label Label1 
            Caption         =   "Line"
            Height          =   285
            Left            =   120
            TabIndex        =   19
            Top             =   240
            Width           =   555
         End
      End
      Begin VB.Frame Frame3 
         Caption         =   "ActiveFlag"
         Height          =   1905
         Left            =   3330
         TabIndex        =   8
         Top             =   1020
         Width           =   3735
         Begin VB.TextBox couplingUnit 
            Height          =   330
            Left            =   1620
            Locked          =   -1  'True
            TabIndex        =   180
            Text            =   "0"
            Top             =   1470
            Width           =   435
         End
         Begin VB.TextBox carType 
            Height          =   315
            Left            =   1620
            Locked          =   -1  'True
            TabIndex        =   15
            Text            =   "0"
            Top             =   1080
            Width           =   465
         End
         Begin VB.TextBox carNo 
            Height          =   315
            Left            =   1620
            Locked          =   -1  'True
            TabIndex        =   14
            Text            =   "000"
            Top             =   690
            Width           =   465
         End
         Begin VB.TextBox timeData 
            Height          =   315
            Left            =   1620
            Locked          =   -1  'True
            TabIndex        =   13
            Text            =   "00/08/10 10:00:00"
            Top             =   300
            Width           =   1965
         End
         Begin VB.TextBox carTypeValid 
            Height          =   315
            Left            =   210
            Locked          =   -1  'True
            TabIndex        =   12
            Text            =   "CarType"
            Top             =   1080
            Width           =   885
         End
         Begin VB.TextBox carNoValid 
            Height          =   315
            Left            =   210
            Locked          =   -1  'True
            TabIndex        =   11
            Text            =   "CarNo"
            Top             =   690
            Width           =   615
         End
         Begin VB.TextBox TimeADJ 
            Height          =   315
            Left            =   930
            Locked          =   -1  'True
            TabIndex        =   10
            Text            =   "ADJ"
            Top             =   300
            Width           =   615
         End
         Begin VB.TextBox timeValid 
            Height          =   315
            Left            =   210
            Locked          =   -1  'True
            TabIndex        =   9
            Text            =   "TIME"
            Top             =   300
            Width           =   615
         End
         Begin VB.Label Label12 
            Caption         =   "CouplingUit"
            Height          =   225
            Left            =   240
            TabIndex        =   181
            Top             =   1530
            Width           =   825
         End
      End
      Begin VB.Frame Frame2 
         Caption         =   "TEST"
         Height          =   765
         Left            =   150
         TabIndex        =   3
         Top             =   240
         Width           =   2985
         Begin VB.TextBox testItemNo 
            Height          =   345
            Left            =   2430
            Locked          =   -1  'True
            TabIndex        =   7
            Text            =   "01"
            Top             =   240
            Width           =   375
         End
         Begin VB.TextBox testCat 
            Height          =   315
            Index           =   0
            Left            =   1470
            Locked          =   -1  'True
            TabIndex        =   6
            Text            =   "CAT1"
            Top             =   240
            Width           =   615
         End
         Begin VB.TextBox testCat 
            Height          =   315
            Index           =   1
            Left            =   810
            Locked          =   -1  'True
            TabIndex        =   5
            Text            =   "CAT2"
            Top             =   240
            Width           =   615
         End
         Begin VB.TextBox testExecute 
            Height          =   315
            Left            =   150
            Locked          =   -1  'True
            TabIndex        =   4
            Text            =   "EXEC"
            Top             =   240
            Width           =   615
         End
      End
   End
End
Attribute VB_Name = "frmSDR"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
 Option Explicit

Public Sub DoModeless()

    If menteMode = False Then
        Me.cmdSDRMonitor.Caption = "SDR Monitor"
        Me.TBLFrame.Visible = False
    End If
    
    Me.Show

End Sub

Private Sub cmdClose_Click()
    
    Unload Me

End Sub

Private Sub cmdSD_Click()

    SDFrame.ZOrder 0
    
End Sub

Private Sub cmdSDRMonitor_Click()

    If menteMode = True Then
        TBLFrame.ZOrder 0
    Else
        Me.TBLFrame.Visible = False
        SDRFrame.ZOrder 0
    End If
    
End Sub

Private Sub Command1_Click()

    SDRFrame.ZOrder 0

End Sub

Private Sub Form_Load()

    On Error Resume Next

    If menteMode = False Then
        Me.cmdSDRMonitor.Caption = "SDR Monitor"
        Me.TBLFrame.Visible = False
    End If
    
    If MDIMain.MenteCOMM.OnLine = False Then
        MsgBox "Not Online"
        Exit Sub
    End If
    MDIMain.MenteCOMM.SDRMonitor True

    frmSDR.Show vbModeless
    If Err <> 0 Then
        frmSDR.Hide
    Else
        frmSDR.ZOrder 1
    End If

End Sub

Private Sub Form_Unload(Cancel As Integer)

    MDIMain.MenteCOMM.SDRMonitor False
    MDIMain.MenteCOMM.OffLine

End Sub

