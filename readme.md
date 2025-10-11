# Design Project 5: BLE Temperature and Humidity Client
Special Study: IoT System Design 

Fall 2025

ECE 4984/5984

Virginia Tech  

# Getting Started
Use VSCode and open the project with the PlatformIO extension. It should install all the libraries when you try to build the project, or hopefully sooner. Go to [.pio\libdeps\seeed_wio_terminal\Seeed_Arduino_LCD\User_Setup_Select.h](.pio\libdeps\seeed_wio_terminal\Seeed_Arduino_LCD\User_Setup_Select.h) and comment line 160, 

`#include <User_Setups/Setup666_XIAO_ILI9341.h> // Default  Setup file for Seeed XIAO`

 and uncomment line 161 
 
`// #include <User_Setups/Setup500_Seeed_Wio_Terminal.h> // Setup file for Seeed Wio Terimal with SPI ILI9341 320x240`

It shouldn't give you any more errors.

# Review