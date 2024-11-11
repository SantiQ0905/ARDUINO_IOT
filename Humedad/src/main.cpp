/*
PROGRAMA DE CONTROL DE SENSOR DE HUMEDAD USANDO ESP8266.

Autor: 
Irvin David Ornelas García A00839065

Equipo #1: “Null”
Javier Santos Pérez A01198909
Alejandro Orta Rodríguez A00840490
Santiago Quintana Moreno A01571222      
*/

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>


const char* ssid = "IZZI-AAE0";
const char* password = "AIzzi21JORNB.!";

const int sensor_ID = 2;
const int plant_ID = 1;

const int sensor = A0;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

String URL = "http://192.168.1.11:3000/iot/api/insertSoilMoisture";


void setup() {
  Serial.begin(9600); 
  Serial.println("***Inicializando conexión a My SQL***");
  
  pinMode(A0,INPUT);

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
    int humedad = map(analogRead(sensor), 0, 1024, 100, 0);

    if (isnan(humedad)) {
      Serial.println("Error obteniendo los datos del sensor");
      return;
    }

    unsigned long unixTime = getTime();

    if(isnan(unixTime)) {
      Serial.println("Error obteniendo la fecha del servidor");
      return;
    }

    Serial.print("\nHumedad: ");
    Serial.print(humedad);
    Serial.print(" %");

    Serial.print("\nUnixTime: ");
    Serial.print(unixTime);

    logIntento(humedad,unixTime);
      
    delay(10000);
}

void logIntento(float humedad, unsigned long unixTime) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient wClient;
    HTTPClient httpClient;


    // Crear el JSON a enviar
    String jsonPayload = String("{") +
                    "\"ID_EHSensor\": " + String(sensor_ID) + ", " +
                    "\"ID_Plant\": " + String(plant_ID) + ", " +
                    "\"Value\": " + String(humedad) + ", " +
                    "\"Date\": " + String(unixTime) +
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
    WiFi.reconnect();
    delay(5000);
  }
}

unsigned long getTime() {
  timeClient.update();
  unsigned long now = timeClient.getEpochTime();
  return now;
}

