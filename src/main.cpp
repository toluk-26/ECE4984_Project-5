/*
  Virginia Tech
  ECE 4984/5984 - Fall 2025
  Design Project 5 Starter Code
  Scott Midkiff (midkiff@vt.edu)
  September 28, 2025

  Bluetooth Low Energy (BLE) Client Consuming Temperature Data from a Remote
  Server and Displaying Temperature

  Acknowledgments:
  - "Bluetooth Overview" and other wiki pages at Seeed Studio:
    https://wiki.seeedstudio.com/Wio-Terminal-Bluetooth-Overview/.

  - Seeed Arduino rpcBLE library:
  https://github.com/Seeed-Studio/Seeed_Arduino_rpcBLE.

  - This code build from the BLE_client.ino example from the Seeed Arduino
  library:
    https://github.com/Seeed-Studio/Seeed_Arduino_rpcBLE/tree/master/examples/BLE_client.

  - General use of the Wio Terminal based on examples from Wio Terminal
    Classroom, Lakshantha Dissanayake (lakshanthad), at:
    https://github.com/lakshanthad/Wio_Terminal_Classroom_Arduino.
*/


/**
 * Modified: Oct 24, 2025 - Tolu Kolade (toluk@vt.edu), Alex Halloran (alexanderha22@vt.edu)
 * ECE 4984/5984 - Fall 2025
 * Design Project 5
 * instructions are in the readme.md
 */

#include <Arduino.h>
#include "functions.hpp"
#include "screens.hpp"

// Setup function.
void setup() {
        // Initialize the display by starting the TFT LCD and setting screen
    // rotation. The screen rotation is set for "landscape" mode with the USB
    // connector at the bottom of the screen.
    tft.begin();
    tft.setRotation(3);

    screenHome();

#ifdef DEBUG
    Serial.begin(115200);
    while (!Serial) {
    };
#endif


    // Display blank area for data source. Draw the area for displaying
    // temperature.
    displaySource("");
    drawValues();

    // Indicate that the device is initializing and delay by DELAY_STATEPAUSE
    // seconds to keep the state visible.
    clientState = STATE_INIT;
    displayState("Initializing");
    delay(DELAY_STATEPAUSE * 1000);

#ifdef DEBUG
    Serial.println("STATUS: Starting BLE temperature client");
#endif
    BLEDevice::init("");

    // Retrieve a scanner and set the callback routine to use to be informed
    // when a new device is detected. Specify active scanning and start the scan
    // to run for BLE_SCAN_DURATION seconds. The scan interval and scan window
    // times here seem to work well, but other values might also work well.
    BLEScan *pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setInterval(1349);
    pBLEScan->setWindow(449);
    pBLEScan->setActiveScan(true);

    // Update state and display.
    clientState = STATE_SCAN;
    displayState("Scanning");

    // Start scanning.
    pBLEScan->start(BLE_SCAN_DURATION, false);

    // Set a time past the end of the scan to know that the scan has failed to
    // find an advertising server of interest.
    scanFailedTime = millis() + (BLE_SCAN_DURATION * 1000) + 1000;
}

// Main loop function.
void loop() {

    // If the state is STATE_SERVFND (server found), then the client has scanned
    // and found a BLE server that has the target MAC address. Attempt to have
    // the client connect to the advertised device. The call to
    // connectToServer() returns true if the client connects and finds the
    // target service and characteristic and can read data and receive
    // notifications for the characteristic. Otherwise, it returns false. Update
    // the state and state message based on success or failure.
    if (clientState == STATE_SERVFND) {

        if (connectToServer()) {
#ifdef DEBUG
            Serial.println("STATUS: Client connected to remote BLE server");
#endif

            // Update state and display.
            clientState = STATE_ACTIVE;
            displayState("Connection Active");
        } else {
#ifdef DEBUG
            Serial.println(
                "ERROR: Client failed to connect to the server, so stopping");
#endif

            // Update state and display;
            clientState = STATE_CONNFAIL;
            displayState("Connection Failed");
        }

    }

    // If the client is in the STATE_SCAN (scanning) state and the scan duration
    // has been exceeded, the the scan has failed to find a server with the
    // target address. The client updates the state and displays the state.
    else if (clientState == STATE_SCAN && scanFailedTime < millis()) {
#ifdef DEBUG
        Serial.println("ERROR: Scanning ended without finding a target device, "
                       "so stopping");
#endif

        // Update state and display.
        clientState = STATE_SCANFAIL;
        displayState("Scan Failed");
    }

    // Wait a bit for continuing loop().
    delay(1000);
}