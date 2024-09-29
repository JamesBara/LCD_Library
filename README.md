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
Define either LCD_8_BIT_MODE or LCD_4_BIT_MODE, depending on the amount of data lines you are planning to use. This library is compatible only with gcc.

# How to port the library:
## This part of the readme has been modified and is @todo. 



To port the library modify the appropriate hardware callback functions of the lcd.h file.

STM32 example for GPIOA pin 0 set as the LCD E pin:

```

__attribute__((always_inline)) static inline void LCD_SET_E(uint32_t bit)
{
	GPIOA->BSRR = (bit & 0x1U) ? GPIO_BSRR_BS0 : GPIO_BSRR_BR0;
}


static inline void LCD_SETUP(void)
{
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    while (!(RCC->AHB2ENR & RCC_AHB2ENR_GPIOAEN));
	GPIOA->MODER &= ~GPIO_MODER_MODE0;
	GPIOA->MODER |= GPIO_MODER_MODE0_0;
}
```


# How to use the library:
Include lcd.h, call the lcd_init function while providing the amount of lines, the font, and a millisecond delay callback function to startup the LCD.
The lcd_init function performs "initialization by instructions", as shown in the datasheet. 

The following is an STM32 example, where the LCD is initialized as 1 line, 5x8 dot font, using HAL_Delay as millisecond delay callback function, 
the display is turned on, and the cursor is visible but not blinking, the cursor is incrementing, the display shift is enabled, and "Hello World." appears
on the LCD:

```

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    (void)lcd_init(1, 8, HAL_Delay);
	(void)lcd_display(true, true, false);
	(void)lcd_increment_cursor(true);
	(void)lcd_clear_display();
	(void)lcd_print_string("Hello World.");

	while (1)
	{

	}

}
```