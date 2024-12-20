#include <SPI.h>
#include <SD.h>
#include "SparkFun_LIS331.h"
#include <Wire.h>
#include <RTCZero.h>

LIS331 xl;
RTCZero rtc;
File myFile;
int contador = 0;
const int chipSelect = 4;



void setup() {
  
  pinMode(9,INPUT);    
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  rtc.begin();
  Wire.begin();
  xl.setI2CAddr(0x19); 
  xl.begin(LIS331::USE_I2C);
  xl.intSrcConfig(LIS331::INT_SRC, 1);
  xl.setIntDuration(50, 1); 

  rtc.setHours(12);
  rtc.setMinutes(0);
  rtc.setSeconds(0);

  rtc.setDay(19);
  rtc.setMonth(12);
  rtc.setYear(24);
  Serial.println("RTC configurado.");
  // Open serial communications and wait for port to open:
  Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("setup.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to test.txt...");
    myFile.println("testing 1, 2, 3.");
    // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening setup.txt");
  }

  // re-open the file for reading:
  myFile = SD.open("setup.txt");
  if (myFile) {
    Serial.println("test.txt:");

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error lectura setup.txt");
  }
}

void loop() {
  // nothing happens after setup
  int16_t x, y, z;
  xl.readAxes(x, y, z);
  int fuerzaG = sqrt(x*x+y*y+z*z)/9.8;
  int fuerzaGConLibreria = xl.convertToG(80,x)+xl.convertToG(80,y)+xl.convertToG(80,z);
  char timeString[20]; // Buffer para el string
  snprintf(timeString, sizeof(timeString), "%02d-%02d-%02d %02d:%02d", 
           rtc.getYear(), rtc.getMonth(), rtc.getDay(), 
           rtc.getHours(), rtc.getMinutes());
  String jsonData = "{";
   
    
    jsonData += "\"acceleration\":{";
    jsonData += "\"x\":" + String(x) + ",";
    jsonData += "\"y\":" + String(y) + ",";
    jsonData += "\"z\":" + String(z);
    jsonData += "},";
    jsonData += "\"fuerzaG\":" + String(fuerzaG) + ",";
    jsonData += "\"fuerzaGConLibreria\":" + String(fuerzaGConLibreria) ;
    jsonData += "\"Fecha\":" + String(timeString) ;
    jsonData += "}";

  if (fuerzaG > 4){
    myFile = SD.open("test" + String(contador) + ".txt", FILE_WRITE);
    //myFile2 = SD.open( "setup.txt", FILE_WRITE);
    contador++;
  }
  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to test.txt...");
    myFile.println(jsonData);
    myFile.close();
    Serial.println("done.");
  }
 
  delay(100);
  
  


}