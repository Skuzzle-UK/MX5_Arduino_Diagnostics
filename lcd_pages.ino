enum LcdPages {
  none,
  product_info,
  scanning_codes,
  no_codes_found
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

constexpr float LOADING_DELAY = 100;

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
