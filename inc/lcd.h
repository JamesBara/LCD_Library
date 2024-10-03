#ifndef LCD_H
#define LCD_H

#include <stdint.h>
#include <stdbool.h>

#ifndef __GNUC__
	#error Unsupported compiler!
#endif

#if !defined(LCD_4_BIT_MODE) && !defined(LCD_8_BIT_MODE)   
	#error "No LCD bit mode is defined."
#endif

typedef enum _lcd_dots_type
{
	LCD_DOTS_5x8 = 0,
	LCD_DOTS_5x10 = 1
}lcd_dots_type;

typedef enum _lcd_line_type
{
	LCD_LINE_1 = 0,
	LCD_LINE_2 = 1
}lcd_line_type;


typedef enum _lcd_return_type
{
	LCD_OK,
	LCD_ERROR,
	LCD_TIMEOUT
}lcd_return_type;

typedef enum _lcd_pin_state_type
{
	LCD_PIN_LOW = 0,
	LCD_PIN_HIGH = 1
}lcd_pin_state_type;


typedef struct _lcd_driver_type
{
	void (*us_delay)(uint32_t max); /*!< Microsecond delay callback.*/
	void (*io_setup)(void); /*!< Initialization of I/O peripheral clocks, all pins selected to act as outputs callback.*/
	void (*set_e)(lcd_pin_state_type bit); /*!< Select E pin state callback .*/
	void (*set_rs)(lcd_pin_state_type bit); /*!< Select E pin state callback.*/
	void (*set_rw)(lcd_pin_state_type bit); /*!< Select E pin state callback.*/
#if defined(LCD_8_BIT_MODE)
	lcd_pin_state_type(*get_db0)(void); /*!< Get DB0 pin state callback.*/
	lcd_pin_state_type(*get_db1)(void); /*!< Get DB1 pin state callback.*/
	lcd_pin_state_type(*get_db2)(void); /*!< Get DB2 pin state callback.*/
	lcd_pin_state_type(*get_db3)(void); /*!< Get DB3 pin state callback.*/
#endif
	lcd_pin_state_type(*get_db4)(void); /*!< Get DB4 pin state callback.*/
	lcd_pin_state_type(*get_db5)(void); /*!< Get DB5 pin state callback.*/
	lcd_pin_state_type(*get_db6)(void); /*!< Get DB6 pin state callback.*/
	lcd_pin_state_type(*get_db7)(void); /*!< Get DB7 pin state callback.*/
#if defined(LCD_8_BIT_MODE)
	void (*set_db0)(lcd_pin_state_type bit); /*!< Select DB0 pin state callback.*/
	void (*set_db1)(lcd_pin_state_type bit); /*!< Select DB1 pin state callback.*/
	void (*set_db2)(lcd_pin_state_type bit); /*!< Select DB2 pin state callback.*/
	void (*set_db3)(lcd_pin_state_type bit); /*!< Select DB3 pin state callback.*/
#endif
	void (*set_db4)(lcd_pin_state_type bit); /*!< Select DB4 pin state callback.*/
	void (*set_db5)(lcd_pin_state_type bit); /*!< Select DB5 pin state callback.*/
	void (*set_db6)(lcd_pin_state_type bit); /*!< Select DB6 pin state callback.*/
	void (*set_db7)(lcd_pin_state_type bit); /*!< Select DB7 pin state callback.*/
#if defined(LCD_8_BIT_MODE)
	void (*db0_input)(void); /*!< Select DB0 pin to act as input callback.*/
	void (*db1_input)(void); /*!< Select DB1 pin to act as input callback.*/
	void (*db2_input)(void); /*!< Select DB2 pin to act as input callback.*/
	void (*db3_input)(void); /*!< Select DB3 pin to act as input callback.*/
#endif
	void (*db4_input)(void); /*!< Select DB4 pin to act as input callback.*/
	void (*db5_input)(void); /*!< Select DB5 pin to act as input callback.*/
	void (*db6_input)(void); /*!< Select DB6 pin to act as input callback.*/
	void (*db7_input)(void); /*!< Select DB7 pin to act as input callback.*/
#if defined(LCD_8_BIT_MODE)
	void (*db0_output)(void); /*!< Select DB0 pin to act as output callback.*/
	void (*db1_output)(void); /*!< Select DB1 pin to act as output callback.*/
	void (*db2_output)(void); /*!< Select DB2 pin to act as output callback.*/
	void (*db3_output)(void); /*!< Select DB3 pin to act as output callback.*/
#endif
	void (*db4_output)(void); /*!< Select DB4 pin to act as output callback.*/
	void (*db5_output)(void); /*!< Select DB5 pin to act as output callback.*/
	void (*db6_output)(void); /*!< Select DB6 pin to act as output callback.*/
	void (*db7_output)(void); /*!< Select DB7 pin to act as output callback.*/
}lcd_driver_type;


lcd_return_type lcd_init(lcd_line_type lines, lcd_dots_type dots, lcd_driver_type *const driver);
lcd_return_type lcd_clear_display(void);
lcd_return_type lcd_return_home(void);
lcd_return_type lcd_increment_cursor(bool shift_display);
lcd_return_type lcd_decrement_cursor(bool shift_display);
lcd_return_type lcd_display(bool display_on, bool cursor_on, bool blink_on);
lcd_return_type lcd_shift_display_right(void);
lcd_return_type lcd_shift_display_left(void);
lcd_return_type lcd_shift_cursor_right(void);
lcd_return_type lcd_shift_cursor_left(void);
lcd_return_type lcd_set_cursor_position(lcd_line_type line, uint8_t position);
lcd_return_type lcd_get_cursor_position(lcd_line_type *line, uint8_t *position);
lcd_return_type lcd_create_custom_char(lcd_dots_type dots, uint8_t position, uint8_t* data);
lcd_return_type lcd_print_char(char ch);
lcd_return_type lcd_print_string(char *ch);

#endif /*LCD_H*/
