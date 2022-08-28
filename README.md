# Digital Hygrometer, WiFi #

This repository contains source code that is to run on the ESP8266 SOM as found on the digital hygrometer.  

### ARCHITECTURE ###
The ESP8266 SOM can be bootloaded over the USB-to-serial interface, but when not bootloading, the SOM's serial port is connected to the application processor (*PIC18F67K22*).  The application processor serves up a CLI to the user which allows him/her to enter email addresses, wifi parameters, etc.  Information pertaining to WiFi, and thus any information needing to be sent to the user via email or other web services, is forwarded to the ESP82666 SOM via way of a JSON formatted sting.  The JSON format was chosen due to its human readability, the fact that it's easy to work with, and because of its general ubiquity.  

### TOOL SUITE ###

The source file(s) are to be compiled using Arduino IDE v1.8.8, or newer.    

It is necessary to include esp8266 libraries. First, it must be verified that the correct board manager URL is included under the preference window.  Navigate to *File > Preferences*, and add the following URL: http://arduino.esp8266.com/stable/package_esp8266com_index.json

After including the necessary board manager URL, navigate to *tools > board > boards manager*, and install the ESP8266 manager.  The appropriate one will be listed as *written by the "community"*.

It is necessary to install the  **ArduinoJson**  library.  To do so, click on *Tools > Include Libraries > Manage Libraries*.  Within the library manager, serach for **ArduinoJson**, and install version 6.18.0 (version installed at the time of writing this).  

### COMPILING ###
The digital hygrometer utilizes a bare ESP8266 WiFi SOM.  For proper compilation, the target board shall be  *Generic ESP8266 Module*.  

### VERSIONS ###
* V0.0.1 -- New email method is working, but this version desires a good amount of cleanup.  This version is not ready to be deployed.

* V0.0.2 -- The email sending routine is working really well.  However, it won't be tagged as a release until it is tested in the system.  All network/account parameters are passed in via JSON.

* v1.0.0 -- Working really well, even with the hygrometer naming key.  The baud rate has been reduced to 9600 to support necessary algorithm changes at the MCU.  
