#include <stddef.h>
#include "lcd.h"

#define _DB0_BIT_POS 0
#define _DB1_BIT_POS 1
#define _DB2_BIT_POS 2
#define _DB3_BIT_POS 3
#define _DB4_BIT_POS 4
#define _DB5_BIT_POS 5
#define _DB6_BIT_POS 6
#define _DB7_BIT_POS 7

#define _BYTE_TO_DB(byte, pos) ((byte) & (0x1U << (pos))) ? 1 : 0

#define _DB_TO_BYTE(byte, fun, pos) do \
{ \
    uint8_t bit = fun; \
    byte |= (bit << (pos));\
}while(0)

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

#define __LCD_ERROR_ASSERT(par) do \
{ \
    uint32_t val = (par); \
    if (!val) \
    { \
        return LCD_ERROR; \
    } \
}while(0)

#define __LCD_TIMEOUT_ASSERT(par) do \
{ \
    lcd_return_type ret = par; \
    if (ret != LCD_OK) \
    { \
        return LCD_TIMEOUT; \
    } \
}while(0)

/*Store the delay callback.*/
static void (*__lcd_ms_delay)(uint32_t max);
/*Store the number of lines.*/
static uint8_t __lcd_lines; /*<! 0 for 1 line, 1 for 2 lines.*/
/*Store the number of dots.*/
static uint8_t __lcd_dots; /*<! 0 for 5x8 dots, 1 for 5x10 dots.*/

__attribute__((always_inline)) static inline void __LCD_SET_DB_INPUT(void);
__attribute__((always_inline)) static inline void __LCD_SET_DB_OUTPUT(void);
static inline void __LCD_DB_WRITE(uint8_t data);
static inline uint8_t __LCD_DB_READ(void);

static inline void __lcd_write(uint8_t data, uint8_t rs_bit, uint8_t rw_bit);
static inline uint8_t __lcd_read(uint8_t rs_bit, uint8_t rw_bit);

static void __lcd_write_instruction(uint8_t data);
static uint8_t __lcd_read_bf_ac(void);
static void __lcd_write_data(uint8_t data);
static uint8_t __lcd_read_data(void);
static lcd_return_type __busy_bit_read_timeout(uint32_t max);

/**
 * @brief Modify data lines DB0-DB7 to act as inputs.
 * @param  
 */
__attribute__((always_inline)) static inline void __LCD_SET_DB_INPUT(void)
{
#if defined(LCD_8_BIT_MODE)
    LCD_DB0_INPUT();
    LCD_DB1_INPUT();
    LCD_DB2_INPUT();
    LCD_DB3_INPUT();
#endif

    LCD_DB4_INPUT();
    LCD_DB5_INPUT();
    LCD_DB6_INPUT();
    LCD_DB7_INPUT();
}

/**
 * @brief Modify data lines DB0-DB7 to act as outputs.
 * @param  
 */
__attribute__((always_inline)) static inline void __LCD_SET_DB_OUTPUT(void)
{
#if defined(LCD_8_BIT_MODE)
    LCD_DB0_OUTPUT();
    LCD_DB1_OUTPUT();
    LCD_DB2_OUTPUT();
    LCD_DB3_OUTPUT();
#endif
    LCD_DB4_OUTPUT();
    LCD_DB5_OUTPUT();
    LCD_DB6_OUTPUT();
    LCD_DB7_OUTPUT();
}

/**
 * @brief Set the data lines DB0-DB7 to high or low.
 * @param data Byte to transmit. When using 4 bit mode, the bits 0-3 (LSB nibble) are ignored.
 */
static inline void __LCD_DB_WRITE(uint8_t data)
{
#if defined(LCD_8_BIT_MODE)
    LCD_SET_DB0(_BYTE_TO_DB(data, _DB0_BIT_POS));
    LCD_SET_DB1(_BYTE_TO_DB(data, _DB1_BIT_POS));
    LCD_SET_DB2(_BYTE_TO_DB(data, _DB2_BIT_POS));
    LCD_SET_DB3(_BYTE_TO_DB(data, _DB3_BIT_POS));
#endif

    LCD_SET_DB4(_BYTE_TO_DB(data, _DB4_BIT_POS));
    LCD_SET_DB5(_BYTE_TO_DB(data, _DB5_BIT_POS));
    LCD_SET_DB6(_BYTE_TO_DB(data, _DB6_BIT_POS));
    LCD_SET_DB7(_BYTE_TO_DB(data, _DB7_BIT_POS));

}

/**
 * @brief Get the data lines DB0-DB7.
 * @param  
 * @return Byte received. When using 4 bit mode, the bits 0-3 (LSB nibble) are set to 0.
 */
static inline uint8_t __LCD_DB_READ(void)
{
    uint8_t byte = 0;

#if defined(LCD_8_BIT_MODE)
    _DB_TO_BYTE(byte, LCD_GET_DB0(), _DB0_BIT_POS);
    _DB_TO_BYTE(byte, LCD_GET_DB1(), _DB1_BIT_POS);
    _DB_TO_BYTE(byte, LCD_GET_DB2(), _DB2_BIT_POS);
    _DB_TO_BYTE(byte, LCD_GET_DB3(), _DB3_BIT_POS);
#endif

    _DB_TO_BYTE(byte, LCD_GET_DB4(), _DB4_BIT_POS);
    _DB_TO_BYTE(byte, LCD_GET_DB5(), _DB5_BIT_POS);
    _DB_TO_BYTE(byte, LCD_GET_DB6(), _DB6_BIT_POS);
    _DB_TO_BYTE(byte, LCD_GET_DB7(), _DB7_BIT_POS);

    return byte;
}

/**
 * @brief Transmit a byte to the lcd screen.
 * @param data A byte to to transmit. Can be an instruction, or a casted character.
 * @param rs_bit Value of the rs bit. Can be 0 or 1.
 * @param rw_bit Value of the rw bit. Can be 0 or 1.
 */
static inline void __lcd_write(uint8_t data, uint8_t rs_bit, uint8_t rw_bit)
{
    /*Clear E.*/
    LCD_SET_E(0);
    __lcd_ms_delay(1);
    /*Set DB0-DB7 or DB0-DB3 as ouputs.*/
    __LCD_SET_DB_OUTPUT();
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

/**
 * @brief Receive a byte from the lcd screen.
 * @param rs_bit Value of the rs bit. Can be 0 or 1.
 * @param rw_bit Value of the rw bit. Can be 0 or 1.
 * @return Byte received.
 */
static inline uint8_t __lcd_read(uint8_t rs_bit, uint8_t rw_bit)
{
    uint8_t data = 0;

    /*Clear E.*/
    LCD_SET_E(0);
    __lcd_ms_delay(1);
    /*Set DB0-DB7 or DB0-DB3 as inputs.*/
    __LCD_SET_DB_INPUT();
    /*Set RS and RW.*/
    LCD_SET_RS((rs_bit & 0x1U));
    LCD_SET_RW((rw_bit & 0x1U));

#if defined(LCD_8_BIT_MODE)
    /*Read the data.*/
    LCD_SET_E(1);
    lcd_ms_delay(1);
    data = LCD_DB_READ();
    LCD_SET_E(0);
    lcd_ms_delay(1);
#elif defined(LCD_4_BIT_MODE)
    /*Read the high byte.*/
    LCD_SET_E(1);
    lcd_ms_delay(1);
    data = (LCD_DB_READ() & (0xFU << 4));
    LCD_SET_E(0);
    lcd_ms_delay(1);

    /*Read low byte.*/
    LCD_SET_E(1);
    lcd_ms_delay(1);
    data |= ((LCD_DB_READ() >> 4) & 0xFU);
    LCD_SET_E(0);
    lcd_ms_delay(1);
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

/**
 * @brief Transmit an instruction to the lcd screen.
 * @note IR write as an internal operation.
 * @param data Instruction to transmit.
 */
static void __lcd_write_instruction(uint8_t data)
{
    __lcd_write(data, 0, 0);
}

/**
 * @brief Receive the busy flag and the address counter of the lcd screen.
 * @note Read busy flag (DB7) and address counter.
 * @param  
 * @return Byte received.
 */
static uint8_t __lcd_read_bf_ac(void)
{
    return __lcd_read(0,1);
}

/**
 * @brief Transmit data to the lcd screen.
 * @note DR write as an internal operation.
 * @param data Byte to transmit.
 */
static void __lcd_write_data(uint8_t data)
{
    __lcd_write(data, 1, 0);
}

/**
 * @brief Receive data from the lcd screen.
 * @note DR read as an internal operation.
 * @param  
 * @return Byte received.
 */
static uint8_t __lcd_read_data(void)
{
    return __lcd_read(1, 1);
}

/**
 * @brief Read the busy bit until it clears or the timeout limit is reached.
 * @param max Times to read the busy bit.
 * @return LCD_OK or LCD_TIMEOUT.
 */
static lcd_return_type __busy_bit_read_timeout(uint32_t max)
{
    uint32_t timeout = 0;
    while (__lcd_read_bf_ac() & (0x1U << _DB7_BIT_POS))
    {
        if (timeout >= max)
        {
            return LCD_TIMEOUT;
        }
        timeout++;
    }
    return LCD_OK;
}

/**
 * @brief Initialize the lcd screen.
 * @param number_of_lines Number of lines to use. Can be 1 or 2.
 * @param number_of_dots Number of dots to use. Can be 8 or 10.
 * @param ms_delay Callback to a user defined delay function in milliseconds.
 * @return LCD_OK,	LCD_ERROR, or LCD_TIMEOUT.
 */
lcd_return_type lcd_init(uint8_t number_of_lines, uint8_t number_of_dots, void (*ms_delay)(uint32_t max))
{
    /*Number of lines can be 1 or 2.*/
    __LCD_ERROR_ASSERT(number_of_lines == 1 || number_of_lines == 2);
    /*Store the number of lines.*/
    __lcd_lines = (number_of_lines >> 1);
    /*Number of dots can be 8 or 10.*/
    __LCD_ERROR_ASSERT(number_of_dots == 8 || number_of_dots == 10);
    __lcd_dots = ((number_of_dots >> 1) & 0x1U);
    /*Cannot display two lines for 5 × 10 dot character fond.*/
    __LCD_ERROR_ASSERT(!(number_of_lines == 2 && number_of_dots == 10));
    /*Delay callback cannot be empty.*/
    __LCD_ERROR_ASSERT(ms_delay != NULL);

    __lcd_ms_delay = ms_delay;

    LCD_SETUP();
    
    /*Wait for 40ms*/
    __lcd_ms_delay(41);
#if defined(LCD_8_BIT_MODE)
    lcd_write_instruction((LCD_FUNCTION_SET | LCD_DL_8_BIT));
    /*Wait for 4.1 ms.*/
    lcd_ms_delay(5);
    lcd_write_instruction((LCD_FUNCTION_SET | LCD_DL_8_BIT));
    /*Wait for 100 us.*/
    lcd_ms_delay(1);
    lcd_write_instruction((LCD_FUNCTION_SET | LCD_DL_8_BIT));
    lcd_write_instruction((LCD_FUNCTION_SET | LCD_DL_8_BIT | (lcd_lines << 3) | (lcd_dots << 2)));

    /*Display off.*/
    lcd_write_instruction(LCD_DISPLAY_ONOFF_CTRL);
    __LCD_TIMEOUT_ASSERT(busy_bit_read_timeout(LCD_TIMEOUT_MAX));
    /*Clear display.*/
    lcd_write_instruction(LCD_CLEAR_DISPLAY);
    __LCD_TIMEOUT_ASSERT(busy_bit_read_timeout(LCD_TIMEOUT_MAX));
    /*Entry mode set.*/
    lcd_write_instruction(LCD_ENTRY_MODE_SET);
    __LCD_TIMEOUT_ASSERT(busy_bit_read_timeout(LCD_TIMEOUT_MAX));

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

    /*Display off.*/
    lcd_write_instruction(0);
    lcd_write_instruction(LCD_DISPLAY_ONOFF_CTRL);
    __LCD_TIMEOUT_ASSERT(busy_bit_read_timeout(LCD_TIMEOUT_MAX));
    /*Clear display.*/
    lcd_write_instruction(0);
    lcd_write_instruction(LCD_CLEAR_DISPLAY);
    __LCD_TIMEOUT_ASSERT(busy_bit_read_timeout(LCD_TIMEOUT_MAX));
    /*Entry mode set.*/
    lcd_write_instruction(0);
    lcd_write_instruction(LCD_ENTRY_MODE_SET);
    __LCD_TIMEOUT_ASSERT(busy_bit_read_timeout(LCD_TIMEOUT_MAX));
#endif
    return LCD_OK;
}

/**
 * @brief Clears the display.
 * @param  
 * @return LCD_OK, LCD_ERROR, or LCD_TIMEOUT.
 */
lcd_return_type lcd_clear_display(void)
{
    __lcd_write_instruction(LCD_CLEAR_DISPLAY);
    __LCD_TIMEOUT_ASSERT(__busy_bit_read_timeout(LCD_TIMEOUT_MAX));
    return LCD_OK;
}

/**
 * @brief Returns the cursor to the starting point.
 * @param  
 * @return LCD_OK, LCD_ERROR, or LCD_TIMEOUT.
 */
lcd_return_type lcd_return_home(void)
{
    __lcd_write_instruction(LCD_RETURN_HOME);
    __LCD_TIMEOUT_ASSERT(__busy_bit_read_timeout(LCD_TIMEOUT_MAX));
    return LCD_OK;
}

/**
 * @brief Entry Mode Set. Sets the cursor to auto increment.
 * @param shift_display Set to true to shift the display when writing.
 * @return LCD_OK, LCD_ERROR, or LCD_TIMEOUT.
 */
lcd_return_type lcd_increment_cursor(bool shift_display)
{
    __lcd_write_instruction(LCD_ENTRY_MODE_SET | LCD_ID_INCREMENT | (uint8_t)shift_display);
    __LCD_TIMEOUT_ASSERT(__busy_bit_read_timeout(LCD_TIMEOUT_MAX));
    return LCD_OK;
}

/**
 * @brief Entry Mode Set. Sets the cursor to auto decrement.
 * @param shift_display Set to true to shift the display when writing.
 * @return LCD_OK, LCD_ERROR, or LCD_TIMEOUT.
 */
lcd_return_type lcd_decrement_cursor(bool shift_display)
{
    __lcd_write_instruction(LCD_ENTRY_MODE_SET | (uint8_t)shift_display);
    __LCD_TIMEOUT_ASSERT(__busy_bit_read_timeout(LCD_TIMEOUT_MAX));
    return LCD_OK;
}

/**
 * @brief Display On/Off Control 
 * @param display_on Set to true to turn the lcd on.
 * @param cursor_on Set to true to turn the cursor on.
 * @param blink_on Set to true to blink the cursor.
 * @return LCD_OK, LCD_ERROR, or LCD_TIMEOUT.
 */
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

    __lcd_write_instruction(instr);
    __LCD_TIMEOUT_ASSERT(__busy_bit_read_timeout(LCD_TIMEOUT_MAX));
    return LCD_OK;
}

/**
 * @brief Cursor or Display Shift. Shift the display to the right.
 * @param  
 * @return LCD_OK, LCD_ERROR, or LCD_TIMEOUT.
 */
lcd_return_type lcd_shift_display_right(void)
{
    __lcd_write_instruction(LCD_CURSOR_OR_DISPLAY_SHIFT | LCD_SC_DISPLAY_SHIFT | LCD_RL_SHIFT_RIGHT);
    __LCD_TIMEOUT_ASSERT(__busy_bit_read_timeout(LCD_TIMEOUT_MAX));
    return LCD_OK;
}

/**
 * @brief Cursor or Display Shift. Shift the display to the left.
 * @param  
 * @return LCD_OK, LCD_ERROR, or LCD_TIMEOUT.
 */
lcd_return_type lcd_shift_display_left(void)
{
    __lcd_write_instruction(LCD_CURSOR_OR_DISPLAY_SHIFT | LCD_SC_DISPLAY_SHIFT);
    __LCD_TIMEOUT_ASSERT(__busy_bit_read_timeout(LCD_TIMEOUT_MAX));
    return LCD_OK;
}

/**
 * @brief Cursor or Display Shift. Shift the cursor to the right.
 * @param  
 * @return LCD_OK, LCD_ERROR, or LCD_TIMEOUT.
 */
lcd_return_type lcd_shift_cursor_right(void)
{
    __lcd_write_instruction(LCD_CURSOR_OR_DISPLAY_SHIFT | LCD_RL_SHIFT_RIGHT);
    __LCD_TIMEOUT_ASSERT(__busy_bit_read_timeout(LCD_TIMEOUT_MAX));
    return LCD_OK;
}

/**
 * @brief Cursor or Display Shift. Shift the cursor to the left.
 * @param  
 * @return LCD_OK, LCD_ERROR, or LCD_TIMEOUT.
 */
lcd_return_type lcd_shift_cursor_left(void)
{
    __lcd_write_instruction(LCD_CURSOR_OR_DISPLAY_SHIFT);
    __LCD_TIMEOUT_ASSERT(__busy_bit_read_timeout(LCD_TIMEOUT_MAX));
    return LCD_OK;
}

/**
 * @brief Moves the cursor to a specific position.
 * @param line Line can be 1 or 2. The amount of lines is selected on initilization.
 * @param position Can be 1-80 if the display is set to 1 line. or 1-40 if the display is set to 2 lines.
 * @return LCD_OK, LCD_ERROR, or LCD_TIMEOUT.
 */
lcd_return_type lcd_set_cursor_position(uint8_t line, uint8_t position)
{
    uint8_t pos;
    /*Line and position cannot be 0.*/
    __LCD_ERROR_ASSERT(line && position);

    /*Cannot move the cursor to a line that doesn't exist (or else line has to be less or equal to the stored lines).*/
    __LCD_ERROR_ASSERT((line >> 1) <= __lcd_lines);

    /*1 line display cannot have more than 80 positions.*/
    __LCD_ERROR_ASSERT(!(!__lcd_lines && position > 80));

    /*2 line display cannot have more than 40 positions.*/
    __LCD_ERROR_ASSERT(!(__lcd_lines && position > 40));

    /*Calculate ddram depending on the lines and position.*/
    pos = (!__lcd_lines) ? (uint8_t)(position - 1) : (!(line >> 1)) ? (uint8_t)(position - 1) : (uint8_t)((position - 1) + 0x40U);

    __lcd_write_instruction(LCD_SET_DDRAM_ADDRESS | (pos & 0x7FU));
    __LCD_TIMEOUT_ASSERT(__busy_bit_read_timeout(LCD_TIMEOUT_MAX));
    return LCD_OK;
}

/**
 * @brief Gets the current line and position of the cursor.
 * @param line [out] Current line of the cursor. 
 * @param position [out] Current position of the cursor.
 * @return LCD_OK, LCD_ERROR, or LCD_TIMEOUT.
 */
lcd_return_type lcd_get_cursor_position(uint8_t* line, uint8_t* position)
{
    __LCD_ERROR_ASSERT(line != NULL);
    __LCD_ERROR_ASSERT(position != NULL);

    uint8_t data = (__lcd_read_bf_ac() & 0x7FU);
    *line = (!__lcd_lines) ? 1 : ((data & 0xF0) >= 0x40U) ? 2 : 1;
    *position = (!__lcd_lines) ? ((data & 0xFF) + 1) : ((data & 0xF) + 1);
    return LCD_OK;
}

/**
 * @brief Creates a custom character.
 * @param number_of_dots Can be 8 or 10.
 * @param position For number_of_dots equal to 8, can be 0-7 and for number_of_dots equal to 10 can be 0-4.
 * @param data [in] Array with the design of the custom character.
 * @return LCD_OK, LCD_ERROR, or LCD_TIMEOUT.
 */
lcd_return_type lcd_create_custom_char(uint8_t number_of_dots, uint8_t position, uint8_t *data)
{
    __LCD_ERROR_ASSERT(data != NULL);
    uint8_t cgram_address;
    /*Number of dots can be 8 or 10.*/
    __LCD_ERROR_ASSERT(number_of_dots == 8 && number_of_dots == 10);

    /*For 8 dots the position address cannot be bigger than 0x7.*/
    __LCD_ERROR_ASSERT(!(number_of_dots == 8 && position > 0x7U));

    /*For 10 dots the position address cannot be bigger than 0x4.*/
    __LCD_ERROR_ASSERT(!(number_of_dots == 10 && position > 0x4U));
   
    if (number_of_dots == 8)
    {
        cgram_address = ((position & 0x7U) << 3);
    }

    if (number_of_dots == 10)
    {
        cgram_address = ((position & 0x3U) << 4);
    }

    __lcd_write_instruction(LCD_SET_CGRAM_ADDRESS | cgram_address);
    __LCD_TIMEOUT_ASSERT(__busy_bit_read_timeout(LCD_TIMEOUT_MAX));

    while (number_of_dots--)
    {
        __lcd_write_data(*data);
        data++;
    }
    return LCD_OK;
}

/**
 * @brief Prints a character to the current cursor position.
 * @param ch Character.
 * @return LCD_OK, LCD_ERROR, or LCD_TIMEOUT.
 */
lcd_return_type lcd_print_char(char ch)
{
    __lcd_write_data((uint8_t)ch);
    __LCD_TIMEOUT_ASSERT(__busy_bit_read_timeout(LCD_TIMEOUT_MAX));
    return LCD_OK;
}

/**
 * @brief Print characters to the current cursor position until the NULL character is reached.
 * @param ch Character array.
 * @return LCD_OK, LCD_ERROR, or LCD_TIMEOUT.
 */
lcd_return_type lcd_print_string(char* ch)
{
    __LCD_ERROR_ASSERT(ch != NULL);
    while (*ch != '\0')
    {
        __LCD_TIMEOUT_ASSERT(lcd_print_char(*ch));
        ch++;
    }
    return LCD_OK;
}