#include <TellstickSensor.h>

// Demonstrator TellstickSensor
// Peter Ranemyr 2015

// Optional Led blink when a package is transmited
#define LED 13
#define blinkdelay 500

//"Creates" sensor1 with transmitter connected to PB0-5
TellstickSensor Sensor1(4);


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

void blinkled(){
  digitalWrite(LED, HIGH);
  delay(blinkdelay);
  digitalWrite(LED, LOW);
}

void loop() {
  Sensor1.Transmitt();            //Transmitts sensor data
  blinkled();
  delay(5000-blinkdelay);         //Set a delay between packages   
}


