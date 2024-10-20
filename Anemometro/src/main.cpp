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

// Define el pin de entrada analógica.
int windPin = A0;

// Define el voltaje que emite el sensor cuando el ventilador no está girando.
float zeroWindVoltage = 0.04;

// Define el cambio de voltaje por cada unidad de velocidad del viento (mV por kph).
float voltagePerkph = 0.1;

// Función 'setup' que se ejecuta una vez al inicio cuando se enciende o reinicia el microcontrolador.
void setup() {
  // Inicializa la comunicación serial.
  Serial.begin(115200); // El ESP8266 normalmente usa una tasa de baudios más alta.
}

void loop() {
  // Lee el voltaje del sensor de viento.
  float voltage = analogRead(windPin) * 3.3 / 1023.0; // El ESP8266 usa una referencia de 3.3V.

  // Calcula la velocidad del viento en kph.
  float windSpeedkph = (voltage - zeroWindVoltage) / voltagePerkph;

  // Asegura que la velocidad del viento siempre sea positiva.
  if (windSpeedkph < 0) {
    windSpeedkph = 0;
  }

  // Imprime la velocidad del viento en el monitor serial.
  Serial.print("Wind speed: ");
  Serial.print(windSpeedkph);
  Serial.println(" kph");

  // Espera un periodo corto antes de tomar la siguiente medición.
  delay(1000);
}
