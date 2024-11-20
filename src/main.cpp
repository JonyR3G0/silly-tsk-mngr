#include <Arduino.h>

// Definir el pin del LED integrado en el ESP32
#define LED_BUILTIN 2  // La mayoría de las tarjetas ESP32 usan el pin GPIO 2 para el LED integrado

void setup() {
  // Configurar el pin del LED como salida
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);  // Encender el LED
  delay(1000);                      // Esperar 1 segundo
  digitalWrite(LED_BUILTIN, LOW);   // Apagar el LED
  delay(1000);                      // Esperar 1 segundo
}