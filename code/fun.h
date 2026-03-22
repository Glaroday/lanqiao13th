#ifndef _fun_h
#define _fun_h
#include "stm32g4xx.h"                  // Device header
extern uint8_t rec_data;
void uart_data_rec(void);
void b1_scan(void);
void b2_scan(void);
void b3_scan(void);
void b4_scan(void);
void led_alert(void);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void led_show(uint8_t led,uint8_t mode);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void lcd_show(void);
#endif
