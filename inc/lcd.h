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

/**
 * @brief Set the DB0 pin to act as an output.
 * @param  
 */
__attribute__((always_inline)) static inline void LCD_DB0_OUTPUT(void)
{
	;
}

/**
 * @brief Set the DB1 pin to act as an output.
 * @param
 */
__attribute__((always_inline)) static inline void LCD_DB1_OUTPUT(void)
{
	;
}

/**
 * @brief Set the DB2 pin to act as an output.
 * @param
 */
__attribute__((always_inline)) static inline void LCD_DB2_OUTPUT(void)
{
	;
}

/**
 * @brief Set the DB3 pin to act as an output.
 * @param
 */
__attribute__((always_inline)) static inline void LCD_DB3_OUTPUT(void)
{
	;
}

/**
 * @brief Set the DB4 pin to act as an output.
 * @param
 */
__attribute__((always_inline)) static inline void LCD_DB4_OUTPUT(void)
{
	;
}

/**
 * @brief Set the DB5 pin to act as an output.
 * @param
 */
__attribute__((always_inline)) static inline void LCD_DB5_OUTPUT(void)
{
	;
}

/**
 * @brief Set the DB6 pin to act as an output.
 * @param
 */
__attribute__((always_inline)) static inline void LCD_DB6_OUTPUT(void)
{
	;
}

/**
 * @brief Set the DB7 pin to act as an output.
 * @param
 */
__attribute__((always_inline)) static inline void LCD_DB7_OUTPUT(void)
{
	;
}

/**
 * @brief Set the DB0 pin to act as an input.
 * @param
 */
__attribute__((always_inline)) static inline void LCD_DB0_INPUT(void)
{
	;
}

/**
 * @brief Set the DB1 pin to act as an input.
 * @param
 */
__attribute__((always_inline)) static inline void LCD_DB1_INPUT(void)
{
	;
}

/**
 * @brief Set the DB2 pin to act as an input.
 * @param
 */
__attribute__((always_inline)) static inline void LCD_DB2_INPUT(void)
{
	;
}

/**
 * @brief Set the DB3 pin to act as an input.
 * @param
 */
__attribute__((always_inline)) static inline void LCD_DB3_INPUT(void)
{
	;
}

/**
 * @brief Set the DB4 pin to act as an input.
 * @param
 */
__attribute__((always_inline)) static inline void LCD_DB4_INPUT(void)
{
	;
}

/**
 * @brief Set the DB5 pin to act as an input.
 * @param
 */
__attribute__((always_inline)) static inline void LCD_DB5_INPUT(void)
{
	;
}

/**
 * @brief Set the DB6 pin to act as an input.
 * @param
 */
__attribute__((always_inline)) static inline void LCD_DB6_INPUT(void)
{
	;
}

/**
 * @brief Set the DB7 pin to act as an input.
 * @param
 */
__attribute__((always_inline)) static inline void LCD_DB7_INPUT(void)
{
	;
}

/**
 * @brief Set the DB0 pin ouput to HIGH or LOW.
 * @param bit 1 for HIGH or 0 for LOW.
 */
__attribute__((always_inline)) static inline void LCD_SET_DB0(uint32_t bit)
{
	;
}

/**
 * @brief Set the DB1 pin ouput to HIGH or LOW.
 * @param bit 1 for HIGH or 0 for LOW.
 */
__attribute__((always_inline)) static inline void LCD_SET_DB1(uint32_t bit)
{
	;
}

/**
 * @brief Set the DB2 pin ouput to HIGH or LOW.
 * @param bit 1 for HIGH or 0 for LOW.
 */
__attribute__((always_inline)) static inline void LCD_SET_DB2(uint32_t bit)
{
	;
}

/**
 * @brief Set the DB3 pin ouput to HIGH or LOW.
 * @param bit 1 for HIGH or 0 for LOW.
 */
__attribute__((always_inline)) static inline void LCD_SET_DB3(uint32_t bit)
{
	;
}

/**
 * @brief Set the DB4 pin ouput to HIGH or LOW.
 * @param bit 1 for HIGH or 0 for LOW.
 */
__attribute__((always_inline)) static inline void LCD_SET_DB4(uint32_t bit)
{
	;
}

/**
 * @brief Set the DB5 pin ouput to HIGH or LOW.
 * @param bit 1 for HIGH or 0 for LOW.
 */
__attribute__((always_inline)) static inline void LCD_SET_DB5(uint32_t bit)
{
	;
}

/**
 * @brief Set the DB6 pin ouput to HIGH or LOW.
 * @param bit 1 for HIGH or 0 for LOW.
 */
__attribute__((always_inline)) static inline void LCD_SET_DB6(uint32_t bit)
{
	;
}

/**
 * @brief Set the DB7 pin ouput to HIGH or LOW.
 * @param bit 1 for HIGH or 0 for LOW.
 */
__attribute__((always_inline)) static inline void LCD_SET_DB7(uint32_t bit)
{
	;
}

/**
 * @brief Get the DB0 input value.
 * @param  
 * @return 1 for HIGH or 0 for LOW.
 */
__attribute__((always_inline)) static inline uint32_t LCD_GET_DB0(void)
{
	return(0);
}

/**
 * @brief Get the DB1 input value.
 * @param
 * @return 1 for HIGH or 0 for LOW.
 */
__attribute__((always_inline)) static inline uint32_t LCD_GET_DB1(void)
{
	return(0);
}

/**
 * @brief Get the DB2 input value.
 * @param
 * @return 1 for HIGH or 0 for LOW.
 */
__attribute__((always_inline)) static inline uint32_t LCD_GET_DB2(void)
{
	return(0);
}

/**
 * @brief Get the DB3 input value.
 * @param
 * @return 1 for HIGH or 0 for LOW.
 */
__attribute__((always_inline)) static inline uint32_t LCD_GET_DB3(void)
{
	return(0);
}

/**
 * @brief Get the DB4 input value.
 * @param
 * @return 1 for HIGH or 0 for LOW.
 */
__attribute__((always_inline)) static inline uint32_t LCD_GET_DB4(void)
{
	return(0);
}

/**
 * @brief Get the DB5 input value.
 * @param
 * @return 1 for HIGH or 0 for LOW.
 */
__attribute__((always_inline)) static inline uint32_t LCD_GET_DB5(void)
{
	return(0);
}

/**
 * @brief Get the DB6 input value.
 * @param
 * @return 1 for HIGH or 0 for LOW.
 */
__attribute__((always_inline)) static inline uint32_t LCD_GET_DB6(void)
{
	return(0);
}

/**
 * @brief Get the DB7 input value.
 * @param
 * @return 1 for HIGH or 0 for LOW.
 */
__attribute__((always_inline)) static inline uint32_t LCD_GET_DB7(void)
{
	return(0);
}

/**
 * @brief Set the RS pin ouput to HIGH or LOW.
 * @param bit 1 for HIGH or 0 for LOW.
 */
__attribute__((always_inline)) static inline void LCD_SET_RS(uint32_t bit)
{
	;
}

/**
 * @brief Set the RW pin ouput to HIGH or LOW.
 * @param bit 1 for HIGH or 0 for LOW.
 */
__attribute__((always_inline)) static inline void LCD_SET_RW(uint32_t bit)
{
	;
}

/**
 * @brief Set the E pin ouput to HIGH or LOW.
 * @param bit 1 for HIGH or 0 for LOW.
 */
__attribute__((always_inline)) static inline void LCD_SET_E(uint32_t bit)
{
	;
}

/**
 * @brief Setup all the I/O peripheral clocks and all the pins DB0-7 (or DB4-7), RS, RW, E as outputs.
 * @param  
 */
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
