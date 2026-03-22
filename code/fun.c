#include "headfile.h"
extern TIM_HandleTypeDef htim3;
extern UART_HandleTypeDef huart1;
char text[20];
int b1_state,b1_laststate,b2_state,b2_laststate,b3_state,b3_laststate,b4_state,b4_laststate;
int b1_activate,b2_activate,b3_activate;
int b1_value,b2_value,b3_value;
int password_1 =1;
int password_2 =2;
int password_3 =3;
int led2_state = 0;
int frequency = 1000;
float duty = 0.5;
int alert_state = 0;
int lcd_state;
int wrong_times;
int alert_times;
int lcd_shownow =1;
int rec_flag;
uint8_t rec_data,count;
uint8_t rec_buff[20];
char send_buff[20];
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	if (huart == &huart1){
		TIM4->CNT =0;
		rec_flag =1;
		rec_buff[count] = rec_data;
		count++;
		HAL_UART_Receive_IT(&huart1,&rec_data,1);
	}
	
}
void uart_data_rec(){
	if (rec_flag){
		if (TIM4->CNT > 15){
			if ((rec_buff[0] - '0') == password_1 && (rec_buff[1] - '0') == password_2 && (rec_buff[2] - '0')== password_3){
				password_1 = rec_buff[4] - '0';
				password_2 = rec_buff[5] - '0';
				password_3 = rec_buff[6] - '0';
				sprintf(text,"%d",password_1);
				HAL_UART_Transmit(&huart1,(uint8_t *)text,sizeof(text),50);
			}
			memset(rec_buff,0,sizeof(rec_buff));
			count = 0;
			rec_flag = 0;
		}
	}
	
}

void b1_scan(){
	b1_state = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);
	if (b1_state == 0 && b1_laststate == 1){
		if (b1_activate == 0){
			b1_activate = 1;
			b1_value = 0;
		}else{
			b1_value ++;
		}
	}
	if (b1_value > 9 ){
		b1_value =0;
	}
	b1_laststate = b1_state;
}
void b2_scan(){
	b2_state = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
	if (b2_state == 0 && b2_laststate == 1){
		if (b2_activate == 0){
			b2_activate = 1;
			b2_value = 0;
		}else{
			b2_value ++;
		}
	}
	if (b2_value > 9 ){
		b2_value =0;
	}
	b2_laststate = b2_state;
}
void b3_scan(){
	b3_state = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2);
	if (b3_state == 0 && b3_laststate == 1){
		if (b3_activate == 0){
			b3_activate = 1;
			b3_value = 0;
		}else{
			b3_value ++;
		}
	}
	if (b3_value > 9 ){
		b3_value =0;
	}
	b3_laststate = b3_state;
}

void b4_scan(){
	b4_state = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);
	if (b4_state == 0 && b4_laststate == 1){
		if (b1_value == password_1 && b2_value == password_2 && b3_value == password_3){
			wrong_times = 0;
			lcd_state = 1;
			frequency = 2000;
			duty = 0.1;
			TIM1->CNT = 0;
		}else{
			wrong_times ++;
			b1_activate = 0;
			b2_activate = 0;
			b3_activate = 0;
		}
	}
	b4_laststate = b4_state;
}

void led_show(uint8_t led,uint8_t mode){
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
	if (mode){
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8 << (led - 1),GPIO_PIN_RESET);
	}else{
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8 << (led - 1),GPIO_PIN_SET);
	}
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
}

void led_alert(){
	led_show(1,0);
	if (wrong_times >=3){
		if (alert_times <51){
			alert_state = 1;
		}else{
			alert_state = 0;
			alert_times = 0;
			wrong_times = 0;
			led2_state =0;
		}
	}else{
			alert_state = 0;
			alert_times = 0;
			led2_state =0;
	}
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if (htim == &htim3){
		if (alert_state == 1){
			lcd_shownow=0;
			led_show(1,0);
			led_show(3,0);
			led_show(4,0);
			led_show(5,0);
			led_show(6,0);
			led_show(7,0);
			led_show(8,0);
			if (led2_state){
				led_show(2,1);
				led2_state ++;
				led2_state %=2;
				alert_times ++;
			}else{
				led_show(2,0);
				led2_state ++;
				led2_state %=2;
				alert_times ++;
				}
			lcd_shownow=1;
			}
		}
}


void lcd_show(){
	TIM2->ARR = (100000/frequency) - 1;
	TIM2->CCR2 = (TIM2->ARR + 1) * duty;
	if (lcd_state == 0){
		frequency = 1000;
		duty = 0.5;
		b1_scan();
		b2_scan();
		b3_scan();
		b4_scan();
		if (lcd_shownow == 1){
			sprintf(text,"       PSD");
			LCD_DisplayStringLine(Line1,(uint8_t *)text);

			if (b1_activate == 0){
				sprintf(text,"    B1:@");
				LCD_DisplayStringLine(Line3,(uint8_t *)text);
			}else{
				sprintf(text,"    B1:%d",b1_value);
				LCD_DisplayStringLine(Line3,(uint8_t *)text);
			}
			
			if (b2_activate == 0){
				sprintf(text,"    B2:@");
				LCD_DisplayStringLine(Line4,(uint8_t *)text);
			}else{
				sprintf(text,"    B2:%d",b2_value);
				LCD_DisplayStringLine(Line4,(uint8_t *)text);
			}
			
			if (b3_activate == 0){
				sprintf(text,"    B3:@");
				LCD_DisplayStringLine(Line5,(uint8_t *)text);
			}else{
				sprintf(text,"    B3:%d",b3_value);
				LCD_DisplayStringLine(Line5,(uint8_t *)text);
			}
		}
		led_alert();
	}else{
		sprintf(text,"       STA");
		LCD_DisplayStringLine(Line1,(uint8_t *)text);
		sprintf(text,"    F:%dHz",frequency);
		LCD_DisplayStringLine(Line3,(uint8_t *)text);
		sprintf(text,"    D:%.0f%%",duty * 100);
		LCD_DisplayStringLine(Line4,(uint8_t *)text);
		sprintf(text,"                 ");
		LCD_DisplayStringLine(Line5,(uint8_t *)text);
		sprintf(text,"    TIM:%dHz",TIM1->CNT);
		LCD_DisplayStringLine(Line6,(uint8_t *)text);
		led_show(1,1);
		if (TIM1->CNT >= 50000){
			lcd_state = 0;
			LCD_Clear(Black);
		}
	}
}
