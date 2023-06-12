import VL53L1X
import time
import serial
# Making a pipe to write too so that the state machine can read it


# Open and start the VL53L1X sensor.
# If you've previously used change-address.py then you
# should use the new i2c address here.
# If you're using a software i2c bus (ie: HyperPixel4) then
# you should `ls /dev/i2c-*` and use the relevant bus number.
tof1 = VL53L1X.VL53L1X(i2c_bus=7, i2c_address=0x29)
tof1.open()

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
f = open("TOF_data.txt", 'w')

try:
    while(1):
        distance_in_mm1 = tof1.get_distance()
        
        message = "Distance: " + str(distance_in_mm1) + "mm\n"
        #print(distance_in_mm1)
        print(message)
        f.write(str(distance_in_mm1) + '\n')
        
        time.sleep(0.5) #without this it wont let the keyboard interreupt happen
except KeyboardInterrupt:
        tof1.stop_ranging()
        tof1.close()
        f.close()
        print('done')

