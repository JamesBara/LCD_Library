#ifndef LCD_H
#define LCD_H

#include <stdint.h>
#include <stdbool.h>
#include "YourDefinitions.h"

#ifndef __GNUC__
	#error Unsupported compiler!
#endif

#if !defined(LCD_4_BIT_MODE) && !defined(LCD_8_BIT_MODE)   
	#error "No LCD bit mode is defined."
#endif

#define LCD_TIMEOUT_MAX 5000


__attribute__((always_inline)) static inline void LCD_DB0_OUTPUT(void)
{
	;
}

__attribute__((always_inline)) static inline void LCD_DB1_OUTPUT(void)
{
	;
}

__attribute__((always_inline)) static inline void LCD_DB2_OUTPUT(void)
{
	;
}

__attribute__((always_inline)) static inline void LCD_DB3_OUTPUT(void)
{
	;
}

__attribute__((always_inline)) static inline void LCD_DB4_OUTPUT(void)
{
	;
}

__attribute__((always_inline)) static inline void LCD_DB5_OUTPUT(void)
{
	;
}

__attribute__((always_inline)) static inline void LCD_DB6_OUTPUT(void)
{
	;
}

__attribute__((always_inline)) static inline void LCD_DB7_OUTPUT(void)
{
	;
}

__attribute__((always_inline)) static inline void LCD_DB0_INPUT(void)
{
	;
}

__attribute__((always_inline)) static inline void LCD_DB1_INPUT(void)
{
	;
}
__attribute__((always_inline)) static inline void LCD_DB2_INPUT(void)
{
	;
}
__attribute__((always_inline)) static inline void LCD_DB3_INPUT(void)
{
	;
}
__attribute__((always_inline)) static inline void LCD_DB4_INPUT(void)
{
	;
}
__attribute__((always_inline)) static inline void LCD_DB5_INPUT(void)
{
	;
}
__attribute__((always_inline)) static inline void LCD_DB6_INPUT(void)
{
	;
}
__attribute__((always_inline)) static inline void LCD_DB7_INPUT(void)
{
	;
}

__attribute__((always_inline)) static inline void LCD_SET_DB0(uint32_t bit)
{
	;
}

__attribute__((always_inline)) static inline void LCD_SET_DB1(uint32_t bit)
{
	;
}

__attribute__((always_inline)) static inline void LCD_SET_DB2(uint32_t bit)
{
	;
}

__attribute__((always_inline)) static inline void LCD_SET_DB3(uint32_t bit)
{
	;
}

__attribute__((always_inline)) static inline void LCD_SET_DB4(uint32_t bit)
{
	;
}

__attribute__((always_inline)) static inline void LCD_SET_DB5(uint32_t bit)
{
	;
}

__attribute__((always_inline)) static inline void LCD_SET_DB6(uint32_t bit)
{
	;
}

__attribute__((always_inline)) static inline void LCD_SET_DB7(uint32_t bit)
{
	;
}

__attribute__((always_inline)) static inline uint32_t LCD_GET_DB0(void)
{
	return(0);
}

__attribute__((always_inline)) static inline uint32_t LCD_GET_DB1(void)
{
	return(0);
}

__attribute__((always_inline)) static inline uint32_t LCD_GET_DB2(void)
{
	return(0);
}

__attribute__((always_inline)) static inline uint32_t LCD_GET_DB3(void)
{
	return(0);
}

__attribute__((always_inline)) static inline uint32_t LCD_GET_DB4(void)
{
	return(0);
}

__attribute__((always_inline)) static inline uint32_t LCD_GET_DB5(void)
{
	return(0);
}

__attribute__((always_inline)) static inline uint32_t LCD_GET_DB6(void)
{
	return(0);
}

__attribute__((always_inline)) static inline uint32_t LCD_GET_DB7(void)
{
	return(0);
}


__attribute__((always_inline)) static inline void LCD_SET_RS(uint32_t bit)
{
	;
}

__attribute__((always_inline)) static inline void LCD_SET_RW(uint32_t bit)
{
	;
}

__attribute__((always_inline)) static inline void LCD_SET_E(uint32_t bit)
{
	;
}


static inline void LCD_SETUP(void)
{
	;
}

typedef enum
{
	LCD_OK,
	LCD_ERROR,
	LCD_TIMEOUT
}lcd_return_type;

lcd_return_type lcd_init(uint8_t number_of_lines, uint8_t number_of_dots, void (*ms_delay)(uint32_t max));
lcd_return_type lcd_clear_display(void);
lcd_return_type lcd_return_home(void);
lcd_return_type lcd_increment_cursor(bool shift_display);
lcd_return_type lcd_decrement_cursor(bool shift_display);
lcd_return_type lcd_display(bool display_on, bool cursor_on, bool blink_on);
lcd_return_type lcd_shift_display_right(void);
lcd_return_type lcd_shift_display_left(void);
lcd_return_type lcd_shift_cursor_right(void);
lcd_return_type lcd_shift_cursor_left(void);
lcd_return_type lcd_set_cursor_position(uint8_t line, uint8_t position);
lcd_return_type lcd_get_cursor_position(uint8_t *line, uint8_t *position);
lcd_return_type lcd_create_custom_char(uint8_t number_of_dots, uint8_t position, uint8_t* data);
lcd_return_type lcd_print_char(char ch);
lcd_return_type lcd_print_string(char *ch);

#endif /*LCD_H*/
