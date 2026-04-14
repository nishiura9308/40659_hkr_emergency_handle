import time
import serial 
import serial.tools.list_ports
import os 
import threading 

# *****************************  データ受信   *********************************
def ser_read():
    # pass
    global  Ser
    while(1):
        if Ser != '':
            # data = ser.read()
            data = Ser.readline()
            data = data.strip()
            data = data.decode('utf-8')
            print(data)


# *****************************  データ送信   *********************************
def ser_send():
    # pass
    global Ser
    while(1):
        if Ser !='':
            data = input() +  '\r\n'
            data = data.encode('utf-8')
            Ser.write(data)


# ***********************  シリアル通信オープン   ******************************
def ser_open():
    # pass
    global  Ser
    # serial_port =
    # for i, port in enumerate(serial.tools.list_ports.comports()):
    #     serial_port[str(i)] = port.device

    # port_val = serial_ports[ input(f'ポート番号を選択してください。{serial_ports}:')]
    port_val = 'COM5'
    # boud_val = int(input('ボーレートbpsを数値で入力してください。:'))
    boud_val = int(9600)
    
    Ser = serial.Serial(port = port_val, baudrate = boud_val, timeout = 0.1)
    print(f'open{port_val}/{boud_val}')

    # # 制御コード (STX と ETX) を使ってデータを分割
    start_code = b'\x02'
    end_code = b'\x03'

    ki = {
			"STX": 0x02, "ETX": 0x03, "DEF": 0x7f, "SOH": 0x01, "1": 0x31, "2": 0x32, "3": 0x33, "4": 0x34, "5": 0x35,
			"A": 0x41, "B": 0x42, "V1":0x5631, "V2":0x5632, "V3":0x5633, "V4":0x5634, "V5":0x5635, "C": 0x43, "D": 0x44, 
			"E": 0x45, "F": 0x46, "G": 0x47, "H": 0x48, "I": 0x49, "J": 0x4a, "K": 0x4b, "L": 0x4c, "M": 0x4d, "N": 0x4e,
			"O": 0x4f, "P": 0x50, "Q": 0x51, "R": 0x52, "S": 0x53, "T": 0x54, "U": 0x55, "V": 0x56, "W": 0x57, "X": 0x58, 
			"Y": 0x59, "Z": 0x5a
        }

    # # 送信する信号を選択（この部分は実際のシグナルに応じて変更）
    signal_key = "V1"  
    # signal_key = input("送信する信号を入力してください: ").upper() 

    # 受信するバイト数を選択（`ki`辞書から取得）
    bytes_to_receive = ki.get(signal_key, 2)  

    # シリアルポートがオープンである限りループ
    while Ser.is_open:

        # 信号に基づいてデータを送信
        signal_value = ki.get(signal_key)  # signal_keyに対応するバイト値を取得
        if 0 <= signal_value <= 30:
            # 制御コード（STX）と信号データを送信
            Ser.write(start_code + bytes([signal_value]) + end_code)
            print(f"Sent: {chr(signal_value)}")  # 送信した信号を表示
        else:
            # print(f"Invalid signal_key: {signal_key}")
            # print(signal_key) 
            print()

        # # （指定した数分）受信するバイト数を決定してデータを読み込む
        # s = ser.read(bytes_to_receive) 
        s = Ser.readline(bytes_to_receive)          #   改行までデータを続けて受信する
        print(s) 
        
        # # 受信データを表示
        # print(bytes_to_receive, s)
        # print(f"Received {bytes_to_receive} bytes: {s}")

        # 受信したデータが特定の値でない場合
        if s != b' press the button ':
            print(s)  # 受信したデータをそのまま表示
        else:
            print(".")
        send_binary = bytes(signal_value) 

        # 0.1秒待機
        time.sleep(0.1)
        
        
    Ser.close()   
    # シリアルポートが閉じられたことを表示
    print("Serial connection closed")

if __name__ == '__main__':
    # pass
    Ser = ''
    ser_open()
    thread_1 = threading.Thread(target = ser_read)
    thread_2 = threading.Thread(target = ser_send)
    thread_1.start()
    thread_2.start()