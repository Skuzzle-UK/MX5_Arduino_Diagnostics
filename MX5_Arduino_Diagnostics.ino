#include <LiquidCrystal.h>

// initialize LCD interface pin with the arduino pin number it is connected to.
constexpr int RS = 12, EN = 11, D4 = 5, D5 = 4, D6 = 3, D7 = 2;
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

constexpr unsigned int FLASHPIN = 8;
constexpr int MAX_NUMBER_OF_CODES = 37;
constexpr float INITIALIZE_TIME = 5000;
constexpr float LONG_FLASH_TIME = 1000;
constexpr float SHORT_FLASH_TIME = 300;
constexpr float DARK_TIME = 1600;
constexpr float SEPERATION_TIME = 4000;
constexpr float TIME_ERROR_ALLOWED = 200;

constexpr int MAX_LENGTH_OF_STRING = 40;

const char FAULT_CODE_LOOKUP_TABLE[MAX_NUMBER_OF_CODES][MAX_LENGTH_OF_STRING] PROGMEM = {
"0",
"1 Ignition pulse",
"2 Ne signal",
"3 G signal",
"4 SGT signal (1.8L)",
"5",
"6",
"7",
"8 Airflow meter",
"9 Water thermistor",
"10 Intake air thermistor in airflow meter",
"11",
"12 Throttle position sensor",
"13",
"14 Atmospheric pressure sensor",
"15 Oxygen sensor(output too low)",
"16 EGR function sensor (1.8L)",
"17 Oxygen sensor(output not changing)",
"18",
"19",
"20",
"21",
"22",
"23",
"24",
"25 PRC solenoid valve (1.8L)",
"26 Solenoid valve(Evaporative canister purge)",
"27 EGR solenoid valve(vacuum) (1.8L)",
"28 EGR solenoid valve(vent) (1.8L)",
"29",
"30",
"31",
"32",
"33",
"34 Idle air control (1.8L)",
"35",
"36 Idle speed control valve"
};

unsigned long last_millis = 0;
unsigned long new_millis = 0;
unsigned int current_code = 0;
unsigned int number_of_codes_present = 0;
unsigned int fault_array[MAX_NUMBER_OF_CODES];
bool codes_present[MAX_NUMBER_OF_CODES];

void setup() {
  pinMode(FLASHPIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(FLASHPIN), FlashPin_Interrupt, CHANGE);
  lcd.begin(16,2);
}

void loop() {
  UpdateDisplay();
}

void FlashPin_Interrupt() {
  //Runs when the state of the pin changes
  new_millis = millis();
  New_Flash_State(digitalRead(FLASHPIN), new_millis - last_millis);
  last_millis = new_millis;
}

// registers what changed state of input pin means 
void New_Flash_State(bool state, float state_time){
  //check for reverse condition as we are looking at the condition after the counted state_time.
  //i.e. when light goes dark we are now looking at the length of time it was light.
  if (state == true){ //code for dark state
    if (fabsf(SEPERATION_TIME - state_time) < TIME_ERROR_ALLOWED){
      //logs fault code and prepares for new one. No point logging or rebuilding fault array if code already confirmed as present.
      if (!codes_present[current_code]) {
        codes_present[current_code] = true;
        RebuildFaultArray();
      }
      current_code = 0;
    }
  }
  else if (state == false){ //code for illuminated state
    if (fabsf(LONG_FLASH_TIME - state_time) < TIME_ERROR_ALLOWED){ //Long flash
      current_code += 10;
    }
    else if (fabsf(SHORT_FLASH_TIME - state_time) < TIME_ERROR_ALLOWED){ //Long flash
      current_code += 1;
    }
  }
}

// rebuilds an array of the current faults by iterating through the codes_present array looking for true.
void RebuildFaultArray() {
  number_of_codes_present = 0;
  for (int i = 0; i < MAX_NUMBER_OF_CODES; i++) {
    if (codes_present[i]){
      number_of_codes_present++;
      fault_array[number_of_codes_present] = i;
    }
  }
}

// updates LCD display to show desired information.
void UpdateDisplay() {
  //@TODO
  if (millis() < 5000) { //boot up display and show product info
    LCD_ProductInfo();
  }
  else { //begin program display
    if(number_of_codes_present == 0 && millis() < 20000){ //start looking for codes
      LCD_ScanningCodes();
    }
    else if (number_of_codes_present == 0 && millis() > 20000) { //give up looking for codes
      LCD_NoCodesFound();
    }
    else if (number_of_codes_present > 0) {
      LCD_CodesPresent();
    }
  }
}
