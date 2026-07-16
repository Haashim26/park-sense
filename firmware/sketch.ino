#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "secrets.h"

// Buttons for 5 slots
int buttonPins[5] = {14, 27, 26, 25, 33};
int slotState[5]  = {0, 0, 0, 0, 0}; // 0=free, 1=occupied

WiFiClientSecure client;

void sendToFirebase(int slot, String value) {
  // .json?auth=<secret> — writes only the status field, preserving type
  String path = "/parkingAreas/area1/slots/slot" + String(slot + 1) + "/status.json?auth=" FIREBASE_DB_SECRET;

  if (client.connect(FIREBASE_HOST, 443)) {
    String body = "\"" + value + "\"";

    client.println("PUT " + path + " HTTP/1.1");
    client.println("Host: " FIREBASE_HOST);
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(body.length());
    client.println();
    client.println(body);

    Serial.print("Slot ");
    Serial.print(slot + 1);
    Serial.print(" → ");
    Serial.println(value);

    // Wait for and print the actual HTTP response so we can confirm success/failure
    unsigned long timeout = millis();
    while (client.connected() && !client.available()) {
      if (millis() - timeout > 5000) {
        Serial.println("  ⚠️ Response timed out");
        client.stop();
        return;
      }
    }
    String statusLine = client.readStringUntil('\n');
    Serial.print("  → Firebase response: ");
    Serial.println(statusLine);

    if (statusLine.indexOf("200") == -1) {
      Serial.println("  ⚠️ Write likely FAILED — check secret/rules");
    }
  } else {
    Serial.println("  ⚠️ Could not connect to Firebase host");
  }
  client.stop();
}

void setup() {
  Serial.begin(115200);

  for (int i = 0; i < 5; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  client.setInsecure(); // REQUIRED for Wokwi
}

void loop() {
  for (int i = 0; i < 5; i++) {
    if (digitalRead(buttonPins[i]) == LOW) {
      slotState[i] = !slotState[i];
      sendToFirebase(i, slotState[i] ? "occupied" : "free");
      delay(400); // debounce
    }
  }
}
