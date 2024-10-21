/*
PROGRAMA DE CONTROL DE SERVO MOTOR USANDO ESP8266.

Autor: 
Santiago Quintana Moreno A01571222

Equipo #1: “Null”
Javier Santos Pérez A01198909
Alejandro Orta Rodríguez A00840490  (Encargado asignado del sensor, no contribuyó)    
Irvin David Ornelas García A00839065
*/

#include <Arduino.h> // Incluye la librería principal de Arduino.

void setup() {
  Serial.begin(115200); // Inicializar la comunicación serial a 115200 baudios
  pinMode(A0, INPUT); // Configurar el pin A0 como entrada.
}

void loop() {
    
  int sensorValue = analogRead(A0); // Leer el valor del pin analógico A0
  float voltage = sensorValue * (1.0 / 1023.0); // Convertir el valor leído (0-1023) a un voltaje (0-1V en el ESP8266)

  // Imprimir el valor del sensor y el voltaje a la consola serial
  Serial.print("Valor del sensor: ");
  Serial.print(sensorValue);
  Serial.print(" - Voltaje: ");
  Serial.println(voltage);

  delay(500); // Pequeña pausa.
}
