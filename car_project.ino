#include <SoftwareSerial.h>

SoftwareSerial bluetooth(10, 11); // TX, RX
short led = 5;
short ldr = A0;
short echoPin = 6;
short trigPin = 7;
short buzzer = 8;
short motor1pin1 = A1;
short motor1pin2 = A2;
short motor2pin1 = A3;
short motor2pin2 = A4;

short right_Motor=0, left_Motor=0;
void setup()
{
  //Serial.begin(9600);
  bluetooth.begin(9600);
  bluetooth.println("Merhaba dunya");
  pinMode(led,OUTPUT);
  pinMode(ldr,INPUT);
  
  pinMode(trigPin,OUTPUT);
  pinMode(buzzer,OUTPUT);
  pinMode(echoPin,INPUT);

  pinMode(motor1pin1,OUTPUT);
  pinMode(motor1pin2,OUTPUT);
  
  pinMode(motor2pin1,OUTPUT);
  pinMode(motor2pin2,OUTPUT);
}

void loop()
{
 bluetoothmodule();
 light();
 dist();
}
void bluetoothmodule(){
  char ch = bluetooth.read();
  switch (ch){
    case 'a': startEngine(1,0); break;
    case 'd': startEngine(0,1); break;
    case 'w': startEngine(1,1); break;
    case 's': startEngine(0,0); break;
    case 'g': startEngine(-1,-1); break; //stop engine
    defaut: goman();
  }
    
}
void startEngine(short rightMotor, int leftMotor){
  right_Motor = rightMotor;
  left_Motor = leftMotor;
  goman();
}
void goman(){
  if(left_Motor==-1 || right_Motor == -1){
    digitalWrite(motor2pin1,0);
    digitalWrite(motor2pin2,0);
    digitalWrite(motor1pin1,0);
    digitalWrite(motor1pin2,0);
  }else{
    digitalWrite(motor1pin1,right_Motor);
    digitalWrite(motor1pin2,!right_Motor);
    digitalWrite(motor2pin1,left_Motor);
    digitalWrite(motor2pin2,!left_Motor);
    
  }
}
void light(){
  int brightness = analogRead(ldr);
  //Serial.println(brightness);
  if(brightness<500){
    digitalWrite(led,HIGH);
  }
  else{
    digitalWrite(led,LOW);
  }
}
void dist(){
  int distan = calcdistance();
  if(distan<10 && distan != 0){
    buz();
  }
}
int calcdistance(){
  long duration, distance;
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  return pulseIn(echoPin, HIGH)/58.2;
}
void buz(){
  tone(buzzer, 1000);
  delay(100);
  noTone(buzzer);
}
