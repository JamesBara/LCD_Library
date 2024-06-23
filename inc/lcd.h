#ifndef LCD_H
#define LCD_H

#include <stdint.h>
#include <stdbool.h>

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
/*@todo cgram_stuff*/
lcd_return_type lcd_set_cursor_position(uint8_t line, uint8_t position);
uint8_t lcd_get_cursor_position(void);
lcd_return_type lcd_print_char(char ch);
lcd_return_type lcd_print_string(char *ch);

#endif /*LCD_H*/
