VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
  Persistable = 0  'NotPersistable
  DataBindingBehavior = 0  'vbNone
  DataSourceBehavior  = 0  'vbNone
  MTSTransactionMode  = 0  'NotAnMTSObject
END
Attribute VB_Name = "clsMenteComm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False

Option Explicit

Event DebugSend(ByVal str As String)
Event DebugRecv(ByVal str As String)
Event RecvSDR(ByVal str As String)
Event RecvSD(ByVal str As String)
Event RecvTBL(ByVal str As String)
Event RecvTrace(ByVal str As String)
Event RecvOK(ByVal bOK As Boolean)
Event RecvDownLoad(ByVal filename As String, ByVal nowSize As Long, ByVal allSize As Long, ByVal bOK As Boolean)
Event CommAction(ByVal msg As String)
Event recvLDR(ByVal str As String)
Event recvLD(ByVal str As String)

Private requestType As Integer

Dim gintCommPort As Integer             '// 通信ポートの番号を設定または取得します
Dim gstrSettings As String              '// 通信速度，パリティ，データ長，ストップ ビット長を文字列で設定または取得します

Dim onlineCount As Integer

Dim frmInvisual As InvisualForm
Dim WithEvents CheckTimer As Timer
Attribute CheckTimer.VB_VarHelpID = -1
Dim WithEvents TimeOutTimer As Timer
Attribute TimeOutTimer.VB_VarHelpID = -1
Dim WithEvents mmscMSComm As MSComm     '// 通信ｺﾝﾄﾛｰﾙ
Attribute mmscMSComm.VB_VarHelpID = -1
Dim WithEvents SimulateTimer As Timer
Attribute SimulateTimer.VB_VarHelpID = -1

Private requestSDRMonitor As Boolean
Private sdrPhase As Integer
Private mblnResponseFlag As Boolean             '// 送信したｺﾏﾝﾄﾞが返信されたか判定 (True = OK  False = NG)
Private mintRetriedCount As Integer             '// ｺﾏﾝﾄﾞの現在のﾘﾄﾗｲ数
Private mstrRecvBuff As String          '// 通信用仮想ﾊﾞｯﾌｧ
Private recvPhase As Integer
Private recvBcc1 As Long                '// 受信中BCC
Private recvBcc2 As Long                '// 受信中BCC
Private recvBcc3 As Long                '// 受信中BCC

'ダウンロード用
Private iSequence As Integer            '// 連番カウンター
Private downFD As Integer               '// ダウンロードファイルID
Private downTextMode As Boolean
Private bdata() As Byte
Private downFileName As String
Private downRemainSize As Long
Private downFileDate As Date            '// ファイルの日時

Public AbortFlag As Boolean             '転送キャンセルフラグ

Private simPhase As Integer

Dim bytSend() As Byte

Private Sub Class_Initialize()

    Set frmInvisual = New InvisualForm
    Load frmInvisual

    Set CheckTimer = frmInvisual.CheckTimer
    Set TimeOutTimer = frmInvisual.TimeOutTimer
    Set mmscMSComm = frmInvisual.mscMSComm
    Set SimulateTimer = frmInvisual.SimTimer

    gintCommPort = 1
    gstrSettings = "9600,n,8,1"

    onlineCount = 0
    recvPhase = 0
    requestType = 0

    '// すべてのﾘﾄﾗｲ処理をｸﾘｱ
    RetriedTimerClear
End Sub

Private Sub Class_Terminate()

    On Error Resume Next
    
    mmscMSComm.PortOpen = False
        
    '// ｺﾝﾄﾛｰﾙの解放
    Set mmscMSComm = Nothing
    Set CheckTimer = Nothing
    Set TimeOutTimer = Nothing
    Set SimulateTimer = Nothing
    Unload frmInvisual
    Set frmInvisual = Nothing

End Sub

'/////  通信ポートの取得　/////
Public Property Get CommPort() As Integer
    CommPort = gintCommPort
    
End Property

'/////  通信ポートの設定　/////
Public Property Let CommPort(ByVal intCommPort As Integer)

    '// ﾌﾟﾛﾊﾟﾃｨの範囲ﾁｪｯｸ
    If 1 <= intCommPort And intCommPort <= 16 Then
        '// 範囲外の値ならば無視
        gintCommPort = intCommPort
        mmscMSComm.CommPort = gintCommPort
    End If
    
End Property

'/////  通信プロパティの取得　/////
Public Property Get Settings() As String
    Settings = gstrSettings
    
End Property

'/////  通信プロパティの設定　/////
Public Property Let Settings(ByRef strSettings As String)
    gstrSettings = strSettings
    
End Property

'/////  通信ポート開閉の取得　/////
Public Property Get PortOpen() As Boolean
    PortOpen = mmscMSComm.PortOpen
    
End Property

'/////  通信ポート開閉の設定　/////
Public Property Let PortOpen(ByVal blnPortOpen As Boolean)

    '// ｴﾗｰ時は無視してしまう
    On Error Resume Next

    '// ﾎﾟｰﾄの開閉
    mmscMSComm.PortOpen = blnPortOpen
    
End Property

Public Property Get RequestMode() As Integer
    RequestMode = requestType
End Property

Public Property Get OnlineStatus() As Integer
    OnlineStatus = onlineCount
End Property

'/////  通信イベント  /////
Private Sub mmscMSComm_OnComm()

    '// 通信処理
    Select Case mmscMSComm.CommEvent
        '// ｴﾗｰ処理
        Case comEventBreak      '// 中断信号が受信された
        Case comEventCDTO       '// CD (RLSD)ﾀｲﾑｱｳﾄ
        Case comEventCTSTO      '// CTSﾀｲﾑｱｳﾄ
        Case comEventDSRTO      '// DSRﾀｲﾑｱｳﾄ
        Case comEventFrame      '// ﾌﾚｰﾑｴﾗｰ
    RaiseEvent DebugRecv("Frame Error")
        Case comEventOverrun    '// ﾃﾞｰﾀが失われた
    RaiseEvent DebugRecv("Overrun")
        Case comEventRxOver     '// 受信ﾊﾞｯﾌｧがｵｰﾊﾞｰﾌﾛｰ
    RaiseEvent DebugRecv("RxOver")
        Case comEventRxParity   '// ﾊﾟﾘﾃｨｴﾗｰ
    RaiseEvent DebugRecv("Parity Error")
        Case comEventTxFull     '// 送信ﾊﾞｯﾌｧがいっぱい
        Case comEventDCB        '// ﾃﾞﾊﾞｲｽｺﾝﾄﾛｰﾙﾌﾞﾛｯｸ(DCB)が取得される予期しないｴﾗｰ
        
        '// ｲﾍﾞﾝﾄ処理
        Case comEvCD            '// CD ﾗｲﾝの状態が変化
        Case comEvCTS           '// CTS ﾗｲﾝの状態が変化
        Case comEvDSR           '// DSR ﾗｲﾝの状態が変化
        Case comEvRing          '// ﾘﾝｸﾞｲﾝｼﾞｹﾞｰﾀの状態が変化
        Case comEvReceive       '// 受信ｲﾍﾞﾝﾄ (受信ﾊﾞｯﾌｧに RThreshold ﾌﾟﾛﾊﾟﾃｨで指定された数の文字を受信)
            MSCommRecvEvents
        Case comEvSend          '// 送信ｲﾍﾞﾝﾄ (送信ﾊﾞｯﾌｧに SThreshold ﾌﾟﾛﾊﾟﾃｨで指定された数の文字が入っている)
            MSCommSendEvents
        Case comEvEOF           '// 入力ｽﾄﾘｰﾑ内に EOF 文字が検出された
    End Select

End Sub

'/////  送信イベント　/////
Private Sub MSCommSendEvents()
    
    '＜特になし＞

End Sub

'/////  受信イベント　/////
Private Sub MSCommRecvEvents()
    Dim sRecvData As String

    'すべての RS-232C受信処理は、いったんここで処理される
    
    '// 変数宣言及び初期設定
    Dim blnRecvFlag As Boolean
    
    '// 受信処理
    If SetRecvBuff(sRecvData) = True Then    '// 仮想受信ﾊﾞｯﾌｧにｺﾏﾝﾄﾞがある場合
        
        TimeOutTimer.Enabled = False
        
        blnRecvFlag = RecvCommandLoop(sRecvData)
        
        TimeOutTimer.Enabled = True
    End If

End Sub

'/////　仮想受信バッファ格納処理　/////
'
'
'   <STX>+<TEXT>+<BCC text 2char.>+<ETX>
'   ex.
'        1   23   45  6
'       02h 'AB' '83' 03h
'            ^^   ^^
'            ||   ||
'            ||   |+--> recvbcc1
'            ||   +---> recvbcc2
'            |+-------> recvbcc3
'            +-------->
'
Private Function SetRecvBuff(ByRef sRecvData As String) As Boolean

    '// 変数宣言及び初期設定
    Dim intRecvCount As Integer
    Dim intCount As Integer
    Dim intBuffMax As Integer
    Dim varByteBuff As Variant
    Dim bytRecv() As Byte
    Dim iPos As Integer
    
    '// 受信ﾊﾞｯﾌｧ格納
    SetRecvBuff = False
    intRecvCount = mmscMSComm.InBufferCount '// 受信ﾊﾞｯﾌｧの確認
    
    If intRecvCount <> 0 Then               '// 受信ﾊﾞｯﾌｧが 0でない場合は仮想ﾊﾞｯﾌｧに格納する
        '// Byte型の配列の格納
        varByteBuff = mmscMSComm.Input
        bytRecv = varByteBuff
        
        '// 仮想受信ﾊﾞｯﾌｧの処理
        For intCount = LBound(bytRecv) To UBound(bytRecv)
            If bytRecv(intCount) = 2 Then   'STX
               If mstrRecvBuff <> "" Then
 Debug.Print "Illegal : "; mstrRecvBuff
                    RaiseEvent DebugRecv(mstrRecvBuff)
                End If
                mstrRecvBuff = ""
                recvPhase = 1
                recvBcc1 = 0
                recvBcc2 = 0
                recvBcc3 = 0
            ElseIf recvPhase = 1 Then
                If bytRecv(intCount) = 3 Then 'ETX
                    recvPhase = 0
                    iPos = Len(mstrRecvBuff)
                    If iPos > 2 Then
                        recvBcc1 = Val("&H" + Mid$(mstrRecvBuff, iPos - 1, 2))
                        If recvBcc1 = (recvBcc3 And 255) Then
                            SetRecvBuff = True
                            sRecvData = Mid$(mstrRecvBuff, 1, iPos - 2)
                            mstrRecvBuff = ""
'Debug.Print mstrRecvBuff
                        Else
                        RaiseEvent DebugRecv("BCC Error")
                        End If
                    End If
                Else
                    recvBcc3 = recvBcc2
                    recvBcc2 = recvBcc1
                    recvBcc1 = recvBcc1 + bytRecv(intCount)
                    If bytRecv(intCount) >= &H20 And bytRecv(intCount) <= &H7F Then
                        mstrRecvBuff = mstrRecvBuff & Chr$("&H" + Right$("00" + Hex$(bytRecv(intCount)), 2))
                    Else
                        recvPhase = 0
                    End If
                End If
            Else
                If bytRecv(intCount) >= &H20 And bytRecv(intCount) <= &H7F Then
                    mstrRecvBuff = mstrRecvBuff & Chr$("&H" + Right$("00" + Hex$(bytRecv(intCount)), 2))
                End If
            End If
        Next
    End If
'Debug.Print mstrRecvBuff
End Function

'/////  データ受信処理　/////
Private Function RecvCommandLoop(ByVal sRecvData As String) As Boolean
    '// 変数宣言及び初期設定
    Dim strRecvCommand As String
    
    RecvCommandLoop = False

    RaiseEvent DebugRecv(sRecvData)

    TimeOutTimer.Enabled = False
    TimeOutTimer.Enabled = True
    
    If AbortFlag = True Then
        AbortFlag = False

        CANCELRequest
        
        RecvCommandLoop = True
        Exit Function
    End If

    If Mid$(sRecvData, 1, 3) = "SDR" Then
        If requestType <= 1 Then
            mblnResponseFlag = True
            mintRetriedCount = 0
        End If
        If Mid$(sRecvData, 1, 4) = "SDRB" Then
            requestType = 0
        End If
        RaiseEvent RecvSDR(sRecvData)
        RaiseEvent CommAction("Recv SDR")
        ACKRequest
        RecvCommandLoop = True
        Exit Function
    End If
    
    If Mid$(sRecvData, 1, 2) = "SD" Then
        If requestType <= 1 Then
            mblnResponseFlag = True
            mintRetriedCount = 0
        End If
        If Mid$(sRecvData, 1, 3) = "SD5" Then
            requestType = 0
        End If
        RaiseEvent RecvSD(sRecvData)
        RaiseEvent CommAction("Recv SD")
        ACKRequest
        RecvCommandLoop = True
        Exit Function
    End If
    
    If Mid$(sRecvData, 1, 3) = "TRB" Then
        If requestType <= 1 Then
            mblnResponseFlag = True
            mintRetriedCount = 0
        End If
        If Mid$(sRecvData, 1, 4) = "TRB9" Then
            requestType = 0
        End If
        RaiseEvent RecvTBL(sRecvData)
        RaiseEvent CommAction("Recv TRB")
        ACKRequest
        RecvCommandLoop = True
        Exit Function
    End If
    
    If Mid$(sRecvData, 1, 5) = "TRACE" Then
        If requestType = 3 Then
            mblnResponseFlag = True
            mintRetriedCount = 0
        End If
        RaiseEvent RecvTrace(Mid$(sRecvData, 6))
        If Mid$(sRecvData, 6) = "END" Then
            requestType = 0
        End If
        RaiseEvent CommAction("Recv TRACE")
        ACKRequest
        RecvCommandLoop = True
        Exit Function
    End If
    
    If Mid$(sRecvData, 1, 4) = "WAIT" Then
        mintRetriedCount = 0
        RecvCommandLoop = True
        RaiseEvent CommAction("Recv WAIT")
        mblnResponseFlag = True
        ACKRequest
'        requestType = 0
        Exit Function
    End If

    If Mid$(sRecvData, 1, 2) = "OK" Then
        RaiseEvent CommAction("Recv OK")
        Select Case requestType
        Case 1  'Wait Ack
            RaiseEvent RecvOK(True)
            requestType = 0
            mblnResponseFlag = True
            mintRetriedCount = 0
            RecvCommandLoop = True
            Exit Function
        Case 2  'Wait Ack
            RaiseEvent RecvOK(True)
            requestType = 0
            mblnResponseFlag = True
            mintRetriedCount = 0
            RecvCommandLoop = True
            Exit Function
        Case 3  'TRACE
            RaiseEvent RecvOK(True)
            requestType = 0
            mblnResponseFlag = True
            mintRetriedCount = 0
            RecvCommandLoop = True
            Exit Function
        Case 4  'DownLoad
            RaiseEvent RecvDownLoad(downFileName, LOF(downFD) - downRemainSize, LOF(downFD), True)
            DownContinue
            mintRetriedCount = 0
            RecvCommandLoop = True
            Exit Function
        Case 5  'Wait Download END
            RaiseEvent RecvDownLoad(downFileName, 0, 0, True)
            requestType = 0
            mblnResponseFlag = True
            mintRetriedCount = 0
            RecvCommandLoop = True
            Exit Function
        Case 6  'LDR
            RaiseEvent recvLDR("OK")
            requestType = 0
            mblnResponseFlag = True
            mintRetriedCount = 0
            RecvCommandLoop = True
            Exit Function
        End Select
    End If
            
    If Mid$(sRecvData, 1, 2) = "NG" Then
        RaiseEvent CommAction("Recv NG")
        mintRetriedCount = mintRetriedCount + 1
        If mintRetriedCount < 3 Then
            '// ｺｰﾄﾞ送信
            If bSimulate = 0 Then
                mmscMSComm.Output = bytSend
            Else
                SimulateTimer.Interval = 500
                SimulateTimer.Enabled = True
            End If
            Exit Function
        End If
    
        mblnResponseFlag = True
        mintRetriedCount = 0
        Select Case requestType
        Case 1 'Wait ACK
            RaiseEvent RecvOK(False)
            requestType = 0
            RecvCommandLoop = True
            Exit Function
        Case 2 'Wait ACK
            RaiseEvent RecvOK(False)
            requestType = 0
            RecvCommandLoop = True
            Exit Function
        Case 3 'TRACE
            RaiseEvent RecvOK(False)
            requestType = 0
            RecvCommandLoop = True
            Exit Function
        Case 4  'DownLoad
            RaiseEvent RecvDownLoad(downFileName, LOF(downFD) - downRemainSize, LOF(downFD), False)
            requestType = 0
            RecvCommandLoop = True
            Exit Function
        Case 5  'Wait Download END
            RaiseEvent RecvDownLoad(downFileName, 0, 0, False)
            requestType = 0
            RecvCommandLoop = True
            Exit Function
        Case 6  'LDR Send END
            RaiseEvent recvLDR("NG")
            requestType = 0
            RecvCommandLoop = True
            Exit Function
        End Select
    End If
    
    If Mid$(sRecvData, 1, 2) = "LD" Then
        RaiseEvent recvLD("LD OK")
        RaiseEvent CommAction("Recv LD")
        requestType = 0
        ACKRequest
        RecvCommandLoop = True
        Exit Function
    End If
    
    RaiseEvent CommAction("Recv unknown")
   
    RecvCommandLoop = True
   
End Function

'/////　リトライ処理を初期化　/////
Private Sub RetriedTimerClear()

    '// ﾘﾄﾗｲ関連をｸﾘｱ
    mblnResponseFlag = False
    mintRetriedCount = 0

End Sub


Public Function OnLine() As Boolean

    On Error Resume Next

    AbortFlag = False

    If mmscMSComm.PortOpen = True Then
        onlineCount = onlineCount + 1

        OnLine = True
        Exit Function
    End If

    onlineCount = 1
    
    mmscMSComm.CommPort = gintCommPort
    mmscMSComm.Settings = gstrSettings
    
    Err.Clear
    mmscMSComm.PortOpen = True
    If Err <> 0 Then
        Exit Function
    End If
    
    CheckTimer.Enabled = True
    
    RaiseEvent CommAction("On line")
    
    OnLine = True

End Function

Public Sub OffLine()
    On Error Resume Next

    AbortFlag = False
    
    If onlineCount > 0 Then
        onlineCount = onlineCount - 1
        If onlineCount > 0 Then Exit Sub
    End If
        
    RaiseEvent CommAction("Off line")
    
    mmscMSComm.PortOpen = False
    
    CheckTimer.Enabled = False
    TimeOutTimer.Enabled = False
        
    requestType = 0
    mblnResponseFlag = True
    mintRetriedCount = 0

End Sub
'
'
'

Public Sub SDRMonitor(ByVal bOn As Boolean)

    If bOn = True Then
        RaiseEvent CommAction("SDR/SD Monitor ON")
    Else
        RaiseEvent CommAction("SDR/SD Monitor OFF")
    End If

    requestSDRMonitor = bOn

End Sub

'/////  START要求コマンド　/////
Public Function STARTRequest() As Boolean
    '// 変数宣言及び初期設定
    Dim intCount As Integer
    Dim lngBCC As Long
    Dim sBCC As String

    If requestType <> 0 Then Exit Function

    ReDim bytSend(0 To 8)
    
    '// 送信ｺｰﾄﾞ作成
    bytSend(0) = &H2
    bytSend(1) = Asc("S")
    bytSend(2) = Asc("T")
    bytSend(3) = Asc("A")
    bytSend(4) = Asc("R")
    bytSend(5) = Asc("T")
    bytSend(8) = &H3
    
    lngBCC = 0
    For intCount = 1 To 5
        lngBCC = lngBCC + bytSend(intCount)
    Next

    sBCC = Right$("00" + Hex$(lngBCC), 2)
    bytSend(6) = Asc(Mid$(sBCC, 1, 1))
    bytSend(7) = Asc(Mid$(sBCC, 2, 1))
    
    '// ｺｰﾄﾞ送信
    If bSimulate = 0 Then
        mmscMSComm.Output = bytSend
    Else
        SimulateTimer.Interval = 500
        SimulateTimer.Enabled = True
    End If
    
    requestType = 1

    mblnResponseFlag = False
    TimeOutTimer.Enabled = False
    TimeOutTimer.Interval = 2000
    TimeOutTimer.Enabled = True

    RaiseEvent CommAction("Send START")

    STARTRequest = True
End Function

'/////  STOP要求コマンド　/////
Public Function STOPRequest() As Boolean
    '// 変数宣言及び初期設定
    Dim intCount As Integer
    Dim lngBCC As Long
    Dim sBCC As String

    If requestType <> 0 Then Exit Function

    ReDim bytSend(0 To 7)
    
    '// 送信ｺｰﾄﾞ作成
    bytSend(0) = &H2
    bytSend(1) = Asc("S")
    bytSend(2) = Asc("T")
    bytSend(3) = Asc("O")
    bytSend(4) = Asc("P")
    bytSend(7) = &H3
    
    lngBCC = 0
    For intCount = 1 To 4
        lngBCC = lngBCC + bytSend(intCount)
    Next

    sBCC = Right$("00" + Hex$(lngBCC), 2)
    bytSend(5) = Asc(Mid$(sBCC, 1, 1))
    bytSend(6) = Asc(Mid$(sBCC, 2, 1))
    
    '// ｺｰﾄﾞ送信
    If bSimulate = 0 Then
        mmscMSComm.Output = bytSend
    Else
        SimulateTimer.Interval = 500
        SimulateTimer.Enabled = True
    End If

    requestType = 1

    mblnResponseFlag = False
    TimeOutTimer.Enabled = False
    TimeOutTimer.Interval = 2000
    TimeOutTimer.Enabled = True

    RaiseEvent CommAction("Send STOP")

    STOPRequest = True
End Function


'/////  SDR要求コマンド　/////
Public Function SDRRequest() As Boolean
    '// 変数宣言及び初期設定
    Dim intCount As Integer
    Dim lngBCC As Long
    Dim sBCC As String

    If requestType <> 0 Then Exit Function

    ReDim bytSend(0 To 9)
    
    '// 送信ｺｰﾄﾞ作成
    bytSend(0) = &H2
    bytSend(1) = Asc("T")
    bytSend(2) = Asc("M")
    bytSend(3) = Asc("S")
    bytSend(4) = Asc("S")
    bytSend(5) = Asc("D")
    bytSend(6) = Asc("R")
    bytSend(9) = &H3
    
    lngBCC = 0
    For intCount = 1 To 6
        lngBCC = lngBCC + bytSend(intCount)
    Next

    sBCC = Right$("00" + Hex$(lngBCC), 2)
    bytSend(7) = Asc(Mid$(sBCC, 1, 1))
    bytSend(8) = Asc(Mid$(sBCC, 2, 1))
    
    '// ｺｰﾄﾞ送信
    If bSimulate = 0 Then
        mmscMSComm.Output = bytSend
    Else
        SimulateTimer.Interval = 500
        SimulateTimer.Enabled = True
    End If

    requestType = 1
    
    mblnResponseFlag = False
    TimeOutTimer.Enabled = False
    TimeOutTimer.Interval = 2000
    TimeOutTimer.Enabled = True

    RaiseEvent CommAction("Send SDR")

    SDRRequest = True
End Function


'/////  SD要求コマンド　/////
Public Function SDRequest() As Boolean

    '// 変数宣言及び初期設定
    Dim intCount As Integer
    Dim lngBCC As Long
    Dim sBCC As String

    If requestType <> 0 Then Exit Function

    ReDim bytSend(0 To 9) As Byte
    
    '// 送信ｺｰﾄﾞ作成
    bytSend(0) = &H2
    bytSend(1) = Asc("T")
    bytSend(2) = Asc("M")
    bytSend(3) = Asc("S")
    bytSend(4) = Asc("S")
    bytSend(5) = Asc("D")
    bytSend(6) = Asc(" ")
    bytSend(9) = &H3
    
    lngBCC = 0
    For intCount = 1 To 6
        lngBCC = lngBCC + bytSend(intCount)
    Next

    sBCC = Right$("00" + Hex$(lngBCC), 2)
    bytSend(7) = Asc(Mid$(sBCC, 1, 1))
    bytSend(8) = Asc(Mid$(sBCC, 2, 1))
    
    '// ｺｰﾄﾞ送信
    If bSimulate = 0 Then
        mmscMSComm.Output = bytSend
    Else
        SimulateTimer.Interval = 500
        SimulateTimer.Enabled = True
    End If

    requestType = 1

    mblnResponseFlag = False
    TimeOutTimer.Enabled = False
    TimeOutTimer.Interval = 2000
    TimeOutTimer.Enabled = True

    RaiseEvent CommAction("Send SD")

    SDRequest = True
End Function

'/////  Trouble要求コマンド　/////
Public Function TBLRequest() As Boolean

    '// 変数宣言及び初期設定
    Dim intCount As Integer
    Dim lngBCC As Long
    Dim sBCC As String

    If requestType <> 0 Then Exit Function

    ReDim bytSend(0 To 10) As Byte
    
    '// 送信ｺｰﾄﾞ作成
    bytSend(0) = &H2
    bytSend(1) = Asc("T")
    bytSend(2) = Asc("R")
    bytSend(3) = Asc("O")
    bytSend(4) = Asc("U")
    bytSend(5) = Asc("B")
    bytSend(6) = Asc("L")
    bytSend(7) = Asc("E")
    bytSend(10) = &H3
    
    lngBCC = 0
    For intCount = 1 To 7
        lngBCC = lngBCC + bytSend(intCount)
    Next

    sBCC = Right$("00" + Hex$(lngBCC), 2)
    bytSend(8) = Asc(Mid$(sBCC, 1, 1))
    bytSend(9) = Asc(Mid$(sBCC, 2, 1))
    
    '// ｺｰﾄﾞ送信
    If bSimulate = 0 Then
        mmscMSComm.Output = bytSend
    Else
        SimulateTimer.Interval = 500
        SimulateTimer.Enabled = True
    End If

    requestType = 1

    mblnResponseFlag = False
    TimeOutTimer.Enabled = False
    TimeOutTimer.Interval = 2000
    TimeOutTimer.Enabled = True

    RaiseEvent CommAction("Send TROUBLE")

    TBLRequest = True
End Function

'/////  設定コマンド　/////
Public Function SetRequest(ByVal sdata As String) As Boolean

    '// 変数宣言及び初期設定
    Dim intCount As Integer
    Dim lngBCC As Long
    Dim sBCC As String
    Dim i As Integer

    If requestType <> 0 Then Exit Function

    RaiseEvent DebugSend(sdata)

    ReDim bytSend(0 To Len(sdata) + 3) As Byte
    
    '// 送信ｺｰﾄﾞ作成
    bytSend(0) = &H2
    lngBCC = 0
    For i = 1 To Len(sdata)
        bytSend(i) = Asc(Mid$(sdata, i, 1))
        lngBCC = lngBCC + bytSend(i)
    Next i
    sBCC = Right$("00" + Hex$(lngBCC), 2)
    bytSend(i) = Asc(Mid$(sBCC, 1, 1))
    bytSend(i + 1) = Asc(Mid$(sBCC, 2, 1))
    bytSend(i + 2) = &H3
    
    '// ｺｰﾄﾞ送信
    If bSimulate = 0 Then
        mmscMSComm.Output = bytSend
    Else
        SimulateTimer.Interval = 500
        SimulateTimer.Enabled = True
    End If

    requestType = 2

    mblnResponseFlag = False
    TimeOutTimer.Enabled = False
    TimeOutTimer.Interval = 2000
    TimeOutTimer.Enabled = True

    RaiseEvent CommAction("Send SetData")

    SetRequest = True
End Function

'/////  TRACE要求コマンド　/////
Public Function TRACERequest() As Boolean

    '// 変数宣言及び初期設定
    Dim intCount As Integer
    Dim lngBCC As Long
    Dim sBCC As String

    If requestType <> 0 Then Exit Function

    ReDim bytSend(0 To 8) As Byte
    
    '// 送信ｺｰﾄﾞ作成
    bytSend(0) = &H2
    bytSend(1) = Asc("T")
    bytSend(2) = Asc("R")
    bytSend(3) = Asc("A")
    bytSend(4) = Asc("C")
    bytSend(5) = Asc("E")
    bytSend(8) = &H3
    
    lngBCC = 0
    For intCount = 1 To 5
        lngBCC = lngBCC + bytSend(intCount)
    Next

    sBCC = Right$("00" + Hex$(lngBCC), 2)
    bytSend(6) = Asc(Mid$(sBCC, 1, 1))
    bytSend(7) = Asc(Mid$(sBCC, 2, 1))
    
    '// ｺｰﾄﾞ送信
    If bSimulate = 0 Then
        mmscMSComm.Output = bytSend
    Else
        SimulateTimer.Interval = 500
        SimulateTimer.Enabled = True
    End If

    requestType = 3

    mblnResponseFlag = False
    TimeOutTimer.Enabled = False
    TimeOutTimer.Interval = 2000
    TimeOutTimer.Enabled = True

    RaiseEvent CommAction("Send TRACE")

    TRACERequest = True
End Function

Private Sub DownContinue()
    '// 変数宣言及び初期設定
    Dim intCount As Integer
    Dim lngBCC As Long
    Dim sBCC As String
    Dim sdata As String
    Dim i As Integer

    If EOF(downFD) = True Then
        sdata = "DOWN " + Left$(downFileName + Space$(12), 12) + "E" + Right$("0" + Hex$(iSequence), 1) + Format$(downFileDate, "yyyymmddhhnnss")
        Close downFD
    
        requestType = 5
    Else
        If downTextMode = True Then
            downRemainSize = LOF(downFD) - Loc(downFD) * 128
            If downRemainSize < 0 Then downRemainSize = 0
            Line Input #downFD, sdata
            sdata = "DOWN " + Left$(downFileName + Space$(12), 12) + "T" + Right$("0" + Hex$(iSequence), 1) + sdata
            requestType = 4
        Else
            downRemainSize = LOF(downFD) - Loc(downFD)
            If downRemainSize = 0 Then
                sdata = "DOWN " + Left$(downFileName + Space$(12), 12) + "E" + Right$("0" + Hex$(iSequence), 1) + Format$(downFileDate, "yyyymmddhhnnss")
                requestType = 5
                Close downFD
            ElseIf downRemainSize >= 128 Then
                ReDim bdata(0 To 127) As Byte
                Get #downFD, , bdata
                sdata = "DOWN " + Left$(downFileName + Space$(12), 12) + "B" + Right$("0" + Hex$(iSequence), 1)
                For i = 0 To 127
                    sdata = sdata + Right$("00" + Hex$(bdata(i)), 2)
                Next i
                requestType = 4
            Else
                ReDim bdata(0 To downRemainSize - 1) As Byte
                Get #downFD, , bdata
                sdata = "DOWN " + Left$(downFileName + Space$(12), 12) + "B" + Right$("0" + Hex$(iSequence), 1)
                For i = 0 To downRemainSize - 1
                    sdata = sdata + Right$("00" + Hex$(bdata(i)), 2)
                Next i
                requestType = 4
            End If
        End If

        iSequence = iSequence + 1
        If iSequence > 9 Then
            iSequence = 1
        End If
    
    End If

    RaiseEvent DebugSend(Left$(sdata, 19))
    RaiseEvent CommAction("Send File")

    ReDim bytSend(0 To Len(sdata) + 3) As Byte
    
    '// 送信ｺｰﾄﾞ作成
    bytSend(0) = &H2
    lngBCC = 0
    For i = 1 To Len(sdata)
        bytSend(i) = Asc(Mid$(sdata, i, 1))
        lngBCC = lngBCC + bytSend(i)
    Next i
    sBCC = Right$("00" + Hex$(lngBCC), 2)
    bytSend(i) = Asc(Mid$(sBCC, 1, 1))
    bytSend(i + 1) = Asc(Mid$(sBCC, 2, 1))
    bytSend(i + 2) = &H3
    
    '// ｺｰﾄﾞ送信
    If bSimulate = 0 Then
        mmscMSComm.Output = bytSend
    Else
        SimulateTimer.Interval = 500
        SimulateTimer.Enabled = True
    End If

    mblnResponseFlag = False
    TimeOutTimer.Enabled = False
    TimeOutTimer.Interval = 10000
    TimeOutTimer.Enabled = True

End Sub

'/////  Program Download (DVAU1.MOT/DVAU2.MOT)要求コマンド　/////
Public Function DownProgRequest(ByVal sFilename As String) As Boolean

    If requestType <> 0 Then Exit Function

    downFileDate = FileDateTime(sFilename)
    downFD = FreeFile
    Open sFilename For Input As downFD

    downFileName = SplitPath(sFilename, True) + ".MOT"
    
    iSequence = 0
    downTextMode = True

    DownContinue

    DownProgRequest = True
End Function

'/////  Configuration Download 要求コマンド　/////
Public Function DownConfRequest1(ByVal sFilename As String) As Boolean
    
    If requestType <> 0 Then Exit Function

    downFileDate = FileDateTime(sFilename)
    downFD = FreeFile
    Open sFilename For Binary As downFD

    downFileName = SplitPath(sFilename, False)

    iSequence = 0
    downTextMode = False

    DownContinue

    DownConfRequest1 = True
End Function

'/////  REBOOT要求コマンド　/////
Public Function REBOOTRequest() As Boolean

    '// 変数宣言及び初期設定
    Dim intCount As Integer
    Dim lngBCC As Long
    Dim sBCC As String

    If requestType <> 0 Then Exit Function

    ReDim bytSend(0 To 9) As Byte
    
    '// 送信ｺｰﾄﾞ作成
    bytSend(0) = &H2
    bytSend(1) = Asc("R")
    bytSend(2) = Asc("E")
    bytSend(3) = Asc("B")
    bytSend(4) = Asc("O")
    bytSend(5) = Asc("O")
    bytSend(6) = Asc("T")
    bytSend(9) = &H3
    
    lngBCC = 0
    For intCount = 1 To 6
        lngBCC = lngBCC + bytSend(intCount)
    Next

    sBCC = Right$("00" + Hex$(lngBCC), 2)
    bytSend(7) = Asc(Mid$(sBCC, 1, 1))
    bytSend(8) = Asc(Mid$(sBCC, 2, 1))
    
    '// ｺｰﾄﾞ送信
    If bSimulate = 0 Then
        mmscMSComm.Output = bytSend
    Else
        SimulateTimer.Interval = 500
        SimulateTimer.Enabled = True
    End If

    requestType = 2

    mblnResponseFlag = False
    TimeOutTimer.Enabled = False
    TimeOutTimer.Interval = 2000
    TimeOutTimer.Enabled = True

    RaiseEvent CommAction("Send REBOOT")

    REBOOTRequest = True
End Function

'/////  RECONFIG要求コマンド　/////
Public Function RECONFIGRequest() As Boolean

    '// 変数宣言及び初期設定
    Dim intCount As Integer
    Dim lngBCC As Long
    Dim sBCC As String

    If requestType <> 0 Then Exit Function

    ReDim bytSend(0 To 9) As Byte
    
    '// 送信ｺｰﾄﾞ作成
    bytSend(0) = &H2
    bytSend(1) = Asc("R")
    bytSend(2) = Asc("E")
    bytSend(3) = Asc("C")
    bytSend(4) = Asc("O")
    bytSend(5) = Asc("N")
    bytSend(6) = Asc("F")
    bytSend(9) = &H3
    
    lngBCC = 0
    For intCount = 1 To 6
        lngBCC = lngBCC + bytSend(intCount)
    Next

    sBCC = Right$("00" + Hex$(lngBCC), 2)
    bytSend(7) = Asc(Mid$(sBCC, 1, 1))
    bytSend(8) = Asc(Mid$(sBCC, 2, 1))
    
    '// ｺｰﾄﾞ送信
    If bSimulate = 0 Then
        mmscMSComm.Output = bytSend
    Else
        SimulateTimer.Interval = 500
        SimulateTimer.Enabled = True
    End If

    requestType = 2

    mblnResponseFlag = False
    TimeOutTimer.Enabled = False
    TimeOutTimer.Interval = 2000
    TimeOutTimer.Enabled = True

    RaiseEvent CommAction("Send RECONFIG")

    RECONFIGRequest = True
End Function

'/////  受信ＯＫの送信、次行を要求　/////
Public Function ACKRequest() As Boolean
    ReDim bytSend(0 To 0) As Byte
    
    '// 送信ｺｰﾄﾞ作成
    bytSend(0) = &H6
    
    '// ｺｰﾄﾞ送信
    If bSimulate = 0 Then
        mmscMSComm.Output = bytSend
    Else
        SimulateTimer.Interval = 500
        SimulateTimer.Enabled = True
    End If

    TimeOutTimer.Enabled = False
    TimeOutTimer.Interval = 2000
    TimeOutTimer.Enabled = True

    RaiseEvent DebugSend("ACK")

    ACKRequest = True
End Function

'/////  受信ＮＧの送信、再送信を要求　/////
Public Function NAKRequest() As Boolean
    ReDim bytSend(0 To 0) As Byte
    
    '// 送信ｺｰﾄﾞ作成
    bytSend(0) = &H15
    
    '// ｺｰﾄﾞ送信
    If bSimulate = 0 Then
        mmscMSComm.Output = bytSend
    Else
        SimulateTimer.Interval = 500
        SimulateTimer.Enabled = True
    End If

    TimeOutTimer.Enabled = False
    TimeOutTimer.Interval = 2000
    TimeOutTimer.Enabled = True

    RaiseEvent DebugSend("NAK")

    NAKRequest = True
End Function

'/////  受信キャンセルの送信　/////
Public Function CANCELRequest() As Boolean
    ReDim bytSend(0 To 0) As Byte
    
    '// 送信ｺｰﾄﾞ作成
    bytSend(0) = &H1B
    
    '// ｺｰﾄﾞ送信
    If bSimulate = 0 Then
        mmscMSComm.Output = bytSend
    Else
        SimulateTimer.Interval = 500
        SimulateTimer.Enabled = True
    End If

    TimeOutTimer.Enabled = False
    TimeOutTimer.Interval = 2000

    requestType = 0
    mblnResponseFlag = True
    mintRetriedCount = 0
    
    RaiseEvent DebugSend("Cancel")

    CANCELRequest = True
End Function

'/////  LDR要求コマンド　/////
Public Function LDRRequest(ByVal SendData As String) As Boolean

    '// 変数宣言及び初期設定
    Dim intCount As Integer
    Dim lngBCC As Long
    Dim sBCC As String

    If requestType <> 0 Then Exit Function

    ReDim bytSend(0 To 38) As Byte
    
    '// 送信ｺｰﾄﾞ作成
    bytSend(0) = &H2
    bytSend(1) = Asc("L")
    bytSend(2) = Asc("D")
    bytSend(3) = Asc("R")
    For intCount = 0 To 31
        bytSend(intCount + 4) = Asc(Mid$(SendData, intCount + 1, 1))
    Next
    bytSend(38) = &H3
    
    lngBCC = 0
    For intCount = 1 To 35
        lngBCC = lngBCC + bytSend(intCount)
    Next

    sBCC = Right$("00" + Hex$(lngBCC), 2)
    bytSend(36) = Asc(Mid$(sBCC, 1, 1))
    bytSend(37) = Asc(Mid$(sBCC, 2, 1))
    
    '// ｺｰﾄﾞ送信
    If bSimulate = 0 Then
        mmscMSComm.Output = bytSend
    Else
        SimulateTimer.Interval = 500
        SimulateTimer.Enabled = True
    End If

    requestType = 6

    mblnResponseFlag = False
    TimeOutTimer.Enabled = False
    TimeOutTimer.Interval = 2000
    TimeOutTimer.Enabled = True

    RaiseEvent CommAction("Send LDR")

    LDRRequest = True
End Function


Private Sub CheckTimer_Timer()
        
    If requestSDRMonitor = False Then Exit Sub

'    If mblnResponseFlag = False Then Exit Sub

    If requestType <> 0 Then Exit Sub

    '// ﾘﾄﾗｲ関連をｸﾘｱ
    mblnResponseFlag = False
    mintRetriedCount = 0
    mstrRecvBuff = ""
    mmscMSComm.InBufferCount = 0
    mmscMSComm.OutBufferCount = 0

    If sdrPhase = 0 Then
        SDRRequest
        sdrPhase = 1
    ElseIf sdrPhase = 1 Then
        SDRequest
        If menteMode = True Then
            sdrPhase = 2
        Else
            sdrPhase = 0
        End If
    Else
        TBLRequest
        sdrPhase = 0
    End If

End Sub

Private Sub SimulateTimer_Timer()

    SimulateTimer.Enabled = False
    If bytSend(0) = &H2 Then
        If bytSend(1) = Asc("S") And bytSend(2) = Asc("T") And bytSend(3) = Asc("A") And bytSend(4) = Asc("R") And bytSend(5) = Asc("T") Then
            RecvCommandLoop "OK"
        ElseIf bytSend(1) = Asc("T") And bytSend(2) = Asc("M") And bytSend(3) = Asc("S") And bytSend(4) = Asc("S") And bytSend(5) = Asc("D") And bytSend(6) = Asc("R") Then
            simPhase = 1
            RecvCommandLoop "SDR1                                                                   "
        ElseIf bytSend(1) = Asc("T") And bytSend(2) = Asc("M") And bytSend(3) = Asc("S") And bytSend(4) = Asc("S") And bytSend(5) = Asc("D") And bytSend(6) = Asc(" ") Then
            simPhase = 11
            RecvCommandLoop "SD1                                                                   "
        ElseIf bytSend(1) = Asc("D") And bytSend(2) = Asc("O") And bytSend(3) = Asc("W") And bytSend(4) = Asc("N") Then
            RecvCommandLoop "OK"
            simPhase = 100
        ElseIf bytSend(1) = Asc("R") And bytSend(2) = Asc("E") Then
            simPhase = 100
            SimulateTimer.Enabled = True
            RecvCommandLoop "WAIT"
        End If
    ElseIf bytSend(0) = &H6 Then
        Select Case simPhase
        Case 1
            RecvCommandLoop "SDR2                                                                   "
            simPhase = 2
        Case 2
            RecvCommandLoop "SDR3                                                                   "
            simPhase = 3
        Case 3
            RecvCommandLoop "SDR4                                                                   "
            simPhase = 4
        Case 4
            RecvCommandLoop "SDR5                                                                   "
            simPhase = 5
        Case 5
            RecvCommandLoop "SDR6                                                                   "
            simPhase = 6
        Case 6
            RecvCommandLoop "SDR7                                                                   "
            simPhase = 7
        Case 7
            RecvCommandLoop "SDR8                                                                   "
            simPhase = 8
        Case 8
            RecvCommandLoop "SDR9                                                                   "
            simPhase = 9
        Case 9
            RecvCommandLoop "SDRA                                                                   "
            simPhase = 10
        Case 10
            RecvCommandLoop "SDRB                                                                   "
            simPhase = 0
            
        Case 11
            RecvCommandLoop "SD2                                                                   "
            simPhase = 12
        Case 12
            RecvCommandLoop "SD3                                                                   "
            simPhase = 13
        Case 13
            RecvCommandLoop "SD4                                                                   "
            simPhase = 14
        Case 14
            RecvCommandLoop "SD5                                                                   "
            simPhase = 0

        Case Else
            simPhase = simPhase + 1
            If simPhase < 110 Then
                RecvCommandLoop "WAIT"
            Else
                RecvCommandLoop "OK"
            End If
        End Select
    ElseIf bytSend(0) = &H15 Then
            RecvCommandLoop "NG"
    Else
            RecvCommandLoop "NG"
    End If

End Sub

Private Sub TimeoutTimer_Timer()

    If requestType = 0 Then
        TimeOutTimer.Enabled = False
        Exit Sub
    End If

    mintRetriedCount = mintRetriedCount + 1
    If mintRetriedCount < 3 Then
        
        If mblnResponseFlag = True Then
            NAKRequest
            Exit Sub
        End If
    
        RaiseEvent CommAction("Send Retry")
        
        '// ｺｰﾄﾞ送信
        If bSimulate = 0 Then
            mmscMSComm.Output = bytSend
        Else
            SimulateTimer.Interval = 500
            SimulateTimer.Enabled = True
        End If
        Exit Sub
    End If

    '// すべてのﾘﾄﾗｲ処理をｸﾘｱ
    RetriedTimerClear

    RaiseEvent CommAction("Timeout")
        
    Select Case requestType
    Case 1  'Wait ACK
        RaiseEvent RecvOK(False)
        requestType = 0
        Exit Sub
    Case 2  'Wait ACK
        RaiseEvent RecvOK(False)
        requestType = 0
        Exit Sub
    Case 3 'TRACE
        RaiseEvent RecvOK(False)
        requestType = 0
        Exit Sub
    Case 4  'Download
        RaiseEvent RecvDownLoad(downFileName, 0, 0, False)
        requestType = 0
        Exit Sub
    Case 5  'Download END
        RaiseEvent RecvDownLoad(downFileName, 0, 0, False)
        requestType = 0
        Exit Sub
    Case 6 'LDR
        RaiseEvent RecvOK(False)
        requestType = 0
        Exit Sub
    End Select

End Sub
