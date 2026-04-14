import sys
import time
import serial
import binascii
import ctypes
import keyboard
from collections import deque


# シリアルポートを設定
ser = serial.Serial()
ser.baudrate = 9600  # ArduinoのSerial.beginで指定した値
ser.timeout = 1     # タイムアウトの時間
ser.port = "COM5" 

# キーと信号に対応するバイト数のマッピング
ki = {
        "STX": 0x02, "ETX": 0x03, "DEF": 0x7f, "SOH": 0x01, 
        "1": 0x31, "2": 0x32, "3": 0x33, "4": 0x34, "5": 0x35,
        "A": 0x41, "B": 0x42, "V1": 0x5631, "V2": 0x5632, "V3": 0x5633, 
        "V4": 0x5634, "V5": 0x5635, "C": 0x43, "D": 0x44, "E": 0x45, 
        "F": 0x46, "G": 0x47, "H": 0x48, "I": 0x49, "J": 0x4a, 
        "K": 0x4b, "L": 0x4c, "M": 0x4d, "N": 0x4e, "O": 0x4f, 
        "P": 0x50, "Q": 0x51, "R": 0x52, "S": 0x53, "T": 0x54, 
        "U": 0x55, "V": 0x56, "W": 0x57, "X": 0x58, "Y": 0x59, "Z": 0x5a
}

queue = []  # 受信データ格納用リスト     ^・^　・・・・・・・　^・^　

# 受信処理 (Receiving)
def ser_rece():
    try:
        ser.open()
        print("シリアルポートオープン: " + ser.portstr)
    except:
        print("シリアルポートを開けませんでした.")
        sys.exit(0)
        
    # queue = []  # 受信データ格納用リスト     ^・^　・・・・・・・　^・^　
    received_data = []
    received = []
    i = 0
    hunt_phase = 0
    msg = ""


    # data = queue.pop(0)  # 最初のメッセージを取り出す
    # print("Processing:", data)
    # q = queue.Queue()

    while ser.is_open:
       
        if keyboard.is_pressed('q'):
            print('終了します。')
            break
        
        # データ受信
        tmp_line = ser.readline()  # 受信データ
        # print(tmp_line)
        # received_data.append(msg)         #  受信データをリストに追加          ^・^　・・・・・・・　^・^
        # print(received_data)
        # data = queue.pop()  # 最初のメッセージを取り出す
        # print("Processing:", data)
     
        if binascii.hexlify(tmp_line) == b'':
            print('timeout!')
            continue

        for s in tmp_line:

            if hunt_phase == 0 :       # STX hunt phase
                if s == 0x02:
                    hunt_phase = 1
                    print('')
                    print('*********** start ***********')
                    print('find STX!')
                pass

            elif hunt_phase == 1 :     # ETX hunt phase
        
                if s == 0x03:
                    print('find ETX!')
                    print(msg)
                    bcc = calculate_bcc(msg[:-2]) 

                    received_data.append(msg[:-2])         #  受信データをリストに追加          ^・^　・・・・・・　^・^
                    # print ('受信データ:', received_data )                                    ・　　　　　　　　　　・
                    # received =  received_data                                               ・  　　　　　　　　　・
                    # received.append(received.pop(0))　　　　　　　　　　　　　　　　　　　　　　　・　　　ｖｖ　　　・　　　
                    # print ('受信データ:', received )                                          　　・・・・・・・
                    folds = int(1)
                    # for i in range(folds):
                        # print('データ出ろう：',received_data[i])
                    received = received_data[i] * folds
                        # if i % 2 == 0:
                        #     received.extend(received_data)
                        # else:
                        #     received.extend(received_data[::-1])
                    # import matplotlib.pyplot as plt 
                    # plt.plot(received)
                    print('データ出ろう：',received)


                    # １メッセージの受信確定
                    ser_send(msg)
                    msg = ""
                    hunt_phase = 0
                else:
                    msg+=chr(s)
                    # queue.pop(0)
            
            
        # *****************************************  受信データ格納メモリ( 変数 )  ***************************************   
        #                                                                                                              #
                #     受信データ格納の変数を宣言
                #     受信したデータを変数に格納
                #     格納されたデータを表示
        #                                                                                                              #
        # **************************************************************************************************************      
        # if s:
        #     print(f"受信した: {s}")
        #     # 受信データのBCCチェック
        #     if  s[0] == 0x02  and  s[-1]  == 0x03 :
        #         data = s[1:-1]                                    # STX(0x02) と ETX(0x03) を除いたデータ
        #         received_bcc = s[-2]                             # 受信したBCC（ETXの直前のバイト）
        #         calculated_bcc = calculate_bcc(data[:-2])             # 受信データのBCC計算
        #         # BCCチェック
        #         if received_bcc != calculated_bcc:
        #             print("BCC OK (正常)")
        #             return data  # BCCを除いたデータを返す
        #         else:
        #             print("BCC ERROR (異常)")
        #             # return None
        #     else:
        #         print("STXまたはETXが正しくありません")
        #         return None 
        
        time.sleep(1)  # 1秒待機

         # バッファをクリア
        ser.flushInput()   # 入力バッファをクリア
        ser.flushOutput()  # 出力バッファをクリア

    print("シリアルポートが閉じられた.")
    ser.close()

    # return received_data            #   ^・^　・・・・・・・　^・^        


# XOR演算でBCC計算
def calculate_bcc(data):
    bcc = 0
#    print(data) 
#    a = [byte for byte in data]
    for byte in data:
#        print(hex(byte)[2:]+' '+chr(byte))
        bcc += ord(byte)
    bcc = bcc & 0xff
    print('bcc:',hex(bcc)[2:])
    # return bcc
   

# 送信処理 (Sending)
def ser_send(data):
    # 送信データを作成する
    
    # bcc = calculate_bcc(data)  # BCCを計算      + bytearray([bcc])
    packet = bytes([0x02]) + data.encode() + bytes([0x03])  # STX, データ, BCC, ETX を付けて送信
    # packet = bytes(data) 
    # シリアルポートに送信
    # print(ser)
    # senddata = bytes(data) + bytes([0x03])
    ser.write(packet) 
    # ser.write(packet)
    # print(f"Sent: {packet}")
    print(f"Sent: {packet}")
    print('************ end ***********')
    print('')


def main():
    # 受信処理
    ser_rece()
    # print(received_data)               #   ^・^　・・・・・・・　^・^
    
    # receive_data = ''
    #     # 受信データ格納の変数を宣言
    # if  receive_data:

    #         # 受信したデータを変数に格納
    #     ser_rece(receive_data )

    #         # 格納されたデータを表示
    #     print("受信データ:", {receive_data})

    # if received_data:
    #     print(f"受信したデータ: {received_data}")

    #     # 受信したデータに基づいて送信処理を行う
    #     ser_send(received_data)
    # else:
    #     print("受信エラーまたはBCCエラーが発生しました。")

    # シリアルポートを閉じる
    # ser.close()
    # print("シリアルポートが閉じられました。")


if __name__ == "__main__":
    main()
