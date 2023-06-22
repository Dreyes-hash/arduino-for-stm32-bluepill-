#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <IRremote.h>
#include "SSD1306_Defs.h"

void setLED();
void sendSignal();

float voltage;
int IRPin = PA4; 
int ledMode = 0;
IRsend irsend(IRPin); //configura el pin PA4 para enviar la señal
Adafruit_SSD1306 display(128, 64, &Wire, -1);

void setup() {
  // configura el puerto PA5,PA7 Como entradas
  pinMode(PA5, INPUT);  
  pinMode(PA7, INPUT);
  attachInterrupt(PA5, sendSignal, RISING); // configura la interrupción de PA5
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Inicializar la pantalla OLED
  display.setTextColor(SSD1306_WHITE);
}

void loop() {
  if (digitalRead(PA7) == HIGH) {
    	setLED(); // Realizar el cambio de estado del LED si el botón PA7 está presionado
    	delay(50); // Agregar un retardo 
  }
  display.clearDisplay();
  display.setTextSize(2);
  int rawValue = analogRead(PA6); // Leer el valor analógico de PA6
  voltage = map(rawValue, 0, 1022, 0, 330) / 100.0; // Mapear el valor a un porcentaje (0-100)
  display.setCursor(0, 30);
  display.print("Lectura: \n");
  display.print(voltage);
  display.print("V");
  display.display();

  delay(100); // Agregar un pequeño retardo para evitar la actualización excesiva de la pantalla
}

void setLED() {
  if (ledMode==0){
        display.clearDisplay();
  	voltage=330;
  	ledMode=1;
  	display.setCursor(0, 0);
  	display.print("Led ON");
  }
  else{
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print("Led OFF");
  	voltage=0;
  	ledMode=0;
  }
  // Convertir el valor de percentage a un formato adecuado para el protocolo 
  unsigned long tData = map(voltage, 0, 330, 0, 0xFFFF); // Convertir el porcentaje a un valor adecuado para el protocolo NEC
  irsend.sendNEC(0x0,tData, 4); // Enviar la señal de infrarrojos con los datos convertidos
  
  display.display();
  delay(2000);	
}

void sendSignal() {
  // Convertir el valor de percentage a un formato adecuado para el protocolo 
  voltage=voltage*100;
  unsigned long tData = map(voltage, 0, 330, 0, 0xFFFF); // Convertir el porcentaje a un valor adecuado para el protocolo NEC
  irsend.sendNEC(0x0,tData, 4); // Enviar la señal de infrarrojos con los datos convertidos
  display.setCursor(0, 0);
  display.print("enviado");
  display.display();
  delay(300);	
}


