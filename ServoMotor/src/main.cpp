/*
PROGRAMA DE CONTROL DE SERVO MOTOR USANDO ESP8266.

Autor: 
Santiago Quintana Moreno A01571222

Equipo #1: “Null”
Javier Santos Pérez A01198909
Alejandro Orta Rodríguez A00840490      
Irvin David Ornelas García A00839065
*/

#include <Arduino.h>                    // Incluye la librería principal de Arduino.
#include <Servo.h>                      // Incluye la librería Servo que permite controlar motores servo.

Servo myServo;                          // Crea un objeto llamado 'myServo' de la clase Servo.
const int servoPin = 13;                // Define la constante 'servoPin' que será el pin GPIO 13, equivalente al pin D7.

// Función 'setup' que se ejecuta una vez al inicio cuando se enciende o reinicia el microcontrolador.
void setup() {
  myServo.attach(servoPin);             // Asocia el pin GPIO 13 al servo motor, estableciendo que este pin enviará señales PWM al servo.
  Serial.begin(115200);                 // Inicia la comunicación serial con una velocidad de 115200 baudios.
}

// En el bucle principal, que se ejecuta repetidamente.
void loop() {
  
  myServo.write(0);                      // Envía una señal PWM que posiciona el servo a 0 grados (la posición mínima).
  Serial.println("Servo a 0 grados.");  // Imprime un mensaje en el monitor serial para indicar que el servo está a 0 grados.
  delay(1000);                           // Pausa el programa por 1000 milisegundos (1 segundo) antes de continuar.

  myServo.write(90);                        // Envía una señal PWM que posiciona el servo a 90 grados (posición intermedia).
  Serial.println("Servo a 90 grados.");    // Imprime un mensaje en el monitor serial indicando que el servo está a 90 grados.
  delay(1000);                              // Pausa el programa por 1000 milisegundos antes de continuar.

  myServo.write(180);                       // Envía una señal PWM que posiciona el servo a 180 grados (la posición máxima).
  Serial.println("Servo a 180 grados.");   // Imprime un mensaje en el monitor serial indicando que el servo está a 180 grados.
  delay(1000);                              // Pausa el programa por 1000 milisegundos antes de repetir el bucle.
}
