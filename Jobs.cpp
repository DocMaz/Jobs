#include <ArduinoBLE.h>

BLEService jobsService("1101");
BLECharacteristic triggerActionCharacteristic("2101", BLEWrite, sizeof(bool));

bool doorOpen = false;

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(9, OUTPUT); // Pin controlling the door actuator

  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    while (1);
  }

  BLE.setLocalName("JobsActionModel");
  BLE.setAdvertisedService(jobsService);
  jobsService.addCharacteristic(triggerActionCharacteristic);
  BLE.addService(jobsService);
  triggerActionCharacteristic.writeValue(doorOpen);

  BLE.advertise();
  Serial.println("Bluetooth device active, waiting for commands...");
}

void loop() {
  BLEDevice central = BLE.central();

  if (central) {
    Serial.print("Connected to central: ");
    Serial.println(central.address());

    while (central.connected()) {
      if (triggerActionCharacteristic.written()) {
        doorOpen = triggerActionCharacteristic.value();
        
        if (doorOpen) {
          Serial.println("Opening door...");
          digitalWrite(9, HIGH);
        } else {
          Serial.println("Door closing...");
          digitalWrite(9, LOW);
        }
      }
    }

    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
}
