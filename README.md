# Arduino para STM32 BLUEPILL
Profesor: Dr. Adán Geovanni Medrano Chávez  
UEA: Microcontroladores
## Integrantes del equipo:
Diego Reyes Blancas
## Objetivo de la práctica
1. Utilizar la capa de abstraccion de ardware de Arduino para trabajar con el microcontrolador "Bluepill STM32F103C8T6"
2. Configurar la pantalla OLED SDD136 con la biblioteca correspondiente
3. En la pantalla OLED debe presentarse el voltaje de lectura que
corresponda al valor leído de la entrada analógica (0,3.3v)
5. Al oprimir un botón A en el transmisor, el porcentaje debe enviarse
por infrarrojo al receptor.
6. El voltaje recibido por el receptor debe mostrarse en la pantalla
OLED.
7. El voltaje recibido debe afectar el brillo del LED del receptor.
8. Al oprimir un botón B en el transmisor, el LED del receptor debe conmutar entre totalmente encendido, totalmente apagado
   o control de brillo analógico. Esto implica que el transmisor tiene tres modos.
## compilacion y carga en el microcontrolador
para realizar la compilacion del programa basta con escribir el siguiente comando
````
platformio run
````
para realizar la carga del programa al procesador del microcontrolador basta con escribir la siguiente linea de codigo
````
platformio run --target upload
````
## Programa para enviar la señal infraroja
este programa se encarga de leer la entrada analogica correspondiente a el potenciometro e imprimir el valor en voltios en la pantalla oled,tambien
configura los botones encargados de enviar la señal infraroja y de cambiar el modo de trabajo del led.
## configuración del hardware
### configuracion del led infrarojo utilizando la biblioteca IRremote.h
estas líneas de código permiten configurar el pin PA4 como el pin de salida para enviar señales IR mediante la creación de un objeto IRsend y pasando el número de pin correspondiente al constructor de la clase. 
````
int IRPin = PA4; 
IRsend irsend(IRPin); //configura el pin PA4 para enviar la señal
````
### configuración de las entradas 
en la función setup() se configuran los pines PA5 y PA7 como entradas y se establece una interrupción en el pin PA5 para ejecutar la función sendSignal() cuando se detecte un flanco de subida.
````
void setup() {
  // configura el puerto PA5,PA7 Como entradas
  pinMode(PA5, INPUT);  
  pinMode(PA7, INPUT);
  attachInterrupt(PA5, sendSignal, RISING); // configura la interrupción de PA5
  ...
}
````
### configuracion de la pantalla OLED
estas líneas de código crean un objeto display que representa la pantalla OLED y lo inicializan para establecer la comunicación con el Arduino. También se configura el color del texto en la pantalla para que sea blanco.
````
Adafruit_SSD1306 display(128, 64, &Wire, -1);
void setup() {
  ...
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Inicializar la pantalla OLED
  display.setTextColor(SSD1306_WHITE); //configura el color de la pantalla
}
````
### Declaraciones globales y por adelantado
````
void setLED();
void sendSignal();

float voltage;
int IRPin = PA4; 
int ledMode = 0;
IRsend irsend(IRPin); //configura el pin PA4 para enviar la señal
Adafruit_SSD1306 display(128, 64, &Wire, -1); //comunicación I2C a través del objeto Wire
````
### Lectura de la entrada digital e impreción en pantalla
en la función loop() se lee el valor analógico del pin PA6, se realiza una conversión a voltaje, se muestra el valor del voltaje en la pantalla OLED 
````
void loop() {
  ...
  int rawValue = analogRead(PA6); // Leer el valor analógico de PA6
  voltage = map(rawValue, 0, 1022, 0, 330) / 100.0; // Mapear el valor a un porcentaje (0-100)
  display.setCursor(0, 30);
  display.print("Lectura: \n");
  display.print(voltage);
  display.print("V");
  display.display();
  delay(100); // Agregar un retardo para evitar la actualización excesiva de la pantalla
}
````
### Función sendSignal 
````
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
````
### Función setLED
````
void setLED() {
unsigned long tData; //variable para enviar la señal 
  if(ledMode==0){
        display.clearDisplay();
  	ledMode=1;
  	display.setCursor(0, 0);
  	display.print("Led ON");
  	tData = 0x1 //variable para identificar el modo LED ON
  }
  else if(ledMode==1){
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print("Led OFF");
  	voltage=0;
  	ledMode=2;
  	tData = 0x2 //variable para identificar el modo LED OFF
  }
  else{
  	display.clearDisplay();
        display.setCursor(0, 0);
        display.print("Analog");
        ledMode=0;	
        tData = 0x3; //variable para identificar el modo control analogico
  }
  irsend.sendNEC(0x0,tData, 4); // Enviar la señal de infrarrojos con los datos convertidos
  display.display();
  delay(1200);	
}
````
