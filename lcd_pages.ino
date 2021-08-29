enum LcdPages {
  none,
  product_info,
  scanning_codes
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



const char lcd_scan_array[16][16] {
  ".               ",
  " .              ",
  "  .             ",
  "   .            ",
  "    .           ",
  "     .          ",
  "      .         ",
  "       .        ",
  "        .       ",
  "         .      ",
  "          .     ",
  "           .    ",
  "            .   ",
  "             .  ",
  "              . ",
  "               ."
};

int lcd_scan_position = 0;
float lcd_scan_change_millis = 0;

constexpr float LOADING_DELAY = 500;

void LCD_ScanningCodes() {
  if(lcdpage != scanning_codes) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Scanning codes  ");
    lcdpage = scanning_codes;
  }
  for (int i = 0; i < 15; i++) {
  lcd.setCursor(i,1);
  lcd.print(lcd_scan_array[lcd_scan_position][i]);
  }
  if (lcd_scan_position < 15) {
    if (millis() - lcd_scan_change_millis > LOADING_DELAY) {
      lcd_scan_position++;
      lcd_scan_change_millis = millis();
    }
  }
  else {
    lcd_scan_position = 0;
  }
}
