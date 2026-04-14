Attribute VB_Name = "basMain"
Option Explicit

Public menteMode As Boolean
Public downBusy As Boolean

'CONFIG.DAT Map
Public Const TOP_LANG = 1
Public Const LANG_SIZE = 2
Public Const TOP_CHIME = TOP_LANG + LANG_SIZE
Public Const CHIME_SIZE = 4
Public Const TOP_VERSION = TOP_CHIME + CHIME_SIZE
Public Const VERSION_SIZE = 2
Public Const TOP_VOICE = TOP_VERSION + VERSION_SIZE
Public Const ONE_VOICE_SIZE = 1
Public Const VOICE_SIZE = ONE_VOICE_SIZE * 8

Public Const SEQUENCE_MAX = 6
Public Const SEQUENCE_SIZE = 4 * SEQUENCE_MAX
'ANNOUNCE.DAT Map
Public Const TOP_DOOR = 1
Public Const ONE_DOOR_SIZE = SEQUENCE_SIZE
Public Const DOOR_SIZE = ONE_DOOR_SIZE * 4
Public Const TOP_EMGMA = TOP_DOOR + DOOR_SIZE
Public Const ONE_EMGMA_SIZE = 1 + 12 + SEQUENCE_SIZE
Public Const EMGMA_SIZE = ONE_EMGMA_SIZE * 32
Public Const TOP_SPCMA = TOP_EMGMA + EMGMA_SIZE
Public Const ONE_SPCMA_SIZE = 1 + 12 + SEQUENCE_SIZE
Public Const SPCMA_SIZE = ONE_SPCMA_SIZE * 64
Public Const TOP_DVAU_VERSION = TOP_SPCMA + SPCMA_SIZE
Public Const OWN_VERSION_SIZE = 2 * 3

Public Const MAX_STATION = 64
Public Const STATION_SIZE = 1 + 1 + 6 + 1 + 1 + SEQUENCE_SIZE
Public Const ROUTE_SIZE = 1 + 1 + 6 + STATION_SIZE * MAX_STATION

Public DataDir As String
Public befDateConfig As Date
Public befDateMode As Boolean

Public bSimulate As Integer

' ------------------ WIN32API ﾌﾟﾛﾄﾀｲﾌﾟ ------------------
Declare Function GetPrivateProfileString Lib "kernel32" Alias "GetPrivateProfileStringA" (ByVal lpApplicationName As String, ByVal lpKeyName As Any, ByVal lpDefault As String, ByVal lpReturnedString As String, ByVal nSize As Long, ByVal lpFileName As String) As Long
Declare Function GetPrivateProfileInt Lib "kernel32" Alias "GetPrivateProfileIntA" (ByVal lpApplicationName As String, ByVal lpKeyName As String, ByVal nDefault As Long, ByVal lpFileName As String) As Long
Declare Function WritePrivateProfileString Lib "kernel32" Alias "WritePrivateProfileStringA" (ByVal lpApplicationName As String, ByVal lpKeyName As Any, ByVal lpString As Any, ByVal lpFileName As String) As Long
Declare Function GetDiskFreeSpace Lib "kernel32" Alias "GetDiskFreeSpaceA" (ByVal lpRootPathName As String, lpSectorsPerCluster As Long, lpBytesPerSector As Long, lpNumberOfFreeClusters As Long, lpTtoalNumberOfClusters As Long) As Long
Declare Function GetTempFileName Lib "kernel32" Alias "GetTempFileNameA" (ByVal lpszPath As String, ByVal lpPrefixString As String, ByVal wUnique As Long, ByVal lpTempFileName As String) As Long
Declare Function GetTempPath Lib "kernel32" Alias "GetTempPathA" (ByVal nBufferLength As Long, ByVal lpBuffer As String) As Long
Declare Function FlashWindow Lib "user32" (ByVal hwnd As Long, ByVal bInvert As Long) As Long
Declare Function WNetAddConnection Lib "mpr.dll" Alias "WNetAddConnectionA" (ByVal lpszNetPath As String, ByVal lpszPassword As String, ByVal lpszLocalName As String) As Long
Declare Sub Sleep Lib "kernel32" (ByVal dwMilliseconds As Long)

'============================================================
'機能概要：環境データの読み込みサブルーティン
'   レジストリーからデータを読み込む
'   実行ファイルからの ＩＮＩファイルを読み込む
'   ＷＩＮＤＯＷＳディレクトリーからの ＩＮＩファイルを読み込む
'
'============================================================
Public Sub ReadIniItem(ByVal AppName As String, ByVal SectionName As String, ByVal KeyName As String, ByVal itemDefault, item)
    Dim buff As String * 250
    Dim lRet As Long
    Dim i As Integer

    Select Case VarType(item)
    Case vbString
        'レジストリー
        item = itemDefault
'        item = GetSetting(AppName, SectionName, KeyName, itemDefault)
        'カレント
        lRet = GetPrivateProfileString(SectionName & Chr$(0), KeyName & Chr$(0), item & Chr$(0), buff, 250, App.Path & "\" & AppName & ".INI" & Chr$(0))
        lRet = Len(StrConv(buff, 0))
        item = Left$(buff, lRet)
        'Windows
        lRet = GetPrivateProfileString(SectionName & Chr$(0), KeyName & Chr$(0), item & Chr$(0), buff, 250, AppName & ".INI" & Chr$(0))
        lRet = Len(StrConv(buff, 0))
        item = Left$(buff, lRet)
    
    Case vbInteger
        'レジストリー
        'item = CInt(GetSetting(AppName, SectionName, KeyName, itemDefault))
        item = itemDefault
        'カレント
        item = CInt(GetPrivateProfileInt(SectionName & Chr$(0), KeyName & Chr$(0), CLng(item), App.Path & "\" & AppName & ".INI" & Chr$(0)))
        'Windows
        item = CInt(GetPrivateProfileInt(SectionName & Chr$(0), KeyName & Chr$(0), CLng(item), AppName & ".INI" & Chr$(0)))
    
    Case vbLong
        'レジストリー
        'item = CLng(GetSetting(AppName, SectionName, KeyName, itemDefault))
        item = itemDefault
        'カレント
        item = CLng(GetPrivateProfileInt(SectionName & Chr$(0), KeyName & Chr$(0), item, App.Path & "\" & AppName & ".INI" & Chr$(0)))
        'Windows
        item = CLng(GetPrivateProfileInt(SectionName & Chr$(0), KeyName & Chr$(0), item, AppName & ".INI" & Chr$(0)))
    
    Case vbDate
        'レジストリー
        'item = CDate(GetSetting(AppName, SectionName, KeyName, itemDefault))
        item = itemDefault
        'カレント
        lRet = GetPrivateProfileString(SectionName & Chr$(0), KeyName & Chr$(0), str$(item) & Chr$(0), buff, 250, App.Path & "\" & AppName & ".INI" & Chr$(0))
        lRet = Len(StrConv(buff, 0))
        item = CDate(Left$(buff, lRet))
        'Windows
        lRet = GetPrivateProfileString(SectionName & Chr$(0), KeyName & Chr$(0), str$(item) & Chr$(0), buff, 250, AppName & ".INI" & Chr$(0))
        lRet = Len(StrConv(buff, 0))
        item = CDate(Left$(buff, lRet))
    End Select

End Sub

Sub Main()

    If App.PrevInstance = True Then Exit Sub

    ReadIniItem "Maintenance", "CONFIG", "ChangeDate", "00/01/01 00:00:00", befDateConfig
    ReadIniItem "Maintenance", "DEBUG", "Simulate", 0, bSimulate

    If Now < befDateConfig Then
        befDateMode = True
    Else
        befDateMode = False
    End If

    MDIMain.Show

End Sub

Function GetRouteName(ByVal routeCode As Integer) As String
    Dim i As Integer
    Dim buff As String
    Dim fno As Integer

    On Error Resume Next

    GetRouteName = ""
    fno = FreeFile
    Open App.Path & "\ROUTE.NAM" For Input As fno
    For i = 0 To 256
        Line Input #fno, buff
        If routeCode = i Then
            GetRouteName = buff
            Exit For
        End If
    Next i
    Close fno

End Function

Public Sub SetRouteName(ByVal routeCode As Integer, ByVal routeName As String)
    Dim fnoS As Integer
    Dim fnoD As Integer
    Dim buff As String
    Dim i As Integer
    
    On Error Resume Next

    If GetRouteName(routeCode) <> routeName Then
        fnoS = FreeFile
        Open App.Path & "\ROUTE.NAM" For Input As fnoS
        fnoD = FreeFile
        Open App.Path & "\ROUTE.TMP" For Output As fnoD
        For i = 0 To 256
            Line Input #fnoS, buff
            If i = routeCode Then
                Print #fnoD, routeName
            Else
                Print #fnoD, buff
            End If
        Next i
        Close fnoS
        Close fnoD
        Kill App.Path & "\ROUTE.NAM"
        Name App.Path & "\ROUTE.TMP" As App.Path & "\ROUTE.NAM"
    End If

End Sub

Function GetStationName(ByVal stationCode As Integer) As String
    Dim i As Integer
    Dim buff As String
    Dim fno As Integer

    On Error Resume Next

    GetStationName = ""
    fno = FreeFile
    Open App.Path & "\STATION.NAM" For Input As fno
    For i = 0 To 100
        Line Input #fno, buff
        If stationCode = i Then
            GetStationName = buff
            Exit For
        End If
    Next i
    Close fno

End Function

Public Sub SetStationName(ByVal stationCode As Integer, ByVal StationName As String)
    Dim fnoS As Integer
    Dim fnoD As Integer
    Dim buff As String
    Dim i As Integer
    
    On Error Resume Next

    If StationName <> GetStationName(stationCode) Then
        fnoS = FreeFile
        Open App.Path & "\STATION.NAM" For Input As fnoS
        fnoD = FreeFile
        Open App.Path & "\STATION.TMP" For Output As fnoD
        For i = 0 To 99
            Line Input #fnoS, buff
            If i = stationCode Then
                Print #fnoD, StationName
            Else
                Print #fnoD, buff
            End If
        Next i
        Close fnoS
        Close fnoD
        Kill App.Path & "\STATION.NAM"
        Name App.Path & "\STATION.TMP" As App.Path & "\STATION.NAM"
    End If
End Sub

Function GetLineName(ByVal selRouteNo As Integer, ByVal lineCode As Integer) As String
    Dim i As Integer
    Dim buff As String
    Dim fno As Integer
    Dim fname As String

    On Error Resume Next

    GetLineName = ""
    fno = FreeFile
    fname = App.Path & "\LINE" & Hex$(selRouteNo) & ".NAM"
    Open fname For Input As fno
    For i = 0 To 7
        Line Input #fno, buff
        If lineCode = i Then
            GetLineName = buff
            Exit For
        End If
    Next i
    Close fno

End Function

Public Sub SetLineName(ByVal selRouteNo As Integer, ByVal lineCode As Integer, ByVal lineName As String)
    Dim fnoS As Integer
    Dim fnoD As Integer
    Dim buff As String
    Dim fname As String
    Dim i As Integer
    
    On Error Resume Next

    If lineName <> GetLineName(selRouteNo, lineCode) Then
        fname = App.Path & "\LINE" & Hex$(selRouteNo) & ".NAM"
        fnoS = FreeFile
        Open fname For Input As fnoS
        fnoD = FreeFile
        Open App.Path & "\LINE.TMP" For Output As fnoD
        For i = 0 To 7
            Line Input #fnoS, buff
            If i = lineCode Then
                Print #fnoD, lineName
            Else
                Print #fnoD, buff
            End If
        Next i
        Close fnoS
        Close fnoD
        Kill fname
        Name App.Path & "\LINE.TMP" As fname
    End If

End Sub

Function GetStationNumberName(ByVal selRouteNo As Integer, ByVal stationNumber As Integer) As String
    Dim i As Integer
    Dim buff As String
    Dim fno As Integer
    Dim fname As String

    On Error Resume Next

    GetStationNumberName = ""
    fno = FreeFile
    fname = App.Path & "\STN" & Hex$(selRouteNo) & ".NAM"
    Open fname For Input As fno
    For i = 0 To 31
        Line Input #fno, buff
        If stationNumber = i Then
            GetStationNumberName = buff
            Exit For
        End If
    Next i
    Close fno

End Function

Public Sub SetStationNumberName(ByVal selRouteNo As Integer, ByVal stationNumber As Integer, ByVal stationNumberName As String)
    Dim fnoS As Integer
    Dim fnoD As Integer
    Dim buff As String
    Dim fname As String
    Dim i As Integer
    
    On Error Resume Next

    If stationNumberName <> GetStationNumberName(selRouteNo, stationNumber) Then
        fname = App.Path & "\STN" & Hex$(selRouteNo) & ".NAM"
        fnoS = FreeFile
        Open fname For Input As fnoS
        fnoD = FreeFile
        Open App.Path & "\STN.TMP" For Output As fnoD
        For i = 0 To 31
            Line Input #fnoS, buff
            If i = stationNumber Then
                Print #fnoD, stationNumberName
            Else
                Print #fnoD, buff
            End If
        Next i
        Close fnoS
        Close fnoD
        Kill fname
        Name App.Path & "\STN.TMP" As fname
    End If

End Sub


Public Function getMCPString(ByRef data() As Byte) As String

    getMCPString = Chr$(data(0)) & Chr$(data(1)) & Chr$(data(2)) & Chr$(data(3)) & Chr$(data(4)) & Chr$(data(5))

End Function
