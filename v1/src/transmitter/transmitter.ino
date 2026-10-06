#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN
const byte address = "00001"; 

const int pinY = A0; // Ось Вперед / Назад
const int pinX = A1; // Ось Влево / Вправо

void setup() {
  Serial.begin(9600);
  if (!radio.begin()) { while (1); }
  
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);      // MIN для тестов на столе
  radio.setDataRate(RF24_250KBPS);    
  radio.setAutoAck(true);             
  radio.stopListening();              
}

void loop() {
  int yVal = analogRead(pinY); // Читаем ось Y (0-1023)
  int xVal = analogRead(pinX); // Читаем ось X (0-1023)
  
  int command = 0; // По умолчанию - СТОП

  // Проверяем приоритет движений (сначала проверяем наклон вперед/назад)
  if (yVal > 700) {
    command = 1; // Вперед
  } else if (yVal < 300) {
    command = 2; // Назад
  } else if (xVal < 300) {
    command = 3; // Влево
  } else if (xVal > 700) {
    command = 4; // Вправо
  } else {
    command = 0; // Центр (Стоп)
  }

  radio.write(&command, sizeof(command));
  delay(50);
}
