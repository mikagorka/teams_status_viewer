#include <Wire.h>
#include "HT_SSD1306Wire.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

static SSD1306Wire display(0x3c, 500000, SDA_OLED, SCL_OLED, GEOMETRY_128_64, RST_OLED);

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
String currentStatus = "available";
unsigned long lastBlinkTime = 0;
bool minusVisible = true;

// UUIDs for BLE Service
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// Function declarations
void displayStatusAvailable();
void displayStatusInCall();
void transitionAnimation();

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      Serial.println("Device Connected");
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      Serial.println("Device Disconnected");
      BLEDevice::startAdvertising();
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      String value = pCharacteristic->getValue().c_str();
      
      if (value.length() > 0) {
        Serial.print("Received: ");
        Serial.println(value);
        
        if (value == "available" && currentStatus != "available") {
          transitionAnimation();
          currentStatus = "available";
          displayStatusAvailable();
        }
        else if (value == "incall" && currentStatus != "incall") {
          transitionAnimation();
          currentStatus = "incall";
          minusVisible = true;
          displayStatusInCall();
        }
      }
    }
};

void setup() {
  Serial.begin(115200);
  
  // Turn on Vext (Display power supply)
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, LOW);
  delay(100);
  
  // Initialize display
  display.init();
  display.clear();
  
  // Initialize BLE
  BLEDevice::init("Teams Status");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  
  BLEService *pService = pServer->createService(SERVICE_UUID);
  
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE
                    );
  
  pCharacteristic->setCallbacks(new MyCallbacks());
  pCharacteristic->addDescriptor(new BLE2902());
  
  pService->start();
  
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  
  Serial.println("Bluetooth started! Name: Teams Status");
  
  displayStatusAvailable();
}

void loop() {
  // Blink logic only for "In Call" - every 500ms
  if (currentStatus == "incall") {
    if (millis() - lastBlinkTime >= 500) {
      minusVisible = !minusVisible;
      displayStatusInCall();
      lastBlinkTime = millis();
    }
  }
  delay(10);
}

void transitionAnimation() {
  // Zoom-out animation
  for (int r = 28; r > 0; r -= 4) {
    display.clear();
    display.setColor(WHITE);
    display.fillCircle(64, 32, r);
    display.display();
    delay(30);
  }
  
  // Short pause
  display.clear();
  display.display();
  delay(100);
  
  // Zoom-in animation
  for (int r = 0; r <= 28; r += 4) {
    display.clear();
    display.setColor(WHITE);
    display.fillCircle(64, 32, r);
    display.display();
    delay(30);
  }
}

void displayStatusAvailable() {
  int centerX = 64;
  int centerY = 32;
  int radius = 28;
  
  display.clear();
  display.setColor(WHITE);
  display.fillCircle(centerX, centerY, radius);
  
  display.setColor(BLACK);
  // Draw checkmark
  for(int i = 0; i < 6; i++) {
    display.drawLine(centerX - 12, centerY + i - 2, centerX - 5, centerY + 8 + i - 2);
  }
  for(int i = 0; i < 6; i++) {
    display.drawLine(centerX - 5, centerY + 8 + i - 2, centerX + 12, centerY - 10 + i - 2);
  }
  
  display.display();
}

void displayStatusInCall() {
  int centerX = 64;
  int centerY = 32;
  int radius = 28;
  
  display.clear();
  
  // Gray circle using dot pattern (dithering)
  display.setColor(WHITE);
  for (int y = -radius; y <= radius; y++) {
    for (int x = -radius; x <= radius; x++) {
      // Check if point is inside circle
      if (x*x + y*y <= radius*radius) {
        // Checkerboard pattern for gray effect
        if ((x + y) % 2 == 0) {
          display.setPixel(centerX + x, centerY + y);
        }
      }
    }
  }
  
  // Minus only when visible
  if (minusVisible) {
    display.setColor(BLACK);
    for(int i = 0; i < 6; i++) {
      display.drawLine(centerX - 12, centerY + i - 3, centerX + 12, centerY + i - 3);
    }
  }
  
  display.display();
}
