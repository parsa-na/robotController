/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd16x2.h"
/* USER CODE END Includes */
#include "math.h"
/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart3_rx;
DMA_HandleTypeDef hdma_usart3_tx;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART3_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */
enum State{
	WAIT,
	PASSWORD,ERR,



	MENU,
		MENU_RUN,
		LEARNING,
		PARAMETER_SETTING,
		SHOW,
		LOG,
		OTHERS,
		MANUAL_CONTROL,



		MODE1,MODE2, // for learning
			LEARN_QUESTION,
			WAIT_FOR_SENSOR,TIME_TO_WAIT,
			SENSOR_LIST,

		SPEED, // for parameter_setting
		BREAK,

			SPEED_MODE1,
			SPEED_MODE2,


			SPEED_MODE1_CALIBRATION,
			SPEED_MODE1_CYCLE,

			SPEED_MODE2_CALIBRATION,
			SPEED_MODE2_CYCLE,

			BREAK_VERTICAL_ACTUATOR,
			BREAK_HORIZONTAL_ACTUATOR,

			BREAK_VERTICAL_ACTUATOR_UP,
			BREAK_VERTICAL_ACTUATOR_DOWN,

			BREAK_HORIZONTAL_ACTUATOR_FRONT,
			BREAK_HORIZONTAL_ACTUATOR_BACK,

	LOG_WORKING_TIME, LOG_CYCLES,LOG_RESET, // for log
			LOG_RESET_WORKING_TIME
			,LOG_RESET_CYCLES,
	MANUAL_RUN,

	MENU_RUN_COIL1,MENU_RUN_COIL0,MENU_RUN_COIL0_RUN,

		MENU_RUN_COIL0_MODE1,MENU_RUN_COIL0_MODE2,


	RUN,
	};



// DEFINED FUNCTIONS **************************

char get_key();
void set_char(char c);
void set_char_for_sensor(char c);
void set_char_for_sensor_andor(char c);
void set_lcd(enum State s);
void set_speed(char c,uint16_t *num);
uint16_t crc16( uint8_t *buf, uint16_t len );
void send_write_register(uint16_t address, uint16_t data);
void send_read_one_register(uint16_t address);
void send_read_one_coil_show(uint16_t address);
void send_read_one_coil(uint16_t address,uint8_t *data);
uint16_t int_hex(uint16_t data);
//void show_cursor(uint8_t c);
void show_cursor(enum State c);
enum State st;
void run_repeat_command_manual();
void run_repeat_command_learning();
void send_write_coil(uint16_t address, uint16_t data);
void set_counter(uint8_t data);
void send_write_two_register(uint16_t address1,uint16_t address2, uint32_t data);
void set_time_wait(char c,uint32_t *num);
void set_counter32(uint32_t data);
void send_read_two_register(uint16_t address,uint32_t *data);
char get_key_for_sensor_number();
char get_key_for_sensor_andor();

enum State cursor;
uint16_t wait_calib=0;
uint8_t counter=0;
uint8_t show_coil;
char chars[3]={'0','0','0'};
char chars1[10]={'0','0','0','0','0','0','0','0','0','0'};
char PASS[4]={'1','2','3','4'};
uint32_t log_temp=0;
uint8_t counterP=0;
char pass[4]={};


char SENSOR[5]={'1','2','3','4','5'};
char sensor_numbers[5]={};
uint8_t sensor_temp[5]={0,0,0,0,0};
uint8_t sensor_counter=0;
uint8_t sensor_num_cursor=1;
char sensor_andor[4]={};
uint8_t sensor_temp_andor[4]={0,0,0,0};
uint8_t sensor_counter_andor=0;




uint8_t flag=0;
uint8_t col=0;
uint8_t col_t=0;
uint8_t data_back[8];
uint8_t cc=0;
uint8_t mm=1;

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  HAL_Init();
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  st=MENU;
  uint8_t f=0;
  cursor=MENU_RUN;
  uint16_t speed_man=0;
  uint32_t waiting_time=0;
  uint32_t log_waiting_time=0;
    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    lcd16x2_init_4bits(GPIOB, RS_PIN_Pin, E_PIN_Pin, GPIOA, D4_Pin, D5_Pin, D6_Pin, D7_Pin);


   lcd16x2_clear();
  lcd16x2_printf("enter password:");
  lcd16x2_2ndLine();
  //st=WAIT;
      /* Infinite loop */
      /* USER CODE BEGIN WHILE */
    	 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); // row 1 reset
      	 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
      	 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
      	 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
  char key;
      /* Infinite loop */
      /* USER CODE BEGIN WHILE */
      while (1)
       {
  //    	if(st==WAIT){
  //
  //    	}
      	if(st==PASSWORD){
      			key=get_key();
      			if(key=='s'){
      				for(int i=0;i<4;i++){
      					if(pass[i]!=PASS[i]-48){
      						f=1;
      						break;
      					}
      				}
      					if(f!=1){
      						st=MENU;

      						cursor=MENU_RUN;
      					}
      					else{
      						st=ERR;
      					}
      					counterP=0;
      				}
      			else{
      				set_char(key);
      				HAL_Delay(100);
      			}
      		}
      	else if(st== ERR){
      		lcd16x2_clear();
      		lcd16x2_printf("Incorrect!!");
      		key=get_key();
      		if(key=='s'){
      			st=PASSWORD;
      			pass[0]=0;
      			pass[1]=0;
      			pass[2]=0;
      			pass[3]=0;
      			lcd16x2_clear();
      			lcd16x2_printf("Enter Password:");
      			lcd16x2_2ndLine();
      		}
      	}
      	else if(st==MENU){    /// MENU STATE **********************************************************
      			if(col_t==0){
      				lcd16x2_clear();
      				show_cursor(cursor);
      				lcd16x2_cursorShow(0);
      				col_t=1;
      			}

      			key=get_key();
      			if(key=='s'){
      				st=cursor;
      				lcd16x2_clear();
      				if(st==LEARNING)
      					cursor=MODE1;
      				if(st==PARAMETER_SETTING)
      					cursor=SPEED;
      				if(st==LOG)
      					cursor=LOG_WORKING_TIME;

      				col_t=0;
      			}
      			if(key=='d'){
      				cursor++;
      				col_t=0;
      				if(cursor==MANUAL_CONTROL+1)
      					cursor=MENU_RUN;
      			}
      			if(key=='u'){
      				cursor--;
      				col_t=0;
      			    if(cursor==MENU_RUN-1)
      			    	cursor=MANUAL_CONTROL;
      			 }
      		}


    //*********************************   MENU_RUN  **********************************************
      	else if(st==MENU_RUN){
      		uint8_t coil;
      		send_read_one_coil(0xa0d, &coil);//0xa0d
      		if(coil==0){
      			st==MENU_RUN_COIL0;
      			cursor=MENU_RUN_COIL0_RUN;
      		}
      		else if(coil==1){
      			st=MENU_RUN_COIL1;
      		}
      	}
      	else if(st==MENU_RUN_COIL0){
      		if(col_t==0){
      			lcd16x2_clear();
      			show_cursor(cursor);
      			lcd16x2_cursorShow(0);
      			col_t=1;
      		}
      		key=get_key();
      		if(key=='d'){
      			cursor++;
      			col_t=0;
      			if(cursor==MENU_RUN_COIL0_MODE2+1)
      				cursor=MENU_RUN_COIL0_RUN;
      		}
      		if(key=='u'){
      			cursor--;
      			col_t=0;
      			if(cursor==MENU_RUN_COIL0_RUN-1)
      				cursor=MENU_RUN_COIL1_MODE2;
      		}
      		if(key=='s'){
      			if(cursor==MENU_RUN_COIL0_RUN){
      				send_write_coil(0xa0d, 0xff00);
      				st=MENU_RUN_COIL1;
      			}else{
      				st=cursor;
      			}
      			col_t=0;
      		}
      		if(key=='m'){
      			col_t=0;
      			st=MENU;
      			cursor=MENU_RUN;
      		}

      	}
      	else if(st==MENU_RUN_COIL1){
      		if(col_t==0){
      			lcd16x2_clear();
      			lcd16x2_1stLine("Stop");
      			col_t=1;
      		}
      		key=get_key();
      		if(key=='s'){
      			send_write_coil(0xa0d, 0x00);
      			st=MENU_RUN_COIL0;
      			col_t=0;
      		}
      		if(key=='m'){
      			col_t=0;
      			st=MENU;
      			cursor=MENU_RUN;
      		}
      	}
      	else if(st==MENU_RUN_COIL0_MODE1){
      		uint32_t cycles;
      		if(col_t==0){
      			lcd16x2_clear();
      			lcd16x2_1stLine("Enter Cycles:");
      			lcd16x2_2ndLine();
      			col_t=1;
      		}
      		key=get_key(); //0x11b8
      		if(key=='s'){ // 0

      			send_write_two_register(0x11b9, 0x11b8, cycles);
      			st=MENU_RUN_COIL0;
      			cursor=MENU_RUN_COIL0_RUN;
      			col_t=0;
      		}
      	  set_time_wait(char c,uint32_t *num);
      	}





      	else if(st==LEARNING){  /// LEARNING STATE ****************************************************
      			if(col_t==0){
      			    lcd16x2_clear();
      			    show_cursor(cursor);
      			    lcd16x2_cursorShow(0);
      			    col_t=1;
      			}
      			key=get_key();
      			if(key=='d'){
      				cursor++;
      				col_t=0;
      			    if(cursor==MODE2+1)
      			    	cursor=MODE1;
      			}
      			 if(key=='u'){
      			   cursor--;
      			   col_t=0;
      			   if(cursor==MODE1-1)
      			    	cursor=MODE2;
      			 }
      			 if(key=='s'){
      				 st=cursor;
      				 col_t=0;
      			 }
      			 if(key=='m'){
      				 col_t=0;
      			     st=MENU;
      			     cursor=MENU_RUN;
      			 }
      		}

      	else if(st==MODE1){  /// MODE1  *******************************************************
      		if(cc==0){
      			send_write_coil(0x9fe, 0xff00);
      	//	if(mm==1){
      			lcd16x2_clear();
      			lcd16x2_printf("move and");
      			lcd16x2_2ndLine();
      			lcd16x2_printf("set points  ");

      			lcd16x2_cursorShow(0);
      			if(cc==0){
      			send_read_one_register(0x1006);
      			cc++;
      			}
      			set_counter(counter);
      		}
      			run_repeat_command_learning();


//      		}

      		}
      	else if(st==LEARN_QUESTION){
      			lcd16x2_clear();
      			show_cursor(cursor);
      			key=get_key();
      			if(key=='d'){
      				cursor++;
      				if(cursor==WAIT_FOR_SENSOR+1)
      				cursor=TIME_TO_WAIT;
      			}
      			if(key=='u'){
      			    cursor--;
      			   if(cursor==TIME_TO_WAIT-1)
      			    cursor=WAIT_FOR_SENSOR;
      			}
      			if(key=='s'){
      				st=cursor;
      				lcd16x2_clear();
      				//TO DO
      			}
      			if(key=='m'){
      			   st=MODE1;
      			   send_write_coil(0x09e0, 0x0);
      			   cursor=MODE1;
      			}
      		}




      	else if(st==TIME_TO_WAIT){
      			if(col_t==0){
      			    lcd16x2_clear();
      			    lcd16x2_printf("wait time(0-72000):");
      			    lcd16x2_2ndLine();
      			}
      			else{

      			   lcd16x2_setCursor(2, col_t);
      			}

      			key=get_key();
      			if(key=='s'){//speed_man=40;
      			   if( waiting_time>72000){

      			    					// to_Do

      			   }
      			   else{

      				   send_write_two_register(0x100B,0x100A,waiting_time);
      				   waiting_time=0;
      				  lcd16x2_printf("  saved!!");
      			   }
      			 }
      			else if(key=='m'){
      				 st=LEARN_QUESTION;
      				 cursor=TIME_TO_WAIT;
      				 col_t=0;
      			}
      			 else{
      			    set_time_wait(key, &waiting_time);
      			 }

      		}

      	else if(st==WAIT_FOR_SENSOR){

      		if(sensor_num_cursor%2==1){
      			key=get_key_for_sensor_number();
      			if(key=='m'){
      				st=LEARN_QUESTION;
      				col_t=0;
      				cursor=TIME_TO_WAIT;
      				sensor_num_cursor=1;

      				// char sensor_numbers[5]={};
      				for(uint8_t i=0;i<5;i++){
      					sensor_temp[i]=0;
      					if(i<4)
      						sensor_temp_andor[i]=0;
      				}
      				sensor_counter=0;
      				sensor_counter_andor=0;
      			}



      			if(key=='r'){
      				st=WAIT_FOR_SENSOR;
      				sensor_num_cursor=1;
      				for(uint8_t i=0;i<5;i++){
      					sensor_temp[i]=0;
      					if(i<4)
      						sensor_temp_andor[i]=0;
      				}
      				sensor_counter=0;
      				sensor_counter_andor=0;
      				lcd16x2_clear();
      			}
      			set_char_for_sensor(key);


      		}


      		if(sensor_num_cursor%2==0){
      			key=get_key_for_sensor_andor();
      			if(key=='m'){
      				st=LEARN_QUESTION;
      				col_t=0;
      				cursor=TIME_TO_WAIT;
      				sensor_num_cursor=1;

      				// char sensor_numbers[5]={};
      				for(uint8_t i=0;i<5;i++){
      					sensor_temp[i]=0;
      					if(i<4)
      						sensor_temp_andor[i]=0;
      				}
      				sensor_counter=0;
      				sensor_counter_andor=0;
      			}

      			if(key=='s'){
      				uint32_t sum_t=0;
      				uint16_t sum_t_andor=0;
      				for(uint8_t i=0;i<5;i++){
      					sum_t+=sensor_temp[i]*((uint32_t)pow(10,4-i));
      					if(i<4){
      						sum_t_andor+=sensor_temp_andor[i]*((uint16_t)pow(10,3-i));
      					}
      				}
      				send_write_two_register(0x100d, 0x100c, sum_t);
      				send_write_register(0x100e, sum_t_andor);
      				lcd16x2_2ndLine();
      				lcd16x2_printf("saved!!");
      			}







      			if(key=='r'){
      				st=WAIT_FOR_SENSOR;
      				sensor_num_cursor=1;

      				// char sensor_numbers[5]={};
      				for(uint8_t i=0;i<5;i++){
      					sensor_temp[i]=0;
      					if(i<4)
      						sensor_temp_andor[i]=0;
      				}
      				sensor_counter=0;
      				sensor_counter_andor=0;
      				lcd16x2_clear();
      			}
      			set_char_for_sensor_andor(key);
      		}

      		}


      	else if(st==PARAMETER_SETTING){  /// LEARNING STATE ****************************************************
      			if(col_t==0){
      				lcd16x2_clear();
      				show_cursor(cursor);
      				lcd16x2_cursorShow(0);
      				col_t=1;
      			}
      			key=get_key();
      			if(key=='d'){
      				cursor++;
      				col_t=0;
      				if(cursor==BREAK+1)
      					cursor=SPEED;
      			}
      			if(key=='u'){
      				cursor--;
      				col_t=0;
      				if(cursor==SPEED-1)
      					cursor=BREAK;
      			}
      			if(key=='s'){
      				st=cursor;
      				col_t=0;
      				if(st==SPEED)
      					cursor=SPEED_MODE1;
      				if(st==BREAK)
      					cursor=BREAK_VERTICAL_ACTUATOR;
      			}
      			if(key=='m'){
      				st=MENU;
      				col_t=0;
      				cursor=MENU_RUN;
      			}
      		}
      	else if(st==SPEED){  /// LEARNING STATE ****************************************************
      			if(col_t==0){
      				lcd16x2_clear();
      				show_cursor(cursor);
      				lcd16x2_cursorShow(0);
      				col_t=1;
      			}
      			key=get_key();
      			if(key=='d'){
      				cursor++;
      				col_t=0;
      				if(cursor==SPEED_MODE2+1)
      					cursor=SPEED_MODE1;
      			}
      			if(key=='u'){
      				cursor--;
      				col_t=0;
      				if(cursor==SPEED_MODE1-1)
      					cursor=SPEED_MODE2;
      			}
      			if(key=='s'){
      				st=cursor;
      				col_t=0;
      				if(st==SPEED_MODE1)
      					cursor=SPEED_MODE1_CALIBRATION;
      				if(st==SPEED_MODE2)
      					cursor=SPEED_MODE2_CALIBRATION;
      			}
      			if(key=='m'){
      				st=PARAMETER_SETTING;
      				col_t=0;
      				cursor=SPEED;
      			}
      		}
      	else if(st==SPEED_MODE1){  /// SPEED MODE1 STATE ****************************************************
      			if(col_t==0){
      				lcd16x2_clear();
      				show_cursor(cursor);
      				lcd16x2_cursorShow(0);
      				col_t=1;
      			}
      			key=get_key();
      			if(key=='d'){
      				cursor++;
      				col_t=0;
      				if(cursor==SPEED_MODE1_CYCLE+1)
      					cursor=SPEED_MODE1_CALIBRATION;
      			}
      			if(key=='u'){
      				cursor--;
      				col_t=0;
      				if(cursor==SPEED_MODE1_CALIBRATION-1)
      					cursor=SPEED_MODE1_CYCLE;
      			}
      			if(key=='s'){
      				st=cursor;
      				col_t=0;
      			}
      			if(key=='m'){
      				col_t=0;
      				st=SPEED;
      				cursor=SPEED_MODE1;
      			}
      		}


      	else if(st==SPEED_MODE1_CALIBRATION){
      		if(col==0){
      			lcd16x2_clear();
      			lcd16x2_printf("speed(6-40):");
      			lcd16x2_2ndLine();
      		}
      		else{
      			lcd16x2_setCursor(2, col);
      			lcd16x2_cursorShow(1);
      		}
      		key=get_key();
      		if(key=='s'){//speed_man=40;
      			if( speed_man>40 || speed_man<6){

      			    					// to_Do
      			}
      			else{
      				send_write_register(0x119B,speed_man);
      				lcd16x2_printf("   saved!!");
      			}
      		}
      		else if(key=='m'){
      			col_t=0;
      			col=0;
      			st=SPEED_MODE1;
      			cursor=SPEED_MODE1_CALIBRATION;
      			speed_man=0;
      		}
      		else{
      			set_speed(key, &speed_man);
      		}
      	}



      	else if(st==SPEED_MODE1_CYCLE){
      		if(col==0){
      			lcd16x2_clear();
      			lcd16x2_printf("speed(6-40):");
      			lcd16x2_2ndLine();
      		}
      		else{
      			lcd16x2_setCursor(2, col);
      			lcd16x2_cursorShow(1);
      		}
      		key=get_key();
      		if(key=='s'){//speed_man=40;
      			if( speed_man>40 || speed_man<6){

      		    			    					// to_Do
      			}
      			else{
      				send_write_register(0x119c,speed_man);
      				lcd16x2_printf("   saved!!");
      			}
      		}
      		else if(key=='m'){
      			col_t=0;
      			col=0;
      			st=SPEED_MODE1;
      			cursor=SPEED_MODE1_CALIBRATION;
      			speed_man=0;
      		    			    		    			}
      		else{
      			set_speed(key, &speed_man);
      		}
      	}
      	else if(st==SPEED_MODE2){  /// SPEED MODE2 STATE ****************************************************
      		if(col_t==0){
      			lcd16x2_clear();
      			show_cursor(cursor);
      			lcd16x2_cursorShow(0);
      			col_t=1;
      		}
      		key=get_key();
      		if(key=='d'){
      			cursor++;
      			col_t=0;
      			if(cursor==SPEED_MODE2_CYCLE+1)
      				cursor=SPEED_MODE2_CALIBRATION;
      		}
      		if(key=='u'){
      			cursor--;
      			col_t=0;
      			if(cursor==SPEED_MODE2_CALIBRATION-1)
      				cursor=SPEED_MODE2_CYCLE;
      		}
      		if(key=='s'){
      			st=cursor;
      			col_t=0;
      		}
      		if(key=='m'){
      			col_t=0;
      			st=SPEED;
      			cursor=SPEED_MODE1;
      		}
      	}



      	else if(st==SPEED_MODE2_CALIBRATION){
      		if(col==0){
      			lcd16x2_clear();
      			lcd16x2_printf("speed(6-40):");
      			lcd16x2_2ndLine();
      		}
      		else{
      			lcd16x2_setCursor(2, col);
      			lcd16x2_cursorShow(1);
      		}
      		key=get_key();
      		if(key=='s'){//speed_man=40;
      			if( speed_man>40 || speed_man<6){
      		    			    					// to_Do
      			}
      			else{
      				send_write_register(0x119D,speed_man);
      				lcd16x2_printf("   saved!!");
      			}
      		}
      		else if(key=='m'){
      			col_t=0;
      			col=0;
      			st=SPEED_MODE2;
      			cursor=SPEED_MODE2_CALIBRATION;
      			speed_man=0;
      		}
      		else{
      			set_speed(key, &speed_man);
      		}
      	}



      	else if(st==SPEED_MODE2_CYCLE){
      		if(col==0){
      			lcd16x2_clear();
      			lcd16x2_printf("speed(6-40):");
      			lcd16x2_2ndLine();
      		}
      		else{
      			lcd16x2_setCursor(2, col);
      			lcd16x2_cursorShow(1);
      		}
      		key=get_key();
      		if(key=='s'){//speed_man=40;
      			if( speed_man>40 || speed_man<6){

      		    		    			    					// to_Do
      			}
      			else{
      				send_write_register(0x119e,speed_man);
      				lcd16x2_printf("   saved!!");
      			}
      		}
      		else if(key=='m'){
      			col_t=0;
      			col=0;
      			st=SPEED_MODE2;
      			cursor=SPEED_MODE2_CALIBRATION;
      			speed_man=0;
      		}
      		else{
      			set_speed(key, &speed_man);
      		}

      	}

      	else if(st==BREAK){  /// LEARNING STATE ****************************************************
      		if(col_t==0){
      			lcd16x2_clear();
      			show_cursor(cursor);
      			lcd16x2_cursorShow(0);
      			col_t=1;
      		}
      		key=get_key();
      		if(key=='d'){
      			cursor++;
      			col_t=0;
      			if(cursor==BREAK_HORIZONTAL_ACTUATOR+1)
      				cursor=BREAK_VERTICAL_ACTUATOR;
      		}
      		if(key=='u'){
      			cursor--;
      			col_t=0;
      			if(cursor==BREAK_VERTICAL_ACTUATOR-1)
      				cursor=BREAK_HORIZONTAL_ACTUATOR;
      		}
      		if(key=='s'){
      			st=cursor;
      			col_t=0;
      			if(st==BREAK_VERTICAL_ACTUATOR)
      				cursor=BREAK_VERTICAL_ACTUATOR_UP;
      			if(st==BREAK_HORIZONTAL_ACTUATOR)
      				cursor=BREAK_HORIZONTAL_ACTUATOR_FRONT;
      		}
      		if(key=='m'){
      			col_t=0;
      			st=PARAMETER_SETTING;
      			cursor=SPEED;
      		}
      	}

      	else if(st==BREAK_VERTICAL_ACTUATOR){
      		if(col_t==0){
      			lcd16x2_clear();
      			show_cursor(cursor);
      			lcd16x2_cursorShow(0);
      			col_t=1;
      		}
      		key=get_key();
      		if(key=='d'){
      			cursor++;
      			col_t=0;
      			if(cursor==BREAK_VERTICAL_ACTUATOR_DOWN+1)
      				cursor=BREAK_VERTICAL_ACTUATOR_UP;
      		}
      		if(key=='u'){
      			cursor--;
      			col_t=0;
      			if(cursor==BREAK_VERTICAL_ACTUATOR_UP-1)
      				cursor=BREAK_VERTICAL_ACTUATOR_DOWN;
      		}
      		if(key=='s'){
      			st=cursor;
      			col_t=0;
      		}
      		if(key=='m'){
      			col_t=0;
      			st=BREAK;
      			cursor=BREAK_VERTICAL_ACTUATOR;
      		}
      	}
      	else if(st==BREAK_VERTICAL_ACTUATOR_UP){
      			if(col==0){
      			    lcd16x2_clear();
      			    lcd16x2_printf("breaking delay(0-20)(0.1ms):");
      			   	lcd16x2_2ndLine();
      			}
      			else{
      			    lcd16x2_setCursor(2, col);
      			    lcd16x2_cursorShow(1);
      			}
      			key=get_key();
      			if(key=='s'){//speed_man=40;
      			    if( speed_man>20 || speed_man<0){

      			    		    		    			    					// to_Do
      			    }
      			    else{
      			    	send_write_register(0x11aa,speed_man);
      			    	lcd16x2_printf("   saved!!");
      			    }
      			}
      			else if(key=='m'){
      			    col_t=0;
      			    col=0;
      			    st=BREAK_VERTICAL_ACTUATOR;
      			    cursor=BREAK_VERTICAL_ACTUATOR_UP;
      			    speed_man=0;
      			}
      			else{
      			    set_speed(key, &speed_man);
      			}


      		}

      	else if(st==BREAK_VERTICAL_ACTUATOR_DOWN){
      			if(col==0){
      				lcd16x2_clear();
      				lcd16x2_printf("breaking delay(0-20)(0.1ms):");
      				lcd16x2_2ndLine();
      			}
      			else{
      				lcd16x2_setCursor(2, col);
      				lcd16x2_cursorShow(1);
      			}
      			key=get_key();
      			if(key=='s'){//speed_man=40;
      				if( speed_man>20 || speed_man<0){
      					// to_Do
      					}
      				else{
      					send_write_register(0x11ab,speed_man);
      					lcd16x2_printf("   saved!!");
      				}
      			}
      			else if(key=='m'){
      				col_t=0;
      				col=0;
      				st=BREAK_VERTICAL_ACTUATOR;
      				cursor=BREAK_VERTICAL_ACTUATOR_UP;
      				speed_man=0;
      			}
      			else{
      				set_speed(key, &speed_man);
      			}
      		}

      	else if(st==BREAK_HORIZONTAL_ACTUATOR){
      		if(col_t==0){
      			lcd16x2_clear();
      			show_cursor(cursor);
      			lcd16x2_cursorShow(0);
      			col_t=1;
      		}
      		key=get_key();
      		if(key=='d'){
      			cursor++;
      			col_t=0;
      			if(cursor==BREAK_HORIZONTAL_ACTUATOR_BACK+1)
      				cursor=BREAK_HORIZONTAL_ACTUATOR_FRONT;
      		}
      		if(key=='u'){
      			cursor--;
      			col_t=0;
      			if(cursor==BREAK_HORIZONTAL_ACTUATOR_FRONT-1)
      				cursor=BREAK_HORIZONTAL_ACTUATOR_BACK;
      		}
      		if(key=='s'){
      			st=cursor;
      			col_t=0;
      		}
      		if(key=='m'){
      			col_t=0;
      			st=BREAK;
      			cursor=BREAK_VERTICAL_ACTUATOR;
      		}
      	}

      	else if(st==BREAK_HORIZONTAL_ACTUATOR_FRONT){
      			if(col==0){
      				lcd16x2_clear();
      				lcd16x2_printf("breaking delay(0-20)(0.1ms):");
      				lcd16x2_2ndLine();
      				lcd16x2_cursorShow(1);
      			}
      			else{
      				lcd16x2_setCursor(2, col);

      			}
      			key=get_key();
      			if(key=='s'){//speed_man=40;
      				if( speed_man>20 || speed_man<0){
      					// to_Do
      					}
      				else{
      					send_write_register(0x11ac,speed_man);
      					lcd16x2_printf("   saved!!");
      				}
      			}
      			else if(key=='m'){
      				col_t=0;
      				col=0;
      				st=BREAK_HORIZONTAL_ACTUATOR;
      				cursor=BREAK_HORIZONTAL_ACTUATOR_FRONT;
      				speed_man=0;
      			}
      			else{
      				set_speed(key, &speed_man);
      			}
      		}

      	else if(st==BREAK_HORIZONTAL_ACTUATOR_BACK){
      			if(col==0){
      				lcd16x2_clear();
      				lcd16x2_printf("breaking delay(0-20)(0.1ms):");
      				lcd16x2_2ndLine();
      			}
      			else{
      				lcd16x2_setCursor(2, col);
      				lcd16x2_cursorShow(1);
      			}
      			key=get_key();
      			if(key=='s'){//speed_man=40;
      				if( speed_man>20 || speed_man<0){
      					// to_Do
      					}
      				else{
      					send_write_register(0x11ad,speed_man);
      					lcd16x2_printf("   saved!!");
      				}
      			}
      			else if(key=='m'){
      				col_t=0;
      				col=0;
      				st=BREAK_HORIZONTAL_ACTUATOR;
      				cursor=BREAK_HORIZONTAL_ACTUATOR_FRONT;
      				speed_man=0;
      			}
      			else{
      				set_speed(key, &speed_man);
      			}
      		}

  //####################################  LOG #####################################################################

      	else if(st==LOG){
      		if(col_t==0){
      			lcd16x2_clear();
      			show_cursor(cursor);
      			lcd16x2_cursorShow(0);
      			col_t=1;
      		}
      		key=get_key();
      		if(key=='d'){
      			cursor++;
      			col_t=0;
      			if(cursor==LOG_RESET+1)
      				cursor=LOG_WORKING_TIME;
      		}
      		if(key=='u'){
      			cursor--;
      			col_t=0;
      			if(cursor==LOG_WORKING_TIME-1)
      				cursor=LOG_RESET;
      		}
      		if(key=='s'){
      			st=cursor;
      			if(st==LOG_RESET)
      				cursor=LOG_RESET_WORKING_TIME;
      			col_t=0;
      		}
      		if(key=='m'){
      			st=MENU;
      			col_t=0;
      			cursor=MENU_RUN;
      		}

      	}
      	else if(st==LOG_WORKING_TIME){

      		send_read_two_register(0x11b0,&log_waiting_time);
      		if(log_waiting_time!=log_temp||col_t==0){
      			lcd16x2_clear();
      			lcd16x2_printf("working time");
      			lcd16x2_cursorShow(0);
      			lcd16x2_2ndLine();
      			log_temp=log_waiting_time;
      			set_counter32(log_waiting_time);
      			lcd16x2_printf(" hours");
      			col_t=1;
      		}

      		key=get_key();
      		if(key=='m'){
      			st=LOG;
      			col_t=0;
      			cursor=LOG_WORKING_TIME;
      			log_waiting_time=0;
      		}
      	}

      	else if(st==LOG_CYCLES){
      		send_read_two_register(0x11b6,&log_waiting_time);
      		if(log_waiting_time!=log_temp|| col_t==0){
      			lcd16x2_clear();
      			lcd16x2_printf("cycles ");
      			lcd16x2_cursorShow(0);
      			lcd16x2_2ndLine();
      			log_temp=log_waiting_time;
      			set_counter32(log_waiting_time);
      			col_t=1;
      		}

      		key=get_key();
      		if(key=='m'){
      			st=LOG;
      			col_t=0;
      			cursor=LOG_WORKING_TIME;
      			log_waiting_time=0;
      		}
      	}
      	else if(st==LOG_RESET){
      		if(col_t==0){
      			show_cursor(cursor);
      			lcd16x2_cursorShow(0);
      			col_t=1;
      		}


      		key=get_key();
      		if(key=='m'){
      			st=LOG;
      			col_t=0;
      			cursor=LOG_RESET;
      			log_waiting_time=0;
      		}
      		if(key=='d'){
      			cursor++;
      			col_t=0;
      			if(cursor==LOG_RESET_CYCLES+1)
      				cursor=LOG_RESET_WORKING_TIME;
      		}
      		if(key=='u'){
      			cursor--;
      			col_t=0;
      			if(cursor==LOG_RESET_WORKING_TIME-1)
      				cursor=LOG_RESET_CYCLES;
      		}
      		if(key=='s'){
      			st=cursor;
      			col_t=0;
      		}
      	}




      	else if(st==LOG_RESET_WORKING_TIME){
      		send_write_register(0x11b0, 0x00);
      		send_write_register(0x11b1, 0x00);
      		send_write_register(0x11b2, 0x00);
      		send_write_register(0x11b3, 0x00);
      		if(col_t==0){
      			lcd16x2_clear();
      			lcd16x2_cursorShow(0);
      			lcd16x2_printf("DONE!!");
      			col_t=1;
      		}
      		key=get_key();
      		if(key=='m'){
      			st=LOG_RESET;
      			col_t=0;
      			cursor=LOG_RESET_WORKING_TIME;
      			log_waiting_time=0;
      			log_temp=0;
      		}
      	}


      	else if(st==LOG_RESET_CYCLES){
      		send_write_register(0x11b6, 0x00);
      		send_write_register(0x11b7, 0x00);
      		if(col_t==0){
      			lcd16x2_clear();
      			lcd16x2_cursorShow(0);
      			lcd16x2_printf("DONE!!");
      			col_t=1;
      		}
      		key=get_key();
      		if(key=='m'){
      			st=LOG_RESET;
      			col_t=0;
      			cursor=LOG_RESET_WORKING_TIME;
      			log_waiting_time=0;
      			log_temp=0;
      		}
      	}
 //#####################################  SHOW #################################################################
      	else if(st==SHOW){

      		send_read_one_coil_show(0xA07);
      			if(col_t==0){
      				if(show_coil==1){
      					lcd16x2_clear();
      					lcd16x2_printf("STOP");
      				}
      				else if(show_coil==0){
      					lcd16x2_clear();
      					lcd16x2_printf("RUN");
      				}
      				col_t=1;
      			}

      			key=get_key();
      			if(key=='m'){
      				st=MENU;
      				col_t=0;
      				cursor=LEARNING;
      			}
      			if(key=='s'){
      				if(show_coil==1)
      					send_write_coil(0xA07, 0x00);
      				else if(show_coil==0)
      					send_write_coil(0xA07, 0xff00);
      				col_t=0;;
      			}
      	}



  //#####################################   MANUAL CONTROL STATE  ##################################################

      	else if(st==MANUAL_CONTROL){ ///   MANULA CONTROL *********************************************
      		if(col==0){
      			lcd16x2_clear();
      			lcd16x2_printf("speed(2-40):");
      			lcd16x2_2ndLine();
      		}
      		else{
      			//lcd16x2_2ndLine();
      			lcd16x2_setCursor(2, col);
      		}
      			key=get_key();
      			if(key=='s'){//speed_man=40;
      				if( speed_man>40 || speed_man<2){

      					// to_Do

      				}
      				else{
      					speed_man=speed_man*100;
      					send_write_register(0x119A,speed_man);
      					st=MANUAL_RUN;

      				}
      			}
      			else{
      				set_speed(key, &speed_man);
      			}
      		}

      	else if(st==MANUAL_RUN){
      			send_write_coil(0x9ff, 0xff00);
      			lcd16x2_clear();
      			lcd16x2_printf("send command:");
      			run_repeat_command_manual();
      		}


  HAL_Delay(50);
       }


  //key=get_key();
  //set_char(key);
  //HAL_Delay(100);


  }
       /* USER CODE END 3 */
  void run_repeat_command_manual(){

  	 	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); // row 1 reset
  		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
  		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
  		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);



  		  if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3)==GPIO_PIN_SET){ //f1
  		 	while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_3)==GPIO_PIN_SET);
  		  	return ;

  		  }
  		  if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4)==GPIO_PIN_SET ){ // 3
  			  		send_write_coil(0x09f5, 0xff00);
  			  		while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_4)==GPIO_PIN_SET);
  			  		send_write_coil(0x09f5, 0x0000);
  			   return;

  		   }


  		   if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5)==GPIO_PIN_SET){ // 2
  			   while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_5)==GPIO_PIN_SET);
  			   send_write_coil(0x09f6, 0xff00);
  			   return ;
  		    }

  		   if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6)==GPIO_PIN_SET){ // 1
  			   send_write_coil(0x09f4, 0xff00);
  		   	   while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_6)==GPIO_PIN_SET);
  		   	   send_write_coil(0x09f4, 0x0000);
  		   	   return ;
  		   }


  		   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); // row 2 reset
  		   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
  		   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
  		   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);

  		   if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3)==GPIO_PIN_SET){ //up
  		    	while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_3)==GPIO_PIN_SET);
  		    	return ;
  		     }

  		     if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4)==GPIO_PIN_SET ){ // 6
  		    	 while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_4)==GPIO_PIN_SET );
  		    	 send_write_coil(0x9f8, 0xff00);
  		    	 return ;
  		     }
  		    if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5)==GPIO_PIN_SET){ // 5
  		    	while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_5)==GPIO_PIN_SET);
  		    	send_write_coil(0x09fc, 0xff00);
  		    	return;
  		       }

  		      if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6)==GPIO_PIN_SET){ // 4
  		    	  while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_6)==GPIO_PIN_SET);
  		    	  send_write_coil(0x9f9, 0xff00);
  		    	  return ;
  		      }
  		      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);//row 3 reset
  		      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
  		      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
  		      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);



  		   if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3)==GPIO_PIN_SET){ //down
  		    	while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_3)==GPIO_PIN_SET);
  		    	return;

  		     }
  		      if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4)){ // 9
  		    	  while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_4));
  		    	  send_write_coil(0x9fB, 0xff00);
  		   	  	  return ;
  		     }
  		      if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5)==GPIO_PIN_SET){ // 8
  		    	  while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_5)==GPIO_PIN_SET);
  		    	  send_write_coil(0x9f7, 0xff00);
  		     	  return ;
  		       }

  		      if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6)==GPIO_PIN_SET){ // 7
  		    	  while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_6)==GPIO_PIN_SET);
  		    	  send_write_coil(0x9fA, 0xff00);
  		      	  return;
  		        }
  		      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);//row 4 reset
  		      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
  		      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
  		      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);



  			   if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3)==GPIO_PIN_SET){ //menu
  				   send_write_coil(0x9ed, 0xff00);
  				   while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_3)==GPIO_PIN_SET);
  				   send_write_coil(0x9ed, 0x00);
  				   send_write_coil(0x9ff, 0x00);
  				   st=MENU;
  				   return;

  			     }

  		      if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4)==GPIO_PIN_SET ){ // reset
  		   	   while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_4)==GPIO_PIN_SET);
  		   	   return ;

  		     }
  		      if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5)==GPIO_PIN_SET){ // 0
  		    	  while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_5)==GPIO_PIN_SET);
  		     	  send_write_coil(0x09fd, 0xff00);
  		     	  return ;

  		       }

  		      if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6)==GPIO_PIN_SET){ // set
  		      	   while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_6)==GPIO_PIN_SET);
  		      	   return ;
  		      }
  }

  void run_repeat_command_learning(){

  	 	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); // row 1 reset
  		   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
  		   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
  		   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);



  		  if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3)==GPIO_PIN_SET){ //f1
  			  while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_3)==GPIO_PIN_SET);
  			  send_write_coil(0x09EA, 0xff00);
  			  send_write_coil(0x09EA, 0x0000);
  			  st=LEARN_QUESTION;
  			  send_write_coil(0x09E0, 0xff00);
  			  cursor=TIME_TO_WAIT;
  			  return ;
  		  }
  		  if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4)==GPIO_PIN_SET ){ // 3
  			  		send_write_coil(0x09f5, 0xff00);
  			  		while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_4)==GPIO_PIN_SET);
  			  		send_write_coil(0x09f5, 0x0000);
  			   return;

  		   }


  		   if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5)==GPIO_PIN_SET){ // 2
  			   while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_5)==GPIO_PIN_SET);
  			   send_write_coil(0x09f6, 0xff00);
  			   return ;
  		    }

  		   if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6)==GPIO_PIN_SET){ // 1
  			   send_write_coil(0x09f4, 0xff00);
  			   while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_6)==GPIO_PIN_SET);
  			   send_write_coil(0x09f4, 0x0000);
  		   	   return ;
  		   }


  		   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); // row 2 reset
  		   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
  		   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
  		   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);

  		   if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3)==GPIO_PIN_SET){ //up
  			   while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_3)==GPIO_PIN_SET);
  			   send_write_coil(0x9eb, 0xff00);
  			   send_write_coil(0x9eb, 0x00);
  			   return ;
  		     }

  		     if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4)==GPIO_PIN_SET ){ // 6
  		    	 while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_4)==GPIO_PIN_SET );
  		    	 send_write_coil(0x9f8, 0xff00);
  		    	 return ;
  		     }
  		    if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5)==GPIO_PIN_SET){ // 5
  		    	while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_5)==GPIO_PIN_SET);
  		    	send_write_coil(0x09fc, 0xff00);
  		    	return;
  		       }

  		      if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6)==GPIO_PIN_SET){ // 4
  		    	  while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_6)==GPIO_PIN_SET);
  		    	  send_write_coil(0x9f9, 0xff00);
  		    	  return ;
  		      }
  		      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);//row 3 reset
  		      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
  		      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
  		      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);



  		   if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3)==GPIO_PIN_SET){ //down

  		    	while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_3)==GPIO_PIN_SET);
  		    	send_write_coil(0x9ec, 0xff00);
  		    	send_write_coil(0x9ec, 0x00);
  		    	return;

  		     }
  		      if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4)){ // 9
  		    	  while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_4));
  		    	  send_write_coil(0x9fB, 0xff00);
  		    	  return ;
  		     }
  		      if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5)==GPIO_PIN_SET){ // 8
  		    	  while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_5)==GPIO_PIN_SET);
  		    	  send_write_coil(0x9f7, 0xff00);
  		    	  return ;
  		       }

  		      if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6)==GPIO_PIN_SET){ // 7
  		    	  while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_6)==GPIO_PIN_SET);
  		    	  send_write_coil(0x9fA, 0xff00);
  		      	  return;
  		        }
  		      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);//row 4 reset
  		      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
  		      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
  		      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);


  		   if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3)==GPIO_PIN_SET){ //menu
  			   while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_3)==GPIO_PIN_SET);
  		 	   send_write_coil(0x9ed, 0xff00);
  		 	   send_write_coil(0x9fe, 0x00);
  		 	   cc=0;
  		 	   st=LEARNING;
  		 	   return;

  		     }
  		      if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4)==GPIO_PIN_SET ){ // reset
  		    	  while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_4)==GPIO_PIN_SET);
  		    	  send_write_coil(0x9ef, 0xff00);
  		    	  send_write_coil(0x9ef, 0x00);
  		    	  return ;
  		      }
  		      if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5)==GPIO_PIN_SET){ // 0
  		     	   while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_5)==GPIO_PIN_SET);
  		     	   send_write_coil(0x09fd, 0xff00);
  		     	   return ;
  		       }

  		      if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6)==GPIO_PIN_SET){ // set

  		      	   while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_6)==GPIO_PIN_SET);
  		      	   HAL_Delay(10);
  		      	   send_write_coil(0x9ee, 0xff00);
  		      	   send_write_coil(0x9ee, 0x00);
  		      	   cc=0;
  		      	   return ;
  		      }
  }

  //void go_to(State s){
  //	if(s==MENU){
  //		t=1;
  //	}
  //	else if(s==LEARNING){
  //		t=7;
  //	}
  //	else if(s==PARAMETER_SETTING){
  //		t=9;
  //	}
  //	else if(s==SPEED){
  //			t=11;
  //		}
  //	else if(s==BREAK){
  //				t=15;
  //			}
  //	st=s;
  //
  //}


  void show_cursor(enum State c){
  	lcd16x2_clear();
  	if(c==MENU){
  		//lcd16x2_printf("learning");
  	}
  	if(c==LEARNING){
  		lcd16x2_printf("Learning");

  	}
  	else if(c==PARAMETER_SETTING){
  		lcd16x2_printf("Parameter");
  		lcd16x2_2ndLine();
  		lcd16x2_printf("Setting");
  	}
  	else if(c==SHOW){
  		lcd16x2_printf("Show");
  	}
  	else if(c==LOG){
  		lcd16x2_printf("Log");
  	}
  	else if(c==OTHERS){
  		lcd16x2_printf("Others");
  	}
  	else if(c==MANUAL_CONTROL){
  		lcd16x2_printf("Manual");
  		lcd16x2_2ndLine();
  		lcd16x2_printf("Control");
  	}

  	else if(c==MODE1){
  		lcd16x2_printf("Mode1");
  	}
  	else if(c==MODE2){
  			lcd16x2_printf("Mode2");
  	}
  	else if(c==SPEED){
  		lcd16x2_printf("Speed");
  	}
  	else if(c==BREAK){
  		lcd16x2_printf("Break");
  	}
  	else if(c==SPEED_MODE1){
  		lcd16x2_printf("Mode1");
  	}
  	else if(c==SPEED_MODE2){
  		lcd16x2_printf("Mode2");
  	}
  	else if(c==SPEED_MODE1_CALIBRATION){
  		lcd16x2_printf("Calibration");
  		lcd16x2_2ndLine();
  		lcd16x2_printf("Speed");
  	}
  	else if(c==SPEED_MODE1_CYCLE){
  		lcd16x2_printf("Cycle speed");
  	}
  	else if(c==BREAK_VERTICAL_ACTUATOR){
  		lcd16x2_printf("Vertical");
  		lcd16x2_2ndLine();
  		lcd16x2_printf("Actuator");
  	}
  	else if(c==BREAK_HORIZONTAL_ACTUATOR){
  		lcd16x2_printf("Horizontal");
  		lcd16x2_2ndLine();
  		lcd16x2_printf("Actuator");
  	}
  	else if(c==LEARN_QUESTION){
  		cursor=LEARN_QUESTION;

  	}
  	else if(c==TIME_TO_WAIT){
  		lcd16x2_printf("Time to Wait");

  	}
  	else if(c==WAIT_FOR_SENSOR){
  		lcd16x2_printf("Waiting for");
  		lcd16x2_2ndLine();
  		lcd16x2_printf("Sensor");
  	}
  	else if(c==SPEED_MODE2_CALIBRATION){
  		lcd16x2_printf("Calibration");
  		lcd16x2_2ndLine();
  		lcd16x2_printf("Speed");
  	}
  	else if(c==SPEED_MODE2_CYCLE){
  		lcd16x2_printf("Cycle Speed");
  	}

  	else if(c==BREAK_VERTICAL_ACTUATOR_UP){
  		lcd16x2_printf("Up");

  	}
  	else if(c==BREAK_VERTICAL_ACTUATOR_DOWN){
  		lcd16x2_printf("Down");
  	}
  	else if(c==BREAK_HORIZONTAL_ACTUATOR_FRONT){
  		lcd16x2_printf("Front");

  	}
  	else if(c==BREAK_HORIZONTAL_ACTUATOR_BACK){
  		lcd16x2_printf("Back");
  	}
  	else if(c==LOG_WORKING_TIME){
  		lcd16x2_printf("Working Time");
  	}
  	else if(c==LOG_CYCLES){
  		lcd16x2_printf("Cycles");
  	}
  	else if(c==LOG_RESET){
  		lcd16x2_printf("Reset");
  	}
  	else if(c==RUN){
  		lcd16x2_printf("Run");
  	}
	else if(c==LOG_RESET_WORKING_TIME){
		lcd16x2_printf("Reset Working");
		lcd16x2_2ndLine();
		lcd16x2_printf("Time");
	}
	else if(c==LOG_RESET_CYCLES){
		lcd16x2_printf("Reset Cycles");
	}
	else if(c==MENU_RUN){
		lcd16x2_printf("Run");
	}
	else if(c==MENU_RUN_COIL0){
		//	lcd16x2_printf("Run");
		}
	else if(c==MENU_RUN_COIL0_RUN){
			//	lcd16x2_printf("Run");
			}
	else if(c==MENU_RUN_COIL1){
			//lcd16x2_printf("Run");
		}
	else if(c==MENU_RUN_COIL0_MODE1){
			lcd16x2_printf("Mode1");
		}
	else if(c==MENU_RUN_COIL0_MODE2){
			lcd16x2_printf("Mode2");
		}
  }




//  void show_cursor(uint8_t c){//TODO
//  	lcd16x2_clear();
//  	if(c==0){
//  		cursor=MENU;
//  		//lcd16x2_printf("learning");
//  	}
//  	if(c==1){
//  		cursor=LEARNING;
//  		lcd16x2_printf("Learning");
//
//  	}
//  	else if(c==2){
//  		cursor=PARAMETER_SETTING;
//  		lcd16x2_printf("Parameter");
//  		lcd16x2_2ndLine();
//  		lcd16x2_printf("Setting");
//  	}
//  	else if(c==3){
//  		cursor=SHOW;
//  		lcd16x2_printf("Show");
//  	}
//  	else if(c==4){
//  		cursor=LOG;
//  		lcd16x2_printf("Log");
//  	}
//  	else if(c==5){
//  		cursor=OTHERS;
//  		lcd16x2_printf("Others");
//  	}
//  	else if(c==6){
//  		cursor=MANUAL_CONTROL;
//  		lcd16x2_printf("Manual");
//  		lcd16x2_2ndLine();
//  		lcd16x2_printf("Control");
//  	}
//
//  	else if(c==7){
//  		cursor=MODE1;
//  		lcd16x2_printf("Mode1");
//  	}
//  	else if(c==8){
//  			cursor=MODE2;
//  			lcd16x2_printf("Mode2");
//  	}
//  	else if(c==9){
//  		cursor=SPEED;
//  		lcd16x2_printf("Speed");
//  	}
//  	else if(c==10){
//  		cursor=BREAK;
//  		lcd16x2_printf("Break");
//  	}
//  	else if(c==11){
//  		cursor=SPEED_MODE1;
//  		lcd16x2_printf("Mode1");
//  	}
//  	else if(c==12){
//  		cursor=SPEED_MODE2;
//  		lcd16x2_printf("Mode2");
//  	}
//  	else if(c==13){
//  		cursor=SPEED_MODE1_CALIBRATION;
//  		lcd16x2_printf("Calibration");
//  		lcd16x2_2ndLine();
//  		lcd16x2_printf("Speed");
//  	}
//  	else if(c==14){
//  		cursor=SPEED_MODE1_CYCLE;
//  		lcd16x2_printf("Cycle speed");
//  	}
//  	else if(c==15){
//  		cursor=BREAK_VERTICAL_ACTUATOR;
//  		lcd16x2_printf("Vartical");
//  		lcd16x2_2ndLine();
//  		lcd16x2_printf("Actuator");
//  	}
//  	else if(c==16){
//  		cursor=BREAK_HORIZONTAL_ACTUATOR;
//  		lcd16x2_printf("Horizontal");
//  		lcd16x2_2ndLine();
//  		lcd16x2_printf("Actuator");
//  	}
//  	else if(c==17){
//  		cursor=LEARN_QUESTION;
//
//  	}
//  	else if(c==18){
//  		cursor=TIME_TO_WAIT;
//  		lcd16x2_printf("Time to Wait");
//
//  	}
//  	else if(c==19){
//  		cursor=WAIT_FOR_SENSOR;
//  		lcd16x2_printf("Waiting for");
//  		lcd16x2_2ndLine();
//  		lcd16x2_printf("Sensor");
//  	}
//  	else if(c==20){
//  		cursor=SPEED_MODE2_CALIBRATION;
//  		lcd16x2_printf("Calibration");
//  		lcd16x2_2ndLine();
//  		lcd16x2_printf("Speed");
//  	}
//  	else if(c==21){
//  		cursor=SPEED_MODE2_CYCLE;
//  		lcd16x2_printf("Cycle Speed");
//  	}
//
//  	else if(c==22){
//  		cursor=BREAK_VERTICAL_ACTUATOR_UP;
//  		lcd16x2_printf("Up");
//
//  	}
//  	else if(c==23){
//  		cursor=BREAK_VERTICAL_ACTUATOR_DOWN;
//  		lcd16x2_printf("Down");
//
//  	}
//  	else if(c==24){
//  		cursor=BREAK_HORIZONTAL_ACTUATOR_FRONT;
//  		lcd16x2_printf("Front");
//
//  	}
//  	else if(c==25){
//  		cursor=BREAK_HORIZONTAL_ACTUATOR_BACK;
//  		lcd16x2_printf("Back");
//
//  	}
//  	else if(c==26){
//  		cursor=LOG_WORKING_TIME;
//  		lcd16x2_printf("Working Time");
//
//  	}
//  	else if(c==27){
//  		cursor=LOG_CYCLES;
//  		lcd16x2_printf("Cycles");
//
//  	}
//  	else if(c==28){
//  		cursor=LOG_RESET;
//  		lcd16x2_printf("Reset");
//
//  	}
//  	else if(c==29){
//  		cursor=RUN;
//  		lcd16x2_printf("Run");
//
//  	}
//	else if(c==30){
//		cursor=LOG_RESET_WORKING_TIME;
//		lcd16x2_printf("Reset Working");
//		lcd16x2_2ndLine();
//		lcd16x2_printf("Time");
//
//	}
//	else if(c==31){
//		cursor=LOG_RESET_CYCLES;
//		lcd16x2_printf("Reset Cycles");
//
//	}
//	else if(c==32){
//			cursor=MENU_RUN;
//			lcd16x2_printf("Run");
//		}
//  }




  void set_speed(char c,uint16_t *num){
  	//	uint8_t sum=0;
  		if(c=='1' || c=='2' || c=='3' || c=='4' || c=='5' || c=='6'
  				|| c=='7'|| c=='8'||c=='9' || c=='0'){
  			lcd16x2_printf("%c",c);
  			(*num)=*num*10;
  			*num +=(c-48);
  			col++;
  		}
  	}
  void set_time_wait(char c,uint32_t *num){
  	//	uint8_t sum=0;
  		if(c=='1' || c=='2' || c=='3' || c=='4' || c=='5' || c=='6'
  				|| c=='7'|| c=='8'||c=='9' || c=='0'){
  			lcd16x2_printf("%c",c);
  			(*num)=*num*10;
  			*num +=(c-48);
  			col_t++;
  		}
  	}






  void set_counter(uint8_t data){
  	chars[0]='0';
  	chars[1]='0';
  	chars[2]='0';
  	uint8_t tempd=data;
  	uint8_t i=2;
  	while(i>=0 && tempd>0){
  		chars[i]=(char)((tempd%10)+48);
  		tempd=tempd/10;
  		i--;
  	}
  	lcd16x2_printf("%c",chars[0]);
  	lcd16x2_printf("%c",chars[1]);
  	lcd16x2_printf("%c",chars[2]);

  }

  void set_counter32(uint32_t data){
  	for(uint8_t i=0;i<10;i++){
  			chars1[i]='0';
  		}
  	uint32_t tempd=data;
  	uint8_t i=9;
  	while(i>=0 && tempd>0){
  		chars1[i]=(char)((tempd%10)+48);
  		tempd=tempd/10;
  		i--;
  	}

  	for(uint8_t i=0;i<10;i++){
  		lcd16x2_printf("%c",chars1[i]);
  	}

  }




  uint16_t int_hex(uint16_t data){
  	uint16_t sum=0;
  	uint8_t i=0;
  	uint8_t count=0;
  	while(data>0){
  		i=data%10;
  		sum=sum+(i*pow(16,count));
  		data=data/10;
  		count++;
  	}
  	return sum;
  }

  void send_write_register(uint16_t address, uint16_t data){
  	  uint8_t frame[8];
  	  frame[0]=(uint8_t)0x02;
  	  frame[1]=(uint8_t)0x06;
  	  frame[2]=(uint8_t)(address>>8);
  	  frame[3]=(uint8_t)(address&0x00ff);
  	  frame[4]=(uint8_t)(data>>8);
  	  frame[5]=(uint8_t)(data&0x00ff);
  	  uint16_t crc=crc16(frame,6);
  	  uint8_t lsb=(uint8_t)(crc&0x00ff);
  	  uint8_t msb=(uint8_t)(crc>>8);
  	  frame[6]=lsb;
  	  frame[7]=msb;

  	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
  	  HAL_UART_Transmit(&huart3, frame, 8,10);
  	  HAL_Delay(5);
  }

  void send_write_two_register(uint16_t address1,uint16_t address2, uint32_t data){
  	  uint8_t frame[8];
  	  uint8_t frame1[8];
  	  frame[0]=(uint8_t)0x02;
  	  frame[1]=(uint8_t)0x06;
  	  frame[2]=(uint8_t)(address1>>8);
  	  frame[3]=(uint8_t)(address1&0x00ff);
  	  frame[4]=(uint8_t)(data>>24);
  	  frame[5]=(uint8_t)((data>>16)&0x00ff);
  	  uint16_t crc=crc16(frame,6);
  	  uint8_t lsb=(uint8_t)(crc&0x00ff);
  	  uint8_t msb=(uint8_t)(crc>>8);
  	  frame[6]=lsb;
  	  frame[7]=msb;
  	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
  	  HAL_UART_Transmit(&huart3, frame, 8,10);

  	  HAL_Delay(10);
  	  frame1[0]=(uint8_t)0x02;
  	  frame1[1]=(uint8_t)0x06;
  	  frame1[2]=(uint8_t)(address2>>8);
  	  frame1[3]=(uint8_t)(address2&0x00ff);
  	  frame1[4]=(uint8_t)((data>>8)&0x000000ff);
  	  frame1[5]=(uint8_t)(data&0x000000ff);
  	  crc=crc16(frame1,6);
  	  lsb=(uint8_t)(crc&0x00ff);
  	  msb=(uint8_t)(crc>>8);
  	  frame1[6]=lsb;
  	  frame1[7]=msb;
  	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
  	  HAL_UART_Transmit(&huart3, frame1, 8,10);
  	  HAL_Delay(10);
  }




  void send_write_coil(uint16_t address, uint16_t data){
	  uint8_t frame[8];
  	  frame[0]=(uint8_t)0x02;
  	  frame[1]=(uint8_t)0x05;
  	  frame[2]=(uint8_t)(address>>8);
  	  frame[3]=(uint8_t)(address&0x00ff);
  	  frame[4]=(uint8_t)(data>>8);
  	  frame[5]=(uint8_t)(data&0x00ff);
  	  uint16_t crc=crc16(frame,6);
  	  uint8_t lsb=(uint8_t)(crc&0x00ff);
  	  uint8_t msb=(uint8_t)(crc>>8);
  	  frame[6]=lsb;
  	  frame[7]=msb;

  	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
  	  HAL_UART_Transmit(&huart3, frame, 8,10);
  	  HAL_Delay(10);
  		//  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
  }
  void send_read_one_coil_show(uint16_t address){
	  uint8_t dback[8];
	  uint8_t frame[8];
	  frame[0]=(uint8_t)0x02;
	  frame[1]=(uint8_t)0x01;
	  frame[2]=(uint8_t)(address>>8);
	  frame[3]=(uint8_t)(address&0x00ff);
	  frame[4]=(uint8_t)(0x0001>>8);
	  frame[5]=(uint8_t)(0x0001&0x00ff);
	  uint16_t crc=crc16(frame,6);
	  uint8_t lsb=(uint8_t)(crc&0x00ff);
	  uint8_t msb=(uint8_t)(crc>>8);
	  frame[6]=lsb;
	  frame[7]=msb;
	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
	  HAL_UART_Transmit(&huart3, frame, 8,10);
	  HAL_Delay(2);
	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
	  HAL_UART_Receive(&huart3, dback,8,10);
	  HAL_Delay(10);
	  crc=crc16(dback, 6);
	  lsb=(uint8_t)(crc&0x00ff);
	  msb=(uint8_t)(crc>>8);
	  if(lsb==frame[6]&& msb==frame[7]){
	   	  show_coil=dback[3];
	   	  show_coil=show_coil&(0x01);
	   	  return;
	  }
	  else{
		  send_read_one_coil_show(address);
	  }
    }
  void send_read_one_coil(uint16_t address,uint8_t *data){
  	  uint8_t dback[8];
  	  uint8_t frame[8];
  	  frame[0]=(uint8_t)0x02;
  	  frame[1]=(uint8_t)0x01;
  	  frame[2]=(uint8_t)(address>>8);
  	  frame[3]=(uint8_t)(address&0x00ff);
  	  frame[4]=(uint8_t)(0x0001>>8);
  	  frame[5]=(uint8_t)(0x0001&0x00ff);
  	  uint16_t crc=crc16(frame,6);
  	  uint8_t lsb=(uint8_t)(crc&0x00ff);
  	  uint8_t msb=(uint8_t)(crc>>8);
  	  frame[6]=lsb;
  	  frame[7]=msb;
  	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
  	  HAL_UART_Transmit(&huart3, frame, 8,10);
  	  HAL_Delay(2);
  	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
  	  HAL_UART_Receive(&huart3, dback,8,10);
  	  HAL_Delay(10);
  	  crc=crc16(dback, 6);
  	  lsb=(uint8_t)(crc&0x00ff);
  	  msb=(uint8_t)(crc>>8);
  	  if(lsb==frame[6]&& msb==frame[7]){
  	   	  *data=dback[3];
  	   	  (*data)=(*data)&(0x01);
  	   	  return;
  	  }
  	  else{
  		  send_read_one_coil(address,data);
  	  }
      }



  void send_read_one_register(uint16_t address){
  	  uint8_t frame[7];
  		  data_back[0]=(uint8_t)0x02;
  		  data_back[1]=(uint8_t)0x03;
  		  data_back[2]=(uint8_t)(address>>8);
  		  data_back[3]=(uint8_t)(address&0x00ff);
  		  data_back[4]=(uint8_t)(0x0001>>8);
  		  data_back[5]=(uint8_t)(0x0001&0x00ff);

  		  uint16_t crc=crc16(data_back,6);
  		  uint8_t lsb=(uint8_t)(crc&0x00ff);
  		  uint8_t msb=(uint8_t)(crc>>8);
  		  data_back[6]=lsb;
  		  data_back[7]=msb;

  		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
  		  HAL_UART_Transmit(&huart3, data_back, 8,10);
  		  HAL_Delay(2);
  		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
  		  HAL_UART_Receive(&huart3, frame, 7,5);
  		  HAL_Delay(10);
  		  crc=crc16(frame, 5);
  	  	  lsb=(uint8_t)(crc&0x00ff);
  	  	  msb=(uint8_t)(crc>>8);
  	  	  if(lsb==frame[5]&& msb==frame[6]){
  	  		  counter=frame[4];
  	  		  return;
  	  	  }else{
  	  		send_read_one_register(address);
  	  	  }



  }

  void send_read_two_register(uint16_t address,uint32_t *data){
  	  uint8_t dback[9];
  	  uint8_t frame[8];
  	  frame[0]=(uint8_t)0x02;
  	  frame[1]=(uint8_t)0x03;
  	  frame[2]=(uint8_t)(address>>8);
  	  frame[3]=(uint8_t)(address&0x00ff);
  	  frame[4]=(uint8_t)(0x0002>>8);
  	  frame[5]=(uint8_t)(0x0002&0x00ff);
  	  uint16_t crc=crc16(frame,6);
  	  uint8_t lsb=(uint8_t)(crc&0x00ff);
  	  uint8_t msb=(uint8_t)(crc>>8);
  	  frame[6]=lsb;
  	  frame[7]=msb;

  	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
  	  HAL_UART_Transmit(&huart3, frame, 8,10);
  	  HAL_Delay(2);
  	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
  	  HAL_UART_Receive(&huart3, dback, 9,10);
  	  HAL_Delay(10);
  	  crc=crc16(dback, 7);
  	  lsb=(uint8_t)(crc&0x00ff);
  	  msb=(uint8_t)(crc>>8);
  	  if(lsb==dback[7]&& msb==dback[8]){
  		  *data=(dback[5]<<24);
  		  *data=((*data)|((dback[6]<<16)&0x00ff0000));
  		  *data=((*data)|((dback[3]<<8)&0x0000ff00));
  		  *data=((*data)|((dback[4])&0x000000ff));
  		  return;
  	  }
  	  else{
  		send_read_two_register(address,data);
  	  }

  }




  void set_char(char c){
  	   if(c=='1'){
  		   if(counterP<=4){
  			 pass[counterP]=1;
  			 counterP++;
  		   lcd16x2_printf("1");
  		   }
  		   else{
  			   counterP=0;
  			   st=ERR;
  		   }
  		  //    return;
  	   }
  	   else if(c=='2'){
  		   if(counterP<=4){
  			 pass[counterP]=2;
  			 counterP++;
  		   lcd16x2_printf("2");
  		   }
  		   else{
  			   counterP=0;
  			   st=ERR;
  		   }
  		 //     return;
  	   }
  	   else  if(c=='3'){
  		   if(counterP<=4){
  			 pass[counterP]=3;
  		   lcd16x2_printf("3");
  		   counterP++;
  		   }
  		   else{
  			   counterP=0;
  			   st=ERR;
  		   }
  		  //    return;
  	   }
  	   else if(c=='4'){
  		   if(counterP<=4){
  			 pass[counterP]=4;
  			 counterP++;
  		   lcd16x2_printf("4");
  		   }
  		   else{
  			   counterP=0;
  			   st=ERR;
  		   }
  		 //     return;
  	   }
  	   else   if(c=='5'){
  		   if(counterP<=4){
  			 pass[counterP]=5;
  			 counterP++;
  		   lcd16x2_printf("5");
  		   }
  		   else{
  			   counterP=0;
  			   st=ERR;
  		   }
  		//      return;
  	   }
  	   else  if(c=='6'){
  		   if(counterP<=4){
  			 pass[counterP]=6;
  			 counterP++;
  		   lcd16x2_printf("6");
  		   }
  		   else{
  			   counterP=0;
  			   st=ERR;
  		   }
  		//      return;
  	   }
  	   else  if(c=='7'){
  		   if(counterP<=4){
  			 pass[counterP]=7;
  			 counterP++;
  		   lcd16x2_printf("7");
  		   }
  		   else{
  			   counterP=0;
  			   st=ERR;
  		   }
  		//      return;
  	   }
  	   else  if(c=='8'){
  		   if(counterP<=4){
  			 pass[counterP]=8;
  			 counterP++;
  		   lcd16x2_printf("8");
  		   }
  		   else{
  			   counterP=0;
  			   st=ERR;
  		   }
  	//	      return;
  	   }
  	   else  if(c=='9'){
  		   if(counterP<=4){
  			 pass[counterP]=9;
  			 counterP++;
  		   lcd16x2_printf("9");
  		   }
  		   else{
  			   counterP=0;
  			   st=ERR;
  		   }
  	//	      return;
  	   }
  	   else  if(c=='0'){
  			   if(counterP<=4){
  				 pass[counterP]=0;
  				 counterP++;
  			   lcd16x2_printf("0");
  			   }
  			   else{
  				   counterP=0;
  				   st=ERR;
  			   }
  	//		      return;
  		   }
     }





  void set_char_for_sensor(char c){
	  if(c=='1'){
		  if( sensor_counter<5){
			  sensor_temp[sensor_counter]=1;
			  sensor_num_cursor++;
			  sensor_counter++;
			  lcd16x2_printf("1");
		  }

    		  //    return;
	  }
	  else if(c=='2'){
		  if( sensor_counter<5){
			  sensor_temp[sensor_counter]=2;
			  sensor_num_cursor++;
			  sensor_counter++;
			  lcd16x2_printf("2");
		  }

	  }
	  else  if(c=='3'){
		  if( sensor_counter<5){
		 			  sensor_temp[sensor_counter]=3;
		 			  sensor_num_cursor++;
		 			 sensor_counter++;
		 			  lcd16x2_printf("3");
		 		  }
//		  else{
//			  sensor_num_cursor=0;
//			  st=ERR;
//		  }
    		  //    return;
	  }
	  else if(c=='4'){
		  if( sensor_counter<5){
			  sensor_temp[sensor_counter]=4;
			  sensor_num_cursor++;
			  sensor_counter++;
			  lcd16x2_printf("4");
		  }
		  else{
			  sensor_num_cursor=0;
			  st=ERR;
		  }
		  //     return;
	  }
	  else   if(c=='5'){
		  if( sensor_counter<5){
			  sensor_temp[sensor_counter]=5;
			  sensor_num_cursor++;
			  sensor_counter++;
			  lcd16x2_printf("5");
		  }

	  }


  }


  void set_char_for_sensor_andor(char c){
  	   if(c=='u'){
  		   if(sensor_counter_andor<=4){
  			 sensor_temp_andor[sensor_counter_andor]=1;
  			 sensor_counter_andor++;
  			 sensor_num_cursor++;
  			 lcd16x2_printf("a");
  		   }
  		   else{
  			   counterP=0;
  			   st=ERR;
  		   }
  		  //    return;
  	   }
  	   else if(c=='d'){
  		 if(sensor_counter_andor<=4){
  			 sensor_temp_andor[sensor_counter_andor]=2;
  			 sensor_counter_andor++;
  			 sensor_num_cursor++;
  			 lcd16x2_printf("o");
  		 }
  		 else{
  			 counterP=0;
  			 st=ERR;
  		 }
  		 //     return;
  	   }


     }















//  void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//  {
//  	uint8_t frame[8];
//  	 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
//  	HAL_UART_Receive_IT(&huart3, frame, 8);
//  if(frame[1]==0x03 && frame[2]==0x02){
//    	counter=frame[5];
//  //	counter=counter|(frame[5]&0x00ff);
//  }
//  }
  void set_lcd(enum State s){
  	if(s==LEARNING){
  		lcd16x2_clear();
  		lcd16x2_printf("learning");
  		cursor=LEARNING;
  	}
  		if(s==PARAMETER_SETTING){
  				lcd16x2_clear();
  				lcd16x2_printf("PARAMETER_SETTING");
  				cursor=PARAMETER_SETTING;
  			}
  	if(s==SHOW){
  			lcd16x2_clear();
  			lcd16x2_printf("show");
  			cursor=SHOW;
  		}
  	if(s==LOG){
  			lcd16x2_clear();
  			lcd16x2_printf("log");
  			cursor=LOG;
  		}
  	if(s==MANUAL_CONTROL){
  			lcd16x2_clear();
  			lcd16x2_printf("manual control");
  			cursor=MANUAL_CONTROL;
  		}
  }








  char get_key(){
	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); // row 1 reset
  	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
  	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
  	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);



  	  if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3)==GPIO_PIN_SET){ //f1
  		  while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_3)==GPIO_PIN_SET);
  		  return 'f';

  	  }
  	  if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4)==GPIO_PIN_SET ){ // 3
  		  while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4)==GPIO_PIN_SET);
  		  return '3';

  	   }


  	   if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5)==GPIO_PIN_SET){ // 2
  		   while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_5)==GPIO_PIN_SET);
  		   return '2';
  	    }

  	   if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6)==GPIO_PIN_SET){ // 1
  	   	   while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_6)==GPIO_PIN_SET);
  	   	   return '1';
  	   }


  	   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); // row 2 reset
  	   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
  	   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
  	   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);

  	   if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3)==GPIO_PIN_SET){ //up
  	    	while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_3)==GPIO_PIN_SET);
  	    	return 'u';
  	     }

  	     if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4)==GPIO_PIN_SET ){ // 6
  	    	 while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_4)==GPIO_PIN_SET );
  	    	 return '6';
  	     }
  	    if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5)==GPIO_PIN_SET){ // 5
  	    	while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_5)==GPIO_PIN_SET);
  	    	return '5';
  	    }

  	      if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6)==GPIO_PIN_SET){ // 4
  	    	  while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_6)==GPIO_PIN_SET);
  	    	  return '4';
  	      }
  	      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);//row 3 reset
  	      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
  	      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
  	      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);



  	   if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3)==GPIO_PIN_SET){ //down
  	    	while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_3)==GPIO_PIN_SET);
  	    	return 'd';
  	     }
  	      if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4)){ // 9
  	    	  while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_4));
  	    	  return '9';
  	     }
  	      if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5)==GPIO_PIN_SET){ // 8
  	     	   while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_5)==GPIO_PIN_SET);
  	     	   return '8';
  	       }

  	      if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6)==GPIO_PIN_SET){ // 7
  	      	   while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_6)==GPIO_PIN_SET);
  	      	   return '7';
  	        }
  	      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);//row 4 reset
  	      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
  	      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
  	      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);


  	   if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3)==GPIO_PIN_SET){ //menu
  	    	while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_3)==GPIO_PIN_SET);
  	    	return 'm';

  	     }
  	      if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4)==GPIO_PIN_SET ){ // reset
  	    	  while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_4)==GPIO_PIN_SET);
  	    	  return 'r';

  	     }
  	      if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5)==GPIO_PIN_SET){ // 0
  	    	  while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_5)==GPIO_PIN_SET);
  	    	  return '0';

  	       }

  	      if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6)==GPIO_PIN_SET){ // set
  	      	   while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_6)==GPIO_PIN_SET);
  	      	   return 's';
  	      }

  }

  char get_key_for_sensor_number(){
  	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); // row 1 reset
    	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
    	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
    	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);


    	  if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4)==GPIO_PIN_SET ){ // 3
    		  while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4)==GPIO_PIN_SET);
    		  return '3';

    	   }

    	   if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5)==GPIO_PIN_SET){ // 2
    		   while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_5)==GPIO_PIN_SET);
    		   return '2';
    	    }

    	   if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6)==GPIO_PIN_SET){ // 1
    	   	   while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_6)==GPIO_PIN_SET);
    	   	   return '1';
    	   }

    	   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); // row 2 reset
    	   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
    	   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
    	   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);


    	    if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5)==GPIO_PIN_SET){ // 5
    	    	while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_5)==GPIO_PIN_SET);
    	    	return '5';
    	    }

    	      if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6)==GPIO_PIN_SET){ // 4
    	    	  while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_6)==GPIO_PIN_SET);
    	    	  return '4';
    	      }




    	      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);//row 4 reset
    	      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
    	      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
    	      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);


    	   if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3)==GPIO_PIN_SET){ //menu
    	    	while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_3)==GPIO_PIN_SET);
    	    	return 'm';

    	     }
    	      if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4)==GPIO_PIN_SET ){ // reset
    	    	  while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_4)==GPIO_PIN_SET);
    	    	  return 'r';

    	     }


    }

  char get_key_for_sensor_andor(){



  	   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); // row 2 reset
  	   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
  	   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
  	   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);

  	   if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3)==GPIO_PIN_SET){ //up
  	    	while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_3)==GPIO_PIN_SET);
  	    	return 'u';
  	     }

  	      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);//row 3 reset
  	      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
  	      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
  	      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);



  	   if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3)==GPIO_PIN_SET){ //down
  	    	while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_3)==GPIO_PIN_SET);
  	    	return 'd';
  	     }

  	      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);//row 4 reset
  	      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
  	      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
  	      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);


  	   if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3)==GPIO_PIN_SET){ //menu
  	    	while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_3)==GPIO_PIN_SET);
  	    	return 'm';

  	     }
  	      if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4)==GPIO_PIN_SET ){ // reset
  	    	  while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_4)==GPIO_PIN_SET);
  	    	  return 'r';

  	     }


 	     if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6)==GPIO_PIN_SET){ // set
 	      	   while (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_6)==GPIO_PIN_SET);
 	      	   return 's';
 	      }

  }






  uint16_t crc16( uint8_t *buf, uint16_t len )
  {
  	static const uint16_t table[2] = { 0x0000, 0xA001 };
  	uint16_t crc = 0xFFFF;
  	uint16_t i = 0;
  	uint8_t bit = 0;
  	uint16_t xor = 0;  /* USER CODE END Init */

  	for( i = 0; i < len; i++ )
  	{
  		crc ^= buf[i];
  		for( bit = 0; bit < 8; bit++ )
  		{
  			xor = crc & 0x01;
  			crc >>= 1;
  			crc ^= table[xor];
  		}
  	}
  	return crc;
  }

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 38400;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_2;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);
  /* DMA1_Channel3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6
                          |GPIO_PIN_7|D4_Pin|D5_Pin|D6_Pin
                          |D7_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, RS_PIN_Pin|RW_PIN_Pin|GPIO_PIN_13|E_PIN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA1 PA4 PA5 PA6
                           PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6
                          |GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : RS_PIN_Pin RW_PIN_Pin PB13 E_PIN_Pin */
  GPIO_InitStruct.Pin = RS_PIN_Pin|RW_PIN_Pin|GPIO_PIN_13|E_PIN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : D4_Pin D5_Pin D6_Pin D7_Pin */
  GPIO_InitStruct.Pin = D4_Pin|D5_Pin|D6_Pin|D7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB3 PB4 PB5 PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
