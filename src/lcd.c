#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "lcd.h"
#include "lcd_user_defines.h"

/*Instructions*/
#define LCD_CLEAR_DISPLAY (0x1U << 0)
#define LCD_RETURN_HOME (0x1U << 1)
#define LCD_ENTRY_MODE_SET (0x1U << 2) /*<! Entry Mode Set instruction.*/
#define LCD_ID_INCREMENT (0x1U << 1) /*<! Entry Mode Set instruction flag. Increment the cursor.*/
#define LCD_S_DISPLAY_SHIFT (0x1U << 0) /*<! Entry Mode Set instruction flag. Automatically shift the display.*/
#define LCD_FUNCTION_SET (0x1U << 5) /*<! Function set instruction.*/
#define LCD_DL_8_BIT (0x1U << 4) /*<! Function set instruction flag. Use 8 bit data length.*/
#define LCD_N_2_LINES (0x1U << 3) /*<! Function set instruction flag. Use both bottom and top display lines.*/
#define LCD_F_5_MUL_10_DOTS (0x1U << 2) /*<! Function set instruction flag. 5x10 dots.*/
#define LCD_DISPLAY_ONOFF_CTRL (0x1U << 3) /*<! Display on/off control instruction.*/
#define LCD_D_ON (0x1U << 2) /*<! Display on/off control instruction flag. Turns on the display.*/
#define LCD_CURSOR_ON (0x1U << 1) /*<! Display on/off control instruction flag. Turns on the cursor.*/
#define LCD_CURSOR_BLINK (0x1U << 0) /*<! Display on/off control instruction flag. Blinks the cursor.*/
#define LCD_CURSOR_OR_DISPLAY_SHIFT (0x1U << 4) /*<! Cursor or display shift instruction. Moves cursor and shifts display without changing DDRAM contents*/
#define LCD_SC_DISPLAY_SHIFT (0x1U << 3) /*<! Cursor or display shift instruction flag. Moves the display.*/
#define LCD_RL_SHIFT_RIGHT (0x1U << 2) /*<! Cursor or display shift instruction flag. Shifts to the right.*/
#define LCD_SET_CGRAM_ADDRESS (0x1U << 6) /*<! Set cgram address instruction.*/
#define LCD_SET_DDRAM_ADDRESS (0x1U << 7) /*<! Set ddram address instruction.*/

#define LCD_ERROR_ASSERT(pr) do \
{ \
    if (!(pr)) \
    { \
        return LCD_ERROR; \
    } \
}while(0)

#define LCD_TIMEOUT_ASSERT(pr) do \
{ \
    if ((pr) != LCD_OK) \
    { \
        return LCD_TIMEOUT; \
    } \
}while(0)


/*Store the delay callback.*/
static void (*lcd_ms_delay)(uint32_t max);
/*Store the number of lines.*/
static uint8_t lcd_lines; /*<! 0 for 1 line, 1 for 2 lines.*/
/*Store the number of dots.*/
static uint8_t lcd_dots; /*<! 0 for 5x8 dots, 1 for 5x10 dots.*/

static inline void lcd_write(uint8_t data, uint8_t rs_bit, uint8_t rw_bit);
static inline uint8_t lcd_read(uint8_t rs_bit, uint8_t rw_bit);

static void lcd_write_instruction(uint8_t data);
static uint8_t lcd_read_bf_ac(void);
static void lcd_write_data(uint8_t data);
static uint8_t lcd_read_data(void);
static lcd_return_type busy_bit_read_timeout(uint32_t max);

static inline void lcd_write(uint8_t data, uint8_t rs_bit, uint8_t rw_bit)
{
    /*Clear E.*/
    LCD_SET_E(0);
    lcd_ms_delay(1);
    /*Set DB0-DB7 or DB0-DB3 as ouputs.*/
    LCD_SET_DB_OUTPUT();
    /*Set RS and RW.*/
    LCD_SET_RS((rs_bit & 0x1U));
    LCD_SET_RW((rw_bit & 0x1U));

#if defined(LCD_8_BIT_MODE)
    /*Write the data.*/
    LCD_DB_WRITE(data);
    /*Send the data.*/
    LCD_SET_E(1);
    lcd_ms_delay(1);
    LCD_SET_E(0);
    lcd_ms_delay(1);
#elif defined(LCD_4_BIT_MODE)
    /*Write high byte.*/
    LCD_DB_WRITE((data & (0xFU << 4)));
    /*Send the data.*/
    LCD_SET_E(1);
    lcd_ms_delay(1);
    LCD_SET_E(0);
    lcd_ms_delay(1);

    /*Write low byte.*/
    LCD_DB_WRITE(((data & 0xFU) << 4));
    /*Send the data.*/
    LCD_SET_E(1);
    lcd_ms_delay(1);
    LCD_SET_E(0);
    lcd_ms_delay(1);
#endif
}

static inline uint8_t lcd_read(uint8_t rs_bit, uint8_t rw_bit)
{
    uint8_t data = 0;

    /*Clear E.*/
    LCD_SET_E(0);
    lcd_ms_delay(1);
    /*Set DB0-DB7 or DB0-DB3 as inputs.*/
    LCD_SET_DB_INPUT();
    /*Set RS and RW.*/
    LCD_SET_RS((rs_bit & 0x1U));
    LCD_SET_RW((rw_bit & 0x1U));

#if defined(LCD_8_BIT_MODE)
    /*Read the data.*/
    LCD_SET_E(1);
    lcd_ms_delay(1);
    LCD_DB_READ(data);
    LCD_SET_E(0);
    lcd_ms_delay(1);
#elif defined(LCD_4_BIT_MODE)
    uint8_t tmp = 0;
    /*Read the high byte.*/
    LCD_SET_E(1);
    lcd_ms_delay(1);
    LCD_DB_READ(tmp);
    LCD_SET_E(0);
    lcd_ms_delay(1);
    data = (tmp & (0xFU << 4));

    /*Read low byte.*/
    LCD_SET_E(1);
    lcd_ms_delay(1);
    LCD_DB_READ(tmp);
    LCD_SET_E(0);
    lcd_ms_delay(1);
    data |= ((tmp >> 4) & 0xFU);
#endif

    return data;
}

#if defined(LCD_4_BIT_MODE)
static void lcd_write_4_bit_init_instruction(uint8_t data)
{
    /*Clear E.*/
    LCD_SET_E(0);
    lcd_ms_delay(1);
    /*Set DB0-DB3 as inputs.*/
    LCD_SET_DB_INPUT();
    /*Set RS and RW.*/
    LCD_SET_RS(0);
    LCD_SET_RW(0);

    /*Write high byte.*/
    LCD_DB_WRITE((data & (0xFU << 4)));
    /*Send the data.*/
    LCD_SET_E(1);
    lcd_ms_delay(1);
    LCD_SET_E(0);
    lcd_ms_delay(1);
}
#endif

static void lcd_write_instruction(uint8_t data)
{
    lcd_write(data, 0, 0);
}

static uint8_t lcd_read_bf_ac(void)
{
    return lcd_read(0,1);
}

static void lcd_write_data(uint8_t data)
{
    lcd_write(data, 1, 0);
}

static uint8_t lcd_read_data(void)
{
    return lcd_read(1, 1);
}

static lcd_return_type busy_bit_read_timeout(uint32_t max)
{
    uint32_t timeout;
    while (lcd_read_bf_ac() & (0x1U << 7))
    {
        if (timeout >= max)
        {
            return LCD_TIMEOUT;
        }
        timeout++;
    }
    return LCD_OK;
}

lcd_return_type lcd_init(uint8_t number_of_lines, uint8_t number_of_dots, void (*ms_delay)(uint32_t max))
{
    /*Number of lines can be 1 or 2.*/
    LCD_ERROR_ASSERT(number_of_lines == 1 || number_of_lines == 2);
    /*Store the number of lines.*/
    lcd_lines = (number_of_lines >> 1);
    /*Number of dots can be 8 or 10.*/
    LCD_ERROR_ASSERT(number_of_dots == 8 || number_of_dots == 10);
    lcd_dots = ((number_of_dots >> 1) & 0x1U);
    /*Cannot display two lines for 5 × 10 dot character fond.*/
    LCD_ERROR_ASSERT(!(number_of_lines == 2 && number_of_dots == 10));
    /*Delay callback cannot be empty.*/
    LCD_ERROR_ASSERT(ms_delay != NULL);

    lcd_ms_delay = ms_delay;

    /* Hardware init.*/
    LCD_HARDWARE_INIT();
    
    /*Wait for 40ms*/
    lcd_ms_delay(41);
#if defined(LCD_8_BIT_MODE)
    lcd_write_instruction((LCD_FUNCTION_SET | LCD_DL_8_BIT));
    /*Wait for 4.1 ms.*/
    lcd_ms_delay(5);
    lcd_write_instruction((LCD_FUNCTION_SET | LCD_DL_8_BIT));
    /*Wait for 100 us.*/
    lcd_ms_delay(1);
    lcd_write_instruction((LCD_FUNCTION_SET | LCD_DL_8_BIT));
    lcd_write_instruction((LCD_FUNCTION_SET | LCD_DL_8_BIT | (lcd_lines << 3) | (lcd_dots << 2)));
    LCD_TIMEOUT_ASSERT(busy_bit_read_timeout(LCD_TIMEOUT_MAX));
    /*Display off.*/
    lcd_write_instruction(LCD_DISPLAY_ONOFF_CTRL);
    LCD_TIMEOUT_ASSERT(busy_bit_read_timeout(LCD_TIMEOUT_MAX));
    /*Clear display.*/
    lcd_write_instruction(LCD_CLEAR_DISPLAY);
    LCD_TIMEOUT_ASSERT(busy_bit_read_timeout(LCD_TIMEOUT_MAX));
    /*Entry mode set.*/
    lcd_write_instruction(LCD_ENTRY_MODE_SET);
    LCD_TIMEOUT_ASSERT(busy_bit_read_timeout(LCD_TIMEOUT_MAX));

#elif defined(LCD_4_BIT_MODE)
    lcd_write_4_bit_init_instruction((LCD_FUNCTION_SET | LCD_DL_8_BIT));
    /*Wait for 4.1 ms.*/
    lcd_ms_delay(5);
    lcd_write_4_bit_init_instruction((LCD_FUNCTION_SET | LCD_DL_8_BIT));
    /*Wait for 100 us.*/
    lcd_ms_delay(1);
    lcd_write_4_bit_init_instruction((LCD_FUNCTION_SET | LCD_DL_8_BIT));
    
    /*Function set 4 bit and user setting for number of lines and dots.*/
    lcd_write_4_bit_init_instruction(LCD_FUNCTION_SET);
    lcd_write_instruction(LCD_FUNCTION_SET | (lcd_lines << 3) | ((number_of_dots >> 1) & 0x1U));
    LCD_TIMEOUT_ASSERT(busy_bit_read_timeout(LCD_TIMEOUT_MAX));
    /*Display off.*/
    lcd_write_instruction(0);
    lcd_write_instruction(LCD_DISPLAY_ONOFF_CTRL);
    LCD_TIMEOUT_ASSERT(busy_bit_read_timeout(LCD_TIMEOUT_MAX));
    /*Clear display.*/
    lcd_write_instruction(0);
    lcd_write_instruction(LCD_CLEAR_DISPLAY);
    LCD_TIMEOUT_ASSERT(busy_bit_read_timeout(LCD_TIMEOUT_MAX));
    /*Entry mode set.*/
    lcd_write_instruction(0);
    lcd_write_instruction(LCD_ENTRY_MODE_SET);
    LCD_TIMEOUT_ASSERT(busy_bit_read_timeout(LCD_TIMEOUT_MAX));
#endif
    return LCD_OK;
}

lcd_return_type lcd_clear_display(void)
{
    lcd_write_instruction(LCD_CLEAR_DISPLAY);
    LCD_TIMEOUT_ASSERT(busy_bit_read_timeout(LCD_TIMEOUT_MAX));
    return LCD_OK;
}

lcd_return_type lcd_return_home(void)
{
    lcd_write_instruction(LCD_RETURN_HOME);
    LCD_TIMEOUT_ASSERT(busy_bit_read_timeout(LCD_TIMEOUT_MAX));
    return LCD_OK;
}

lcd_return_type lcd_increment_cursor(bool shift_display)
{
    lcd_write_instruction(LCD_ENTRY_MODE_SET | LCD_ID_INCREMENT | shift_display);
    LCD_TIMEOUT_ASSERT(busy_bit_read_timeout(LCD_TIMEOUT_MAX));
    return LCD_OK;
}

lcd_return_type lcd_decrement_cursor(bool shift_display)
{
    lcd_write_instruction(LCD_ENTRY_MODE_SET | shift_display);
    LCD_TIMEOUT_ASSERT(busy_bit_read_timeout(LCD_TIMEOUT_MAX));
    return LCD_OK;
}

lcd_return_type lcd_display(bool display_on, bool cursor_on, bool blink_on)
{
    uint8_t instr = LCD_DISPLAY_ONOFF_CTRL;

    if (blink_on)
    {
        instr |= (LCD_D_ON | LCD_CURSOR_ON | LCD_CURSOR_BLINK);
    }
    else if (cursor_on)
    {
        instr |= (LCD_D_ON | LCD_CURSOR_ON);
    }
    else if (display_on)
    {
        instr |= LCD_D_ON;
    }

    lcd_write_instruction(instr);
    LCD_TIMEOUT_ASSERT(busy_bit_read_timeout(LCD_TIMEOUT_MAX));
    return LCD_OK;
}

lcd_return_type lcd_shift_display_right(void)
{
    lcd_write_instruction(LCD_CURSOR_OR_DISPLAY_SHIFT | LCD_SC_DISPLAY_SHIFT | LCD_RL_SHIFT_RIGHT);
    LCD_TIMEOUT_ASSERT(busy_bit_read_timeout(LCD_TIMEOUT_MAX));
    return LCD_OK;
}

lcd_return_type lcd_shift_display_left(void)
{
    lcd_write_instruction(LCD_CURSOR_OR_DISPLAY_SHIFT | LCD_SC_DISPLAY_SHIFT);
    LCD_TIMEOUT_ASSERT(busy_bit_read_timeout(LCD_TIMEOUT_MAX));
    return LCD_OK;
}

lcd_return_type lcd_shift_cursor_right(void)
{
    lcd_write_instruction(LCD_CURSOR_OR_DISPLAY_SHIFT | LCD_RL_SHIFT_RIGHT);
    LCD_TIMEOUT_ASSERT(busy_bit_read_timeout(LCD_TIMEOUT_MAX));
    return LCD_OK;
}

lcd_return_type lcd_shift_cursor_left(void)
{
    lcd_write_instruction(LCD_CURSOR_OR_DISPLAY_SHIFT);
    LCD_TIMEOUT_ASSERT(busy_bit_read_timeout(LCD_TIMEOUT_MAX));
    return LCD_OK;
}

lcd_return_type lcd_set_cursor_position(uint8_t line, uint8_t position)
{
    uint8_t pos;
    /*Line and position cannot be 0.*/
    LCD_ERROR_ASSERT(line && position);

    /*Cannot move the cursor to a line that doesn't exist (or else line has to be less or equal to the stored lines).*/
    LCD_ERROR_ASSERT((line >> 1) <= lcd_lines);

    /*1 line display cannot have more than 80 positions.*/
    LCD_ERROR_ASSERT(!(!lcd_lines && position > 80));

    /*2 line display cannot have more than 40 positions.*/
    LCD_ERROR_ASSERT(!(lcd_lines && position > 40));

    /*Calculate ddram depending on the lines and position.*/
    pos = (!lcd_lines) ? (uint8_t)(position - 1) : (!(line >> 1)) ? (uint8_t)(position - 1) : (uint8_t)((position - 1) + 0x40U);

    lcd_write_instruction(LCD_SET_DDRAM_ADDRESS | (pos & 0x7FU));
    LCD_TIMEOUT_ASSERT(busy_bit_read_timeout(LCD_TIMEOUT_MAX));
    return LCD_OK;
}

uint8_t lcd_get_cursor_position(void)
{
    return (lcd_read_bf_ac() & 0x7FU);
}

lcd_return_type lcd_create_custom_char(uint8_t number_of_dots, uint8_t position, uint8_t *data)
{
    uint8_t cgram_address;
    /*Number of dots can be 8 or 10.*/
    LCD_ERROR_ASSERT(number_of_dots == 8 && number_of_dots == 10);

    /*For 8 dots the position address cannot be bigger than 0x7.*/
    LCD_ERROR_ASSERT(!(number_of_dots == 8 && position > 0x7U));

    /*For 10 dots the position address cannot be bigger than 0x4.*/
    LCD_ERROR_ASSERT(!(number_of_dots == 10 && position > 0x4U));
   
    if (number_of_dots == 8)
    {
        cgram_address = ((position & 0x7U) << 3);
    }

    if (number_of_dots == 10)
    {
        cgram_address = ((position & 0x3U) << 4);
    }

    lcd_write_instruction(LCD_SET_CGRAM_ADDRESS | cgram_address);
    LCD_TIMEOUT_ASSERT(busy_bit_read_timeout(LCD_TIMEOUT_MAX));

    while (number_of_dots--)
    {
        lcd_write_data(*data);
        data++;
    } 
}

lcd_return_type lcd_print_char(char ch)
{
    lcd_write_data((uint8_t)ch);
    LCD_TIMEOUT_ASSERT(busy_bit_read_timeout(LCD_TIMEOUT_MAX));
    return LCD_OK;
}

lcd_return_type lcd_print_string(char* ch)
{
    while (*ch != '\0')
    {
        LCD_TIMEOUT_ASSERT(lcd_print_char(*ch));
        ch++;
    }
    return LCD_OK;
}