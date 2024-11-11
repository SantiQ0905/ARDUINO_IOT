/*
PROGRAMA DE CONTROL DE SENSOR DE HUMEDAD USANDO ESP8266.

Autor: 
Irvin David Ornelas García A00839065

Equipo #1: “Null”
Javier Santos Pérez A01198909
Alejandro Orta Rodríguez A00840490
Santiago Quintana Moreno A01571222      
*/

#include <Arduino.h> // Incluye la librería principal de Arduino.
#include <ESP8266WiFi.h> // Incluye la librería de ESP8266 para conexión WiFi.
#include <ESP8266HTTPClient.h> // Incluye la librería de ESP8266 para realizar peticiones HTTP.

int sensor = A0; // Declara una variable entera llamada 'sensor' y la asigna al pin analógico A0.

const char* ssid = "INFINITUM8B9C_2.4";
const char* password = "f54cBnRD7e";

String URL = "http://192.168.1.194:3000/insertSoilMoistureLog";

void logIntento(int humedad);

// Función 'setup' que se ejecuta una vez al inicio cuando se enciende o reinicia el microcontrolador.
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
  delay(500);
}

void loop() {
  int humedad = analogRead(sensor);  

  if (isnan(humedad)) {
    Serial.println("Error al leer el sensor de humedad!");
    return;
  }

  Serial.println(humedad); 
  
  logIntento(humedad);
  
  delay(1000); 
}

void logIntento(int humedad) {
  if (WiFi.status() == WL_CONNECTED){
    WiFiClient wClient;
    HTTPClient httpClient;

    String jsonPayload = "{\"humedad\":" + String(humedad) + "}";
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