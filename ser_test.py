import serial
import time
import sys

port = sys.argv[1]

ser = serial.Serial(port, 9600)
tmp = 0

inn = 0
inn_status = (t, m, b, f, o, e)


while True:
    try:
        if tmp > 9:
            tmp = 0
        data = '#m7 1422 7 12  1 0  ' + str(tmp)  + '  11230Gordon        Brach         *'
        ser.write(data)
        tmp += 1
        time.sleep(2)
        
    except KeyboardInterrupt:
        ser.close()
        print '\nDone'
        break
