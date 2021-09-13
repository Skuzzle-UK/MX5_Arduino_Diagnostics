# MX5 Arduino Diagnostics
 A small project to read the blink codes from an 89-95 Mazda MX5 diagnostic port and display them as text on an LCD screen.  
 *Feel free to get involved and test this out or even help to improve it.*
 
 Built on an Arduino Mega 2560 with a 2x16 standard kit LCD.  
 *Schematic now included in pdf. Incorrect naming of diagnostic socket pin FEN shown as FEM in schematic*
 
## Noteworthy files
 
### MX5_Arduino_Diagnostic.ino
 Contains the basic program functions.
 - `void setup()` - *sets up pins, lcd and attaches interrupt*
 - `void loop()` - *main program loop*
 - `void LogCode()` - *logs an error code when found into fault_array*
 - `void FlashPin_Interrupt()` - *called when pin 21 changes state*
 - `void NewFlashState()` - *decides what new state means i.e long flash, short flash etc*
 - `void RebuildFaultArray()` - *rebuilds fault_array*
 - `void UpdateDisplay()` - *updates the LCD display. Called from loop()*

### lcd_pages.ino
 Sets up all of the different display pages of the LCD display.
 - `void LCD_ProductInfo()` - *shows the product information*
 - `void LCD_ScanningCodes()` - *displays a visually pleasing scanning codes page so user has feedback to see scanner is doing something*
 - `void LCD_NoCodesFound()` - *best result for user as displays no fault codes found in vehicle*
 - `void LCD_CodesFound()` - *page that is displayed when fault codes are present*
 - *Contains other functions used by the 4 noted above*

### Schematic PDF.pdf
 *Incorrect naming of diagnostic socket pin FEN shown as FEM in schematic*  
 Circuit schematic to make the hardware the same as I have using an Arduino Mega 2560 and the 1602A LCD screen.  
 Pins B+, GND, TEN and FEN used from the Mazda MX5 under bonnet diagnostic socket.  
 #### Diagnostic socket possible pin meanings
 - B+ : battery positive
 - GND : battery negative
 - TEN : test engine - *drops engine into test mode for setting idle speed, ignition timing etc and prepares ecu for fault code output*
 - FEN : fault engine - *reads out the fault codes via a sequence of going low*
 *Potentiometer is included to fine tune the contrast of the LCD screen, but once a nice resistance value is found that gives the best contrast for chosen LCD display, R1 could be swapped out for a resistor of that value.*
