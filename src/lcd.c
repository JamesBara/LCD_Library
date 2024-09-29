#include <stddef.h>
#include "lcd.h"

#define __LCD_TIMEOUT_MAX 10000

#define __DB0_BIT_POS 0
#define __DB1_BIT_POS 1
#define __DB2_BIT_POS 2
#define __DB3_BIT_POS 3
#define __DB4_BIT_POS 4
#define __DB5_BIT_POS 5
#define __DB6_BIT_POS 6
#define __DB7_BIT_POS 7

#define __DB_GET_BIT(byte, pos) ((byte) & (0x1U << (pos))) ? 1 : 0

#define __DB_SET_BIT(byte, fun, pos) do \
{ \
    uint8_t bit = fun; \
    byte |= (bit << (pos));\
}while(0)

/*Instructions*/
#define __LCD_CLEAR_DISPLAY (0x1U << 0)
#define __LCD_RETURN_HOME (0x1U << 1)
#define __LCD_ENTRY_MODE_SET (0x1U << 2) /*<! Entry Mode Set instruction.*/
#define __LCD_ID_INCREMENT (0x1U << 1) /*<! Entry Mode Set instruction flag. Increment the cursor.*/
#define __LCD_S_DISPLAY_SHIFT (0x1U << 0) /*<! Entry Mode Set instruction flag. Automatically shift the display.*/
#define __LCD_FUNCTION_SET (0x1U << 5) /*<! Function set instruction.*/
#define __LCD_DL_8_BIT (0x1U << 4) /*<! Function set instruction flag. Use 8 bit data length.*/
#define __LCD_N_2_LINES (0x1U << 3) /*<! Function set instruction flag. Use both bottom and top display lines.*/
#define __LCD_F_5_MUL_10_DOTS (0x1U << 2) /*<! Function set instruction flag. 5x10 dots.*/
#define __LCD_DISPLAY_ONOFF_CTRL (0x1U << 3) /*<! Display on/off control instruction.*/
#define __LCD_D_ON (0x1U << 2) /*<! Display on/off control instruction flag. Turns on the display.*/
#define __LCD_CURSOR_ON (0x1U << 1) /*<! Display on/off control instruction flag. Turns on the cursor.*/
#define __LCD_CURSOR_BLINK (0x1U << 0) /*<! Display on/off control instruction flag. Blinks the cursor.*/
#define __LCD_CURSOR_OR_DISPLAY_SHIFT (0x1U << 4) /*<! Cursor or display shift instruction. Moves cursor and shifts display without changing DDRAM contents*/
#define __LCD_SC_DISPLAY_SHIFT (0x1U << 3) /*<! Cursor or display shift instruction flag. Moves the display.*/
#define __LCD_RL_SHIFT_RIGHT (0x1U << 2) /*<! Cursor or display shift instruction flag. Shifts to the right.*/
#define __LCD_SET_CGRAM_ADDRESS (0x1U << 6) /*<! Set cgram address instruction.*/
#define __LCD_SET_DDRAM_ADDRESS (0x1U << 7) /*<! Set ddram address instruction.*/

#define __LCD_ERROR_ASSERT(par) do \
{ \
    uint32_t val = par; \
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
}while (0)

/*Store a pointer to the callbacks.*/
static lcd_driver_type* __drv;
/*Store the number of lines.*/
static lcd_line_type __lcd_lines; /*<! 0 for 1 line, 1 for 2 lines.*/
/*Store the number of dots.*/
static lcd_dots_type __lcd_dots; /*<! 0 for 5x8 dots, 1 for 5x10 dots.*/

static inline void __lcd_set_db_input(void);
static inline void __lcd_set_db_output(void);
static inline void __lcd_set_db(uint8_t data);
static inline uint8_t __lcd_get_db(void);

static inline void __lcd_write(uint8_t data, uint8_t rs_bit, uint8_t rw_bit);
static inline uint8_t __lcd_read(uint8_t rs_bit, uint8_t rw_bit);
#if defined(LCD_4_BIT_MODE)
static void __lcd_write_4_bit_init_instruction(uint8_t data);
#endif
static void __lcd_write_instruction(uint8_t data);
static uint8_t __lcd_read_bf_ac(void);
static void __lcd_write_data(uint8_t data);
static uint8_t __lcd_read_data(void);
static lcd_return_type __busy_bit_read_timeout(uint32_t max);

/**
 * @brief Modify data lines DB0-DB7 to act as inputs.
 * @param  
 */
static inline void __lcd_set_db_input(void)
{
#if defined(LCD_8_BIT_MODE)
    __drv->db0_input();
    __drv->db1_input();
    __drv->db2_input();
    __drv->db3_input();
#endif
    __drv->db4_input();
    __drv->db5_input();
    __drv->db6_input();
    __drv->db7_input();
}

/**
 * @brief Modify data lines DB0-DB7 to act as outputs.
 * @param  
 */
static inline void __lcd_set_db_output(void)
{
#if defined(LCD_8_BIT_MODE)
    __drv->db0_output();
    __drv->db1_output();
    __drv->db2_output();
    __drv->db3_output();
#endif
    __drv->db4_output();
    __drv->db5_output();
    __drv->db6_output();
    __drv->db7_output();
}

/**
 * @brief Set the data lines DB0-DB7 to high or low.
 * @param data Byte to transmit. When using 4 bit mode, the bits 0-3 (LSB nibble) are ignored.
 */
static inline void __lcd_set_db(uint8_t data)
{
#if defined(LCD_8_BIT_MODE)
    __drv->set_db0(__DB_GET_BIT(data, __DB0_BIT_POS));
    __drv->set_db1(__DB_GET_BIT(data, __DB1_BIT_POS));
    __drv->set_db2(__DB_GET_BIT(data, __DB2_BIT_POS));
    __drv->set_db3(__DB_GET_BIT(data, __DB3_BIT_POS));
#endif

    __drv->set_db4(__DB_GET_BIT(data, __DB4_BIT_POS));
    __drv->set_db5(__DB_GET_BIT(data, __DB5_BIT_POS));
    __drv->set_db6(__DB_GET_BIT(data, __DB6_BIT_POS));
    __drv->set_db7(__DB_GET_BIT(data, __DB7_BIT_POS));

}

/**
 * @brief Get the data lines DB0-DB7.
 * @param  
 * @return Byte received. When using 4 bit mode, the bits 0-3 (LSB nibble) are set to 0.
 */
static inline uint8_t __lcd_get_db(void)
{
    uint8_t byte = 0;

#if defined(LCD_8_BIT_MODE)
    __DB_SET_BIT(byte, __drv->get_db0(), __DB0_BIT_POS);
    __DB_SET_BIT(byte, __drv->get_db1(), __DB1_BIT_POS);
    __DB_SET_BIT(byte, __drv->get_db2(), __DB2_BIT_POS);
    __DB_SET_BIT(byte, __drv->get_db3(), __DB3_BIT_POS);
#endif

    __DB_SET_BIT(byte, __drv->get_db4(), __DB4_BIT_POS);
    __DB_SET_BIT(byte, __drv->get_db5(), __DB5_BIT_POS);
    __DB_SET_BIT(byte, __drv->get_db6(), __DB6_BIT_POS);
    __DB_SET_BIT(byte, __drv->get_db7(), __DB7_BIT_POS);

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
    __drv->set_e(0);
    __drv->us_delay(1);
    /*Set DB0-DB7 or DB0-DB3 as ouputs.*/
    __lcd_set_db_output();
    /*Set RS and RW.*/
    __drv->set_rs((rs_bit & 0x1U));
    __drv->set_rw((rw_bit & 0x1U));

#if defined(LCD_8_BIT_MODE)
    /*Write the data.*/
    __lcd_set_db(data);
    /*Send the data.*/
    __drv->set_e(1);
    __drv->us_delay(1);
    __drv->set_e(0);
    __drv->us_delay(1);
#elif defined(LCD_4_BIT_MODE)
    /*Write high byte.*/
    __lcd_set_db((data & (0xFU << 4)));
    /*Send the data.*/
    __drv->set_e(1);
    __drv->us_delay(1);
    __drv->set_e(0);
    __drv->us_delay(1);

    /*Write low byte.*/
    __lcd_set_db(((data & 0xFU) << 4));
    /*Send the data.*/
    __drv->set_e(1);
    __drv->us_delay(1);
    __drv->set_e(0);
    __drv->us_delay(1);
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
    __drv->set_e(0);
    __drv->us_delay(1);
    /*Set DB0-DB7 or DB0-DB3 as inputs.*/
    __lcd_set_db_input();
    /*Set RS and RW.*/
    __drv->set_rs((rs_bit & 0x1U));
    __drv->set_rw((rw_bit & 0x1U));

#if defined(LCD_8_BIT_MODE)
    /*Read the data.*/
    __drv->set_e(1);
    __drv->us_delay(1);
    data = __lcd_get_db();
    __drv->set_e(0);
    __drv->us_delay(1);
#elif defined(LCD_4_BIT_MODE)
    /*Read the high byte.*/
    __drv->set_e(1);
    __drv->us_delay(1);
    data = (__lcd_get_db() & (0xFU << 4));
    __drv->set_e(0);
    __drv->us_delay(1);

    /*Read low byte.*/
    __drv->set_e(1);
    __drv->us_delay(1);
    data |= ((__lcd_get_db() >> 4) & 0xFU);
    __drv->set_e(0);
    __drv->us_delay(1);
#endif

    return data;
}

#if defined(LCD_4_BIT_MODE)
static void __lcd_write_4_bit_init_instruction(uint8_t data)
{
    /*Clear E.*/
    __drv->set_e(0);
    __drv->us_delay(1);
    /*Set DB4-DB7 as outputs.*/
    __lcd_set_db_output();
    /*Set RS and RW.*/
    __drv->set_rs(0);
    __drv->set_rw(0);

    /*Write high byte.*/
    __lcd_set_db((data & (0xFU << 4)));
    /*Send the data.*/
    __drv->set_e(1);
    __drv->us_delay(1);
    __drv->set_e(0);
    __drv->us_delay(1);
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
    while (__lcd_read_bf_ac() & (0x1U << __DB7_BIT_POS))
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
 * @param lines Number of lines to use. Can be 1 or 2.
 * @param number_of_dots Number of dots to use. Can be 8 or 10.
 * @param ms_delay Callback to a user defined delay function in milliseconds.
 * @return LCD_OK,	LCD_ERROR, or LCD_TIMEOUT.
 */
lcd_return_type lcd_init(lcd_line_type lines, lcd_dots_type dots, lcd_driver_type *driver)
{
    /*Number of lines can be 1 or 2.*/
    __LCD_ERROR_ASSERT(lines == LCD_LINE_1 || lines == LCD_LINE_2);
    /*Store the number of lines.*/
    __lcd_lines = lines;
    /*Number of dots can be 8 or 10.*/
    __LCD_ERROR_ASSERT(dots == LCD_DOTS_5x8 || dots == LCD_DOTS_5x10);
    __lcd_dots = dots;
    /*Cannot display two lines for 5 × 10 dot character fond.*/
    __LCD_ERROR_ASSERT(!(lines == LCD_LINE_2 && dots == LCD_DOTS_5x10));
    /*Callbacks cannot be empty.*/
    __LCD_ERROR_ASSERT(driver != NULL);
    __LCD_ERROR_ASSERT(driver->io_setup != NULL);
    __LCD_ERROR_ASSERT(driver->us_delay != NULL);
    __LCD_ERROR_ASSERT(driver->set_e != NULL);
    __LCD_ERROR_ASSERT(driver->set_rs != NULL);
    __LCD_ERROR_ASSERT(driver->set_rw != NULL);
    __LCD_ERROR_ASSERT(driver->get_db0 != NULL);
    __LCD_ERROR_ASSERT(driver->get_db1 != NULL);
    __LCD_ERROR_ASSERT(driver->get_db2 != NULL);
    __LCD_ERROR_ASSERT(driver->get_db3 != NULL);
    __LCD_ERROR_ASSERT(driver->get_db4 != NULL);
    __LCD_ERROR_ASSERT(driver->get_db5 != NULL);
    __LCD_ERROR_ASSERT(driver->get_db6 != NULL);
    __LCD_ERROR_ASSERT(driver->get_db7 != NULL);
    __LCD_ERROR_ASSERT(driver->set_db0 != NULL);
    __LCD_ERROR_ASSERT(driver->set_db1 != NULL);
    __LCD_ERROR_ASSERT(driver->set_db2 != NULL);
    __LCD_ERROR_ASSERT(driver->set_db3 != NULL);
    __LCD_ERROR_ASSERT(driver->set_db4 != NULL);
    __LCD_ERROR_ASSERT(driver->set_db5 != NULL);
    __LCD_ERROR_ASSERT(driver->set_db6 != NULL);
    __LCD_ERROR_ASSERT(driver->set_db7 != NULL);
    __LCD_ERROR_ASSERT(driver->db0_input != NULL);
    __LCD_ERROR_ASSERT(driver->db1_input != NULL);
    __LCD_ERROR_ASSERT(driver->db2_input != NULL);
    __LCD_ERROR_ASSERT(driver->db3_input != NULL);
    __LCD_ERROR_ASSERT(driver->db4_input != NULL);
    __LCD_ERROR_ASSERT(driver->db5_input != NULL);
    __LCD_ERROR_ASSERT(driver->db6_input != NULL);
    __LCD_ERROR_ASSERT(driver->db7_input != NULL);
    __LCD_ERROR_ASSERT(driver->db0_output != NULL);
    __LCD_ERROR_ASSERT(driver->db1_output != NULL);
    __LCD_ERROR_ASSERT(driver->db2_output != NULL);
    __LCD_ERROR_ASSERT(driver->db3_output != NULL);
    __LCD_ERROR_ASSERT(driver->db4_output != NULL);
    __LCD_ERROR_ASSERT(driver->db5_output != NULL);
    __LCD_ERROR_ASSERT(driver->db6_output != NULL);
    __LCD_ERROR_ASSERT(driver->db7_output != NULL);

    __drv = driver;

    __drv->io_setup();
    
    /*Wait for 55ms*/
    __drv->us_delay(55000);
#if defined(LCD_8_BIT_MODE)
    __lcd_write_instruction((__LCD_FUNCTION_SET | __LCD_DL_8_BIT));
    /*Wait for 4.1 ms.*/
    __drv->us_delay(4100);
    __lcd_write_instruction((__LCD_FUNCTION_SET | __LCD_DL_8_BIT));
    /*Wait for 100 us.*/
    __drv->us_delay(100);
    __lcd_write_instruction((__LCD_FUNCTION_SET | __LCD_DL_8_BIT));
    __lcd_write_instruction((__LCD_FUNCTION_SET | __LCD_DL_8_BIT | (__lcd_lines << 3) | (__lcd_dots << 2)));

    /*Display off.*/
    __lcd_write_instruction(__LCD_DISPLAY_ONOFF_CTRL);
    __LCD_TIMEOUT_ASSERT(__busy_bit_read_timeout(__LCD_TIMEOUT_MAX));
    /*Clear display.*/
    __lcd_write_instruction(__LCD_CLEAR_DISPLAY);
    __LCD_TIMEOUT_ASSERT(__busy_bit_read_timeout(__LCD_TIMEOUT_MAX));
    /*Entry mode set.*/
    __lcd_write_instruction(__LCD_ENTRY_MODE_SET);
    __LCD_TIMEOUT_ASSERT(__busy_bit_read_timeout(__LCD_TIMEOUT_MAX));

#elif defined(LCD_4_BIT_MODE)
    __lcd_write_4_bit_init_instruction((__LCD_FUNCTION_SET | __LCD_DL_8_BIT));
    /*Wait for 4.1 ms.*/
    __drv->us_delay(4100);
    __lcd_write_4_bit_init_instruction((__LCD_FUNCTION_SET | __LCD_DL_8_BIT));
    /*Wait for 100 us.*/
    __drv->us_delay(100);
    __lcd_write_4_bit_init_instruction((__LCD_FUNCTION_SET | __LCD_DL_8_BIT));
    
    /*Function set 4 bit and user setting for number of lines and dots.*/
    __lcd_write_4_bit_init_instruction(__LCD_FUNCTION_SET);
    __lcd_write_instruction(__LCD_FUNCTION_SET | (__lcd_lines << 3) | (__lcd_dots << 2));

    /*Display off.*/
    __lcd_write_instruction(0);
    __lcd_write_instruction(__LCD_DISPLAY_ONOFF_CTRL);
    __LCD_TIMEOUT_ASSERT(__busy_bit_read_timeout(__LCD_TIMEOUT_MAX));
    /*Clear display.*/
    __lcd_write_instruction(0);
    __lcd_write_instruction(__LCD_CLEAR_DISPLAY);
    __LCD_TIMEOUT_ASSERT(__busy_bit_read_timeout(__LCD_TIMEOUT_MAX));
    /*Entry mode set.*/
    __lcd_write_instruction(0);
    __lcd_write_instruction(__LCD_ENTRY_MODE_SET);
    __LCD_TIMEOUT_ASSERT(__busy_bit_read_timeout(__LCD_TIMEOUT_MAX));
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
    __lcd_write_instruction(__LCD_CLEAR_DISPLAY);
    __LCD_TIMEOUT_ASSERT(__busy_bit_read_timeout(__LCD_TIMEOUT_MAX));
    return LCD_OK;
}

/**
 * @brief Returns the cursor to the starting point.
 * @param  
 * @return LCD_OK, LCD_ERROR, or LCD_TIMEOUT.
 */
lcd_return_type lcd_return_home(void)
{
    __lcd_write_instruction(__LCD_RETURN_HOME);
    __LCD_TIMEOUT_ASSERT(__busy_bit_read_timeout(__LCD_TIMEOUT_MAX));
    return LCD_OK;
}

/**
 * @brief Entry Mode Set. Sets the cursor to auto increment.
 * @param shift_display Set to true to shift the display when writing.
 * @return LCD_OK, LCD_ERROR, or LCD_TIMEOUT.
 */
lcd_return_type lcd_increment_cursor(bool shift_display)
{
    __lcd_write_instruction(__LCD_ENTRY_MODE_SET | __LCD_ID_INCREMENT | (uint8_t)shift_display);
    __LCD_TIMEOUT_ASSERT(__busy_bit_read_timeout(__LCD_TIMEOUT_MAX));
    return LCD_OK;
}

/**
 * @brief Entry Mode Set. Sets the cursor to auto decrement.
 * @param shift_display Set to true to shift the display when writing.
 * @return LCD_OK, LCD_ERROR, or LCD_TIMEOUT.
 */
lcd_return_type lcd_decrement_cursor(bool shift_display)
{
    __lcd_write_instruction(__LCD_ENTRY_MODE_SET | (uint8_t)shift_display);
    __LCD_TIMEOUT_ASSERT(__busy_bit_read_timeout(__LCD_TIMEOUT_MAX));
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
    uint8_t instr = __LCD_DISPLAY_ONOFF_CTRL;

    if (blink_on)
    {
        instr |= (__LCD_D_ON | __LCD_CURSOR_ON | __LCD_CURSOR_BLINK);
    }
    else if (cursor_on)
    {
        instr |= (__LCD_D_ON | __LCD_CURSOR_ON);
    }
    else if (display_on)
    {
        instr |= __LCD_D_ON;
    }

    __lcd_write_instruction(instr);
    __LCD_TIMEOUT_ASSERT(__busy_bit_read_timeout(__LCD_TIMEOUT_MAX));
    return LCD_OK;
}

/**
 * @brief Cursor or Display Shift. Shift the display to the right.
 * @param  
 * @return LCD_OK, LCD_ERROR, or LCD_TIMEOUT.
 */
lcd_return_type lcd_shift_display_right(void)
{
    __lcd_write_instruction(__LCD_CURSOR_OR_DISPLAY_SHIFT | __LCD_SC_DISPLAY_SHIFT | __LCD_RL_SHIFT_RIGHT);
    __LCD_TIMEOUT_ASSERT(__busy_bit_read_timeout(__LCD_TIMEOUT_MAX));
    return LCD_OK;
}

/**
 * @brief Cursor or Display Shift. Shift the display to the left.
 * @param  
 * @return LCD_OK, LCD_ERROR, or LCD_TIMEOUT.
 */
lcd_return_type lcd_shift_display_left(void)
{
    __lcd_write_instruction(__LCD_CURSOR_OR_DISPLAY_SHIFT | __LCD_SC_DISPLAY_SHIFT);
    __LCD_TIMEOUT_ASSERT(__busy_bit_read_timeout(__LCD_TIMEOUT_MAX));
    return LCD_OK;
}

/**
 * @brief Cursor or Display Shift. Shift the cursor to the right.
 * @param  
 * @return LCD_OK, LCD_ERROR, or LCD_TIMEOUT.
 */
lcd_return_type lcd_shift_cursor_right(void)
{
    __lcd_write_instruction(__LCD_CURSOR_OR_DISPLAY_SHIFT | __LCD_RL_SHIFT_RIGHT);
    __LCD_TIMEOUT_ASSERT(__busy_bit_read_timeout(__LCD_TIMEOUT_MAX));
    return LCD_OK;
}

/**
 * @brief Cursor or Display Shift. Shift the cursor to the left.
 * @param  
 * @return LCD_OK, LCD_ERROR, or LCD_TIMEOUT.
 */
lcd_return_type lcd_shift_cursor_left(void)
{
    __lcd_write_instruction(__LCD_CURSOR_OR_DISPLAY_SHIFT);
    __LCD_TIMEOUT_ASSERT(__busy_bit_read_timeout(__LCD_TIMEOUT_MAX));
    return LCD_OK;
}

/**
 * @brief Moves the cursor to a specific position.
 * @param line Line can be 1 or 2. The amount of lines is selected on initilization.
 * @param position Can be 1-80 if the display is set to 1 line. or 1-40 if the display is set to 2 lines.
 * @return LCD_OK, LCD_ERROR, or LCD_TIMEOUT.
 */
lcd_return_type lcd_set_cursor_position(lcd_line_type line, uint8_t position)
{
    uint8_t pos;
    /*Position cannot be 0.*/
    __LCD_ERROR_ASSERT(position);

    /*Cannot move the cursor to line 2 if it's not there.*/
    __LCD_ERROR_ASSERT(!(line == LCD_LINE_2 && __lcd_lines == LCD_LINE_1));

    /*1 line display cannot have more than 80 positions.*/
    __LCD_ERROR_ASSERT(!(__lcd_lines == LCD_LINE_1 && position > 80));

    /*2 line display cannot have more than 40 positions.*/
    __LCD_ERROR_ASSERT(!(__lcd_lines == LCD_LINE_2 && position > 40));

    /*Calculate ddram depending on the lines and position.*/
    pos = (line == LCD_LINE_1) ? (uint8_t)(position - 1) : (uint8_t)((position - 1) + 0x40U);

    __lcd_write_instruction(__LCD_SET_DDRAM_ADDRESS | (pos & 0x7FU));
    __LCD_TIMEOUT_ASSERT(__busy_bit_read_timeout(__LCD_TIMEOUT_MAX));
    return LCD_OK;
}

/**
 * @brief Gets the current line and position of the cursor.
 * @param line [out] Current line of the cursor. 
 * @param position [out] Current position of the cursor.
 * @return LCD_OK, LCD_ERROR, or LCD_TIMEOUT.
 */
lcd_return_type lcd_get_cursor_position(lcd_line_type* line, uint8_t* position)
{
    __LCD_ERROR_ASSERT(line != NULL);
    __LCD_ERROR_ASSERT(position != NULL);

    uint8_t data = (__lcd_read_bf_ac() & 0x7FU);
    *line = (__lcd_lines == LCD_LINE_1) ? LCD_LINE_1 : ((data & 0xF0) >= 0x40U) ? LCD_LINE_2 : LCD_LINE_1;
    *position = (__lcd_lines == LCD_LINE_1) ? ((data & 0xFF) + 1) : ((data & 0xF) + 1);
    return LCD_OK;
}

/**
 * @brief Creates a custom character.
 * @param dots Can be 8 or 10.
 * @param position For number_of_dots equal to 8, can be 0-7 and for number_of_dots equal to 10 can be 0-4.
 * @param data [in] Array with the design of the custom character.
 * @return LCD_OK, LCD_ERROR, or LCD_TIMEOUT.
 */
lcd_return_type lcd_create_custom_char(lcd_dots_type dots, uint8_t position, uint8_t *data)
{
    __LCD_ERROR_ASSERT(data != NULL);
    uint8_t cgram_address, dot_len;

    /*Number of dots can be 8 or 10.*/
    __LCD_ERROR_ASSERT(dots == LCD_DOTS_5x8 || dots == LCD_DOTS_5x10);

    /*For 8 dots the position address cannot be bigger than 0x7.*/
    __LCD_ERROR_ASSERT(!(dots == LCD_DOTS_5x8 && position > 0x7U));

    /*For 10 dots the position address cannot be bigger than 0x4.*/
    __LCD_ERROR_ASSERT(!(dots == LCD_DOTS_5x10 && position > 0x4U));
   

    cgram_address = (dots == LCD_DOTS_5x8) ? ((position & 0x7U) << 3) : ((position & 0x3U) << 4);
    dot_len = (dots == LCD_DOTS_5x8) ? 8 : 10;

    __lcd_write_instruction(__LCD_SET_CGRAM_ADDRESS | cgram_address);
    __LCD_TIMEOUT_ASSERT(__busy_bit_read_timeout(__LCD_TIMEOUT_MAX));

    while (dot_len--)
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
    __LCD_TIMEOUT_ASSERT(__busy_bit_read_timeout(__LCD_TIMEOUT_MAX));
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