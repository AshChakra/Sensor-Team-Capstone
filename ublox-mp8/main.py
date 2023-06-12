import io
import pynmea2
import serial
import time

ser = serial.Serial('/dev/ttyTHS0', 9600, timeout=5.0)
sio = io.TextIOWrapper(io.BufferedRWPair(ser, ser))

f = open("coord.txt", 'w')

while 1:
    try:
        line = sio.readline()
        msg = pynmea2.parse(line)
        try:
            print(f"Long: {msg.longitude}")
            print(f"Lat: {msg.latitude}")
            
            f.write(f"{msg.longitude} {msg.latitude}\r\n")
        except:
            #print('Message Error: AAA')
            continue
    except serial.SerialException as e:
        print('Device error: {}'.format(e))
        break
    except pynmea2.ParseError as e:
        print('Parse error: {}'.format(e))
        continue
    except KeyboardInterrupt:
        print("exiting")
        f.close()
        break
