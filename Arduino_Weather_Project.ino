#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <DS3231.h>
#include "DS3231.h"
#include "DHT.h"
#include "Wire.h"
#include "SPI.h"
#include <stdint.h>
#include "SparkFunBME280.h"

#define DHTPIN 8
#define DHTTYPE DHT22 

DHT dht(DHTPIN, DHTTYPE);
Adafruit_PCD8544 display = Adafruit_PCD8544( 5, 4, 3);
DS3231  rtc(SDA, SCL);
BME280 mySensor;

boolean backlight = false;
int contrast=55;

int menuitem = 1;
int page = 0;
char tempF[6];
char humF[6]; 
volatile boolean up = false;
volatile boolean down = false;
volatile boolean select = false;
volatile boolean back = false;

int downButtonState = 0;
int upButtonState = 0;  
int selectButtonState = 0;  
int backButtonState = 0;    
int lastDownButtonState = 0;
int lastSelectButtonState = 0;
int lastUpButtonState = 0;
int lastBackButtonState = 0;

void setup() {
  dht.begin();
  pinMode(2, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  pinMode(0, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(7,OUTPUT);

  digitalWrite(7,HIGH); //Turn Backlight ON
  mySensor.settings.commInterface = I2C_MODE;
  mySensor.settings.I2CAddress = 0x76;
  mySensor.settings.runMode = 3;
  mySensor.settings.tStandby = 0;
  mySensor.settings.filter = 0;
  mySensor.settings.tempOverSample = 1;
  mySensor.settings.pressOverSample = 1;
  mySensor.settings.humidOverSample = 1;
  Serial.begin(57600);
  delay(10);  //Make sure sensor had enough time to turn on. BME280 requires 2ms to start up.
  Serial.println(mySensor.begin(), HEX);

  //Build a first-row of column headers
  Serial.println("");
  rtc.begin();
  rtc.setDOW(SUNDAY);   
  rtc.setTime(23, 18, 0);
  rtc.setDate(16, 12, 2017);
  
  display.begin();      
  display.setContrast(contrast); //Set contrast to 50
  display.clearDisplay(); 
  display.display();   
}

void loop() {
  float temp,hum,pressure;
  temp = dht.readTemperature();
  hum = dht.readHumidity();

  dtostrf(temp, 5, 1, tempF);
  dtostrf(hum, 5, 1, humF);
  
  drawMenu();

  downButtonState = digitalRead(2);
  selectButtonState = digitalRead(1);
  upButtonState =   digitalRead(0); 
  backButtonState =   digitalRead(6);
 
  checkIfDownButtonIsPressed();
  checkIfUpButtonIsPressed();
  checkIfSelectButtonIsPressed();
  checkIfBackButtonIsPressed();
  if (up) {
    if ((page == 1 )||(page == 2 )||(page == 3 )){
      up = false;
      menuitem--;
      if (menuitem==0)
      {
        menuitem=3;
      }
    }
    else if (page == 4 ) {
      up = false;
      contrast--;
      setContrast();
    }
    else if (page == 6 ) {
      up = false;
      page = 8;
    }
    else if (page == 7 ) {
      up = false;
      page = 6;
    }
    else if (page == 8 ) {
      up = false;
      page = 7;
    }
  }
  if (down) {
    if ((page == 1 )||(page == 2 )||(page == 3 )){
      down = false;
      menuitem++;
      if (menuitem==4) 
      {
        menuitem=1;
      }      
    }
    else if (page == 4 ) {
      down = false;
      contrast++;
      setContrast();
    }
     else if (page == 6 ) {
      down = false;
      page = 7;
     }
     else if (page == 7 ) {
      down = false;
      page = 8;
     }
     else if (page == 8 ) {
      down = false;
      page = 6;
     }
  }
  if (select) {
    select = false;
    if(page ==0)
    {
      page=1;
    }
    else if (page == 1)
    {
      if (menuitem==1)
      {
        page=2;
      }
      else if (menuitem==2)
      {
        page=3;
        menuitem=1;
      }
      else if (menuitem==3)
      {
        page=5;
      }
    }
    else if (page == 2)
    {
      if (menuitem==1)
      {
        page=6;
      }
      else if (menuitem==2)
      {
        page=7;
      }
      else if (menuitem==3)
      {
        page=8;
      }
    }
    else if (page == 3)
    {
      if (menuitem==2) 
      {
        if (backlight) 
        {
          backlight = false;
          turnBacklightOff();
        }
        else 
        {
          backlight = true; 
          turnBacklightOn();
        }
      }

    else if(menuitem ==3)
      {
        resetDefaults();
      }
  
    else if (menuitem==1)
      {
        page=4;
      }
   }
  }
  if (back) {
    back = false;
      if (page == 2) {
      page=1;
      menuitem=1;
     }
     else if(page == 3) {
      page=1;
      menuitem=2;
     }
      else if(page == 4) {
      page=3;
      menuitem=1;
     }
     else if(page == 5) {
      page=1;
      menuitem=3;
     }
     else if ((page == 6)||(page == 7)||(page == 8)) {
      page=2;
     }
   }
}  

void checkIfDownButtonIsPressed()
{
    if (downButtonState != lastDownButtonState) 
  {
    if (downButtonState == 0) 
    {
      down=true;
    }
    delay(50);
  }
   lastDownButtonState = downButtonState;
  }

void checkIfUpButtonIsPressed()
{
  if (upButtonState != lastUpButtonState) 
  {
    if (upButtonState == 0) {
      up=true;
    }
    delay(50);
  }
   lastUpButtonState = upButtonState;
}

void checkIfSelectButtonIsPressed()
{
   if (selectButtonState != lastSelectButtonState) 
  {
    if (selectButtonState == 0) {
      select=true;
    }
    delay(50);
  }
   lastSelectButtonState = selectButtonState;
}

void checkIfBackButtonIsPressed()
{
    if (backButtonState != lastBackButtonState) 
  {
    if (backButtonState == 0) 
    {
      back=true;
    }
    delay(50);
  }
   lastBackButtonState = backButtonState;
  }
  
  void drawMenu()
  {
  if (page==0)
  {
    display.setTextSize(1);
    //display.clearDisplay();
    //display.setTextColor(BLACK, WHITE);
    display.setCursor(18, 0);
    display.print("Weather");
    display.setCursor(18, 10);
    display.print("Station");
    display.drawFastHLine(0,20,83,BLACK);
    display.setTextSize(0.5);
    display.setCursor(0, 30);
    display.print("ArduinoProject");
    display.display();
  }
  if (page==1) //Main Menu
  {
    display.setTextSize(1);
    display.clearDisplay();
    display.setTextColor(BLACK, WHITE);
    display.setCursor(15, 0);
    display.print("MAIN MENU");
    display.drawFastHLine(0,10,83,BLACK);
    //1o Stoixeio
    display.setCursor(0, 15);
    if (menuitem==1) 
    { 
      display.setTextColor(WHITE, BLACK);
    }
    else 
    {
      display.setTextColor(BLACK, WHITE);
    }
    display.print(">Sensors");
    display.display();
    //2o Stoixeio
    display.setCursor(0, 25);   
    if (menuitem==2) 
    {
      display.setTextColor(WHITE, BLACK);
    }
    else 
    {
      display.setTextColor(BLACK, WHITE);
    }    
    display.print(">Options");
    display.display();
    //3o Stoixeio
    display.setCursor(0, 35);
    if (menuitem==3) 
    { 
      display.setTextColor(WHITE, BLACK);
    }
    else 
    {
      display.setTextColor(BLACK, WHITE);
    }  
    display.print(">Infos");
    display.display();
  }
  else if (page==2) //Sensors
  {    
    display.setTextSize(1);
    display.clearDisplay();
    display.setTextColor(BLACK, WHITE);
    display.setCursor(15, 0);
    display.print("SENSORS");
    display.drawFastHLine(0,10,83,BLACK);
    //1o Stoixeio
    display.setCursor(0, 15);
    if (menuitem==1) 
    { 
      display.setTextColor(WHITE, BLACK);
    }
    else 
    {
      display.setTextColor(BLACK, WHITE);
    }
    display.print(">Temperature");
    display.display();
    //2o Stoixeio
    display.setCursor(0, 25);   
    if (menuitem==2) 
    {
      display.setTextColor(WHITE, BLACK);
    }
    else 
    {
      display.setTextColor(BLACK, WHITE);
    }    
    display.print(">Humidity");
    display.display();
    //3o Stoixeio
    display.setCursor(0, 35);
    if (menuitem==3) 
    { 
      display.setTextColor(WHITE, BLACK);
    }
    else 
    {
      display.setTextColor(BLACK, WHITE);
    }  
    display.print(">Pressure");
    display.display();
  }
  else if (page==3) //Options
  {    
    display.setTextSize(1);
    display.clearDisplay();
    display.setTextColor(BLACK, WHITE);
    display.setCursor(15, 0);
    display.print("OPTIONS");
    display.drawFastHLine(0,10,83,BLACK);
    display.setCursor(0, 15);
   
    if (menuitem==1) 
    { 
      display.setTextColor(WHITE, BLACK);
    }
    else 
    {
      display.setTextColor(BLACK, WHITE);
    }
    display.print(">Contrast");
    display.display();
    display.setCursor(0, 25);
   
    if (menuitem==2) 
    {
      display.setTextColor(WHITE, BLACK);
    }
    else 
    {
      display.setTextColor(BLACK, WHITE);
    }    
    display.print(">Light: ");
    display.display();
    if (backlight) 
    {
      display.print("OFF");
    }
    else 
    {
      display.print("ON");
    }
    display.display();
    
    if (menuitem==3) 
    { 
      display.setTextColor(WHITE, BLACK);
    }
    else 
    {
      display.setTextColor(BLACK, WHITE);
    }  
    display.setCursor(0, 35);
    display.print(">Reset");
    display.display();
  }
  else if (page==4) //Contrast
  {
    
    display.setTextSize(1);
    display.clearDisplay();
    display.setTextColor(BLACK, WHITE);
    display.setCursor(15, 0);
    display.print("CONTRAST");
    display.drawFastHLine(0,10,83,BLACK);
    display.setCursor(5, 15);
    display.print("Value");
    display.setTextSize(2);
    display.setCursor(5, 25);
    display.print(contrast);
 
    display.setTextSize(2);
    display.display();
  }  
  else if (page==5) //Date & Time
  {
    display.setTextSize(1);
    display.clearDisplay();
    display.setTextColor(BLACK, WHITE);
    display.setCursor(15, 0);
    display.print("DATE/TIME");
    display.drawFastHLine(0,10,83,BLACK);
    display.setCursor(23, 30);
    display.print(rtc.getDOWStr());
    display.setCursor(11, 40);
    display.print(rtc.getDateStr());
    display.setCursor(17, 15);
    display.print(rtc.getTimeStr());
    display.display();
    
  }
  else if (page==6) //Temperature
  {
    display.setTextSize(1);
    display.clearDisplay();
    display.setTextColor(BLACK, WHITE);
    display.setCursor(8, 0);
    display.print("TEMPERATURE");
    display.drawFastHLine(0,10,83,BLACK);
    display.setTextSize(2);
    display.setCursor(0,20);
    display.print(tempF);
    display.setCursor(62, 20);
    display.print("C");
    display.display();
  }
  else if (page==7) //Humidity
  {
    display.setTextSize(1);
    display.clearDisplay();
    display.setTextColor(BLACK, WHITE);
    display.setCursor(15, 0);
    display.print("HUMIDITY");
    display.drawFastHLine(0,10,83,BLACK);
    display.setTextSize(2);
    display.setCursor(0, 20);
    display.print(humF);
    display.setCursor(65, 20);
    display.print("%");
    display.display();
  }
  else if (page==8) //Pressure
  {
    display.setTextSize(1);
    display.clearDisplay();
    display.setTextColor(BLACK, WHITE);
    display.setCursor(18, 0);
    display.print("PRESSURE");
    display.drawFastHLine(0,10,83,BLACK);
    display.setCursor(5, 15);
    display.print(mySensor.readFloatPressure());
    display.setCursor(67,15);
    display.print("Pa");
    display.setCursor(20, 30);
    display.print(mySensor.readFloatAltitudeMeters());
    display.setCursor(52, 30);
    display.print("m");
    display.display();
    Serial.print(mySensor.readTempC(), 2);
    Serial.print(mySensor.readFloatPressure(), 0);
    Serial.print(mySensor.readFloatAltitudeMeters(), 3);
    Serial.println();
    delay(100);
  }
 }
  void resetDefaults()
  {
    contrast = 50;
    setContrast();
    backlight = true;
    turnBacklightOn();
  }

  void setContrast()
  {
    display.setContrast(contrast);
    display.display();
  }

  void turnBacklightOn()
  {
    digitalWrite(7,LOW);
  }

    void turnBacklightOff()
  {
    digitalWrite(7,HIGH);
  }



