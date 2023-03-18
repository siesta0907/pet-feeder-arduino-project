#include <WiFi.h>
#include "time.h"
#include <ESP32_Servo.h>
#include "BluetoothSerial.h"

#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
DFRobotDFPlayerMini mp3;

static const int servoPin = 4;
static const int infrared = 14;


BluetoothSerial SerialBT;
Servo servo1;

const char* ssid       = "hee";
const char* password   = "89008900";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 32400;
const int   daylightOffset_sec = 0;

int servoDegrees = 90;
char timeHour[3],timeMin[3], timeSec[3];

void servoMove(){
    for(int posDegrees = 0; posDegrees <= servoDegrees; posDegrees++) {
        servo1.write(posDegrees);
        delay(20);
    }
    delay(1000);
    for(int posDegrees = servoDegrees; posDegrees >= 0; posDegrees--) {
        servo1.write(posDegrees);
        delay(20);
    }
}

void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  //시간출력
  Serial.println(&timeinfo);

  //시간저장
  strftime(timeHour,3, "%H", &timeinfo);
  strftime(timeMin,3, "%M", &timeinfo);
  strftime(timeSec,3, "%S", &timeinfo);
}

void test(){
  delay(10000);
  
  servoMove();
}

void setup()
{
  Serial.begin(115200);
  servo1.attach(servoPin);
  pinMode(infrared, INPUT);
  SerialBT.begin("ESP_32_C_PetFeeder");
  
  //connect to WiFi
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println(" CONNECTED");
  
  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();
  
  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);


  //--------------------
  // DFPlayer mini
  //--------------------
  Serial2.begin(9600);
  delay(100);
  
  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  
  if (!mp3.begin(Serial2)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true);
  }
  Serial.println(F("DFPlayer Mini online."));
  
  mp3.setTimeOut(500); //Set serial communictaion time out 500ms
  
  //----Set volume----
  mp3.volume(15);  //Set volume value (0~30).
  mp3.volumeUp(); //Volume Up
  mp3.volumeDown(); //Volume Down
  
  //----Set different EQ----
  mp3.EQ(DFPLAYER_EQ_NORMAL);
  
  //----Set device we use SD----
  mp3.outputDevice(DFPLAYER_DEVICE_SD);

}

void mp3player(int i){
  mp3.play(i); 
  delay(5000);
  mp3.pause(); 
}

void loop()
{
  
  //아침
  if (!strcmp(timeHour, "13") and  !strcmp(timeMin, "10") and !strcmp(timeSec, "00")){
     Serial.println("움직여용");
     mp3player(1);
   servoMove();
      
  }
  //점심
  if (!strcmp(timeHour, "13") and  !strcmp(timeMin, "12") and !strcmp(timeSec, "00")){
        Serial.println("움직여용");
        mp3player(2);
        servoMove();
         
  }
  //저녁
  if (!strcmp(timeHour, "13") and  !strcmp(timeMin, "15") and !strcmp(timeSec, "00")){
        Serial.println("움직여용");
        mp3player(3);
        servoMove();
        
  } 
  
  //0이면 감지0, 이외의 값 감지X
  int state = digitalRead(infrared);
  if (state ==0){
    Serial.println("반려동물이 감지되었음!");
    SerialBT.println("반려동물이 감지되었음!");
  }
  printLocalTime();

  
  //test();
  delay(1000);
}
