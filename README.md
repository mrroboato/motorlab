# sensorslab

#### CMU Mechatronics 2017 Team A Motor Lab



## How to run

1. Run the arduino program
	* Install both libraries in /arduino/libraries/ (in arduino IDE, Sketch > Include Library > Add .ZIP library)
	* Open /arduino/main/main.ino with the arduino IDE and upload to arduino.

2. GUI Server
	* Precompiled binaries are located in /executables/. Unzip your OS named zip.
	* Navigate to the unzipped directory in commnad line. Execute "./gui <i>portname</i>" on mac/linux, and "gui.exe <i>portname</i>" on Windows. You may have to chmod the binary.
		- In Windows, alt+d in file explorer and type cmd. 
		- Easiest way to find the portname is with the arduino IDE. 
	* Connect to localhost:2441 in a browser to see sensor data and motor control.


## Common Errors

1. When uploading to arduino on linux, you might encounter this error(with differing portnames):

		avrdude: ser_open(): can't open device "/dev/ttyACM0": Permission denied
		ioctl("TIOCMGET"): Inappropriate ioctl for device

	To fix it, enter the command:

		$ sudo usermod -a -G dialout <username>
		$ sudo chmod a+rw /dev/ttyACM0
