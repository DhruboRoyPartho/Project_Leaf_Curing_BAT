// Project: Leaf Barn Curing
// Code Author: Dhrubo Roy Partho
// Date: 02/05/2024
// Version: 1.0v

#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>


LiquidCrystal_I2C lcd(0x27, 20, 4);


// Pins
#define temp_pin1 6
#define temp_pin2 5
#define relay1 7
#define relay2 8
#define btn_1 9
#define btn_2 10
#define btn_3 11
#define btn_4 12

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(temp_pin1);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);


// Time counter
unsigned long curr_time = 0;
unsigned long temp_sense_pre_time = 0;


// Global variables
float temp1_value = 0.0;

int numberOfDevices; // Number of temperature devices DS18B20
DeviceAddress tempDeviceAddress;  // For DS18B20

String line[4] = {};
float tempC[2] = {};
float tempF[2] = {};


void setup(void)
{
  // Start serial communication for debugging purposes
  Serial.begin(9600);

  // DS18B20 initialization
  sensors.begin();
  numberOfDevices = sensors.getDeviceCount();
  // Serial
  Serial.print("Locating devices...");
  Serial.print("Found ");
  Serial.print(numberOfDevices, DEC);
  Serial.println(" devices.");
  // DS18B20 find address
  for(int i=0;i<numberOfDevices; i++) {
    // Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i)) {
      Serial.print("Found device ");
      Serial.print(i, DEC);
      Serial.print(" with address: ");
      printAddress(tempDeviceAddress);
      Serial.println();
		} else {
		  Serial.print("Found ghost device at ");
		  Serial.print(i, DEC);
		  Serial.print(" but could not detect address. Check power and cabling");
		}
  }

  // LCD initialization
  lcd.init();
  lcd.clear();
  lcd.backlight();

  // Pin modes
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(btn_1, INPUT_PULLUP);
  pinMode(btn_2, INPUT_PULLUP);
  pinMode(btn_3, INPUT_PULLUP);
  pinMode(btn_4, INPUT_PULLUP);

  lcd.print("Staring");
  for(int i=0;i<3;i++){
    lcd.setCursor(8+i,0);
    lcd.print(".");
    delay(300);
  }
  delay(1000);
}

void loop(void){
  curr_time = millis(); 
  temp_measure();
}

// Temperature sensing
void temp_measure(){
  if(curr_time - temp_sense_pre_time > 3000){
    // Call sensors.requestTemperatures() to issue a global temperature and Requests to all devices on the bus
    sensors.requestTemperatures();

    // Loop through each device, print out temperature data
    for(int i=0;i<numberOfDevices; i++) {
        // Search the wire for address
        if(sensors.getAddress(tempDeviceAddress, i)){
        
          // Output the device ID
          Serial.print("Temperature for device: ");
          Serial.println(i,DEC);

          // Print the data
          tempC[i] = sensors.getTempC(tempDeviceAddress);
          Serial.print("Temp C: ");
          Serial.print(tempC[i]);
          Serial.print(" Temp F: ");
          tempF[i] = DallasTemperature::toFahrenheit(tempC[i]);
          Serial.println(tempF[i]);    //DallasTemperature::toFahrenheit(tempC)   // Converts tempC to Fahrenheit
        } 	
    }

    // Display string making
    for(int i=0;i<2;i++){
      line[i] = "s"+ (String)(i+1) +":" + (String)tempC[i] + (char)0xDF + "C  " + (String)tempF[i] + (char)0xDF + "F";
    }
    display(line);
    temp_sense_pre_time = curr_time; 
  }
}

// function to print a device address
void printAddress(DeviceAddress deviceAddress) {
  for (uint8_t i = 0; i < 8; i++) {
    if (deviceAddress[i] < 16) Serial.print("0");
      Serial.print(deviceAddress[i], HEX);
  }
}

// LCD display
void display(String line[]){
  lcd.clear();
  // lcd.setCursor(0, 0);
  // lcd.print(line1);
  // lcd.setCursor(0, 1);
  // lcd.print(line2);
  // lcd.setCursor(0, 2);
  // lcd.print(line3);
  // lcd.setCursor(0, 3);
  // lcd.print(line4);
  for(int i=0;i<4;i++){
    lcd.setCursor(0, i);
    lcd.print(line[i]);
  }
}