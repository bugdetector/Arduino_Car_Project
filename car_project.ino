#include <TaskScheduler.h>
#include <SoftwareSerial.h>
#include <SD.h>

SoftwareSerial bluetooth(2, 3); // TX, RX
short led = 5;
short ldr = A0;
short echoPin = 6;
short trigPin = 7;
short buzzer = 8;
short motor1pin1 = A1;
short motor1pin2 = A2;
short motor2pin1 = A3;
short motor2pin2 = A4;
short chipSelect = 4;

Sd2Card card;
SdVolume volume;
SdFile root;
File file;

char lastbluetoothch = 'g';
short brightness;
short distance;
short right_Motor=-1, left_Motor=-1;

void bluetoothmodule();
void light();
void dist();
void writeToFile();
void mutebuz();
void goman();

Task one(125,TASK_FOREVER,&bluetoothmodule);
Task two(1000,TASK_FOREVER,&light);
Task three(50,TASK_FOREVER,&dist);
Task four(2000,TASK_FOREVER,&writeToFile);
Task five(0,1,&mutebuz);
Task six(100,TASK_FOREVER,&goman);

Scheduler runner;
void setup()
{
  Serial.begin(9600);
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

  pinMode(chipSelect, OUTPUT);

  if (!card.init(SPI_QUARTER_SPEED, chipSelect) && !volume.init(card)) {
    Serial.println(1);
  }
  root.openRoot(volume);
  root.ls(LS_R | LS_DATE | LS_SIZE);
  if (!SD.begin(chipSelect)) {
    Serial.println(2);
  } else {
    file = SD.open("test.txt", FILE_WRITE);    // open file for writing
    if (!file) {  // if file can be opened, write to it
      Serial.println(3);
    }
    file.close();
  }
  runner.init();
  runner.addTask(one);
  runner.addTask(two);
  runner.addTask(three);
  runner.addTask(four);
  runner.addTask(six);
  
  one.enable();
  two.enable();
  three.enable();
  four.setInterval(2000);
  four.enable();
  six.setInterval(500);
  six.enable();
}

void loop()
{
  runner.execute();
}
void writeToFile(){
  file = SD.open("test.txt", FILE_WRITE);
  file.print(lastbluetoothch);
  file.print("  ");
  file.print(brightness);
  file.print("  ");
  file.print(distance);
  file.print("  ");
  file.print(right_Motor);
  file.print("  ");
  file.println(left_Motor);
  file.close();
  //sprintf(buf,"%c\t%d\t%d\t%d\t%d",bluetoothch,brightness,distance,right_Motor,left_Motor);
}
void bluetoothmodule(){
  if(bluetooth.available()){
    char bluetoothch = bluetooth.read();
    lastbluetoothch = bluetoothch;
    switch (bluetoothch){
      case 'a': startEngine(1,0); break;
      case 'd': startEngine(0,1); break;
      case 'w': startEngine(1,1); break;
      case 's': startEngine(0,0); break;
      case 'g': startEngine(-1,-1); break; //stop engine
      defaut: goman();
    }
  }
    
}
void startEngine(short rightMotor, int leftMotor){
  right_Motor = rightMotor;
  left_Motor = leftMotor;
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
  brightness = analogRead(ldr);
  if(brightness<500){
    digitalWrite(led,HIGH);
  }
  else{
    digitalWrite(led,LOW);
  }
}
void dist(){
  distance = calcdistance();
  if(distance<15 && distance != 0){
    buz();
    if(right_Motor && left_Motor){
      startEngine(-1,-1);
    }
  }
}
int calcdistance(){
  long duration;
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  return pulseIn(echoPin, HIGH)/58.2;
}
void buz(){
  tone(buzzer, 1000);
  runner.addTask(five);
  five.setInterval(100);
  five.enable();
}
void mutebuz(){
  noTone(buzzer);
}
