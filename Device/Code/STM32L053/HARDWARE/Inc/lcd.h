#ifndef __LCD_H__
#define __LCD_H__

#include "stm32l0xx_hal.h"
#include "main.h"

extern LCD_HandleTypeDef hlcd;

typedef enum
{
    POINT_OFF = 0,
    POINT_ON = 1
}Point_Typedef;

typedef enum
{
    Mode_0 = 0,
	Mode_1,
	Mode_2,
	Mode_3,
	Mode_Err = -1
}Mode_Typedef;

typedef enum
{
    MINUS_OFF = 0,
    MINUS_ON = 1
}Minus_Typedef;

#define MCU_LCD_COM0          LCD_RAM_REGISTER0
#define MCU_LCD_COM1          LCD_RAM_REGISTER2
#define MCU_LCD_COM2          LCD_RAM_REGISTER4
#define MCU_LCD_COM3          LCD_RAM_REGISTER6

#define MCU_LCD_SEG0          (1U << MCU_LCD_SEG0_SHIFT)
#define MCU_LCD_SEG1          (1U << MCU_LCD_SEG1_SHIFT)
#define MCU_LCD_SEG2          (1U << MCU_LCD_SEG2_SHIFT)
#define MCU_LCD_SEG3          (1U << MCU_LCD_SEG3_SHIFT)
#define MCU_LCD_SEG4          (1U << MCU_LCD_SEG4_SHIFT)
#define MCU_LCD_SEG5          (1U << MCU_LCD_SEG5_SHIFT)
#define MCU_LCD_SEG6          (1U << MCU_LCD_SEG6_SHIFT)
#define MCU_LCD_SEG7          (1U << MCU_LCD_SEG7_SHIFT)

//#define MCU_LCD_SEG0_SHIFT    0
//#define MCU_LCD_SEG1_SHIFT    1
//#define MCU_LCD_SEG2_SHIFT    2
//#define MCU_LCD_SEG3_SHIFT    3
//#define MCU_LCD_SEG4_SHIFT    4
//#define MCU_LCD_SEG5_SHIFT    5
//#define MCU_LCD_SEG6_SHIFT    6
//#define MCU_LCD_SEG7_SHIFT    7

// 对应MCU的 LCFD_SEGx
#define MCU_LCD_SEG0_SHIFT    15
#define MCU_LCD_SEG1_SHIFT    14
#define MCU_LCD_SEG2_SHIFT    13
#define MCU_LCD_SEG3_SHIFT    12
#define MCU_LCD_SEG4_SHIFT    17
#define MCU_LCD_SEG5_SHIFT    7
#define MCU_LCD_SEG6_SHIFT    8
#define MCU_LCD_SEG7_SHIFT    9

//COM连线是反着的
#define LCD_COM0          MCU_LCD_COM3
#define LCD_COM1          MCU_LCD_COM2
#define LCD_COM2          MCU_LCD_COM1
#define LCD_COM3          MCU_LCD_COM0

#define LCD_SEG0          MCU_LCD_SEG0
#define LCD_SEG1          MCU_LCD_SEG1
#define LCD_SEG2          MCU_LCD_SEG2
#define LCD_SEG3          MCU_LCD_SEG3
#define LCD_SEG4          MCU_LCD_SEG4
#define LCD_SEG5          MCU_LCD_SEG5
#define LCD_SEG6          MCU_LCD_SEG6
#define LCD_SEG7          MCU_LCD_SEG7

#define LCD_DIGIT_COM0              LCD_COM0
#define LCD_DIGIT_COM0_SEG_MASK     ~(LCD_SEG0 | LCD_SEG1 | LCD_SEG2 | LCD_SEG3 | LCD_SEG4 | LCD_SEG5 | LCD_SEG6 | LCD_SEG7)
#define LCD_DIGIT_COM1              LCD_COM1
#define LCD_DIGIT_COM1_SEG_MASK     ~(LCD_SEG0 | LCD_SEG1 | LCD_SEG2 | LCD_SEG3 | LCD_SEG4 | LCD_SEG5 | LCD_SEG6 | LCD_SEG7)
#define LCD_DIGIT_COM2              LCD_COM2
#define LCD_DIGIT_COM2_SEG_MASK     ~(LCD_SEG0 | LCD_SEG1 | LCD_SEG2 | LCD_SEG3 | LCD_SEG4 | LCD_SEG5 | LCD_SEG6 | LCD_SEG7)
#define LCD_DIGIT_COM3              LCD_COM3
#define LCD_DIGIT_COM3_SEG_MASK     ~(LCD_SEG0 | LCD_SEG1 | LCD_SEG2 | LCD_SEG3 | LCD_SEG4 | LCD_SEG5 | LCD_SEG6 | LCD_SEG7)

#define LCD_SEG0_SHIFT          MCU_LCD_SEG0_SHIFT
#define LCD_SEG1_SHIFT          MCU_LCD_SEG1_SHIFT
#define LCD_SEG2_SHIFT          MCU_LCD_SEG2_SHIFT
#define LCD_SEG3_SHIFT          MCU_LCD_SEG3_SHIFT
#define LCD_SEG4_SHIFT          MCU_LCD_SEG4_SHIFT
#define LCD_SEG5_SHIFT          MCU_LCD_SEG5_SHIFT
#define LCD_SEG6_SHIFT          MCU_LCD_SEG6_SHIFT
#define LCD_SEG7_SHIFT          MCU_LCD_SEG7_SHIFT

extern void _Error_Handler(char *, int);
void LCD_Init(void);
void HAL_LCD_MspInit(LCD_HandleTypeDef* lcdHandle);
void HAL_LCD_MspDeInit(LCD_HandleTypeDef* lcdHandle);
void LCD_GLASS_Heartbeat(float src);
void LCD_GLASS_Clear(void);
void LCD_GLASS_BlinkConfig(void);

static void Convert(uint8_t Char, Point_Typedef Point, Minus_Typedef Minus);
static void WriteFloat(float src, Mode_Typedef Mode);


#endif /* __LCD_H__ */
