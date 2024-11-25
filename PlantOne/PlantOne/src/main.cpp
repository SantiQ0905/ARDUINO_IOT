/*
UNIFIED PLANT ONE CONTROL SYSTEM

Autor: 
Santiago Quintana Moreno A01571222

Equipo #1: “Null”
Irvin David Ornelas García A00839065
Alejandro Orta Rodríguez A00840490
Javier Santos Pérez A01198909
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <Servo.h>
#include <SPI.h>


// WiFi Configuration
const char* ssid = "QUINTANA_MORENO";
const char* password = "SaSaVEQM9928";

// Sensor and Plant IDs
const int ID_TSensor = 1; // Temperature/Humidity Sensor ID
const int ID_ASensor = 2; // Wind Sensor ID
const int ID_LSensor = 3; // Light Sensor ID
const int ID_EHSensor = 4; // Soil Moisture Sensor ID
const int ID_Servo = 1; // Servo ID
const int ID_Plant = 1; // Plant ID

// Pin Definitions
#define DHTPIN 0               // D3 -> GPIO0
#define DHTTYPE DHT11
#define SERVO_PIN 14           // D5 -> GPIO14

// ADS1115 Channels
#define PHOTORESISTOR_CHANNEL 1
#define SOIL_MOISTURE_CHANNEL 2

// Sensor Instances
DHT dht(DHTPIN, DHTTYPE);
Adafruit_ADS1115 ads;
Servo servo;

// Timer Variables
unsigned long lastLogTime = 0;
const unsigned long logInterval = 10000; // Log every 10 seconds

void sendDataToServer(String url, String payload, HTTPClient &httpClient, WiFiClient &wClient) {
  httpClient.begin(wClient, url);
  httpClient.addHeader("Content-Type", "application/json");
  int httpResponseCode = httpClient.POST(payload);

  Serial.print("HTTP Response Code: ");
  Serial.println(httpResponseCode);

  if (httpResponseCode > 0) {
    String response = httpClient.getString();
    Serial.print("Server Response: ");
    Serial.println(response);
  } else {
    Serial.print("Error in POST Request: ");
    Serial.println(httpResponseCode);
  }
  httpClient.end();
}

void logSensorData(float t, float h, float windSpeed, float lightVoltage, int soilMoisture) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient wClient;
    HTTPClient httpClient;

    // Log temperature and humidity
    String jsonTempHumidity = "{\"ID_TSensor\": " + String(ID_TSensor) +
                              ", \"ID_Plant\": " + String(ID_Plant) +
                              ", \"Value1\": " + String(t) +
                              ", \"Value2\": " + String(h) + "}";
    String urlTempHumidity = "http://192.168.1.80:3000/iot/api/insertTemperature";
    sendDataToServer(urlTempHumidity, jsonTempHumidity, httpClient, wClient);

    // Log wind speed
    String jsonWind = "{\"ID_ASensor\": " + String(ID_ASensor) +
                      ", \"ID_Plant\": " + String(ID_Plant) +
                      ", \"Value\": " + String(windSpeed) + "}";
    String urlWind = "http://192.168.1.80:3000/iot/api/insertAnemometer";
    sendDataToServer(urlWind, jsonWind, httpClient, wClient);

    // Log light sensor data
    String jsonLight = "{\"ID_LSensor\": " + String(ID_LSensor) +
                       ", \"ID_Plant\": " + String(ID_Plant) +
                       ", \"Value\": " + String(lightVoltage) + "}";
    String urlLight = "http://192.168.1.80:3000/iot/api/insertLight";
    sendDataToServer(urlLight, jsonLight, httpClient, wClient);

    // Log soil moisture
    String jsonSoil = "{\"ID_EHSensor\": " + String(ID_EHSensor) +
                      ", \"ID_Plant\": " + String(ID_Plant) +
                      ", \"Value\": " + String(soilMoisture) + "}";
    String urlSoil = "http://192.168.1.80:3000/iot/api/insertSoilMoisture";
    sendDataToServer(urlSoil, jsonSoil, httpClient, wClient);
  } else {
    Serial.println("Error: Not connected to WiFi.");
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("*Initializing sensors and WiFi*");

  // Initialize WiFi
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi \"");
  Serial.print(ssid);
  Serial.print("\"");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    yield(); // Prevent watchdog resets
  }
  Serial.print("\nConnected! IP: ");
  Serial.println(WiFi.localIP());

  // Initialize ADS1115
  if (!ads.begin()) {
    Serial.println("Error initializing ADS1115. Check connections.");
    while (1) {
      yield(); // Prevent watchdog resets in infinite loop
    }
  }
  Serial.println("ADS1115 initialized successfully.");

  // Initialize DHT sensor
  dht.begin();

  // Initialize Servo
  servo.attach(SERVO_PIN);
  servo.write(90); // Set servo to neutral position
}

void loop() {
  unsigned long currentMillis = millis();

  // Check if it's time to log sensor data
  if (currentMillis - lastLogTime >= logInterval) {
    lastLogTime = currentMillis;

    // Read temperature and humidity
    float t = dht.readTemperature();
    float h = dht.readHumidity();

    if (isnan(t) || isnan(h)) {
      Serial.println("Error reading temperature and humidity sensor");
      return;
    }
    Serial.print("\nTemperature: ");
    Serial.print(t);
    Serial.print(" ºC\tHumidity: ");
    Serial.print(h);
    Serial.println(" %");

    // Simulate wind speed (hardcoded for presentation)
    float windSpeedkph = 15.0; // Example value
    Serial.print("Wind Speed (Simulated): ");
    Serial.print(windSpeedkph);
    Serial.println(" kph");

    // Read light sensor
    int16_t lightRaw = ads.readADC_SingleEnded(PHOTORESISTOR_CHANNEL); // Channel 1
    float lightVoltage = lightRaw * 0.1875 / 1000; // Convert to voltage
    Serial.print("Light Sensor Voltage: ");
    Serial.println(lightVoltage);

    // Read soil moisture sensor
    int16_t soilRaw = ads.readADC_SingleEnded(SOIL_MOISTURE_CHANNEL); // Channel 2
    int soilMoisture = map(soilRaw, 0, 32767, 100, 0); // Map ADC value to percentage
    Serial.print("Soil Moisture: ");
    Serial.print(soilMoisture);
    Serial.println(" %");

    // Control servo based on soil moisture
    bool shouldOpenValve = false;
    if (soilMoisture < 30 || lightVoltage > 1.0 || t > 30 || h < 50 || windSpeedkph > 20) {
      Serial.println("Opening valve...");
      shouldOpenValve = true;
    } else {
      Serial.println("Conditions are fine. Closing valve...");
    }

    servo.write(shouldOpenValve ? 180 : 0);

    // Log sensor data to server
    logSensorData(t, h, windSpeedkph, lightVoltage, soilMoisture);
  }

  yield(); // Prevent watchdog resets
}
