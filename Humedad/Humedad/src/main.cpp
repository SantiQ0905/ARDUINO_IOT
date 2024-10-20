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

int sensor = A0; // Declara una variable entera llamada 'sensor' y la asigna al pin analógico A0.

// Función 'setup' que se ejecuta una vez al inicio cuando se enciende o reinicia el microcontrolador.
void setup() {
  Serial.begin(115200); // Inicia la comunicación serial con una velocidad de 115200 baudios.
}

void loop() {
  int humedad = analogRead(sensor);  
  // Lee el valor del sensor de humedad en el pin A0 utilizando la función 'analogRead' y almacena el resultado en la variable 'humedad'.
  
  Serial.println(humedad); 
  // Envía el valor de 'humedad' al puerto serial para que pueda verse en el monitor serie del IDE de Arduino.
  
  delay(1000); 
  // Pausa la ejecución del código durante 1000 milisegundos (1 segundo) antes de repetir el ciclo.
}
