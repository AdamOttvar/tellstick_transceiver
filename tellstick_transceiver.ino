#include <OneWire.h>
#include <TellstickSensor.h>

// Demonstrator TellstickSensor
// Peter Ranemyr 2015

// Optional Led blink when a package is transmited
#define LED 13
#define blinkdelay 500

//"Creates" sensor1 with transmitter connected to PB0-5
// Which is pin D8 on Arduino UNO
TellstickSensor Sensor1(0);

// Code for accessing and conversion of DS18B20 data taken
// from OneWire example DS18x20_Temperature
// https://www.pjrc.com/teensy/td_libs_OneWire.html
// Creates a OneWire object at pin 10
OneWire  ds(10);

byte i;
byte data[12];
// Adress of DS18B20 taken from DS18x20_Temperature
byte addr[8] = {0x28, 0x2E, 0x6F, 0xB0, 0x6, 0x0, 0x0, 0x85};
float celsius;

void setup() {
  Serial.begin(9600);
  Serial.println("TellstickSensor");
  Sensor1.SetTemperature(266);    //Set temperature
  Sensor1.SetHumidity(88);        //Set humidity
  Sensor1.SetSendCount(3);        //Number of times the sensor data should be transmitted (default 3)
  Sensor1.SetSendCountDelay(10);  //Delay in ms between transmitted data (default 10)
  Sensor1.SetDeviceID(200);       //Device id for sensor 1-255
  pinMode(LED, OUTPUT);
}

void loop() {
  Sensor1.Transmitt();            //Transmitts sensor data
  blinkled();
  delay(5000-blinkdelay);         //Set a delay between packages   
}

void blinkled(){
  digitalWrite(LED, HIGH);
  delay(blinkdelay);
  digitalWrite(LED, LOW);
}

void readTempAndPrint(){
  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end
  
  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }

  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];

  byte cfg = (data[4] & 0x60);
  // at lower res, the low bits are undefined, so let's zero them
  if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
  else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
  else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
  //// default is 12 bit resolution, 750 ms conversion time

  celsius = (float)raw / 16.0;
  Serial.print("  Temperature = ");
  Serial.print(celsius);
  Serial.println(" Celsius");
}

