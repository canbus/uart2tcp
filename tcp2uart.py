import serial
from socket import *
from threading import Thread

def parseJson(jsonStr):
    d = eval(jsonStr)
    # d = dict()
    print("parseJson", d)
    if 't' in d: 
        print(d['t'],d['data'])
    pass

def rxThread(name,tcpfd,uart):
   print(name)
   while True:
      try:
        bytes = tcpfd.recv(100)
        if len(bytes) > 0:
            s = bytes.decode('utf-8')
            # print("tcp:", s)
            parseJson(s)
            uart.write(bytes)
      except timeout:
        #  print("time out")  
        pass
      finally:
         pass


uart = serial.Serial()
tcpfd = socket(AF_INET, SOCK_STREAM)

tcpfd.connect(("127.0.0.1",8800)) #121.37.241.174
tcpfd.settimeout(0.5)

uart.port='com12'
uart.baudrate=115200
uart.bytesize=8
uart.stopbits=1
uart.parity="N"
uart.dtr = 0
uart.rts = 0
uart.timeout = 0.5
uart.open()
if(uart.isOpen()):
 print("serial open success")
else:
 print("serial open fail")

thread1 = Thread(target=rxThread,args=("tcptask",tcpfd,uart))
thread1.start()
while(True):
    str = uart.read(100)
    if len(str) > 0:
        print("uart:" , str)
        tcpfd.send(str)
    
thread1.join() #等待线程结束
uart.close()
tcp.close()
