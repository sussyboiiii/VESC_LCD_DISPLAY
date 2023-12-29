  ///// Libraries /////
#include <DS3231.h>
#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>
#include <SimpleKalmanFilter.h>
#include <VescUart.h>
#include <Wire.h>


  ///// Variables /////
const int offswitch = 4;
const int ntc = A1;

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
float remainingrange;
float temperature;
float output_voltage;
float thermistor_resistance;
float therm_res_ln;
int   thermistor_adc_val;
int   altitude;
float tempmotor;
float tempmosfet;
float tempbattery;
float tripdistance;

  ///// Init /////
LiquidCrystal_I2C lcd(0x27, 20, 4);

VescUart VESC;

SimpleKalmanFilter powerfilter(2, 2, 0.01);

DS3231  rtc(SDA, SCL);

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

Serial.begin(115200);

while (!Serial) {;}

VESC.setSerialPort(&Serial);

// pinMode(offswitch, INPUT);

rtc.begin();

lcd.init();
lcd.backlight(); 

// The following lines can be uncommented to set the date and time
  // rtc.setDOW(FRIDAY);     // Set Day-of-Week to FRIDAY
  // rtc.setTime(13, 27,40);     // Set the time to 12:00:00 (24hr format)


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


// if (digitalRead(offswitch) == 1){                 
//   lcd.clear();
//   lcd.setBacklight(0);
//   exit(0);
// }

if (VESC.getVescValues()){
  rpm = (VESC.data.rpm)/23;                       // The '23' is the number of pole pairs in the motor. This motor has 46 poles, therefore 23 pole pairs
  voltage = (VESC.data.inpVoltage);
  current = (VESC.data.avgMotorCurrent);
  power = voltage*current;
  powerfiltered = powerfilter.updateEstimate(power);
  amphour = (VESC.data.ampHours);
  watthour = amphour*voltage;
  tach = (VESC.data.tachometerAbs)/138;           // The '138' is the number of motor poles multiplied by 3
  tripdistance = tach*3.142*(0.001)*0.7;             // Motor RPM x Pi x (1 / meters in a mile or km) x Wheel diameter
  speed = rpm*3.142*(0.06)*0.7;                  // Motor RPM x Pi x (seconds in a minute / meters in a mile) x Wheel diameter
  batterypercentage = ((voltage-38.4)/12)*100;    // ((Battery voltage - minimum voltage) / number of cells) x 100
  tempmotor = (VESC.data.tempMotor);
  tempmosfet = (VESC.data.tempMosfet);
}


  ///// 1st line /////

// Speed
if (speed <= 0){
  speed = 0;
}
if(speed < 10){
  lcd.setCursor(0, 0);
  lcd.print(" ");
  lcd.print(speed, 2); 
  }
if (speed > 9 && speed < 100){
  lcd.setCursor(0, 0);
  lcd.print(speed, 2);
  }
if (speed > 99){
  lcd.setCursor(0, 0);
  lcd.print(99.99);
}

// Battery percentage
if (batterypercentage < 10){
  lcd.setCursor(9, 0);
  lcd.print("  ");
  lcd.print(batterypercentage, 0);
}
if(batterypercentage < 100 && batterypercentage > 9){
  lcd.setCursor(9, 0);
  lcd.print(" ");
  lcd.print(batterypercentage, 0);
  }
if (batterypercentage == 100){
  lcd.setCursor(9, 0);
  lcd.print(batterypercentage, 0);
  }

// Battery voltage
lcd.setCursor(15, 0);
lcd.print(voltage, 1);


  ///// 2nd line /////

// Power output
if(powerfiltered < 10){
  lcd.setCursor(2, 1);
  lcd.print("   ");
  lcd.print(powerfiltered, 0);
}
if(powerfiltered > 10 && powerfiltered < 100){
  lcd.setCursor(2, 1);
  lcd.print("  ");
  lcd.print(powerfiltered, 0);
}
if(powerfiltered > 100 && powerfiltered < 1000){
  lcd.setCursor(2, 1);
  lcd.print(" ");
  lcd.print(powerfiltered, 0);
}
if(powerfiltered > 1000){
  lcd.setCursor(2, 1);
  lcd.print(powerfiltered, 0);
}

// Current
if (current < 10){
  lcd.setCursor(9, 1);
  lcd.print("  ");
  lcd.print(current, 0);
}
if (current > 9 && current < 100){
  lcd.setCursor(9, 1);
  lcd.print(" ");
  lcd.print(current, 0);
}
if (current > 99){
  lcd.setCursor(9, 1);
  lcd.print(current, 0);
}

// Amphours consumed
lcd.setCursor(15,1);
lcd.print(amphour, 2);


  ///// 3rd line /////

// Battery temp
if (tempbattery < 10){
  lcd.setCursor(2, 1);
  lcd.print("  ");
  lcd.print(tempbattery, 0);
}
if (tempbattery > 9 && tempbattery < 100){
  lcd.setCursor(2, 1);
  lcd.print(" ");
  lcd.print(tempbattery, 0);
}
if (tempbattery > 99){
  lcd.setCursor(2, 1);
  lcd.print(tempbattery, 0);
}

// Motor temp
if (tempmotor < 10){
  lcd.setCursor(8, 1);
  lcd.print("  ");
  lcd.print(tempmotor, 0);
}
if (tempmotor > 9 && tempmotor < 100){
  lcd.setCursor(8, 1);
  lcd.print(" ");
  lcd.print(tempmotor, 0);
}
if (tempmotor > 99){
  lcd.setCursor(8, 1);
  lcd.print(tempmotor, 0);
}

// Mosfet temp
if (tempmosfet < 10){
  lcd.setCursor(15, 1);
  lcd.print("  ");
  lcd.print(current, 0);
}
if (tempmosfet > 9 && tempmosfet < 100){
  lcd.setCursor(15, 1);
  lcd.print(" ");
  lcd.print(tempmosfet, 0);
}
if (tempmosfet > 99){
  lcd.setCursor(15, 1);
  lcd.print(tempmosfet, 0);
}


  ///// 4th line /////

// Time
lcd.setCursor(0, 3);
lcd.print(rtc.getTimeStr());

// Ambient temperature
thermistor_adc_val = analogRead(ntc);                           
output_voltage = ( (thermistor_adc_val * 5.0) / 1023.0 );
thermistor_resistance = ((5 * (10.0 / output_voltage)) - 10) * 1000; 
therm_res_ln = log(thermistor_resistance);
temperature = (1 / (0.001129148 + (0.000234125 * therm_res_ln) + (0.0000000876741 * therm_res_ln * therm_res_ln * therm_res_ln)));
temperature = temperature - 273.15;

if (temperature < -99){
  lcd.setCursor(9, 3);
  lcd.print(-99); 
}
if (temperature < -10 && temperature > -100){
  lcd.setCursor(9, 3);
  lcd.print(temperature, 0); 
}
if (temperature < 0 && temperature > -9){
  lcd.setCursor(9, 3);
  lcd.print(" ");
  lcd.print(temperature, 0); 
}
if (temperature < 9 && temperature > 0){
  lcd.setCursor(9, 3);
  lcd.print("  ");
  lcd.print(temperature, 0); 
}
if (temperature > 10 && temperature < 100){
  lcd.setCursor(9, 3);
  lcd.print(" ");
  lcd.print(temperature, 0); 
}
if (temperature > 99){
  lcd.setCursor(9, 3);
  lcd.print(" ");
  lcd.print(99); 
}

// Trip distance
if (tripdistance < 10){
  lcd.setCursor(14, 3);
  lcd.print("  ");
  lcd.print(tripdistance, 1);
  }
if (tripdistance > 9 && tripdistance < 100){
  lcd.setCursor(14, 3);
  lcd.print(" ");
  lcd.print(tripdistance, 1);
  }
if (tripdistance > 99){
  lcd.setCursor(14, 3);
  lcd.print(tripdistance, 1);
}

delay(50);
}