#ifndef LCD_USER_DEFINES_H
#define LCD_USER_DEFINES_H

#include "<Hardware definitions.h>"

#if !defined(LCD_4_BIT_MODE) && !defined(LCD_8_BIT_MODE)   
    #error "No LCD bit mode is defined."
#endif

#define LCD_TIMEOUT_MAX 5000

/*LCD Screen Pinout mapping*/
/*Port Mapping*/
#define LCD_RS_PORT
#define LCD_RW_PORT
#define LCD_E_PORT
#define LCD_DB0_PORT
#define LCD_DB1_PORT
#define LCD_DB2_PORT
#define LCD_DB3_PORT
#define LCD_DB4_PORT
#define LCD_DB5_PORT
#define LCD_DB6_PORT
#define LCD_DB7_PORT

/*Pin Mapping*/
#define LCD_DB0_PIN
#define LCD_DB1_PIN
#define LCD_DB2_PIN
#define LCD_DB3_PIN
#define LCD_DB4_PIN
#define LCD_DB5_PIN
#define LCD_DB6_PIN
#define LCD_DB7_PIN
#define LCD_RS_PIN
#define LCD_RW_PIN
#define LCD_E_PIN

/*LCD hardware macros need to be defined by user.*/
#if defined(LCD_8_BIT_MODE)
    /*Modify DB0-DB7 to act as inputs.*/
    #define LCD_SET_DB_INPUT()
    /*Modify DB0-DB7 to act as ouputs.*/
    #define LCD_SET_DB_OUTPUT()
    /*Set DB0-DB7 high or low depending on each data bit. */
    #define LCD_DB_WRITE(data)
    /*Read whether DB0-DB7 high or low depending on each data bit.*/
    #define LCD_DB_READ(data)
    /*Initialize all the hardware pinout for 8 bit mode of the lcd.*/
    #define LCD_HARDWARE_INIT() 
#elif defined(LCD_4_BIT_MODE)
    /*Modify DB4-DB7 to act as inputs.*/
    #define LCD_SET_DB_INPUT()
    /*Modify DB4-DB7 to act as ouputs.*/
    #define LCD_SET_DB_OUTPUT()
    /*Set DB4-DB7 high or low depending on each data bit (use high byte). */
    #define LCD_DB_WRITE(data)
    /*Read whether DB4-DB7 high or low depending on each data bit (use high byte).*/
    #define LCD_DB_READ(data)
    /*Initialize all the hardware pinout for 4 bit mode of the lcd.*/
    #define LCD_HARDWARE_INIT() 
#endif

#define LCD_SET_RS(bit)
#define LCD_SET_RW(bit)
#define LCD_SET_E(bit)

#endif /*LCD_USER_DEFINES_H*/