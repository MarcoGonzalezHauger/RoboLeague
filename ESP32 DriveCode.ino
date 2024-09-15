#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>


// Create a BLE Server
BLEServer *pServer = NULL;
// Create a BLE Characteristic
BLECharacteristic *pCharacteristic = NULL;

// UUIDs for the BLE service and characteristic
#define SERVICE_UUID        "12345678-1234-1234-1234-123456789012"
#define CHARACTERISTIC_UUID "abcdefab-cdef-1234-5678-abcdefabcdef"

// Flag to detect if data has been received
bool dataReceived = false;

// Motor A connections
int enA = 22;
int in1 = 4;
int in2 = 18;
// Motor B connections
int enB = 23;
int in3 = 19;
int in4 = 21;


char state;

bool debug = true;


// Callback class to handle when a BLE client writes data to the characteristic
class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    dataReceived = true;  // Set the flag when data is written
  }
};

// Callback class to handle when a BLE client connects or disconnects
class ServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    Serial.println("Client connected.");
  }

  void onDisconnect(BLEServer* pServer) {
    Serial.println("Client disconnected. Restarting advertising...");
    pServer->getAdvertising()->start();  // Restart advertising when disconnected
  }
};

void setup() {
  Serial.begin(115200);

  // Initialize BLE
  BLEDevice::init("RoboLeague-Red-Bot");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());  // Set the server callbacks

  // Create a BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE
                    );

  // Set the initial value for the characteristic
  pCharacteristic->setValue("Waiting for data...");

  // Set the callback for the characteristic to handle write events
  pCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting for a client to connect...");

  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

void loop() {
  // Check if data has been received
  if (dataReceived) {
    dataReceived = false;  // Reset the flag
    
    // Retrieve the value written to the characteristic
    String value = pCharacteristic->getValue();

    // Handle the received value
    for (int i = 0; i < value.length(); i++) {
      state = ((char)value[i]);
    }
  }

  delay(10);  
  directionControl();
}

void directionControl() 
{
  switch (state) 
  {
    case 'F': // front
      leftmotorforward(255);
      rightmotorforward(255);
      if(debug) Serial.println("Forward");
      break;
    case 'X': // front left 
      leftmotorforward(178);
      rightmotorforward(255);
      if(debug) Serial.println("Forward Left Diagonal");
      break;
    case 'Y': // front right 
      leftmotorforward(255);
      rightmotorforward(178);
      if(debug) Serial.println("Forward Right Diagonal");
      break;
    case 'R': // right 
      leftmotorbackward(255);
      rightmotorforward(255);
      if(debug) Serial.println("Right");
      break;
    case 'Z': //back right 
      leftmotorbackward(255);
      rightmotorbackward(178);
      if(debug) Serial.println("Back Right Diagonal");
      break;
    case 'B': //back 
      leftmotorbackward(255);
      rightmotorbackward(255);
      if(debug) Serial.println("Back");
      break;
    case 'W': //back left
      leftmotorbackward(178);
      rightmotorbackward(255);
      if(debug) Serial.println("Back Left Diagonal");
      break;
    case 'L': //left
      leftmotorforward(255);
      rightmotorbackward(255);
      if(debug) Serial.println("Left");
      break;
    case 'N': //Stop
      leftmotorstop();
      rightmotorstop();
      if(debug) Serial.println("Stop");
      break;
    default:
      break;
  }
} 

void leftmotorstop() {
  analogWrite(enA, 0);
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
}

void rightmotorstop() {
  analogWrite(enB, 0);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

void leftmotorforward(int speed) {
  analogWrite(enA, speed);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
}

void rightmotorforward(int speed) {
  analogWrite(enB, speed);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

void leftmotorbackward(int speed) {
  analogWrite(enA, speed);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
}

void rightmotorbackward(int speed) {
  analogWrite(enB, speed);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}