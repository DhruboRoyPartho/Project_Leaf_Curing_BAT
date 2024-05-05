// Project: Leaf Barn Curing
// Code Author: Dhrubo Roy Partho
// Date: 02/05/2024
// Version: 1.0v

#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
// #include <Pushbutton.h>
#include <SD.h>
#include <SPI.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

// millis() variable for reset
extern volatile unsigned long timer0_millis;
unsigned long new_value = 1000;

// Pins
#define temp_pin1 6
#define temp_pin2 5
#define dam_relay1 3
#define dam_relay2 4
#define dir_relay 2
#define btn_1 A0
#define btn_2 A1
#define btn_3 A2
#define btn_4 A3
#define cs_pin 10


// Button Object
// Pushbutton button1(btn_1);
// Pushbutton button2(btn_2);
// Pushbutton button3(btn_3);
// Pushbutton button4(btn_4);


// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(temp_pin1);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);


// Time counter
unsigned long curr_time = 0;
unsigned long pre_time = 0;
unsigned long temp_sense_pre_time = 0;
unsigned long hour = 0, min = 0, sec = 0, pre_h, pre_m, pre_sec;
unsigned long defined_time = 0;


// Global variables
int numberOfDevices; // Number of temperature devices DS18B20
DeviceAddress tempDeviceAddress;  // For DS18B20

String line[4] = {};
float tempC[2] = {};
float tempF[2] = {};

volatile bool state_1 = false;
volatile bool state_2 = false;
volatile bool state_3 = false;
volatile bool state_4 = false;

byte selected_phase = 0;
byte pre_selected_phase = 0;
bool flag = false;



// Phase tem data
const byte phase[4][14][2] = {{{95, 92}, {96, 93}, {98, 94}, {99, 95}, {100, 96}},
                              {{100, 96}, {102, 96}, {104, 97}, {106, 97}, {108, 98}, {110, 98}, {112, 98}, {114, 99}, {116, 99}, {118, 100}, {120, 100}},
                              {{120, 100}, {122, 100}, {124, 101}, {126, 101}, {128, 102}, {130, 102}, {132, 102}, {134, 103}, {136, 103}, {138, 104}, {140, 104}, {142, 104}, {144, 105}, {145, 105}},
                              {{145, 105}, {147, 106}, {151, 107}, {153, 107}, {155, 108}, {157, 108}, {159, 109}, {161, 109}, {163, 110}, {163, 110}, {165, 110}}};

const byte phase_duration_h[4] = {4, 10, 14, 10};
const byte phase_duration_m[4] = {0, 0, 30, 0};

byte loader_icon_counter = 0;

// File log_file;


void setup(void)
{
  // Start serial communication for debugging purposes
  Serial.begin(9600);

  // Pin Change Interruption
    PCICR |= (1<<PCIE1);
    PCMSK1 |= (1<<PCINT8);
    PCMSK1 |= (1<<PCINT9);
    PCMSK1 |= (1<<PCINT10);
    PCMSK1 |= (1<<PCINT11);

  // DS18B20 initialization
  sensors.begin();
  numberOfDevices = sensors.getDeviceCount();
  // Serial
//   Serial.print("Locating devices...");
//   Serial.print("Found ");
//   Serial.print(numberOfDevices, DEC);
//   Serial.println(" devices.");
  // DS18B20 find address
//   for(int i=0;i<numberOfDevices; i++) {
//     // Search the wire for address
//     if(sensors.getAddress(tempDeviceAddress, i)) {
//       Serial.print("Found device ");
//       Serial.print(i, DEC);
//       Serial.print(" with address: ");
//       printAddress(tempDeviceAddress);
//       Serial.println();
// 		} else {
// 		  Serial.print("Found ghost device at ");
// 		  Serial.print(i, DEC);
// 		  Serial.print(" but could not detect address. Check power and cabling");
// 		}
//   }

  // LCD initialization
  lcd.init();
  lcd.clear();
  lcd.backlight();

  // Pin modes
  pinMode(dam_relay1, OUTPUT);
  pinMode(dam_relay2, OUTPUT);
  pinMode(dir_relay, OUTPUT);

  pinMode(btn_1, INPUT_PULLUP);
  pinMode(btn_2, INPUT_PULLUP);
  pinMode(btn_3, INPUT_PULLUP);
  pinMode(btn_4, INPUT_PULLUP);
//   pinMode(btn_1, INPUT_PULLUP);
//   pinMode(btn_2, INPUT_PULLUP);
//   pinMode(btn_3, INPUT_PULLUP);
//   pinMode(btn_4, INPUT_PULLUP);

//   lcd.print("Staring");
//   for(int i=0;i<3;i++){
//     lcd.setCursor(8+i,0);
//     lcd.print(".");
//     delay(300);
//   }
//     lcd.clear();

    // lcd.print("Memory testing...");
    // delay(200);
    // lcd.setCursor(0, 1);
    // if(!SD.begin(cs_pin)){
    //     // Serial.println(F("Memory error."));
    //     lcd.print("Memory error.");
    //     while(1);
    // }

    // if(SD.exists("logfile.txt") == 1){
    //     // read values and delete the file
    //     logFile = SD.open("logFile.txt", FILE_READ);

    //     logFile.close();
    //     delay(20);
    //     SD.remove("logfile.txt");
    // }
    // else{

    // }

    // lcd.print("Memory ok");
    // delay(200);

    // log_file = SD.open("logfile.txt", FILE_WRITE);
    // if(log_file){
    //     Serial.println("Ok");
    // }

    // if(SD.exists("logfile.txt")){
    //     log_file = SD.open("logfile.txt");
    //     while(log_file.available()){
    //         Serial.write(log_file.read());
    //     }
    //     log_file.close();
    // }

    delay(1000);
}

void error_msg(){
    lcd.clear();
    lcd.print("Error");
}


// Interruption service routine
ISR(PCINT1_vect){

    if(digitalRead(btn_1)){
        state_1 = true;
        state_2 = false;
        state_3 = false;
        state_4 = false;
    }

    if(digitalRead(btn_2)){
        state_1 = false;
        state_2 = true;
        state_3 = false;
        state_4 = false;
    }

    if(digitalRead(btn_3)){
        state_1 = false;
        state_2 = false;
        state_3 = true;
        state_4 = false;
    }

    if(digitalRead(btn_4)){
        state_1 = false;
        state_2 = false;
        state_3 = false;
        state_4 = true;
    }

//   if(!button1.isPressed()){
//         // Serial.println("A0 Pressed");
//         state_1 = true;
//         state_2 = false;
//         state_3 = false;
//         state_4 = false;
//     }
//     if(!button2.isPressed()){
//         // Serial.println("A1 Pressed");
//         state_1 = false;
//         state_2 = true;
//         state_3 = false;
//         state_4 = false;
//     }
//     if(!button3.isPressed()){
//         // Serial.println("A2 Pressed");
//         state_1 = false;
//         state_2 = false;
//         state_3 = true;
//         state_4 = false;
//     }
//     if(!button4.isPressed()){
//         // Serial.println("A3 Pressed");
//         state_1 = false;
//         state_2 = false;
//         state_3 = false;
//         state_4 = true;
//     }
}



void loop(void){
  curr_time = millis();
  time_calculator();
  button_process();
  
  // Time and phase
  phase_decision();

  temp_measure();

  // temperature control
 
  if(curr_time - pre_time > 3000){
    pre_time = curr_time;
    display();
  }
}

void temp_control(){
    if((byte)tempF[0] < phase[selected_phase - 1][hour][0]){
        // Blower start
    }
    else{
        // Blower stop
    }

    if((byte)tempF[1] > phase[selected_phase - 1][hour][1]){
        // damper open
    }
    else{
        // damper close
    }
}

void phase_decision(){
    if(curr_time > defined_time){
        pre_selected_phase = selected_phase;
        selected_phase = 0;
        if(flag == true)
            wait_for_next_phase();
        return;
    }
    return;
}

void wait_for_next_phase(){
    unsigned long pre_wait_time = 0;
    unsigned long pre_caution = 0;
    setMillis(0);
    while(selected_phase == 0){
        if(millis() - pre_caution > 2000){
            lcd.clear();
            lcd.print(pre_selected_phase);
            lcd.print(" over.");
            lcd.setCursor(0, 1);
            lcd.print("Waitng for 10sec");
            pre_caution = millis();
        }
        if(millis() > 10000){
            if(pre_selected_phase + 1 > 4){
                lcd.clear();
                lcd.print("All Phase Covered");
                lcd.setCursor(0, 1);
                lcd.print("Please select again.");
                while(1);
            }
            else{
                selected_phase = pre_selected_phase + 1;
                time_definer();
                reset_all_time();
            }
        }
    }
}


void time_definer(){
    if(selected_phase == 1){
        defined_time = 61000;
    }
    else if(selected_phase == 2){
        defined_time = 120000;
    }
    else if(selected_phase == 3){
        defined_time = 240000;
    }
    else if(selected_phase == 4){
        defined_time = 480000;
    }
    else{
        defined_time = 0;
    }
}


// Time calcultaor
void time_calculator(){
    // if(curr_time - pre_h >= 3.6e6){
    //     ++hour;
    //     pre_h = curr_time;
    // }
    // else if(curr_time-pre_m >= 60000){
    //     ++min;
    //     if(min >= 60){
    //         ++hour;
    //         min = 0;
    //     }
    //     pre_m = curr_time;
    // }
    // else 
    if(curr_time - pre_sec >= 1000){
        ++sec;
        if(sec >= 60){
            ++min;
            if(min >= 60){
                ++hour;
                min = 0;
            }
            sec = 0;
        }
        pre_sec = curr_time;
    }
}


// Button process function
void button_process(){
  if(state_1 == true){
        // Serial.println("Yo 1");
        // line[0] = "Yellowing- 4.00H";
        selected_phase = 1;
        // End time
        time_definer();

        reset_all_time();
        display();
        state_1 = false;
        
        flag = true;
    }
     if(state_2 == true){
        // Serial.println("Yo 2");
        // line[0] = "Color fixing- 10.00H";
        
        selected_phase = 2;
        // End time
        time_definer();

        reset_all_time();
        display();

        state_2 = false;

        flag = true;
    }
     if(state_3 == true){
        // Serial.println("Yo 3");
        // line[0] = "Lamina- 12.5H";
        
        selected_phase = 3;
        // End time
        time_definer();

        reset_all_time();
        display();

        state_3 = false;

        flag = true;
    }
    if(state_4 == true){
        // Serial.println("Yo 4");
        // line[0] = "Stem - 10.00 H";
        
        selected_phase = 4;
        // End time
        time_definer();

        reset_all_time();
        display();

        state_4 = false;

        flag = true;
    }
}

// Reset all time
void reset_all_time(){
    curr_time = 0;
    pre_time = 0;
    temp_sense_pre_time = 0;
    hour = 0, min = 0, sec = 0, pre_h = 0, pre_m = 0, pre_sec = 0;
    setMillis(0);
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
        //   Serial.print("Temperature for device: ");
        //   Serial.println(i,DEC);

          // Print the data
          tempC[i] = sensors.getTempC(tempDeviceAddress);
        //   Serial.print("Temp C: ");
        //   Serial.print(tempC[i]);
        //   Serial.print(" Temp F: ");
          tempF[i] = DallasTemperature::toFahrenheit(tempC[i]);
        //   Serial.println(tempF[i]);    //DallasTemperature::toFahrenheit(tempC)   // Converts tempC to Fahrenheit
        } 	
    }

    // Display string making
    for(int i=1;i<3;i++){
      line[i] = "s"+ (String)(i) +":" + (String)tempC[i] + " C  " + (String)tempF[i] + " F";
    }
    // display();
    temp_sense_pre_time = curr_time; 
  }
}

// LCD display
void display(){
    loader_icon_counter++;

    lcd.clear();
    
    //line 1
    lcd.setCursor(0, 0);
    if(selected_phase == 1)
        lcd.print("Yellowing 4.00H");
    else if(selected_phase == 2)
        lcd.print("Color fixing 10.00H");
    else if(selected_phase == 3)
        lcd.print("Lamina 12.30H");
    else if(selected_phase == 4)
        lcd.print("Stem 10.00H");
    else{
        lcd.print("No phase selected");
        return;
    }
    
    // Line 2
    lcd.setCursor(0, 1);
    lcd.print("Dry:");
    lcd.print(phase[selected_phase-1][hour][0]);     //middle hour
    lcd.print("-->");
    lcd.print(tempF[0]);
    lcd.print("F");

    // Line 3
    lcd.setCursor(0, 2);
    lcd.print("Wet:");
    lcd.print(phase[selected_phase-1][hour][1]);     //middle hour
    lcd.print("-->");
    lcd.print(tempF[1]);
    lcd.print("F");

    // Line 4
    lcd.setCursor(0, 3);
    lcd.print("Time: ");
    lcd.print(hour);
    lcd.print(":");
    lcd.print(min);
}


// For reset millis
void setMillis(unsigned long new_millis){
  uint8_t oldSREG = SREG;
  cli();
  timer0_millis = new_millis;
  SREG = oldSREG;
}
