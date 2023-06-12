import VL53L1X
import os
import time
# Making a pipe to write too so that the state machine can read it
pipe_name = 'TOF_data'

try:
    os.mkfifo(pipe_name)
except OSError as oe: 
    print("Failed to create FIFO: %s" % oe)



# Open and start the VL53L1X sensor.
# If you've previously used change-address.py then you
# should use the new i2c address here.
# If you're using a software i2c bus (ie: HyperPixel4) then
# you should `ls /dev/i2c-*` and use the relevant bus number.
tof1 = VL53L1X.VL53L1X(i2c_bus=1, i2c_address=0x29)
tof2 = VL53L1X.VL53L1X(i2c_bus=1, i2c_address=0x2a)
tof3 = VL53L1X.VL53L1X(i2c_bus=1, i2c_address=0x2b)
tof4 = VL53L1X.VL53L1X(i2c_bus=1, i2c_address=0x2c)

tof1.open()
tof2.open()
tof3.open()
tof4.open()

# Optionally set an explicit timing budget
# These values are measurement time in microseconds,
# and inter-measurement time in milliseconds.
# If you uncomment the line below to set a budget you
# should use `tof.start_ranging(0)`
# tof.set_timing(66000, 70)

tof1.start_ranging(2)  # Start ranging
                      # 0 = Unchanged
                      # 1 = Short Range
                      # 2 = Medium Range
                      # 3 = Long Range
tof2.start_ranging(2)  # Start ranging
tof3.start_ranging(2)  # Start ranging
tof4.start_ranging(2)  # Start ranging

# Grab the range in mm, this function will block until
# a reading is returned.
try:
    while(1):
        distance_in_mm1 = tof1.get_distance()
        distance_in_mm2 = tof2.get_distance()
        distance_in_mm3 = tof3.get_distance()
        distance_in_mm4 = tof4.get_distance()
        
        message = str(distance_in_mm1) + ' ' + str(distance_in_mm2) + ' ' + str(distance_in_mm3) + ' ' + str(distance_in_mm4) + '\n'
        # print(message)
        pipeout = os.open(pipe_name, os.O_RDWR)
        os.write(pipeout, message.encode())
        os.close(pipeout)
        time.sleep(0.5) #without this it wont let the keyboard interreupt happen
except KeyboardInterrupt:
        tof1.stop_ranging()
        tof2.stop_ranging()
        tof3.stop_ranging()
        tof4.stop_ranging()
        tof1.close()
        tof2.close()
        tof3.close()
        tof4.close()
        #pipein = open(pipe_name, 'r')
        #try:
        #    while True:
        #        message = pipein.readline()[:-1]
        #        print("Received: '%s'" % message)
        #finally:
        #    pipein.close()
        print('done')



#print("TOF 0x29:", distance_in_mm1, "mm")
#print("TOF 0x2A:", distance_in_mm2, "mm")
#print("TOF 0x2B:", distance_in_mm3, "mm")
#print("TOF 0x2C:", distance_in_mm4, "mm")
#tof1.stop_ranging()
#tof2.stop_ranging()
#tof3.stop_ranging()
#tof4.stop_ranging()
