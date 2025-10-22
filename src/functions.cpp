#include "functions.hpp"
#include "screens.hpp"

BLEAddress TempServer(bd_addr);
unsigned long scanFailedTime = 0;
int clientState = STATE_INIT;

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
                   uint8_t *pData, size_t length, bool isNotify) {
    // Convert value to null terminated string and display it as temperature.
    char valueBuffer[8];
    for (size_t j = 0; j < length; ++j) {
        valueBuffer[j] = (char)*pData;
        ++pData;
    }
    valueBuffer[length] = '\0';
    displayTempText(valueBuffer);

#ifdef DEBUG
    Serial.print("STATUS: Notify callback for temperature characteristic: ");
    Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    Serial.print(" of data length ");
    Serial.println(length);
    Serial.print(" * Temperature: ");
    Serial.println(valueBuffer);
#endif
}

static void
notifyHumiCallback(BLERemoteCharacteristic *pBLERemoteCharacteristic,
                   uint8_t *pData, size_t length, bool isNotify) {
    // Convert value to null terminated string and display it as humidity.
    char valueBuffer[8];
    for (size_t j = 0; j < length; ++j) {
        valueBuffer[j] = (char)*pData;
        ++pData;
    }
    valueBuffer[length] = '\0';
    displayHumidityText(valueBuffer);

#ifdef DEBUG
    Serial.print("STATUS: Notify callback for humidity characteristic: ");
    Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    Serial.print(" of data length ");
    Serial.println(length);
    Serial.print(" * Humidity: ");
    Serial.println(valueBuffer);
#endif
}

void MyClientCallback::onConnect(BLEClient *pclient) {
// onConnect is just for debugging.
#ifdef DEBUG
    Serial.println("STATUS: onConnect called");
#endif
}

void MyClientCallback::onDisconnect(BLEClient *pclient) {
    // Set state to show client is not connected. Update state and display.
    // Delay a little to allow previous state to be visible.
    clientState = STATE_DISCONN;
    delay(DELAY_STATEPAUSE * 1000);
    displayState("Disconnected");

#ifdef DEBUG
    Serial.println("STATUS: onDisconnect called");
#endif
}

void MyAdvertisedDeviceCallbacks::onResult(
    BLEAdvertisedDevice advertisedDevice) {
#ifdef DEBUG
    Serial.print("STATUS: BLE advertised device is ");
    Serial.println(advertisedDevice.toString().c_str());
#endif

    // For this device, check if its 48-bit BLE MAC address matches the
    // target server's MAC address. If it does, then stop scanning, set
    // myDevice to the device that is found, and set the client state to
    // STATE_SERVFND (server found) to indicate that a connection is needed.
    if (memcmp(advertisedDevice.getAddress().getNative(),
               TempServer.getNative(), 6) == 0) {
#ifdef DEBUG
        Serial.print("STATUS: Temperature server device found - ");
        Serial.println(advertisedDevice.toString().c_str());
#endif

        // Stop the scan.
        BLEDevice::getScan()->stop();

        // Set myDevice to point to this advertised device.
        myDevice = new BLEAdvertisedDevice(advertisedDevice);

        // Set new client state that this is the device to which to connect
        // and display.
        clientState = STATE_SERVFND;
        displayState("Server Found");

#ifdef DEBUG
        Serial.println("STATUS: Server Found");
#endif
        
    }
}

// Connect to the server once an advertised device is found that matches the target
// 48-bit BLE MAC address. This routine will:
// - Connect to the server device (mydevice)
// - Verify that the server has the right service (based on UUID)
// - Verify that the service can provide the temperature characteristic via a read and,
//   if so, update the temperature display
// - Verify if the service can provide notifications of the temperature characteristic
//   and, if so, register for notifications
// The routine returns false (failure) if any of the verifications fail. Otherwise,
// the routine returns true.
bool connectToServer() {
  #ifdef DEBUG
  Serial.print("STATUS: Connecting to ");
  Serial.println(myDevice->getAddress().toString().c_str());
  #endif

  // Create a client object which allows access to the server attributes. Set
  // callback routines for the client (onConnect and onDisconnect).
  BLEClient*  pClient  = BLEDevice::createClient();
  pClient->setClientCallbacks(new MyClientCallback());
	
  // Actually connect to the remote server as found from scanning to have the target
  // MAC address. Exit with error if unsuccessful.
  if (pClient->connect(myDevice)) {
    #ifdef DEBUG
    Serial.println("STATUS: Connected to server, but not verified service, etc.");
    #endif   

    // Set state to show that client is now connected. Update state and display.
    // Delay a bit to allow previous state to be visible.
    clientState = STATE_CONN;
    displayState("Connected");
  }
  else {
    #ifdef DEBUG
    Serial.println("ERROR: Connect to server failed");
    #endif

    return(false);
  }

  #ifdef DEBUG
  auto i = pClient->getServices();
    for (auto c :*i)
        Serial.println(c.first.c_str());
  #endif

  // Obtain a reference to the sensor service at the remote BLE server. Disconnect
  // and fail (return false) if the pointer for the remote service is null.
  BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
  if (pRemoteService == nullptr) {
    #ifdef DEBUG
    Serial.print("ERROR: Failed to find service with UUID: ");
    Serial.println(serviceUUID.toString().c_str());
    #endif

    // Disconnect and return false.
    pClient->disconnect();
    return false;
  }

  // Display the data source (the name of the server device). Note that the call
  // to myDevice-getName() returns a string of type std::string. The c_str()
  // method will turn this into a C style null-terminated string.
  char sourceBuffer[32];
  strcpy(sourceBuffer, myDevice->getName().c_str());
  displaySource(sourceBuffer);

  #ifdef DEBUG  
  Serial.print("STATUS: Connected to server ");
  Serial.print(sourceBuffer);
  Serial.print(" with service UUID ");
  Serial.println(serviceUUID.toString().c_str());
  #endif

  // Obtain a reference to the temperature characteristic in the service of the
  // remote BLE server. Exit with failure if the client cannot find the
  // characteristic.
  pRemoteTempCharacteristic = pRemoteService->getCharacteristic(tempCharUUID);
  if (pRemoteTempCharacteristic == nullptr) {
    #ifdef DEBUG
    Serial.print("ERROR: Failed to find temperature characteristic UUID ");
    Serial.println(tempCharUUID.toString().c_str());
    #endif

    // Disconnect and return false.
    pClient->disconnect();
    return false;
  }
  else {
    #ifdef DEBUG
    Serial.print("STATUS: Found temperature characteristic UUID ");
    Serial.println(tempCharUUID.toString().c_str());
    #endif
  }
  
  // Obtain a reference to the humidity characteristic in the service of the
  // remote BLE server. Exit with failure if the client cannot find the
  // characteristic.
  pRemoteHumiCharacteristic = pRemoteService->getCharacteristic(humiCharUUID);
  if (pRemoteHumiCharacteristic == nullptr) {
    #ifdef DEBUG
    Serial.print("ERROR: Failed to find humidity characteristic UUID ");
    Serial.println(humiCharUUID.toString().c_str());
    #endif

    // Disconnect and return false.
    pClient->disconnect();
    return false;
  }
  else {
    #ifdef DEBUG
    Serial.print("STATUS: Found humidity characteristic UUID ");
    Serial.println(humiCharUUID.toString().c_str());
    #endif
  }

    // Read the value of the temperature characteristic and display it. If the
  // client cannot read the characteristic, then fail.
  if(pRemoteTempCharacteristic->canRead()) {
    #ifdef DEBUG
    Serial.println("STATUS: Can read temperature characteristic");
    #endif

    // Get the temperature string and display it.
    std::string value = pRemoteTempCharacteristic->readValue();
    char tempBuffer[8];
    strcpy(tempBuffer, value.c_str());
    displayTempText(tempBuffer);

    #ifdef DEBUG
    Serial.print("STATUS: Temperature is ");
    Serial.println(tempBuffer);
    #endif
  }
  else {
    #ifdef DEBUG
    Serial.println("ERROR: Cannot read temperature characteristic");
    #endif

    // Disconnect and return false.
    pClient->disconnect();
    return false;
  }

  // Read the value of the humidity characteristic and display it. If the
  // client cannot read the characteristic, then fail.
  if(pRemoteHumiCharacteristic->canRead()) {
    #ifdef DEBUG
    Serial.println("STATUS: Can read humidity characteristic");
    #endif

    // Get the humidity string and display it.
    std::string value = pRemoteHumiCharacteristic->readValue();
    char tempBuffer[8];
    strcpy(tempBuffer, value.c_str());
    displayHumidityText(tempBuffer);

    #ifdef DEBUG
    Serial.print("STATUS: Humidity is ");
    Serial.println(tempBuffer);
    #endif
  }
  else {
    #ifdef DEBUG
    Serial.println("ERROR: Cannot read humidity characteristic");
    #endif

    // Disconnect and return false.
    pClient->disconnect();
    return false;
  }

    // Register for notifications for the temperature characteristic. Fail
  // if the client cannot receive notifications for this characteristic.
  if(pRemoteTempCharacteristic->canNotify()) {
    #ifdef DEBUG
    Serial.println("STATUS: Can use notifications for temperature");
    #endif

    pRemoteTempCharacteristic->registerForNotify(notifyTempCallback);
  }
  else {
    #ifdef DEBUG
    Serial.println("ERROR: Notifications not supported for temperature");
    #endif

    // Disconnect and return false.
    pClient->disconnect();
    return false;
  }
    
  // Register for notifications for the humidity characteristic. Fail
  // if the client cannot receive notifications for this characteristic.
  if(pRemoteHumiCharacteristic->canNotify()) {
    #ifdef DEBUG
    Serial.println("STATUS: Can use notifications for humidity");
    #endif

    pRemoteHumiCharacteristic->registerForNotify(notifyHumiCallback);
  }
  else {
    #ifdef DEBUG
    Serial.println("ERROR: Notifications not supported for humidity");
    #endif

    // Disconnect and return false.
    pClient->disconnect();
    return false;
  }

  // Return success (true).
  return true;
}