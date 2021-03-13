/*  # constant_acc_monitor.ino
 *   @version 1.0.0
 *   @author mateusmolina
 *   This sketch was made for the Arduino Nano, but should work with other Arduinos as well.
 *   Set DEBUG=1 to monitor the process through serial
 *   
 *  ## Components list
 *  - MMA8452Q Accelerometer  
 *  - DS3231
 *  - MicroSDCard Adapter (SDFat compatible)
*/

#include <SPI.h>
#include <SdFat.h>
#include<Wire.h>
#include <SparkFun_MMA8452Q.h>
#include <RTClib.h>

#define DEBUG 1
#define PRINTSERIAL 0
#define DELAY 1
#define TH 80


#define SAMPLESPERFILE 10000 // 4934475
#define SAMPLESPERFLUSH 175

int ORI;

// LOG CONST
#define ERROR 'X'
#define ATTENTION '!'
#define INFO 'i'


RTC_DS3231 rtc;
SdFat SD;
MMA8452Q accel;

const int SD_CS = 4; //SD Card CS
short a1, a2;

char DataLine[17]; //142059,-999,-9990
char filename[16] = "20210101_01.csv"; //20210101_01.csv0
char ts[7]; //2359590

int StepsSinceLastFlush = 1;
unsigned long SamplingTime;
unsigned long SampleCounter = 1;
char cc[1];

void Log(String subject, String msg, char signType = INFO);
void setSD();
void setAccel();
void setRTC();
void setTS();

void setFileName();
void setOrientation();
void acquireData();

void setup() {
  Serial.begin(9600);
  
  setAccel();
  setRTC();
  setSD();
//  pinMode(LED_BUILTIN, INPUT);
//  digitalWrite(LED_BUILTIN, LOW);
  Log("ACCEL", "SETTING ORIENTATION");
  delay(2000);
  setOrientation();
  Log("ORIENTATION", (String)ORI);
  delay(2000);
  Log("LOGGER", "STARTING LOGGING PROCESS");
  #if DEBUG
    Log("SIZEPERFILE", (String)((float)SAMPLESPERFILE*sizeof(DataLine)/1024/1024)+"MB");
  #endif
}

void loop() {
  setFileName();
  #if DEBUG
    Log("OPENING FILE", filename);
  #endif
  File dataFile = SD.open(filename, FILE_WRITE);

  SampleCounter = 1;
  setTS();
  SamplingTime = millis();

  #if DEBUG && PRINTSERIAL
    Log("LOGGER", "PRINTING ACCEL DATA!", ATTENTION);
  #endif

//  // Print Header
//  snprintf(DataLine, 17, "ts,a1,a2");
//  dataFile.println(DataLine);

  while (SampleCounter < SAMPLESPERFILE) { 
    acquireData();
    dataFile.println(DataLine);
    
    #if DEBUG && PRINTSERIAL
      Serial.println(DataLine);
    #endif
    
    if (StepsSinceLastFlush >= SAMPLESPERFLUSH) { 
      dataFile.flush();
      setTS();
      StepsSinceLastFlush = 1;
    }

    StepsSinceLastFlush++;
    SampleCounter++;
    #if DELAY
      delay(DELAY);
    #endif
  }
  dataFile.close();  
  #if DEBUG
    Serial.println(DataLine);
    Log("Duration", (String)(millis() - SamplingTime)+"ms");
  #endif

}

/*  # Functions
 *  ## Util
*/
void Log(String subject, String msg, char signType = INFO){
//  Serial.print("[");Serial.print(signType);Serial.print("] ");
  Serial.print(subject);Serial.print(": ");
  Serial.println(msg);
  if (signType == ERROR){
    digitalWrite(LED_BUILTIN, HIGH);
    while (1);
  }
}


// setters
void setSD() {
  while (!SD.begin(SD_CS,SD_SCK_MHZ(50))) {
    Log("SD", "NOT FOUND", ERROR);
  }
  Log("SD","INITIALIZED");
}


void setAccel() {
  Wire.begin();
  if (accel.begin() == false) {
    Log("ACCEL", "NOT FOUND", ERROR);
  }
  Log("ACCEL", "INITIALIZED");
  accel.init(SCALE_8G, ODR_800);
}

void setRTC() {
  if (! rtc.begin()) {
    Log("RTC", "NOT FOUND", ERROR);
  }
  Log("RTC", "INITIALIZED");
  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Sketch comp. timestamp
    Log("RTC", "RESETED", ATTENTION);
  }
}


void setTS(){
  DateTime now = rtc.now();
  sprintf(ts, "%02u%02u%02u", now.hour(),now.minute(),now.second());
}

void setFileName(){
  DateTime now = rtc.now();
  sprintf(filename, "%04u%02u%02u_01.csv", now.year(),now.month(),now.day());
  filename[9] = '0';
  filename[10] = '1';
  
  for (int c = 1; c < 99; c++) {
    itoa(c, cc, 10);
    if (c < 10) { 
      filename[10] = cc[0];
    }
    else if (c < 100) {
      filename[10] = cc[1];
      filename[9] = cc[0];
    }
    if (!SD.exists(filename)) {
      break;
    }

  }
}

void setOrientation(){
  short ax,ay,az;
  if (accel.available()) {
    accel.read();
    ax = abs((short)((float)accel.x/256*100));
    ay = abs((short)((float)accel.y/256*100));
    az = abs((short)((float)accel.z/256*100));
    
    if (az >= TH){
      ORI = 0; 
    }else if (ay>=TH){
      ORI = 1;
    }else {
      ORI = 2;
    }
  }
}

void acquireData() {
  short ax,ay,az;
  if (accel.available()) {
    accel.read();
    ax = (short)((float)accel.x/256*100);
    ay = (short)((float)accel.y/256*100);
    az = (short)((float)accel.z/256*100);

    if (ORI==0){
      a1 = ax;
      a2 = ay; 
    }
    else if (ORI==1){
      a1 = ax;
      a2 = az;
    }
    else if (ORI==2){
      a1 = ay;
      a2 = az;
    }
  }
  snprintf(DataLine, 17, "%s,%i,%i", ts, a1, a2);
}
