# esprepeater
Simplex parrot-style repeater for handheld radios based on ESP32 (Audiokit) boards using Arduino

# Features
This projects makes use of ESP32 Audiokit boards ; which combines an ESP32 microcontoller to an audio player & recorder ; to build a simplex (also called parrot) repeater for VOX-enabled handhled radios (tested with Quansheng, Baofeng and other brands)
Target audience is amateur radios seeking a cheap and simple way to make a radio repeater using Arduino. 


# Required hardware:
1. ESP32 Audiokit board. List of supported boards (https://github.com/pschatzmann/arduino-audio-tools/wiki/Audio-Boards)

![Audio kit1](https://github.com/user-attachments/assets/9f793611-5125-4110-93bb-63933673f182)


2. VOX enabled handheld radio with jack Kenwood-style connector (Baofeng, Quansheng, ...)

3. Audio cable
# Commercial audio cable

Kenwood to 3.5mm 4-pin jack connector  
Jack 3.5mm stereo to mic & speaker jack splitter

![Kenwood/Baofeng connector to speaker & mic jack connection](images/commercialCable.jpg)

You may also replace 3 and 4 with your own DIY audio cable (see details below)

# DIY audio cable
![Baofeng-Kenwood style audio connection](images/BaofengConnector.jpg)

4. PC running Arduino IDE (https://docs.arduino.cc/software/ide) (Windows/Mac OS/Linux is OK)
   Follow this tutorial for details: -https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html)
   You may need to install the driver for the ESP32 board, in my case (https://www.silabs.com/software-and-tools/usb-to-uart-bridge-vcp-drivers?tab=downloads)
   
6. Micro USB cable to connect ESP32 board

# Compiling
I prefer using VS Code with PlatformIO but for simplicity I will describe how to build using Arduino IDE.

1. Download and install latest version of Arduino IDE https://www.arduino.cc/en/software/ (available for Windows/Mac OS/Linux)
2. Download and install Git using defaults https://git-scm.com/install/
3. Download project contents using Git:
```
cd Documents/Arduino
git clone https://github.com/phmougin/esprepeater.git
```
4. Open Arduino IDE and open esprepeater.ino

Accept moving to separate directory:

<img width="503" height="140" alt="image" src="https://github.com/user-attachments/assets/196b0ad0-567d-4515-830d-08338a164103" />

6. Select board as ESP32 Dev Module
<img width="767" height="487" alt="image" src="https://github.com/user-attachments/assets/c49102eb-66ba-4f3c-86a8-a42cec8f781d" />

<img width="518" height="108" alt="image" src="https://github.com/user-attachments/assets/fffe1155-5f20-4ce3-9702-e858e83f351a" />

It will take several minutes to install...

7. Add Audiotools library: 
Download https://github.com/pschatzmann/arduino-audio-tools - Use Code - download ZIP

<img width="405" height="304" alt="image" src="https://github.com/user-attachments/assets/4f37525d-f3a0-4645-8b2e-9ff6eff70b60" />

In menu Sketch : Include library : Add .ZIP library  
Then select the .zip file for the library downloaded earlier.

7. Install AudioDriver library (same as step 6)  https://github.com/pschatzmann/arduino-audio-driver
8. Connect ESP32 Audiokit board to UART connector and click Upload button

# Running the repeater
1. Enable VOX mode on your transceiver and set an appropriate frequency. You may have to fiddle with the volume
2. Connect MIC cable to ESP32 SPEAKER connector
3. Connect SPEAKER cable to ESP32 MIC connector
4. Power up ESP32 board  
The board can run on USB power bank for several days depending on usage

<img width="1638" height="1229" alt="finalSetup" src="https://github.com/user-attachments/assets/46555963-2979-4190-9e22-8095972e10e1" />


# Disclaimer

This project is provided "as is" without warranty of any kind, either expressed or implied, including, but not limited to, the implied warranties of merchantability and fitness for a particular purpose.

## Trademarks

"Baofeng", "Kenwood", "Quansheng" and related marks are trademarks of their respective owners and are not related to this project.
