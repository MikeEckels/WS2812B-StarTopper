# WS2812B-StarTopper
 RGB Christmas tree star topper with 126 individually adressable WS2812B LED's. The main controller is based off of an esp8266-12E embedded into a custom PCB to provide voltage regulation, 5v<-->3.3v level shifting, and an easy programming interface. Alternatively, the software also supports a standard ESP32Devkit. Other than the custom electronics, everything is 3D-printable.
 
 - [Pictures](https://github.com/MikeEckels/WS2812B-StarTopper/tree/main/Photos)
 - [3D Files](https://github.com/MikeEckels/WS2812B-StarTopper/tree/main/3DFiles)
 - [Altium Files](https://github.com/MikeEckels/WS2812B-StarTopper/tree/main/AltiumFiles)
 
 <img src="https://i.imgur.com/AshTQ8v.jpg" width="400">
 
 # Electronics Overview
 All schematics and PCB's were created in Altium Designer. The PCB's were sent to JLCPCB for fabrication. The heart of the system is the ESP8266-12E module. This module was embedded into a small PCB with a BSS138 level shifter for the 5v<-->3.3v conversion between the ESP and the WS2812B LED's. The module also contains a AP2112K-3.3 linear voltage regulator to power the ESP from the main 5V supply. The main star PCB contains 63 WS2812B LED's with power connected in parallel, and thier data lines in series. The ESP board connects to the first stars DIN pin, and the second star's DIN pin connects to the first star's DOUT pin. The entire system is powered by a MeanWell LPV 35-5 powersupply. If a custom PCB is not desired, a standard ESP32Devkit can be used instead.
 
 ## Schematics and Board Files
  - [PCB Files](https://github.com/MikeEckels/WS2812B-StarTopper/blob/main/AltiumFiles/)
  - [Schematic PDF](https://github.com/MikeEckels/WS2812B-StarTopper/blob/main/AltiumFiles/StarTopper.pdf)
  
 ## Power Supply
  - [Purchased Here](https://www.meanwell-web.com/en-gb/ac-dc-single-output-led-driver-constant-voltage-cv-lpv--35--5)
  - [STL's Here](https://github.com/MikeEckels/WS2812B-StarTopper/tree/main/3DFiles/STLs/PowerSupply)
  
 # Code Overveiew
 This library provides a simple wrapper around a popular LED control library called [FastLED](https://github.com/FastLED/FastLED). This enables simple and elegant control of every LED on the star as well as the ability to create and sequence patterns. I also developed a network manager to allow for seamless transitions from one network to another. That can be found [here](https://github.com/MikeEckels/EspWifiManager).
 
 ## Features
  - Quick LED addressing using [FastLED](https://github.com/FastLED/FastLED)
  - Seamless network management [EspWifiManager](https://github.com/MikeEckels/EspWifiManager)
  - App control over the network using [Blynk](https://github.com/blynkkk/blynk-library)
 ## Available Functions
  - Star::Begin(): Initialzie all pins and setup network
    - Arguments: None
    - Return: None
  - Star::Update(): Refreshes LED's and device state.
    - Arguments: None
    - Return: None
  - Star::NextPattern(): Ques the next pattern for the next Update() call
    - Arguments: None
    - Return: None
  - Star::LoopPatterns(unsigned int nSeconds) Ques all patterns to automatically change in sequence after nSeconds
    - Arguments: unsigned int nSeconds
    - Return: None
  - Star::SetPattern(unsigned int patternNumber) Ques a specific pattern from the list
    - Arguments: unsigned int patternNumber
    - Return: None
  - Star::SetMode(bool isPattern) sets the display mode (1 = pattern mode, 0 = color mode)
    - Arguments: bool isPattern
    - Return: None
  - Star::SetColor(unsigned int r, unsigned int g, unsigned int b) sets all LEDs on the star to a desired RGB value
    - Arguments: unsigned int r,g,b
    - Reurn: None
  - Star::SetBrightness(unsigned int brightness) sets all LEDs on the star to a desired brightness
    - Arguments: unsigned int brightness
    - Return: None
  - Star::SetFramesPerSecond(unsigned int fps) sets the refresh rate for patterns when in patern mode
    - Arguments: unsigned int fps
    - Return: None
    
 # App Control
 Using the [Blynk](https://github.com/blynkkk/blynk-library) arduino library and accompanying android and apple app, a simple user interface can be made. This allows for multiple types of widgets to be setup to transfer data back and fourth between the star and the app. In this simple setup, a color picker, a brightness slider, and a pattern mode button are used to give simple wireless functionality to the star. A Terminal widget is also used to display debug/status information.
 
 <img src="https://imgur.com/HmB0y5A.jpg" width="300">
