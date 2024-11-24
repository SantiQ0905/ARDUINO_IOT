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
#include <SPI.H>

// WiFi Configuration
const char* ssid = "QUINTANA_MORENO";
const char* password = "SaSaVEQM9928";



// Sensor IDs
const int ID_TSensor = 1;
const int ID_ASensor = 2;
const int ID_LSensor = 3;
const int ID_EHSensor = 4;

// Plant ID
const int ID_Plant = 1;

// Pin Definitions
#define DHTPIN 0               // D3 -> GPIO0
#define DHTTYPE DHT11
#define SERVO_PIN 14           // D5 -> GPIO14

// ADS1115 Channels
#define ANEMOMETER_CHANNEL 0
#define PHOTORESISTOR_CHANNEL 1
#define SOIL_MOISTURE_CHANNEL 2

// Variables for wind speed calculation
float zeroWindVoltage = 0.04;
float voltagePerkph = 0.1;

// Sensor Instances
DHT dht(DHTPIN, DHTTYPE);
Adafruit_ADS1115 ads;
Servo servo;


void sendDataToServer(String url, String payload, HTTPClient &httpClient, WiFiClient &wClient) {
  httpClient.begin(wClient, url);
  httpClient.addHeader("Content-Type", "application/json");
  int httpResponseCode = httpClient.POST(payload);

  Serial.print("Código de respuesta HTTP: ");
  Serial.println(httpResponseCode);

  if (httpResponseCode > 0) {
    String response = httpClient.getString();
    Serial.print("Respuesta del servidor: ");
    Serial.println(response);
  } else {
    Serial.print("Error en la solicitud POST: ");
    Serial.println(httpResponseCode);
  }
  httpClient.end();
}

void logIntento(float t, float h, float windSpeedkph, float lightVoltage, int soilMoisture) {
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
                      ", \"Value\": " + String(windSpeedkph) + "}";
    String urlWind = "http://192.168.1.80:3000/iot/api/insertAnemometer";
    sendDataToServer(urlWind, jsonWind, httpClient, wClient);

    // Log light sensor
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
    Serial.println("Error: No está conectado a WiFi.");
  }
}



void setup() {
  Serial.begin(115200);
  Serial.println("*Inicializando conexión a MySQL y sensores*");

  // Initialize DHT sensor
  dht.begin();

  // Initialize WiFi
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  WiFi.begin(ssid, password);
  Serial.print("Conectando a red WiFi \"");
  Serial.print(ssid);
  Serial.print("\"");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nConectado! IP: ");
  Serial.println(WiFi.localIP());

  // Initialize ADS1115
  if (!ads.begin()) {
    Serial.println("Error inicializando el ADS1115. Verifique las conexiones.");
    while (1);
  }
  Serial.println("ADS1115 inicializado correctamente.");

  // Initialize Servo
  servo.attach(SERVO_PIN);
  servo.write(90); // Set servo to neutral position
}

void loop() {
  // Read Temperature and Humidity from DHT Sensor
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  if (isnan(t) || isnan(h)) {
    Serial.println("Error obteniendo datos del sensor de Humedad y Temperatura ambiental");
    return;
  }
  Serial.print("\nTemperatura: ");
  Serial.print(t);
  Serial.print(" ºC\t");
  Serial.print("Humedad: ");
  Serial.print(h);
  Serial.println(" %");

  // Read wind speed using ADS1115
  int16_t windRaw = ads.readADC_SingleEnded(ANEMOMETER_CHANNEL); // Channel 0
  float windVoltage = windRaw * 0.1875 / 1000; // Convert to voltage
  float windSpeedkph = (windVoltage - zeroWindVoltage) / voltagePerkph;

  if (isnan(windSpeedkph) || windSpeedkph < 0) {
    windSpeedkph = 0;
  }
  Serial.print("Wind Speed: ");
  Serial.print(windSpeedkph);
  Serial.println(" kph");

  // Read light sensor using ADS1115
  int16_t lightRaw = ads.readADC_SingleEnded(PHOTORESISTOR_CHANNEL); // Channel 1
  float lightVoltage = lightRaw * 0.1875 / 1000; // Convert to voltage
  Serial.print("Light Sensor Voltage: ");
  Serial.println(lightVoltage);

  // Read soil moisture sensor using ADS1115
  int16_t soilRaw = ads.readADC_SingleEnded(SOIL_MOISTURE_CHANNEL); // Channel 2
  int soilMoisture = map(soilRaw, 0, 32767, 100, 0); // Map ADC value to percentage
  Serial.print("Soil Moisture: ");
  Serial.print(soilMoisture);
  Serial.println(" %");

  // Control servo based on soil moisture
  if (soilMoisture < 30) {
    Serial.println("Soil is dry. Opening valve...");
    servo.write(180); // Open valve
  } else {
    Serial.println("Soil is moist. Closing valve...");
    servo.write(0); // Close valve
  }

  // Log data to server
  logIntento(t, h, windSpeedkph, lightVoltage, soilMoisture);

  delay(10000);
}