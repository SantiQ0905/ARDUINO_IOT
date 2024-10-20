/*
PROGRAMA DE CONTROL DE SENSOR DE TEMPERATURA Y HUMEDAD USANDO ESP8266.

Autor: 
Javier Santos Pérez A01198909

Equipo #1: “Null”
Irvin David Ornelas García A00839065
Alejandro Orta Rodríguez A00840490
Santiago Quintana Moreno A01571222      
*/

#include <Arduino.h>
#include <DHT.h> //cargamos  la librera DHT
#define DHTPIN D1 //Seleccionamos el pin en el que se conectar  el sensor
#define DHTTYPE DHT11 //Se selecciona el DHT22(hay otros DHT)
DHT  dht(DHTPIN, DHTTYPE); //Se inicia una variable que ser usada por Arduino para comunicarse  con el sensor

void setup() {
  Serial.begin(115200);
  delay(10);
  dht.begin(); //Se inicia el sensor
}

void loop() {
  float h = dht.readHumidity(); //Se lee la humedad
  float t = dht.readTemperature(); //Se lee la temperatura
  //Se imprimen las  variables
  Serial.println("Humedad: "); 
  Serial.println(h);
  Serial.println("Temperatura:  ");
  Serial.println(t);
  delay(2000); //Se espera 2 segundos para seguir  leyendo //datos 
}