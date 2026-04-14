# coding:utf-8
import socket
import struct
import time
import serial
import sys

if __name__ == "__main__":
    ser = serial.Serial()
    ser.baudrate = 9600  # ArduinoのSerial.beginで指定した値
    ser.timeout = 0.1       # タイムアウトの時間
    ser.port = "COM5"     # Arduinoのポート番号（場合によって変更する必要あり）

    # # 制御コード (STX と ETX) を使ってデータを分割
    start_code = b'\x02'
    end_code = b'\x03'

    # シリアルポートを開く
    try:
        ser.open()
        print("Open " + ser.port)
    except:
        print("Can't open " + ser.port)
        sys.exit(0)

    # キーと信号に対応するバイト数のマッピング
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
    while ser.is_open:

        # 信号に基づいてデータを送信
        signal_value = ki.get(signal_key)  # signal_keyに対応するバイト値を取得
        if 0 <= signal_value <= 30:
            # 制御コード（STX）と信号データを送信
            ser.write(start_code + bytes([signal_value]) + end_code)
            print(f"Sent: {chr(signal_value)}")  # 送信した信号を表示
        else:
            # print(f"Invalid signal_key: {signal_key}")
            # print(signal_key) 
            print()

        # # 受信するバイト数を決定してデータを読み込む
        s = ser.read(bytes_to_receive) 
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

    # シリアルポートが閉じられたことを表示
    print("Serial connection closed")

    # #   送信コード
    # with serial.Serial('COM5', 9600) as s :                   #  pyserial で送信
    #     print('---')
    #     print(s)
    #     s.write(send_binary)
    #     print('---')
    #     print(send_binary)
