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
Add the following to your project's CMakeLists.txt.

```
add_subdirectory(LCD_Library)
target_compile_definitions(LCD_Library INTERFACE LCD_8_BIT_MODE) or target_compile_definitions(LCD_Library INTERFACE LCD_4_BIT_MODE)
target_link_libraries(${CMAKE_PROJECT_NAME} LCD_Library)
```

Please note that the library has been tested only with gcc compiler so only gcc compiler is supported.

# How to use the library:
Include lcd.h, call the lcd_init function while providing the amount of lines, the font, and an lcd_driver_type struct to startup the LCD.
The lcd_init function performs "initialization by instructions", as mentioned in the datasheet.

The following is an STM32 example, using 8 bit mode, where the LCD is initialized as 2 lines, 5x8 dot font, using lcd_driver, the display then gets cleared, turned on, the cursor is set to visible but not blinking, the cursor is set to incrementing, the display shift is enabled, and "Hello World." appears
on the LCD.

```
int main(void)
{
    lcd_driver_type lcd_driver =
    {
        us_delay,
        lcd_io_setup,
        lcd_set_e,
        lcd_set_rs,
        lcd_set_rw,
        lcd_get_db0,
        lcd_get_db1,
        lcd_get_db2,
        lcd_get_db3,
        lcd_get_db4,
        lcd_get_db5,
        lcd_get_db6,
        lcd_get_db7,
        lcd_set_db0,
        lcd_set_db1,
        lcd_set_db2,
        lcd_set_db3,
        lcd_set_db4,
        lcd_set_db5,
        lcd_set_db6,
        lcd_set_db7,
        lcd_db0_input,
        lcd_db1_input,
        lcd_db2_input,
        lcd_db3_input,
        lcd_db4_input,
        lcd_db5_input,
        lcd_db6_input,
        lcd_db7_input,
        lcd_db0_output,
        lcd_db1_output,
        lcd_db2_output,
        lcd_db3_output,
        lcd_db4_output,
        lcd_db5_output,
        lcd_db6_output,
        lcd_db7_output
    };

    HAL_Init();
    SystemClock_Config();

    (void)lcd_init(LCD_LINE_2, LCD_DOTS_5x8, &lcd_driver);
    (void)lcd_clear_display();
    (void)lcd_display(true, true, false);
    (void)lcd_increment_cursor(true);
    (void)lcd_print_string("Hello World.");

    while (1)
    {
    }
}
```
