/*
PROGRAMA DE CONTROL DE ANEMOMETRO USANDO ESP8266.

Autor: 
Santiago Quintana Moreno A01571222

Equipo #1: “Null”
Javier Santos Pérez A01198909
Alejandro Orta Rodríguez A00840490
Irvin David Ornelas García A00839065
     
*/

#include <Arduino.h> // Incluye la librería base de Arduino para poder utilizar sus funciones.
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

int windPin = A0;
float zeroWindVoltage = 0.04;
float voltagePerkph = 0.1;

const char* ssid = "INFINITUM8B9C_2.4";
const char* password = "f54cBnRD7e";

const int ID = 1;
const int ID_ASensor = 4;
const int ID_Plant = 1;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000);

String URL = "http://192.168.1.194:3000/iot/api/insertAnemometer";

void logIntento(float windSpeedkph, String formattedDate);
String formatDateTime(unsigned long unixTime);
unsigned long getTime();

void setup() {
  Serial.begin(9600); 
  Serial.println("***Inicializando conexión a My SQL***");
  
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

  timeClient.begin();

  delay(500);
}

void loop() {
  float voltage = analogRead(windPin) * 3.3 / 1023.0; 
  float windSpeedkph = (voltage - zeroWindVoltage) / voltagePerkph;

  if (isnan(windSpeedkph)) {
    windSpeedkph = 0;
  }

  if (windSpeedkph < 0) {
    windSpeedkph = 0;
  }

  unsigned long unixTime = getTime();
  String formattedDate = formatDateTime(unixTime);

  Serial.print("Wind speed: ");
  Serial.print(windSpeedkph);
  Serial.println(" kph");

  Serial.print("\nFormatted Date: ");
  Serial.println(formattedDate);

  logIntento(windSpeedkph, formattedDate);

  delay(1000);
}

void logIntento(float windSpeedkph, String formattedDate) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient wClient;
    HTTPClient httpClient;

    // Crear el JSON a enviar
    String jsonPayload = String("{") +
                    "\"ID_ASensor\": " + String(ID_ASensor) + ", " +
                    "\"ID_Plant\": " + String(ID_Plant) + ", " +
                    "\"Value\": " + String(windSpeedkph) + ", " +
                    "\"Date\": \"" + formattedDate + "\"" +
                    "}";
    
    Serial.println("Payload JSON: " + jsonPayload);
    Serial.print("Conectando a URL: ");
    Serial.println(URL);

    // Inicia la conexión
    httpClient.begin(wClient, URL);
    httpClient.addHeader("Content-Type", "application/json");

    // Envía la solicitud POST y recibe el código de respuesta
    int httpResponseCode = httpClient.POST(jsonPayload);
    
    Serial.print("Código de respuesta HTTP: ");
    Serial.println(httpResponseCode);

    if (httpResponseCode > 0) {
      // Si la respuesta fue exitosa, imprime el contenido
      String response = httpClient.getString();
      Serial.print("Respuesta del servidor: ");
      Serial.println(response);
    } else {
      // Si hubo un error, muestra información adicional
      Serial.print("Error en la solicitud POST: ");
      Serial.println(httpResponseCode);
      Serial.println("Causas posibles: URL incorrecta, servidor inaccesible o problema de red.");
    }
    
    httpClient.end(); 
  } else {
    Serial.println("Error: No está conectado a WiFi. Intentando reconectar...");
    WiFi.disconnect();
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.print(".");
    }
    Serial.println("\nReconectado a WiFi.");
  }
}

unsigned long getTime() {
  timeClient.update();
  return timeClient.getEpochTime();
}

// Función para convertir Unix timestamp a formato "YYYY-MM-DD HH:MM:SS"
String formatDateTime(unsigned long unixTime) {
  time_t rawTime = static_cast<time_t>(unixTime);  // Convert unsigned long to time_t
  struct tm *dt;
  char buffer[20];
  dt = localtime(&rawTime);
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", dt);
  return String(buffer);
}
