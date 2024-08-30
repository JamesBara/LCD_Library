# A simple portable library for 16x2 LCD HD44780U.

Repo tree:
```
LCD_Library
├───inc
│    └───lcd.h
├───src
│    └───lcd.c
├───CMakeLists.txt
├───LICENSE.txt
└───README.md
```

# How to start with the library:
Decide whether to use 4 data lines or 8 data lines to interface your mcu with LCD after deciding, use the CMakeLists.txt file of the core project add either LCD_8_BIT_MODE or LCD_4_BIT_MODE in the definitions, depending on the amount of data lines. Then start the process of porting the microcontroller gpio definitions to use with the library.

# How to port the library:
To port the library modify the appropriate hardware callback functions of the lcd.h file with the selected microcontroller hardware definitions.

# How to use the library:
Include the lcd.h header file to your project and call the lcd_init function while providing the amount of lines, the font, and a millisecond delay callback function to startup the LCD.
The lcd_init function performs initialization by instructions, as shown in the datasheet.