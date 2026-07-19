#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); 
const byte address = "00001"; 

// Пины ЛЕВОГО мотора
const int in1 = 9;
const int in2 = 10;
// Пины ПРАВОГО мотора
const int in3 = 5;
const int in4 = 6;

unsigned long lastReceiveTime = 0;
const unsigned long timeoutLimit = 500; // Failsafe (стоп при потере связи)

void setup() {
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  
  stopMotors(); // Изначально стоим

  if (!radio.begin()) { while (1); }
  
  radio.openReadingPipe(1, address);
  radio.setPALevel(RF24_PA_MIN);      
  radio.setDataRate(RF24_250KBPS);    
  radio.setAutoAck(true);             
  radio.startListening();             
}

void loop() {
  if (radio.available()) {
    int command = 0;
    radio.read(&command, sizeof(command));
    
    lastReceiveTime = millis(); // Сброс таймера таймаута

    switch(command) {
      case 1: // ВПЕРЕД (Оба мотора крутятся вперед)
        digitalWrite(in1, HIGH); digitalWrite(in2, LOW);
        digitalWrite(in3, HIGH); digitalWrite(in4, LOW);
        break;
        
      case 2: // НАЗАД (Оба мотора крутятся назад)
        digitalWrite(in1, LOW);  digitalWrite(in2, HIGH);
        digitalWrite(in3, LOW);  digitalWrite(in4, HIGH);
        break;
        
      case 3: // ПОВОРОТ ВЛЕВО (Левый назад, правый вперед)
        digitalWrite(in1, LOW);  digitalWrite(in2, HIGH);
        digitalWrite(in3, HIGH); digitalWrite(in4, LOW);
        break;
        
      case 4: // ПОВОРОТ ВПРАВО (Левый вперед, правый назад)
        digitalWrite(in1, HIGH); digitalWrite(in2, LOW);
        digitalWrite(in3, LOW);  digitalWrite(in4, HIGH);
        break;
        
      default: // СТОП (Команда 0 или любая неизвестная)
        stopMotors();
        break;
    }
  }

  // Защита: если пульт выключился, глушим моторы
  if (millis() - lastReceiveTime > timeoutLimit) {
    stopMotors();
  }
}

void stopMotors() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}
