# A simple portable library for 16x2 LCD HD44780U and clones.

Repo tree:
```
LCD_Library
├───inc
│    ├───lcd.h
│    └───lcd_user_defines.h
├───src
│    └───lcd.c
├───CMakeLists.txt
├───LICENSE.txt
└───README.md
```
# How to start with the library:
Decide whether to use 4 data lines or 8 data lines to interface your mcu with LCD after deciding, use the CMakeLists.txt file of the core project add either LCD_8_BIT_MODE or LCD_4_BIT_MODE in the definitions, depending on the amount of data lines. Then start the process of porting the microcontroller gpio definitions to use with the library.

# How to port the library:
To port the library populate the lcd_user_defines.h file with the selected microcontroller hardware definitions.

# STM32L4xx Porting example:

Modify the include directive.
```
#include "stm32l4xx.h"
```

Modify the pinout (ex. 4 bit mode).
```
/*LCD Screen Pinout mapping*/
/*Port Mapping*/
#define LCD_RS_PORT GPIOA
#define LCD_RW_PORT GPIOA
#define LCD_E_PORT GPIOA
#define LCD_DB0_PORT 
#define LCD_DB1_PORT 
#define LCD_DB2_PORT
#define LCD_DB3_PORT 
#define LCD_DB4_PORT GPIOA
#define LCD_DB5_PORT GPIOA
#define LCD_DB6_PORT GPIOA
#define LCD_DB7_PORT GPIOA
/*Pin Mapping*/
#define LCD_DB0_PIN
#define LCD_DB1_PIN
#define LCD_DB2_PIN
#define LCD_DB3_PIN
#define LCD_DB4_PIN 4
#define LCD_DB5_PIN 5
#define LCD_DB6_PIN 6
#define LCD_DB7_PIN 7
#define LCD_RS_PIN 8
#define LCD_RW_PIN 9
#define LCD_E_PIN 10
```

Modify the hardware definitions (ex. 4 bit mode).

```
/*This definition is not part of the library it's only added to reduce the magic numbers.*/
#define GPIO_SET_ODR(port, pin, bit) do \
{ \
	if (!(bit)) \
	{	\
		((port)->BRR = (0x1UL << (pin))); \
	} \
	else \
	{ \
		((port)->BSRR = (0x1UL << (pin))); \
	} \
}while(0)

    /*Modify DB4-DB7 to act as inputs.*/
    #define LCD_SET_DB_INPUT() do \
    { \
        LCD_DB4_PORT->MODER &= ~(0x3UL << (LCD_DB4_PIN << 1)); \
        LCD_DB5_PORT->MODER &= ~(0x3UL << (LCD_DB5_PIN << 1)); \
        LCD_DB6_PORT->MODER &= ~(0x3UL << (LCD_DB6_PIN << 1)); \
        LCD_DB7_PORT->MODER &= ~(0x3UL << (LCD_DB7_PIN << 1)); \
    }while(0)
    /*Modify DB4-DB7 to act as ouputs.*/
    #define LCD_SET_DB_OUTPUT() do \
    { \
        LCD_DB4_PORT->MODER |= 0x1UL << (LCD_DB4_PIN << 1); \
        LCD_DB5_PORT->MODER |= 0x1UL << (LCD_DB5_PIN << 1); \
        LCD_DB6_PORT->MODER |= 0x1UL << (LCD_DB6_PIN << 1); \
        LCD_DB7_PORT->MODER |= 0x1UL << (LCD_DB7_PIN << 1); \
    }while(0)
    /*Set DB4-DB7 high or low depending on each data bit (use high byte). */
    #define LCD_DB_WRITE(data) do \
    { \
        GPIO_SET_ODR(LCD_DB4_PORT, LCD_DB4_PIN, (1 << LCD_DB4_PIN)); \
        GPIO_SET_ODR(LCD_DB5_PORT, LCD_DB5_PIN, (1 << LCD_DB5_PIN)); \
        GPIO_SET_ODR(LCD_DB6_PORT, LCD_DB6_PIN, (1 << LCD_DB6_PIN)); \
        GPIO_SET_ODR(LCD_DB7_PORT, LCD_DB7_PIN, (1 << LCD_DB7_PIN)); \
    }while(0)
    /*Read whether DB4-DB7 high or low depending on each data bit (use high byte).*/
    #define LCD_DB_READ(data) do \
    { \
        data = (!(LCD_DB4_PORT->IDR & (0x1UL << LCD_DB4_PIN))) ? (0 << 4) : (1 << 4); \
        data |= (!(LCD_DB5_PORT->IDR & (0x1UL << LCD_DB5_PIN))) ? (0 << 5) : (1 << 5); \
        data |= (!(LCD_DB6_PORT->IDR & (0x1UL << LCD_DB6_PIN))) ? (0 << 6) : (1 << 6); \
        data |= (!(LCD_DB7_PORT->IDR & (0x1UL << LCD_DB7_PIN))) ? (0 << 7) : (1 << 7); \
    }while(0)
    /*Initialize all the hardware pinout for 4 bit mode of the lcd.*/
    #define LCD_HARDWARE_INIT() do \
    { \
      LCD_DB4_PORT->MODER &= ~(0x3UL << (LCD_DB4_PIN << 1)); \
      LCD_DB5_PORT->MODER &= ~(0x3UL << (LCD_DB5_PIN << 1)); \
      LCD_DB6_PORT->MODER &= ~(0x3UL << (LCD_DB6_PIN << 1)); \
      LCD_DB7_PORT->MODER &= ~(0x3UL << (LCD_DB7_PIN << 1)); \
      LCD_RS_PORT->MODER &= ~(0x3UL << (LCD_RS_PIN << 1)); \
      LCD_RW_PORT->MODER &= ~(0x3UL << (LCD_RW_PIN << 1)); \
      LCD_E_PORT->MODER &= ~(0x3UL << (LCD_E_PIN << 1)); \
      LCD_DB4_PORT->OSPEEDR &= ~(0x3UL << (LCD_DB4_PIN << 1)); \
      LCD_DB5_PORT->OSPEEDR &= ~(0x3UL << (LCD_DB5_PIN << 1)); \
      LCD_DB6_PORT->OSPEEDR &= ~(0x3UL << (LCD_DB6_PIN << 1)); \
      LCD_DB7_PORT->OSPEEDR &= ~(0x3UL << (LCD_DB7_PIN << 1)); \
      LCD_RS_PORT->OSPEEDR &= ~(0x3UL << (LCD_RS_PIN << 1)); \
      LCD_RW_PORT->OSPEEDR &= ~(0x3UL << (LCD_RW_PIN << 1)); \
      LCD_E_PORT->OSPEEDR &= ~(0x3UL << (LCD_E_PIN << 1)); \
      LCD_DB4_PORT->PUPDR &= ~(0x3UL << (LCD_DB4_PIN << 1)); \
      LCD_DB5_PORT->PUPDR &= ~(0x3UL << (LCD_DB5_PIN << 1)); \
      LCD_DB6_PORT->PUPDR &= ~(0x3UL << (LCD_DB6_PIN << 1)); \
      LCD_DB7_PORT->PUPDR &= ~(0x3UL << (LCD_DB7_PIN << 1)); \
      LCD_RS_PORT->PUPDR &= ~(0x3UL << (LCD_RS_PIN << 1)); \
      LCD_RW_PORT->PUPDR &= ~(0x3UL << (LCD_RW_PIN << 1)); \
      LCD_E_PORT->PUPDR &= ~(0x3UL << (LCD_E_PIN << 1)); \
      LCD_DB4_PORT->BRR = (0x1UL << LCD_DB4_PIN); \
      LCD_DB5_PORT->BRR = (0x1UL << LCD_DB5_PIN); \
      LCD_DB6_PORT->BRR = (0x1UL << LCD_DB6_PIN); \
      LCD_DB7_PORT->BRR = (0x1UL << LCD_DB7_PIN); \
      LCD_RS_PORT->BRR = (0x1UL << LCD_RS_PIN); \
      LCD_RW_PORT->BRR = (0x1UL << LCD_RW_PIN); \
      LCD_E_PORT->BRR = (0x1UL << LCD_E_PIN); \
    }while(0)
```
# How to use the library:
Include the lcd.h header file to your project and call the lcd_init function to startup the LCD. 
In the following example the screen is initialized for 2 lines, 5x8 dot character font and uses the HAL_Delay function for the internal timeouts.
If the lcd_init function doesn't return an LCD_OK result the execution stops at the breakpoint.

```
#include "lcd.h"
int main(void)
{
	if (lcd_init(2, 8, HAL_Delay) != LCD_OK)
	{
	   __BKPT(0);
	}

	while(1)
	{
	}
}

```

In the following example after the lcd initialization the display is turned on, the cursor is not turned on, the cursor is moved to line 2, position 5, 
and the string "Hello World" is written in the display. If any of the functions don't return an LCD_OK result the execution stops at the breakpoint.

```
#include "lcd.h"
int main(void)
{
	if (lcd_init(2, 8, HAL_Delay) != LCD_OK)
	{
	   __BKPT(0);
	}
	if (lcd_display(true, false, false) != LCD_OK)
	{
	   __BKPT(0);
	}
	if (lcd_set_cursor_position(2, 5) != LCD_OK)
	{
	   __BKPT(0);
	}
	if (lcd_print_string("Hello World") != LCD_OK)
	{
	   __BKPT(0);
	}
	while(1)
	{
	}
}

```
