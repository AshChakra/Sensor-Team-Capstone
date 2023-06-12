This is a complex folder as it contains a lot. 
Within the dac_spi folder, the commands to interface with the accelerator drive by wire subsystem are contained
Within the tof folder, the library for vl53 interfacing is contained

tof_test.py is code that opens 2 of the 4 TOFs and starts writing to the pipe TOF_data. This is incomplete due to hardware failure, run before running hello_world.c
tof_test_real.py is code that opens all 4 of the TOFs and starts writing to the pipe TOF_data. This is the complete version, run before running hello_world_real.c
object_detection.py is code from the AI team, when the camera detects a human being it uses a pipe to output a 1 and a 0 if not
hello_world.c is code that was written on the fly to finish the hello_world test due to hardware failure, but is not complete
hello_world_real.c is the complete verison, make sure to have tof_test_real.py and object_detection.py running in parralel. 
