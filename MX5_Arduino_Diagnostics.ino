#include <LiquidCrystal.h>

// initialize LCD interface pin with the arduino pin number it is connected to.
constexpr int RS = 12, EN = 11, D4 = 5, D5 = 4, D6 = 3, D7 = 2;
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

constexpr unsigned int FLASHPIN = 21;
constexpr int MAX_NUMBER_OF_CODES = 37;
constexpr float INITIALIZE_TIME = 5000;
constexpr float LONG_FLASH_TIME = 1000;
constexpr float SHORT_FLASH_TIME = 300;
constexpr float DARK_TIME = 1600;
constexpr float SEPERATION_TIME = 4000;
constexpr float TIME_ERROR_ALLOWED = 200;

constexpr int MAX_LENGTH_OF_STRING = 45;

const char FAULT_CODE_LOOKUP_TABLE[MAX_NUMBER_OF_CODES][MAX_LENGTH_OF_STRING] PROGMEM = {
"0 Unknown Fault       0 Unknown Fault        ",
"1 Ignition pulse      1 Ignition pulse       ",
"2 Ne signal     2 Ne signal      2 Ne signal ",
"3 G signal    3 G signal    3 G signal       ",
"4 SGT signal (1.8L)     4 SGT signal (1.8L)  ",
"5 Unknown Fault       5 Unknown Fault        ",
"6 Unknown Fault       6 Unknown Fault        ",
"7 Unknown Fault       7 Unknown Fault        ",
"8 Airflow meter          8 Airflow meter     ",
"9 Water thermistor      9 Water thermistor   ",
"10 Intake air thermistor in airflow meter    ",
"11 Unknown Fault      11 Unknown Fault       ",
"12 Throttle position sensor                  ",
"13 Unknown Fault      13 Unknown Fault       ",
"14 Atmospheric pressure sensor               ",
"15 Oxygen sensor(output too low)             ",
"16 EGR function sensor (1.8L)                ",
"17 Oxygen sensor(output not changing)        ",
"18 Unknown Fault      18 Unknown Fault       ",
"19 Unknown Fault      19 Unknown Fault       ",
"20 Unknown Fault      20 Unknown Fault       ",
"21 Unknown Fault      21 Unknown Fault       ",
"22 Unknown Fault      22 Unknown Fault       ",
"23 Unknown Fault      23 Unknown Fault       ",
"24 Unknown Fault      24 Unknown Fault       ",
"25 PRC solenoid valve (1.8L)                 ",
"26 Solenoid valve(Evaporative canister purge)",
"27 EGR solenoid valve(vacuum) (1.8L)         ",
"28 EGR solenoid valve(vent) (1.8L)           ",
"29 Unknown Fault      29 Unknown Fault       ",
"30 Unknown Fault      30 Unknown Fault       ",
"31 Unknown Fault      31 Unknown Fault       ",
"32 Unknown Fault      32 Unknown Fault       ",
"33 Unknown Fault      33 Unknown Fault       ",
"34 Idle air control     34 Idle air control  ",
"35 Unknown Fault      35 Unknown Fault       ",
"36 Idle speed control valve                  "
};

void setup() {
  Serial.begin(9600);
  pinMode(FLASHPIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(FLASHPIN), FlashPin_Interrupt, CHANGE);
  lcd.begin(16,2);
}

bool dark_state = false;
float time_went_dark = 0;
void loop() {
  UpdateDisplay();
  if(dark_state) {
    if(millis() - time_went_dark > DARK_TIME) {
      LogCode();
    }
  }
}

unsigned int number_of_codes_present = 0; //Always shows 1 more than is actually true
unsigned int fault_array[MAX_NUMBER_OF_CODES]; //fault_array[0] is always true and null
unsigned int current_code = 0;
bool codes_present[MAX_NUMBER_OF_CODES];
void LogCode(){
  if (!codes_present[current_code]) {
    codes_present[current_code] = true;
    RebuildFaultArray();
  }
  current_code = 0;
}

unsigned long last_millis = 0;
unsigned long new_millis = 0;
static void FlashPin_Interrupt() { //runs when the state of the pin changes
  new_millis = millis();
  New_Flash_State(digitalRead(FLASHPIN), new_millis - last_millis);
  last_millis = new_millis;
  Serial.print("Current code: "); //Left in code so that you can watch the code count up in serial monitor
  Serial.print(current_code);
  Serial.print("\n");
}


static void New_Flash_State(bool state, float state_time){ //registers what changed state of input pin means
  //check for reverse condition as we are looking at the condition after the counted state_time.
  //i.e. when light goes dark we are now looking at the length of time it was light.
  if (state == true){ //code for dark state
    dark_state = true;
    time_went_dark = millis();
  }
  else if (state == false){ //code for illuminated state
    dark_state = false;
    time_went_dark = millis();
    if (fabsf(LONG_FLASH_TIME - state_time) < TIME_ERROR_ALLOWED){ //Long flash
      current_code += 10;
    }
    else if (fabsf(SHORT_FLASH_TIME - state_time) < TIME_ERROR_ALLOWED){ //Short flash
      current_code += 1;
    }
  }
}


void RebuildFaultArray() { //rebuilds an array of the current faults by iterating through the codes_present array looking for true.
  number_of_codes_present = 0;
  for (int i = 0; i < MAX_NUMBER_OF_CODES; i++) {
    if (codes_present[i] == true){
      fault_array[number_of_codes_present] = i;
      number_of_codes_present++;
    }
  }
}

void UpdateDisplay() { //updates LCD display to show desired information.
  codes_present[0] = true;
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
    else if (number_of_codes_present > 0) { //display found codes
      LCD_CodesFound();
    }
  }
}
