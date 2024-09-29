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
	LCD_DOTS_5x10 = 1,
}lcd_dots_type;

typedef enum _lcd_line_type
{
	LCD_LINE_1 = 0,
	LCD_LINE_2 = 1,
}lcd_line_type;


typedef enum _lcd_return_type
{
	LCD_OK,
	LCD_ERROR,
	LCD_TIMEOUT
}lcd_return_type;

typedef struct _lcd_driver_type
{
	void (*us_delay)(uint32_t max);
	void (*io_setup)(void);
	void (*set_e)(uint32_t bit);
	void (*set_rs)(uint32_t bit);
	void (*set_rw)(uint32_t bit);
	uint32_t (*get_db0)(void);
	uint32_t (*get_db1)(void);
	uint32_t (*get_db2)(void);
	uint32_t (*get_db3)(void);
	uint32_t (*get_db4)(void);
	uint32_t (*get_db5)(void);
	uint32_t (*get_db6)(void);
	uint32_t (*get_db7)(void);
	void (*set_db0)(uint32_t bit);
	void (*set_db1)(uint32_t bit);
	void (*set_db2)(uint32_t bit);
	void (*set_db3)(uint32_t bit);
	void (*set_db4)(uint32_t bit);
	void (*set_db5)(uint32_t bit);
	void (*set_db6)(uint32_t bit);
	void (*set_db7)(uint32_t bit);
	void (*db0_input)(void);
	void (*db1_input)(void);
	void (*db2_input)(void);
	void (*db3_input)(void);
	void (*db4_input)(void);
	void (*db5_input)(void);
	void (*db6_input)(void);
	void (*db7_input)(void);
	void (*db0_output)(void);
	void (*db1_output)(void);
	void (*db2_output)(void);
	void (*db3_output)(void);
	void (*db4_output)(void);
	void (*db5_output)(void);
	void (*db6_output)(void);
	void (*db7_output)(void);
}lcd_driver_type;


lcd_return_type lcd_init(lcd_line_type lines, lcd_dots_type dots, lcd_driver_type *driver);
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
