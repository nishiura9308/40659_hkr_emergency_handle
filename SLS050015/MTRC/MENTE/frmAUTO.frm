VERSION 5.00
Begin VB.Form frmAUTO 
   BorderStyle     =   1  'ŒÅ’è(ŽÀü)
   Caption         =   "AUTO Mode"
   ClientHeight    =   7185
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   10740
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MDIChild        =   -1  'True
   MinButton       =   0   'False
   ScaleHeight     =   7185
   ScaleWidth      =   10740
   Begin VB.Timer CheckTimer 
      Interval        =   1000
      Left            =   3840
      Top             =   6540
   End
   Begin VB.TextBox txtROUTE 
      Height          =   315
      Left            =   3450
      Locked          =   -1  'True
      TabIndex        =   57
      Text            =   "CAB/CAB"
      Top             =   5400
      Width           =   855
   End
   Begin VB.TextBox txtMCP 
      Height          =   315
      Left            =   4350
      Locked          =   -1  'True
      TabIndex        =   56
      Text            =   "CAB/CAB"
      Top             =   5400
      Width           =   945
   End
   Begin VB.Timer StartTimer 
      Interval        =   1000
      Left            =   3510
      Top             =   5580
   End
   Begin VB.TextBox phaseName 
      Height          =   315
      Left            =   3450
      Locked          =   -1  'True
      TabIndex        =   55
      Text            =   "CAB/CAB"
      Top             =   4980
      Width           =   3195
   End
   Begin VB.Frame ModeFrame 
      Caption         =   "MODE"
      Height          =   1455
      Left            =   3420
      TabIndex        =   47
      Top             =   3270
      Width           =   2475
      Begin VB.TextBox modeRespoince 
         Height          =   315
         Index           =   6
         Left            =   1650
         Locked          =   -1  'True
         TabIndex        =   54
         Text            =   "CHIME"
         Top             =   990
         Width           =   645
      End
      Begin VB.TextBox modeRespoince 
         Height          =   315
         Index           =   5
         Left            =   990
         Locked          =   -1  'True
         TabIndex        =   53
         Text            =   "DOOR"
         Top             =   990
         Width           =   645
      End
      Begin VB.TextBox modeRespoince 
         Height          =   315
         Index           =   4
         Left            =   210
         Locked          =   -1  'True
         TabIndex        =   52
         Text            =   "AUTO"
         Top             =   960
         Width           =   555
      End
      Begin VB.TextBox modeRespoince 
         Height          =   315
         Index           =   3
         Left            =   1410
         Locked          =   -1  'True
         TabIndex        =   51
         Text            =   "SPECIAL"
         Top             =   600
         Width           =   885
      End
      Begin VB.TextBox modeRespoince 
         Height          =   315
         Index           =   2
         Left            =   210
         Locked          =   -1  'True
         TabIndex        =   50
         Text            =   "EMERGENCY"
         Top             =   600
         Width           =   1185
      End
      Begin VB.TextBox modeRespoince 
         Height          =   315
         Index           =   1
         Left            =   1080
         Locked          =   -1  'True
         TabIndex        =   49
         Text            =   "CAB/CAB"
         Top             =   270
         Width           =   1035
      End
      Begin VB.TextBox modeRespoince 
         Height          =   315
         Index           =   0
         Left            =   210
         Locked          =   -1  'True
         TabIndex        =   48
         Text            =   "MANUAL"
         Top             =   270
         Width           =   825
      End
   End
   Begin VB.Frame EAHFrame 
      Caption         =   "EAH Talkback(Physical layout)"
      Height          =   3315
      Left            =   60
      TabIndex        =   6
      Top             =   3240
      Width           =   3315
      Begin VB.CheckBox chkEAH 
         Caption         =   "V3"
         Height          =   345
         Index           =   2
         Left            =   1410
         Style           =   1  '¸Þ×Ì¨¯¸½
         TabIndex        =   46
         Top             =   270
         Width           =   615
      End
      Begin VB.CheckBox chkEAH 
         Caption         =   "V4"
         Height          =   345
         Index           =   3
         Left            =   2040
         Style           =   1  '¸Þ×Ì¨¯¸½
         TabIndex        =   45
         Top             =   270
         Width           =   615
      End
      Begin VB.CheckBox chkEAH 
         Caption         =   "V5"
         Height          =   345
         Index           =   4
         Left            =   2670
         Style           =   1  '¸Þ×Ì¨¯¸½
         TabIndex        =   44
         Top             =   270
         Width           =   615
      End
      Begin VB.CheckBox chkEAH 
         Caption         =   "Z1"
         Height          =   345
         Index           =   5
         Left            =   150
         Style           =   1  '¸Þ×Ì¨¯¸½
         TabIndex        =   43
         Top             =   630
         Width           =   615
      End
      Begin VB.CheckBox chkEAH 
         Caption         =   "Z2"
         Height          =   345
         Index           =   6
         Left            =   780
         Style           =   1  '¸Þ×Ì¨¯¸½
         TabIndex        =   42
         Top             =   630
         Width           =   615
      End
      Begin VB.CheckBox chkEAH 
         Caption         =   "Z3"
         Height          =   345
         Index           =   7
         Left            =   1410
         Style           =   1  '¸Þ×Ì¨¯¸½
         TabIndex        =   41
         Top             =   630
         Width           =   615
      End
      Begin VB.CheckBox chkEAH 
         Caption         =   "Z4"
         Height          =   345
         Index           =   8
         Left            =   2040
         Style           =   1  '¸Þ×Ì¨¯¸½
         TabIndex        =   40
         Top             =   630
         Width           =   615
      End
      Begin VB.CheckBox chkEAH 
         Caption         =   "Z5"
         Height          =   345
         Index           =   9
         Left            =   2670
         Style           =   1  '¸Þ×Ì¨¯¸½
         TabIndex        =   39
         Top             =   630
         Width           =   615
      End
      Begin VB.CheckBox chkEAH 
         Caption         =   "X1"
         Height          =   345
         Index           =   10
         Left            =   150
         Style           =   1  '¸Þ×Ì¨¯¸½
         TabIndex        =   38
         Top             =   990
         Width           =   615
      End
      Begin VB.CheckBox chkEAH 
         Caption         =   "X2"
         Height          =   345
         Index           =   11
         Left            =   780
         Style           =   1  '¸Þ×Ì¨¯¸½
         TabIndex        =   37
         Top             =   990
         Width           =   615
      End
      Begin VB.CheckBox chkEAH 
         Caption         =   "X3"
         Height          =   345
         Index           =   12
         Left            =   1410
         Style           =   1  '¸Þ×Ì¨¯¸½
         TabIndex        =   36
         Top             =   990
         Width           =   615
      End
      Begin VB.CheckBox chkEAH 
         Caption         =   "X4"
         Height          =   345
         Index           =   13
         Left            =   2040
         Style           =   1  '¸Þ×Ì¨¯¸½
         TabIndex        =   35
         Top             =   990
         Width           =   615
      End
      Begin VB.CheckBox chkEAH 
         Caption         =   "X5"
         Height          =   345
         Index           =   14
         Left            =   2670
         Style           =   1  '¸Þ×Ì¨¯¸½
         TabIndex        =   34
         Top             =   990
         Width           =   615
      End
      Begin VB.CheckBox chkEAH 
         Caption         =   "W1"
         Height          =   345
         Index           =   15
         Left            =   150
         Style           =   1  '¸Þ×Ì¨¯¸½
         TabIndex        =   33
         Top             =   1350
         Width           =   615
      End
      Begin VB.CheckBox chkEAH 
         Caption         =   "W2"
         Height          =   345
         Index           =   16
         Left            =   780
         Style           =   1  '¸Þ×Ì¨¯¸½
         TabIndex        =   32
         Top             =   1350
         Width           =   615
      End
      Begin VB.CheckBox chkEAH 
         Caption         =   "W3"
         Height          =   345
         Index           =   17
         Left            =   1410
         Style           =   1  '¸Þ×Ì¨¯¸½
         TabIndex        =   31
         Top             =   1350
         Width           =   615
      End
      Begin VB.CheckBox chkEAH 
         Caption         =   "W4"
         Height          =   345
         Index           =   18
         Left            =   2040
         Style           =   1  '¸Þ×Ì¨¯¸½
         TabIndex        =   30
         Top             =   1350
         Width           =   615
      End
      Begin VB.CheckBox chkEAH 
         Caption         =   "W5"
         Height          =   345
         Index           =   19
         Left            =   2670
         Style           =   1  '¸Þ×Ì¨¯¸½
         TabIndex        =   29
         Top             =   1350
         Width           =   615
      End
      Begin VB.CheckBox chkEAH 
         Caption         =   "Y1"
         Height          =   345
         Index           =   20
         Left            =   150
         Style           =   1  '¸Þ×Ì¨¯¸½
         TabIndex        =   28
         Top             =   1710
         Width           =   615
      End
      Begin VB.CheckBox chkEAH 
         Caption         =   "Y2"
         Height          =   345
         Index           =   21
         Left            =   780
         Style           =   1  '¸Þ×Ì¨¯¸½
         TabIndex        =   27
         Top             =   1710
         Width           =   615
      End
      Begin VB.CheckBox chkEAH 
         Caption         =   "Y3"
         Height          =   345
         Index           =   22
         Left            =   1410
         Style           =   1  '¸Þ×Ì¨¯¸½
         TabIndex        =   26
         Top             =   1710
         Width           =   615
      End
      Begin VB.CheckBox chkEAH 
         Caption         =   "Y4"
         Height          =   345
         Index           =   23
         Left            =   2040
         Style           =   1  '¸Þ×Ì¨¯¸½
         TabIndex        =   25
         Top             =   1710
         Width           =   615
      End
      Begin VB.CheckBox chkEAH 
         Caption         =   "Y5"
         Height          =   345
         Index           =   24
         Left            =   2670
         Style           =   1  '¸Þ×Ì¨¯¸½
         TabIndex        =   24
         Top             =   1710
         Width           =   615
      End
      Begin VB.CheckBox chkEAH 
         Caption         =   "X1"
         Height          =   345
         Index           =   25
         Left            =   150
         Style           =   1  '¸Þ×Ì¨¯¸½
         TabIndex        =   23
         Top             =   2070
         Width           =   615
      End
      Begin VB.CheckBox chkEAH 
         Caption         =   "X2"
         Height          =   345
         Index           =   26
         Left            =   780
         Style           =   1  '¸Þ×Ì¨¯¸½
         TabIndex        =   22
         Top             =   2070
         Width           =   615
      End
      Begin VB.CheckBox chkEAH 
         Caption         =   "X3"
         Height          =   345
         Index           =   27
         Left            =   1410
         Style           =   1  '¸Þ×Ì¨¯¸½
         TabIndex        =   21
         Top             =   2070
         Width           =   615
      End
      Begin VB.CheckBox chkEAH 
         Caption         =   "X4"
         Height          =   345
         Index           =   28
         Left            =   2040
         Style           =   1  '¸Þ×Ì¨¯¸½
         TabIndex        =   20
         Top             =   2070
         Width           =   615
      End
      Begin VB.CheckBox chkEAH 
         Caption         =   "X5"
         Height          =   345
         Index           =   29
         Left            =   2670
         Style           =   1  '¸Þ×Ì¨¯¸½
         TabIndex        =   19
         Top             =   2070
         Width           =   615
      End
      Begin VB.CheckBox chkEAH 
         Caption         =   "Z1"
         Height          =   345
         Index           =   30
         Left            =   150
         Style           =   1  '¸Þ×Ì¨¯¸½
         TabIndex        =   18
         Top             =   2430
         Width           =   615
      End
      Begin VB.CheckBox chkEAH 
         Caption         =   "Z2"
         Height          =   345
         Index           =   31
         Left            =   780
         Style           =   1  '¸Þ×Ì¨¯¸½
         TabIndex        =   17
         Top             =   2430
         Width           =   615
      End
      Begin VB.CheckBox chkEAH 
         Caption         =   "Z3"
         Height          =   345
         Index           =   32
         Left            =   1410
         Style           =   1  '¸Þ×Ì¨¯¸½
         TabIndex        =   16
         Top             =   2430
         Width           =   615
      End
      Begin VB.CheckBox chkEAH 
         Caption         =   "Z4"
         Height          =   345
         Index           =   33
         Left            =   2040
         Style           =   1  '¸Þ×Ì¨¯¸½
         TabIndex        =   15
         Top             =   2430
         Width           =   615
      End
      Begin VB.CheckBox chkEAH 
         Caption         =   "Z5"
         Height          =   345
         Index           =   34
         Left            =   2670
         Style           =   1  '¸Þ×Ì¨¯¸½
         TabIndex        =   14
         Top             =   2430
         Width           =   615
      End
      Begin VB.CheckBox chkEAH 
         Caption         =   "V1"
         Height          =   345
         Index           =   35
         Left            =   150
         Style           =   1  '¸Þ×Ì¨¯¸½
         TabIndex        =   13
         Top             =   2790
         Width           =   615
      End
      Begin VB.CheckBox chkEAH 
         Caption         =   "V2"
         Height          =   345
         Index           =   36
         Left            =   780
         Style           =   1  '¸Þ×Ì¨¯¸½
         TabIndex        =   12
         Top             =   2790
         Width           =   615
      End
      Begin VB.CheckBox chkEAH 
         Caption         =   "V3"
         Height          =   345
         Index           =   37
         Left            =   1410
         Style           =   1  '¸Þ×Ì¨¯¸½
         TabIndex        =   11
         Top             =   2790
         Width           =   615
      End
      Begin VB.CheckBox chkEAH 
         Caption         =   "V4"
         Height          =   345
         Index           =   38
         Left            =   2040
         Style           =   1  '¸Þ×Ì¨¯¸½
         TabIndex        =   10
         Top             =   2790
         Width           =   615
      End
      Begin VB.CheckBox chkEAH 
         Caption         =   "V5"
         Height          =   345
         Index           =   39
         Left            =   2670
         Style           =   1  '¸Þ×Ì¨¯¸½
         TabIndex        =   9
         Top             =   2790
         Width           =   615
      End
      Begin VB.CheckBox chkEAH 
         Caption         =   "V1"
         Height          =   345
         Index           =   0
         Left            =   150
         Style           =   1  '¸Þ×Ì¨¯¸½
         TabIndex        =   8
         Top             =   270
         Width           =   615
      End
      Begin VB.CheckBox chkEAH 
         Caption         =   "V2"
         Height          =   345
         Index           =   1
         Left            =   780
         Style           =   1  '¸Þ×Ì¨¯¸½
         TabIndex        =   7
         Top             =   270
         Width           =   615
      End
   End
   Begin VB.Timer ExecTimer 
      Enabled         =   0   'False
      Interval        =   500
      Left            =   3360
      Top             =   4980
   End
   Begin VB.CommandButton cmdSelectRoute 
      Caption         =   "Select"
      Height          =   615
      Left            =   6990
      TabIndex        =   5
      Top             =   3660
      Width           =   1785
   End
   Begin VB.Frame RouteFrame 
      Caption         =   "Route Name"
      Height          =   3165
      Left            =   30
      TabIndex        =   1
      Top             =   30
      Width           =   10665
      Begin VB.PictureBox routePicture 
         Height          =   2685
         Left            =   120
         ScaleHeight     =   2625
         ScaleWidth      =   10425
         TabIndex        =   2
         Top             =   210
         Width           =   10485
         Begin VB.CheckBox Station 
            Caption         =   "Check1"
            Height          =   375
            Index           =   0
            Left            =   7410
            Style           =   1  '¸Þ×Ì¨¯¸½
            TabIndex        =   3
            Top             =   2100
            Visible         =   0   'False
            Width           =   795
         End
         Begin VB.Label STNtoSTN 
            BackColor       =   &H0000FFFF&
            Height          =   135
            Index           =   0
            Left            =   8190
            TabIndex        =   4
            Top             =   2220
            Visible         =   0   'False
            Width           =   195
         End
      End
   End
   Begin VB.CommandButton cmdEXECUTE 
      Caption         =   "EXECUTE"
      Height          =   615
      Left            =   7020
      TabIndex        =   0
      Top             =   4770
      Width           =   1785
   End
End
Attribute VB_Name = "frmAUTO"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Dim bdata(0 To ROUTE_SIZE - 1) As Byte
Dim mbdata(0 To 5) As Byte

Dim g_phase As Integer
Dim g_send_sdr As String

Dim selroute As Integer
Dim destStation As Integer
Dim stn_mode As Boolean
Dim stntostn_mode As Boolean
Dim wait_time As Integer
Dim now_station As Integer
Dim last_station As Integer
Dim ff_sim As Boolean

Public Sub DoModeless()

    If MDIMain.MenteCOMM.OnLine = False Then
        MsgBox "Not Online"
        Exit Sub
    End If
    MDIMain.MenteCOMM.SDRMonitor True

    frmAUTO.Show vbModeless
    frmAUTO.ZOrder 0
    
End Sub

Private Sub CheckTimer_Timer()
    Dim i As Integer

    For i = 0 To 6
        Me.modeRespoince(i) = frmSDR.modeRespoince(i)
    Next i

End Sub

Private Sub cmdEXECUTE_Click()

    If ExecTimer.Enabled = True Then
        cmdSelectRoute.Enabled = True
        cmdEXECUTE.Caption = "EXECUTE"
        ExecTimer.Enabled = False
    Else
        cmdSelectRoute.Enabled = False
        cmdEXECUTE.Caption = "STOP"
        ExecTimer.Enabled = True
        g_phase = 0
        now_station = 0
        stn_mode = True
        stntostn_mode = False
    End If

End Sub

Private Sub ExecTimer_Timer()
    Dim i As Integer

    If stn_mode = True Then
        For i = 0 To last_station
            If Station(i).Tag = bdata(1 + 1 + 6 + now_station * STATION_SIZE + 0) Then
                If Station(i).Value = 1 Then
                    Station(i).Value = 0
                Else
                    Station(i).Value = 1
                End If
            Else
                Station(i).Value = 0
            End If
            STNtoSTN(i).BackColor = &HFFFF&
        Next i
    End If

    If stntostn_mode = True Then
        For i = 1 To last_station
            If Station(i).Tag = bdata(1 + 1 + 6 + (now_station + 1) * STATION_SIZE + 0) Then
                If STNtoSTN(i).BackColor = &H0 Then
                    STNtoSTN(i).BackColor = &HFFFF&
                Else
                    STNtoSTN(i).BackColor = &H0
                End If
            Else
                STNtoSTN(i).BackColor = &HFFFF&
            End If
            Station(i).Value = 0
        Next i
    End If

    If g_phase = 0 Then
'        Exit Sub
        g_phase = 1
    End If


    g_send_sdr = ""

    Select Case g_phase
        ' ‰ŠúÝ’èÌª°½Þ
    Case 1
        phaseName.Text = "Initialize"

        now_station = 0

        g_send_sdr = "SDR1" & Space$(4) & Space$(4) & Space$(4) & Space$(2)

    Case 2
        g_send_sdr = "SDR3"
        g_send_sdr = g_send_sdr & Format$(Now, "dd/mm/yy hh:nn:ss")

    Case 3
        g_send_sdr = "SDR2"
        g_send_sdr = g_send_sdr + "TIME"
        g_send_sdr = g_send_sdr + "ADJ "
        g_send_sdr = g_send_sdr + "     "
        g_send_sdr = g_send_sdr + "       "
        
    Case 4
        g_send_sdr = "SDR6"
        g_send_sdr = g_send_sdr & "000"

    Case 5
        g_send_sdr = "SDRA"
        g_send_sdr = g_send_sdr + "  "
        g_send_sdr = g_send_sdr + "  "
        g_send_sdr = g_send_sdr + "    "
        g_send_sdr = g_send_sdr + "OFF"
    
    Case 6
        g_send_sdr = "SDR8"
        For i = 0 To 39
            If chkEAH(i).Value = 1 Then
                g_send_sdr = g_send_sdr & Left$(chkEAH(i).Caption + Space$(4), 4)
            Else
                g_send_sdr = g_send_sdr & Space$(4)
            End If
        Next i
        
    Case 7
        g_send_sdr = "SDR9"
        g_send_sdr = g_send_sdr + "  "
        g_send_sdr = g_send_sdr + "  "

    Case 8
        g_send_sdr = "SDR7"
        g_send_sdr = g_send_sdr & Right$(String$(2, "0") + Hex$(bdata(1 + 1 + 6 + now_station * STATION_SIZE + 0)), 2)
        g_send_sdr = g_send_sdr & Right$(String$(2, "0") + Hex$(destStation), 2)
        g_send_sdr = g_send_sdr & Right$(String$(2, "0") + Hex$(bdata(0)), 2)
        g_send_sdr = g_send_sdr & String$(2, "0")

        wait_time = 0
        
    Case 9
        g_send_sdr = "SDRA"
        g_send_sdr = g_send_sdr + "  "
        g_send_sdr = g_send_sdr + "  "
        g_send_sdr = g_send_sdr + "    "
        g_send_sdr = g_send_sdr + "   "
    
        stn_mode = True
        stntostn_mode = False

        Call ReadIniItem("TMSSIM", "AUTO", "DepartureWait", 10, wait_time)
        'Departure wait
    Case 10
        phaseName.Text = "Departure Waiting" & str$(wait_time)
        If wait_time > 0 Then
            wait_time = wait_time - 1
            Exit Sub
        End If

Beep
        Station(0).Value = 0
        
        now_station = 0
        
        g_send_sdr = "SDR9    "

        Station(0).Value = 0
        stn_mode = False
        stntostn_mode = True

        Call ReadIniItem("TMSSIM", "AUTO", "SpeedWait", 10, wait_time)

        'Leaving Wait
    Case 11
        phaseName.Text = "Wait for SPEED" & str$(wait_time)
        If wait_time > 0 Then
            wait_time = wait_time - 1
            Exit Sub
        End If
Beep

        For i = 0 To 5
            mbdata(i) = bdata(1 + 1 + i)
        Next i
        txtROUTE.Text = getMCPString(mbdata)
        
        For i = 0 To 5
            mbdata(i) = bdata(1 + 1 + 6 + (now_station + 1) * STATION_SIZE + 2 + i)
        Next i
        txtMCP.Text = getMCPString(mbdata)
        
        g_send_sdr = "SDR7"
        g_send_sdr = g_send_sdr & Right$(String$(2, "0") + Hex$(bdata(1 + 1 + 6 + (now_station + 1) * STATION_SIZE + 0)), 2)
        g_send_sdr = g_send_sdr & Right$(String$(2, "0") + Hex$(destStation), 2)
        g_send_sdr = g_send_sdr & Right$(String$(2, "0") + Hex$(bdata(0)), 2)
        g_send_sdr = g_send_sdr & String$(2, "0")
        
        'Leaving Broadcast
    Case 12
        phaseName.Text = "Leaving Announce"
Beep
        g_send_sdr = "SDR9"
        g_send_sdr = g_send_sdr + "  "
        g_send_sdr = g_send_sdr + "DB"


        Call ReadIniItem("TMSSIM", "AUTO", "LeavingWait", 10, wait_time)

        'Door
    Case 13
        phaseName.Text = "Leaving Announce" & str$(wait_time)
        
        If wait_time > 0 Then
            wait_time = wait_time - 1
            If ff_sim = True Then
                ff_sim = False
                MDIMain.MenteCOMM.SDRequest
            Else
                g_send_sdr = "SDRA"
                g_send_sdr = g_send_sdr + "  "
                g_send_sdr = g_send_sdr + "  "
                g_send_sdr = g_send_sdr + "    "
                g_send_sdr = g_send_sdr + "   "
    
                If MDIMain.MenteCOMM.SetRequest(g_send_sdr) = True Then
                    ff_sim = True
                End If
            End If
            Exit Sub
        End If

        If frmSDR.modeRespoince(4) = "AUTO" Then
            If ff_sim = True Then
                ff_sim = False
                MDIMain.MenteCOMM.SDRequest
            Else
                g_send_sdr = "SDRA"
                g_send_sdr = g_send_sdr + "  "
                g_send_sdr = g_send_sdr + "  "
                g_send_sdr = g_send_sdr + "    "
                g_send_sdr = g_send_sdr + "   "
    
                If MDIMain.MenteCOMM.SetRequest(g_send_sdr) = True Then
                    ff_sim = True
                End If
            End If
            Exit Sub
        End If

        g_send_sdr = "SDR9    "
    
    Case 14
'        phaseName.Text = "Door Announce"
Beep
        Call ReadIniItem("TMSSIM", "AUTO", "ArrivingWait", 10, wait_time)
    Case 15
        phaseName.Text = "Arriving" & str$(wait_time)
        If wait_time > 0 Then
            wait_time = wait_time - 1
            Exit Sub
        End If

Beep
        For i = 0 To 5
            mbdata(i) = bdata(1 + 1 + i)
        Next i
        txtROUTE.Text = getMCPString(mbdata)
        
        For i = 0 To 5
            mbdata(i) = bdata(1 + 1 + 6 + (now_station + 1) * STATION_SIZE + 2 + i)
        Next i
        txtMCP.Text = getMCPString(mbdata)
        
        'Arriving/Arrived Set
        g_send_sdr = "SDR6"
        g_send_sdr = g_send_sdr & Right$(String$(2, "0") + Hex$(bdata(1 + 1 + 6 + (now_station + 1) * STATION_SIZE + 8) And &H1F), 2)
        g_send_sdr = g_send_sdr & Right$(String$(1, "0") + Hex$((bdata(1 + 1 + 6 + (now_station + 1) * STATION_SIZE + 8) And &HE0) / 32), 1)

        'Arriving Flag
    Case 16
        If bdata(1 + 1 + 6 + (now_station + 1) * STATION_SIZE + 9) And &H80 Then
            phaseName.Text = "Arriving Announce"
Beep
            g_send_sdr = "SDR9"
'            g_send_sdr = g_send_sdr + "PA"
'            g_send_sdr = g_send_sdr + "  "
            g_send_sdr = g_send_sdr + "  "
            g_send_sdr = g_send_sdr + "DB"
        ElseIf bdata(1 + 1 + 6 + (now_station + 1) * STATION_SIZE + 9) And &H40 Then
            phaseName.Text = "Arrived Announce"
Beep
            g_send_sdr = "SDR9"
'            g_send_sdr = g_send_sdr + "PA"
'            g_send_sdr = g_send_sdr + "  "
            g_send_sdr = g_send_sdr + "  "
            g_send_sdr = g_send_sdr + "DB"
        End If
        

        Call ReadIniItem("TMSSIM", "AUTO", "ArrivingAnnounceWait", 10, wait_time)
    Case 17
        If bdata(1 + 1 + 6 + (now_station + 1) * STATION_SIZE + 9) And &H80 Then
            phaseName.Text = "Arriving Announce" & str$(wait_time)
            If wait_time > 0 Then
                wait_time = wait_time - 1
                If ff_sim = True Then
                    ff_sim = False
                    MDIMain.MenteCOMM.SDRequest
                Else
                    g_send_sdr = "SDRA"
                    g_send_sdr = g_send_sdr + "  "
                    g_send_sdr = g_send_sdr + "  "
                    g_send_sdr = g_send_sdr + "    "
                    g_send_sdr = g_send_sdr + "   "
                    
                    If MDIMain.MenteCOMM.SetRequest(g_send_sdr) = True Then
                        ff_sim = True
                    End If
                End If
                Exit Sub
            End If
        ElseIf bdata(1 + 1 + 6 + (now_station + 1) * STATION_SIZE + 9) And &H40 Then
            phaseName.Text = "Arrived Announce" & str$(wait_time)
            If wait_time > 0 Then
                wait_time = wait_time - 1
                If ff_sim = True Then
                    ff_sim = False
                    MDIMain.MenteCOMM.SDRequest
                Else
                    g_send_sdr = "SDRA"
                    g_send_sdr = g_send_sdr + "  "
                    g_send_sdr = g_send_sdr + "  "
                    g_send_sdr = g_send_sdr + "    "
                    g_send_sdr = g_send_sdr + "   "
        
                    If MDIMain.MenteCOMM.SetRequest(g_send_sdr) = True Then
                        ff_sim = True
                    End If
                End If
                Exit Sub
            End If
        End If
        
        If frmSDR.modeRespoince(4) = "AUTO" Then
            If ff_sim = True Then
                ff_sim = False
                MDIMain.MenteCOMM.SDRequest
            Else
                g_send_sdr = "SDRA"
                g_send_sdr = g_send_sdr + "  "
                g_send_sdr = g_send_sdr + "  "
                g_send_sdr = g_send_sdr + "    "
                g_send_sdr = g_send_sdr + "   "
    
                If MDIMain.MenteCOMM.SetRequest(g_send_sdr) = True Then
                    ff_sim = True
                End If
            End If
            Exit Sub
        End If

        
        g_send_sdr = "SDR9    "

    Case 18
        now_station = now_station + 1
        
        If bdata(1 + 1 + 6 + (now_station + 1) * STATION_SIZE + 9) <> 0 Then
            For i = 0 To last_station
                If Station(i).Tag = bdata(1 + 1 + 6 + (now_station + 1) * STATION_SIZE + 0) Then
                    Exit For
                End If
            Next i
            
            If i <= last_station Then
                If bdata(1 + 1 + 6 + (now_station + 1) * STATION_SIZE + 0) = Station(i).Tag Then
                    g_phase = 14
                    Exit Sub
                End If
            End If
        End If


        Call ReadIniItem("TMSSIM", "AUTO", "StopStationWait", 10, wait_time)
        'Station Wait
    Case 19
        stn_mode = True
        stntostn_mode = False
        
        phaseName.Text = "Stop Station" & str$(wait_time)
        
        If wait_time > 0 Then
            wait_time = wait_time - 1
            Exit Sub
        End If

        If bdata(1 + 1 + 6 + (now_station + 1) * STATION_SIZE + 0) <> 0 Then
            g_phase = 11
            
            stn_mode = False
            stntostn_mode = True
            
            phaseName.Text = "Start Station"
Beep
            Exit Sub
        Else
            now_station = 0
            g_phase = 8
            
            stn_mode = False
            stntostn_mode = True
            Exit Sub
        End If
    End Select
        
    If g_send_sdr <> "" Then
        If MDIMain.MenteCOMM.SetRequest(g_send_sdr) = True Then
            g_phase = g_phase + 1
        End If
    Else
        g_phase = g_phase + 1
    End If
        
End Sub

Private Sub cmdSelectRoute_Click()
    Dim selno As Integer
    Dim i As Integer

    selno = frmRouteSelect.DoModal
    If selno = 0 Then
        Unload Me
        Exit Sub
    End If

    selroute = selno

    On Error Resume Next
    For i = 1 To 99
        Unload Station(i)
        Unload STNtoSTN(i)
    Next i
    
    RouteShow

End Sub

Private Sub Form_Load()

    On Error Resume Next

    For selroute = 1 To 99
        If Dir(App.Path & "\ROUTE" & Right$("0" + Hex$(selroute), 2) & ".DAT") <> "" Then
            Exit For
        End If
    Next selroute
    If selroute > 99 Then selroute = 1

    RouteShow

End Sub


Private Sub RouteShow()
    Dim i As Integer
    Dim j As Integer
    Dim fno As Integer

    RouteFrame.Caption = "Route " & Right$("0" + Hex$(selroute), 2) & " : " & GetRouteName(selroute)

    If befDateMode = False Then
        If Dir(App.Path & "\ROUTE" & Right$("0" + Hex$(selroute), 2) & ".DAT") = "" Then
            MsgBox "No Station Data"
            Exit Sub
        End If
    Else
        If Dir(App.Path & "\ROUTE" & Right$("0" + Hex$(selroute), 2) & ".BEF") = "" Then
            MsgBox "No Station Data"
            Exit Sub
        End If
    End If

    fno = FreeFile
    If befDateMode = False Then
        Open App.Path & "\ROUTE" & Right$("0" + Hex$(selroute), 2) & ".DAT" For Binary As fno
    Else
        Open App.Path & "\ROUTE" & Right$("0" + Hex$(selroute), 2) & ".BEF" For Binary As fno
    End If
    Get #fno, , bdata
    Close fno

    j = 0
    For i = 0 To 99
        If j = 0 Then
            Station(j).Caption = GetStationName(CInt(Hex$(bdata(1 + 1 + 6 + i * STATION_SIZE + 0))))
            Station(j).Visible = True
            Station(j).Top = 200
            Station(j).Left = 100
            Station(j).Tag = bdata(1 + 1 + 6 + i * STATION_SIZE + 0)
            Station(j).Value = 0
            j = j + 1
        Else
            If bdata(1 + 1 + 6 + i * STATION_SIZE + 0) = 0 Then Exit For
            
            destStation = CInt(bdata(1 + 1 + 6 + i * STATION_SIZE))

            ' Same Station Code is SKIP
            If Station(j - 1).Tag <> bdata(1 + 1 + 6 + i * STATION_SIZE + 0) Then
                Call Load(STNtoSTN(j))
                STNtoSTN(j).Visible = True
                STNtoSTN(j).Top = Station(j - 1).Top + Station(j - 1).Height / 2 - STNtoSTN(j).Height / 2
                STNtoSTN(j).Left = Station(j - 1).Left + Station(j - 1).Width
                STNtoSTN(j).Tag = bdata(1 + 1 + 6 + (i - 1) * STATION_SIZE + 0)
                STNtoSTN(j).BackColor = &HFFFF&
            
                Call Load(Station(j))
                Station(j).Caption = GetStationName(CInt(Hex$(bdata(1 + 1 + 6 + i * STATION_SIZE + 0))))
                Station(j).Visible = True
                Station(j).Top = Station(j - 1).Top
                Station(j).Left = STNtoSTN(j).Left + STNtoSTN(j).Width
                Station(j).Tag = bdata(1 + 1 + 6 + i * STATION_SIZE + 0)
                Station(j).Value = 0
                last_station = j
    
                If (j Mod 10) = 0 Then
                    Station(j).Top = Station(j).Top + Station(j).Height * 2
                    Station(j).Left = 200
                End If
                j = j + 1
            End If
        End If
    Next i

    If i = 0 Then
        MsgBox "No Station Data"
        Exit Sub
    End If

    If j > 0 Then
        routePicture.Height = Station(j - 1).Top + Station(j - 1).Height * 2
    End If

    RouteFrame.Visible = True
    RouteFrame.Height = RouteFrame.Top + routePicture.Height + 200

    EAHFrame.Top = RouteFrame.Top + RouteFrame.Height + 10
    ModeFrame.Top = EAHFrame.Top
    
    phaseName.Top = ModeFrame.Top + ModeFrame.Height
    txtROUTE.Top = phaseName.Top + phaseName.Height
    txtMCP.Top = txtROUTE.Top
    
    cmdSelectRoute.Top = EAHFrame.Top + 200
    cmdEXECUTE.Top = cmdSelectRoute.Top + 1000
    Me.Height = EAHFrame.Top + EAHFrame.Height + 500
    
End Sub

Private Sub Form_Unload(Cancel As Integer)

    MDIMain.MenteCOMM.OffLine
    Unload frmSDR

End Sub

Private Sub StartTimer_Timer()

    If MDIMain.MenteCOMM.STARTRequest = True Then
        StartTimer.Enabled = False
    End If

End Sub
