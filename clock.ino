#include <Arduino.h>
#include <TM1637Display.h>

#include <NTPClient.h>
#include <WiFi.h> 
#include <WiFiUdp.h>

//pins for 7-seg disp
#define CLK 9
#define DIO 10


const char *ssid     = "ssid";
const char *password = "password";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
TM1637Display display(CLK, DIO);

void setup()
{
  pinMode(1, OUTPUT);
  digitalWrite(1, LOW); //set ground pin for switch
  // switch connected between pin 1 and 4 (SPST)
  pinMode(4, INPUT_PULLUP); //switch input for daylight savings
  Serial.begin(115200);

  WiFi.begin(ssid, password);

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    //Serial.print ( "." );
  }

  timeClient.begin();


  uint8_t data[] = { 0, 0, 0, 0 };
  display.setBrightness(15); //0-15

  // All segments on
  display.setSegments(data);

  
  uint8_t dotCommand = 0b01000000; // bit 6 set => colon on
  uint8_t dotMask = 0b01000000;

  while(1){

    timeClient.update();
    char string[128];
    sprintf(string, "%02d%02d", (!digitalRead(4) ? (timeClient.getHours()+1) : timeClient.getHours()), timeClient.getMinutes()); // if switch set -> daylight saving (+1h), if not normal
    display.showNumberDecEx(atoi(string), dotCommand ^= dotMask , true, 4, 0); // flip bit every second
    Serial.println(string);

    //dimm display according to time 
    if(timeClient.getHours() >= 19 || timeClient.getHours() <= 7){
      display.setBrightness(1);
    }
    else{
        display.setBrightness(15); 
    }

    delay(1000);
  }
 
}

void loop(){

}
