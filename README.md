THE HOURGLASS PROJECT 

The hourglass project delivers an electronic
Hourglass based on Arduino, GY-521, and LED Matrix
displays.  You can simply build and run this on your
Linuz/MacOSX PC and see the pc simulation without the
above mentioned hardware.

The blog for the project with videos and pictures is here:
http://blog.kodeten.com/hourglass-project-final-status-part-6/



To build the hourglas object, you will need:

Hardware:

1.  Arduino compatible board (I used UNO others will
work as well)

2.  GY-521 berakout board (bought online)

3.  And 3 matrix displays driven by MAX7219 (bought
online)

4.  A project housing, I used cardboard

5.  The  usual jumper wires and 9V battery

Libraries:

1. LedControl library available via Arduino IDE/Tools/Manage Library - add it.
2. Arduino/MPU6050, Arduino/I2Cdev directories of then J.Rowberg I2C library https://github.com/jrowberg/i2cdevlib, just copy them to "libraries" directory of you Arduino IDE


Files in this repository:

Geom.hpp:
Geom.cpp:  These are the main onject filles.
Grav object is for gravity, it gets updated by sensor readings.
It also knows how balls fall  for a particular sensor resading via
a constant array grfl[].
Locations object is where all possible locations for the
balls to follow in their path are kept.  Acts like a controller
of the MVC at times.
Ball object is bwhere individual balls are modelled. Balls need
Locations object to do anything useful.

hourglass.ino:  Arduino file.  It has the sleep/wake up function.
It works with the sensor reading files. It uses the LedCotrol
library calls to turn on/off leds as per Locations display data.

mpu6050.hpp:
mpu6050.cpp: The second of these 2 files come from i2cdevice
library, I simply modified them to use only the parts I need
and to define the global array ypr[] needed for the sensor
reading.

test_hg.cpp: Only needed to run the pc simulation.  The UI is
nothing but spitting out rows of character data, adjust the # of
newlines written for each "frame" per your terminal row count.

macros.h:  nothing important here.

The blog for the project with videos and pictures is here:
http://blog.kodeten.com/hourglass-project-final-status-part-6/


