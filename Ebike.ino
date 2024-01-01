  ///// Libraries /////
#include <DS3231.h>
#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>
#include <SimpleKalmanFilter.h>
#include <VescUart.h>
#include "thermistor.h"
#include "HardwareSerial.h"

  ///// Variables /////

const int ambientTempPin = A1;
const int batteryTempPin = A2;

const int SW = 2;

unsigned long previousMillis = 0;
const long interval = 50;

char date;
float rpm;
float voltage;
float current;
float power;
int   powerpercent;
float powerfiltered;
float amphour;
float tach;
float speed;
float watthour;
float batterypercentage;
float averagespeed;
float timedriving;
float temperature;
float tempmotor;
float tempmosfet;
float tempbattery;
float tripdistance;
float lasttrip;
bool istrip = true;

  ///// Init /////

THERMISTOR thermistorab(ambientTempPin, 10000, 3988, 10000);
THERMISTOR thermistorbt(batteryTempPin, 10000, 3988, 10000);

LiquidCrystal_I2C lcd(0x27, 20, 4);

VescUart VESC;

SimpleKalmanFilter powerfilter(2, 2, 0.01);

DS3231 rtc(SDA, SCL);

  ///// Custom lcd characters /////
// https://maxpromer.github.io/LCD-Character-Creator/

byte bat[] = { // battery symbol 
  B01110,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};

byte ph[] = { // per hour
  B11100,
  B10100,
  B11100,
  B10000,
  B10101,
  B00111,
  B00101,
  B00101
};

byte km[] = { // km 
  B01000,
  B01010,
  B01100,
  B01010,
  B10001,
  B11011,
  B10101,
  B10001
};

byte ah[] = { // amphour
  B00100,
  B01010,
  B01110,
  B01010,
  B00000,
  B01010,
  B01110,
  B01010
};

byte dgC[] = { // °C
  B11000,
  B11000,
  B00111,
  B01000,
  B01000,
  B01000,
  B01000,
  B00111
};

byte k[] = { // kilometer
  B01000,
  B01010,
  B01100,
  B01010,
  B01010,
  B00000,
  B00000,
  B00000
};

byte mt[] = { // motor
  B10001,
  B11011,
  B10101,
  B10001,
  B00000,
  B01110,
  B00100,
  B00100
};

byte ms[] = { // mosfet
  B10001,
  B11011,
  B10101,
  B10001,
  B01110,
  B01000,
  B01100,
  B01000
};

void setup() {

previousMillis = millis() - interval;

Serial.begin(115200);

while (!Serial) {;}

VESC.setSerialPort(&Serial);

pinMode(SW, INPUT);

rtc.begin();

// The following lines can be uncommented to set the date and displayTime
  // rtc.setDOW(FRIDAY);     // Set Day-of-Week to FRIDAY
  // rtc.setTime(13, 27,40);     // Set the displayTime to 12:00:00 (24hr format)
  // rtc.setDate(26);     // Set date

EEPROM.get(0, date);
EEPROM.get(4, lasttrip);

if (date != char(rtc.getDateStr())) {
  lasttrip = 0;
  istrip = false;
}

lcd.init();
lcd.backlight(); 

  ///// Custom lcd characters init /////
lcd.createChar(0, k); 
lcd.createChar(1, ph);
lcd.createChar(2, km);
lcd.createChar(3, dgC);
lcd.createChar(4, mt);
lcd.createChar(5, ms);
lcd.createChar(6, ah);
lcd.createChar(7, bat);


// Speed
lcd.setCursor(5, 0);
lcd.write(0);
lcd.setCursor(6, 0);
lcd.write(1);

// Battery percentage
lcd.setCursor(12, 0);
lcd.print((char) 37);

// Voltage
lcd.setCursor(19, 0);
lcd.print("V");

// Wattage
lcd.setCursor(6, 1);
lcd.print("W");

// Current
lcd.setCursor(12, 1);
lcd.print("A");

// Amphours
lcd.setCursor(19, 1);
lcd.write(6);

// Battery temp
lcd.setCursor(5, 2);
lcd.write(3);
lcd.write(7);

// Motor temp
lcd.setCursor(11, 2);
lcd.write(3);
lcd.write(4);

// Mosfet temp
lcd.setCursor(18, 2);
lcd.write(3);
lcd.write(5);

// Trip distance
lcd.setCursor(19, 3);
lcd.write(2);

// Ambient temperature
lcd.setCursor(12, 3);
lcd.write(3);
}

void loop() {

  checkSwitch();

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    getVescValues();
    displayVelocity();
    displayBatteryPercentage();
    displayBatteryVoltage();
    displayPowerOutput();
    displayCurrent();
    displayAmphoursUsed();
    // displayBatteryTemp();
    displayMotorTemp();
    displayMosfetTemp();
    displayTime();
    displayAmbientTemp();
    displayTripDistance();
    }
}

void checkSwitch() {
  
  if (digitalRead(SW) == HIGH){
    // lcd.clear();
    // lcd.setBacklight(0);

    if (istrip) {
      date = char(rtc.getDateStr());
      EEPROM.put(0, date);
      EEPROM.put(4, tripdistance);
    }
    else {
      EEPROM.put(4, tripdistance);
    }
    exit(0);
  }
}

void getVescValues() {
  if (VESC.getVescValues()){
    rpm = (VESC.data.rpm)/23;                       // The '23' is the number of pole pairs in the motor. This motor has 46 poles, therefore 23 pole pairs
    voltage = (VESC.data.inpVoltage);
    current = (VESC.data.avgMotorCurrent);
    if (voltage*current != 0) {powerfiltered = powerfilter.updateEstimate(voltage*current);}
    amphour = (VESC.data.ampHours);
    watthour = amphour*voltage;
    tach = (VESC.data.tachometerAbs)/138;           // The '138' is the number of motor poles multiplied by 3
    tripdistance = lasttrip + (tach*M_PI*0.0007);             // Lasttrip distance + (Motor RPM x Pi x (1 / meters in a mile or km) x Wheel diameter)
    speed = rpm*M_PI*(60/1609.34)*0.7;                  // Motor RPM x Pi x (seconds in a minute / meters in a mile) x Wheel diameter
    batterypercentage = ((voltage-38.4)/12)*100;    // ((Battery voltage - minimum voltage) / number of cells) x 100
    tempmotor = (VESC.data.tempMotor);
    tempmosfet = (VESC.data.tempMosfet);
  }
}

  ///// 1st line /////

void displayVelocity() {
  lcd.setCursor(0, 0);
  if (speed <= 0){
    lcd.print(0.00, 2);
  }
  else if (speed < 10){
    lcd.print(" ");
    lcd.print(speed, 2); 
  }
  else if (speed < 100){
    lcd.print(speed, 2);
  }
  else if (speed > 99){
    lcd.print(99.99);
  }
}

void displayBatteryPercentage() {
  lcd.setCursor(9, 0);
  if (batterypercentage < 10){
    lcd.print("  ");
  }
  else if(batterypercentage < 100){
    lcd.print(" ");
  }
  lcd.print(batterypercentage, 0);
}

void displayBatteryVoltage() {
  lcd.setCursor(15, 0);
  lcd.print(voltage, 1);
}

  ///// 2nd line /////

void displayPowerOutput() {
  lcd.setCursor(2, 1);
  if (powerfiltered < 10){
    lcd.print("   ");
  }
  else if (powerfiltered < 100){
    lcd.print("  ");
  }
  else if (powerfiltered < 1000){
    lcd.print(" ");
  }
  lcd.print(powerfiltered, 0);
}

void displayCurrent() {
  lcd.setCursor(9, 1);
  if (current < 10){
    lcd.print("  ");
  }
  else if (current < 100){
    lcd.print(" ");
  }
  lcd.print(current, 0);
}

void displayAmphoursUsed() {
  lcd.setCursor(15,1);
  lcd.print(amphour, 2);
}

  ///// 3rd line /////

void displayBatteryTemp() {
  tempbattery = thermistorbt.read();
  lcd.setCursor(2, 2);
  if (tempbattery < 10){
    lcd.print("  ");
  }
  else if (tempbattery < 100){
    lcd.print(" ");
  }
  lcd.print(tempbattery, 0);
}

void displayMotorTemp() {
  lcd.setCursor(8, 2);
  if (tempmotor < 10){
    lcd.print("  ");
  }
  else if (tempmotor < 100){
    lcd.print(" ");
  }
  lcd.print(tempmotor, 0);
}

void displayMosfetTemp() {
  lcd.setCursor(15, 2);
  if (tempmosfet < 10){
    lcd.print("  ");
  }
  else if (tempmosfet < 100){
    lcd.print(" ");
  }
  lcd.print(tempmosfet, 0);
}

  ///// 4th line /////

void displayTime() {
  lcd.setCursor(0, 3);
  lcd.print(rtc.getTimeStr());
}

void displayAmbientTemp() {
  temperature = thermistorab.read();
  lcd.setCursor(9, 3);

  if (temperature < 0 && temperature > -9){
    lcd.print(" ");
  }
  else if (temperature < 9 && temperature > 0){
    lcd.print("  ");
  }
  else if (temperature > 10 && temperature < 100){
    lcd.print(" ");
  }
  lcd.print(temperature, 0); 
}

void displayTripDistance() {
  lcd.setCursor(14, 3);
  if (tripdistance < 10){
    lcd.print("  ");
    }
  else if (tripdistance < 100){
    lcd.print(" ");
    }
  lcd.print(tripdistance, 1);
}