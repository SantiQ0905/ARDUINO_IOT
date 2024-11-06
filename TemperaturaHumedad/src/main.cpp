/*
PROGRAMA DE CONTROL DE SENSOR DE TEMPERATURA Y HUMEDAD USANDO ESP8266.

Autor: 
Javier Santos Pérez A01198909 & Santiago Quintana Moreno A01571222

Equipo #1: “Null”
Irvin David Ornelas García A00839065
Alejandro Orta Rodríguez A00840490
Santiago Quintana Moreno A01571222      
*/

#include <DHT.h>  
#include <DHT_U.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define DHTPIN    0
#define DHTTYPE   DHT11
DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "INFINITUM8B9C_2.4";
const char* password = "f54cBnRD7e";

String URL = "http://192.168.1.194:3000/iot/api/insertTemperature";

void logIntento(float t, float h);  // Updated function declaration

void setup() {
  Serial.begin(9600); 
  Serial.println("***Inicializando conexión a My SQL***");
  
  dht.begin(); 

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
  delay(500);
}

void loop() {
    float t = dht.readTemperature();
    float h = dht.readHumidity();

    if (isnan(t) || isnan(h)) {  // Check if both temperature and humidity readings are valid
      Serial.println("Error obteniendo los datos del sensor DHT11");
      return;
    }

    Serial.print("\nTemperatura: ");
    Serial.print(t);
    Serial.print(" ºC\t");

    Serial.print("Humedad: ");
    Serial.print(h);
    Serial.println(" %");

    logIntento(t, h);  // Call logIntento with both temperature and humidity
      
    delay(10000);
}

void logIntento(float t, float h) {  // Updated function to accept both parameters
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient wClient;
    HTTPClient httpClient;

    // Crear el JSON a enviar
    String jsonPayload = "{\"temperature\": " + String(t) + ", \"humidity\": " + String(h) + "}";
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
