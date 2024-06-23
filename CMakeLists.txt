cmake_minimum_required(VERSION 3.29)

project(LCD_Library DESCRIPTION "LCD Library for 16x2 LCD HD44780U and clones.")

add_library(LCD_Library STATIC
	src/lcd.c
	)

target_include_directories(LCD_Library PUBLIC
	inc
	src
	)
