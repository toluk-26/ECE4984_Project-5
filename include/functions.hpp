#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

// Include header files. Based on experience with the server, the two BLE header
// files are included before the others.
#include <BLEAdvertisedDevice.h> // rpcBLE advertised device header file
#include <BLEScan.h>             // rpcBLE scanner header file
#include <rpcBLEDevice.h>        // rpcBLE device header file

// Declare client state values and global client state varialble.
#define STATE_INIT 0     // Initializing
#define STATE_SCAN 1     // Scanning
#define STATE_SCANFAIL 2 // Scan failed
#define STATE_SERVFND 3  // Server found
#define STATE_CONN 4     // Connected
#define STATE_ACTIVE 5   // Connection active
#define STATE_CONNFAIL 6 // Connection failed
#define STATE_DISCONN 7  // Disconnected
static int clientState = STATE_INIT;

// Define delay time for displays in seconds.
#define DELAY_STATEPAUSE 2 // Delay time to see state where needed

// In this client, we restrict the server of interest to a particular BLE device
// which is identified by the 48-bit MAC address of the server's BLE interface.
// An easy way to discover this address is from a device scan by the Nordic nRF
// Connect for Mobile application.
//
// Note that byte order as stored here is reversed from what is diplayed for the
// BLE MAC address. For example, address "2C:F7:F1:0A:18:7D" would be
// initialized as: uint8_t bd_addr[6] = {0x7d, 0x18, 0x0a, 0xf1, 0xf7, 0x2c};
//
// Define the BLE device address of interest and convert it to a BLEAddress
// type. Be sure that bd_addr this is the MAC address of the target server. Be
// careful of byte ordering.
//
// ** UPDATE THIS VALUE TO MATCH YOUR SERVER DEVICE. **
static uint8_t bd_addr[6] = {0x00, 0x00, 0x00,
                             0x00, 0x00, 0x00}; // Kit NN BLE MAC address
extern BLEAddress TempServer;

// Define BLE scan duration in seconds. 5 seconds is generally a good value, but
// while debugging, the client will likely miss the server advertisement when
// resetting since the server takes some time to figure out that it is now
// disconnected. 10 seconds seems to work well for this situation.
#define BLE_SCAN_DURATION 10

// Declare global for time that the scan is considered to have failed. This
// time will be set to be a bit past the current clock plus the scan duration.
extern unsigned long scanFailedTime;

// ***** Initializations for BLE Follow *****

// Define custom UUID values used at the BLE server. These values must match
// those used on the server. In this program, we just use the service UUID and
// the characteristic UUID.
#define SENSOR_SERVICE_UUID "019971b3-31c5-73de-8e9e-e47c22fc0c87"
#define TEMP_CHARACTERISTIC_UUID "019971b3-31c5-73de-8e9e-e47c22fc0c88"
#define HUMIDITY_CHARACTERISTIC_UUID "019971b3-31c5-73de-8e9e-e47c22fc0c8a"

// Convert these UUIDs from character strings to BLEUUID types.
static BLEUUID serviceUUID(SENSOR_SERVICE_UUID);
static BLEUUID tempCharUUID(TEMP_CHARACTERISTIC_UUID);
static BLEUUID humiCharUUID(HUMIDITY_CHARACTERISTIC_UUID);

// Declare global variables for the pointers to the temperature characteristics
// at the remote endpoint (the server) and for the server device which has
// advertised itself.
static BLERemoteCharacteristic *pRemoteTempCharacteristic;
static BLERemoteCharacteristic *pRemoteHumiCharacteristic;
static BLEAdvertisedDevice *myDevice;

// Define the callback routine for new temperature data from the server from a
// notification. Display the temperature value. The callback parameters provide
// a pointer to the remote characteristic, the data (as bytes) and the length of
// the data (number of bytes).
static void
notifyTempCallback(BLERemoteCharacteristic *pBLERemoteCharacteristic,
                   uint8_t *pData, size_t length, bool isNotify);
static void
notifyHumiCallback(BLERemoteCharacteristic *pBLERemoteCharacteristic,
                   uint8_t *pData, size_t length, bool isNotify);

// Define callback routines for the client device. These are called on
// connect and on disconnect. The callback parameter provides a pointer to
// the client connected to the server.
class MyClientCallback : public BLEClientCallbacks {
    void onConnect(BLEClient *pclient);
    void onDisconnect(BLEClient *pclient);
};

// Define callback routine for scanning. onResult() will be called for each
// advertised device found. The onResult() callback routine will check if the
// address of the advertised device mataches the target for this client. The
// callback parameter provides a pointer to the advertised device that has been
// found.
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice);
};

bool connectToServer();

#endif // FUNCTIONS_HPP