/*
PROGRAMA DE CONTROL DE ANEMÓMETRO USANDO ESP8266.

Autor: 
Santiago Quintana Moreno A01571222

Equipo #1: “Null”
Javier Santos Pérez A01198909
Alejandro Orta Rodríguez A00840490      
Irvin David Ornelas García A00839065
*/

#include <Arduino.h> // Incluye la librería principal de Arduino.

int windPin = A0; // Define el pin de entrada analógica.

float zeroWindVoltage = 0.5; // Define el voltaje que emite el sensor cuando el ventilador no está girando.

float voltagePerkph = 0.1; // Define el cambio de voltaje por cada unidad de velocidad del viento (mV por kph).

// Función 'setup' que se ejecuta una vez al inicio cuando se enciende o reinicia el microcontrolador.
void setup() {
  Serial.begin(115200);   // Inicializa la comunicación serial.
}

// En el bucle principal, que se ejecuta repetidamente.
void loop() {
  // Lee el valor crudo del sensor (ADC).
  int rawValue = analogRead(windPin);
  
  // Convierte la lectura cruda en voltaje.
  float voltage = rawValue * 3.3 / 1023.0; // El ESP8266 usa una referencia de 3.3V.

  // Calcula la velocidad del viento en kph.
  float windSpeedkph = (voltage - zeroWindVoltage) / voltagePerkph;

  // Asegura que la velocidad del viento siempre sea positiva.
  if (windSpeedkph < 0) {
    windSpeedkph = 0;
  }

  // Imprime el valor crudo, el voltaje y la velocidad del viento.
  Serial.print("Valor crudo del sensor: ");
  Serial.println(rawValue);
  Serial.print("Voltaje: ");
  Serial.print(voltage);
  Serial.println(" V");
  Serial.print("Velocidad del viento: ");
  Serial.print(windSpeedkph);
  Serial.println(" kph");

  // Espera un periodo corto antes de tomar la siguiente medición.
  delay(1000);
}
