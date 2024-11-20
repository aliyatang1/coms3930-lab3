#include "TFT_eSPI.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

// WiFi credentials
const char* ssid = "Barnard Guest";
const char* password = "";

// Pin definitions
#define BUTTON_LEFT 0
#define BUTTON_RIGHT 35

volatile bool leftButtonPressed = false;
volatile bool rightButtonPressed = false;

// Initialize display
TFT_eSPI tft = TFT_eSPI();

void setup() {
  Serial.begin(115200);

  // Initialize display
  tft.init();
  tft.setRotation(1); 
  tft.fillScreen(TFT_BLACK);

  Serial.print("display dimensions: ");
  Serial.print(tft.width());
  Serial.print(" x ");
  Serial.println(tft.height());

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  // Setup buttons
  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_LEFT), pressedLeftButton, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_RIGHT), pressedRightButton, FALLING);

  // Initial fetch to populate the screen
  fetchAndDisplayResponse();
}

void fetchAndDisplayResponse() {
  HTTPClient http;
  String url = "https://api.thecatapi.com/v1/images/search";  // Replace with your desired URL
  http.begin(url);
  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {
    String response = http.getString();

    // Display the HTTP response on the ESP32's screen
    tft.fillScreen(TFT_BLACK); 
    tft.setCursor(0, 0);      
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(1);     

    int y = 0;
    for (int i = 0; i < response.length(); i += 40) {
      String line = response.substring(i, i + 40);
      tft.drawString(line, 0, y);
      y += 10; 
      if (y > tft.height() - 10) {
        break; 
      }
    }
  } else {
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(0, 0);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.println("Request Failed!");
  }

  http.end();
}

void pressedLeftButton() {
  leftButtonPressed = true;
}

void pressedRightButton() {
  rightButtonPressed = true;
}

void loop() {
  if (leftButtonPressed || rightButtonPressed) {
    fetchAndDisplayResponse();
    leftButtonPressed = false;
    rightButtonPressed = false;
  }
}
