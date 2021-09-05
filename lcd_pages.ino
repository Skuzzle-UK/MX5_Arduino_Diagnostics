enum LcdPages {
  none,
  product_info,
  scanning_codes,
  no_codes_found,
  codes_found
};

LcdPages lcdpage = none;

void LCD_ProductInfo() {
  if(lcdpage != product_info)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Skuzzle MX5 Diag");
    lcd.setCursor(0,1);
    lcd.print("Ver: 0.1        ");
    lcdpage = product_info;
  }
}

constexpr float LOADING_DELAY = 100;
constexpr int LCD_SCAN_ARRAY_SIZE = 19;
const char lcd_scan_array[LCD_SCAN_ARRAY_SIZE][MAX_LENGTH_OF_STRING] PROGMEM = {
  "#               ",
  "##              ",
  "###             ",
  "####            ",
  " ####           ",
  "  ####          ",
  "   ####         ",
  "    ####        ",
  "     ####       ",
  "      ####      ",
  "       ####     ",
  "        ####    ",
  "         ####   ",
  "          ####  ",
  "           #### ",
  "            ####",
  "             ###",
  "              ##",
  "               #"
};

int lcd_scan_position = 0;
float lcd_scan_change_millis = 0;
void LCD_ScanningCodes() {
  if(lcdpage != scanning_codes) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Scanning codes  ");
    lcdpage = scanning_codes;
  }
  for (int i = 0; i < MAX_LENGTH_OF_STRING; i++) {
    lcd.setCursor(i,1);
    lcd.print((char)pgm_read_word(&(lcd_scan_array[lcd_scan_position][i])));
  }
  if (lcd_scan_position < LCD_SCAN_ARRAY_SIZE) {
    if (millis() - lcd_scan_change_millis > LOADING_DELAY) {
      lcd_scan_position++;
      lcd_scan_change_millis = millis();
    }
  }
  else {
    lcd_scan_position = 0;
  }
}

void LCD_NoCodesFound(){
  if(lcdpage != no_codes_found) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("*No fault found*");
    lcdpage = no_codes_found;
  }
}

constexpr float SCROLL_CHARACTER_DELAY = 200;

float character_display_timer = 0;
int displayed_code = 0;
int displayed_character = 0;
bool end_scroll_flag = false;
char code_string[MAX_LENGTH_OF_STRING];
char code_display_string[16] = {"                "};

void LCD_CodesFound() {
  if(lcdpage != codes_found) {
    lcd.clear();
    lcdpage = codes_found;
  }

  if (displayed_code == 0) { //First loop through to set the start time of the loop
    DisplayCode(displayed_code + 1);
  }

  if (end_scroll_flag) { //Advance to next present code
    DisplayCode(displayed_code + 1);
  }

  if (displayed_code > number_of_codes_present) { //Restart loop if showing last code
    DisplayCode(1);
  }
  
  lcd.setCursor(0,0);
  lcd.print("code "); lcd.print(displayed_code); lcd.print("/"); lcd.print(number_of_codes_present - 1); lcd.print(":      ");

  if (millis() - character_display_timer > SCROLL_CHARACTER_DELAY) {
    //advances code_display_string to show code_string[] constantly scrolling
    if (displayed_character > MAX_LENGTH_OF_STRING - 16) { //reset scroll to first character
      displayed_character = 0;
    }
    else {
      for (int i = 0; i < 16; i++) {
        code_display_string[i] = code_string[displayed_character + i];
      }
    }
  }
  
  lcd.setCursor(0,1);
  lcd.print(code_display_string);
  displayed_character++;
}

void DisplayCode(int i) { //Changes the displayed code for the codes_found page
  character_display_timer = millis();
  displayed_code = i;
  displayed_character = 0;
  GetCodeString(i);
  for (int k = 0; k < 16; k++) {
    code_display_string[k] = code_string[k];
  }
}

void GetCodeString(int i) { //gets full string of the current code to be displayed
  int faultcode = fault_array[i]; //@TODO issue in fault_array ?
  for (int j = 0; j < MAX_LENGTH_OF_STRING; j++) {
    code_string[j] = (char)pgm_read_word(&(FAULT_CODE_LOOKUP_TABLE[faultcode][j]));
  }
}
