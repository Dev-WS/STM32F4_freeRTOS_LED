/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#include <string.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "stm32f4xx.h"
#include "task.h"
			



static void prvSetupHardware(void);
static void prvSetupUart(void);
void prvSetupGpio(void);


char usr_msg[250];
void printmsg(char *msg);



void led_task_handler(void *params);
void button_handler(void *params);

#define FALSE 0
#define TRUE 1
#define NOT_PRESSED FALSE
#define PRESSED TRUE

uint8_t button_status_flag = NOT_PRESSED;

int main(void)
{

	RCC_DeInit();
	SystemCoreClockUpdate();
	prvSetupHardware();

	SEGGER_SYSVIEW_Conf();
	SEGGER_SYSVIEW_Start();


	xTaskCreate(led_task_handler, "LED", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

	xTaskCreate(button_handler, "BUTTON", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

	//GPIO_WriteBit(GPIOD, GPIO_Pin_15, 1);
	vTaskStartScheduler();

	for(;;);
}

void led_task_handler(void *params)
{

	while(1){

		if(button_status_flag == PRESSED){

			GPIO_WriteBit(GPIOD, GPIO_Pin_12, Bit_SET);

		}
		else {
			GPIO_WriteBit(GPIOD, GPIO_Pin_12, Bit_RESET);
		}

	}
}


void button_handler(void *params)
{
	while(1){

			if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0))
			{
				button_status_flag = PRESSED;
			}
			else{
				button_status_flag = NOT_PRESSED;
			}

	}
}

static void prvSetupUart(void)
{
	GPIO_InitTypeDef gpio_uart_pins;
	USART_InitTypeDef uart2_init;


		//1 UART Enable
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	memset(&gpio_uart_pins, 0, sizeof(gpio_uart_pins));

	gpio_uart_pins.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	gpio_uart_pins.GPIO_Mode = GPIO_Mode_AF;
	gpio_uart_pins.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &gpio_uart_pins);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);


	memset(&uart2_init, 0, sizeof(uart2_init));

	uart2_init.USART_BaudRate=115200;
	uart2_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	uart2_init.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	uart2_init.USART_Parity = USART_Parity_No;
	uart2_init.USART_StopBits = USART_StopBits_1;
	uart2_init.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART2, &uart2_init);


	//enable UART to gpio
	USART_Cmd(USART2, ENABLE);

}



static void prvSetupHardware(void)
{
	prvSetupUart();
	prvSetupGpio();

}

void printmsg(char *msg)
{
	for (uint32_t i=0; i<strlen(msg); i++){

		while ( USART_GetFlagStatus(USART2,USART_FLAG_TXE) !=SET );
		USART_SendData(USART2, msg[i]);
		//USART_GetFlagStatus(USART2, )
	}

}

void prvSetupGpio(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB2PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);  //SYSCFG AHB2
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);


	GPIO_InitTypeDef led_init;
	led_init.GPIO_Mode = GPIO_Mode_OUT;
	led_init.GPIO_OType = GPIO_OType_PP;
	led_init.GPIO_Pin = GPIO_Pin_12;
	led_init.GPIO_Speed = GPIO_Low_Speed;
	led_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD,&led_init);


	GPIO_InitTypeDef button_init;
	button_init.GPIO_Mode = GPIO_Mode_IN;
	button_init.GPIO_OType = GPIO_OType_PP;
	button_init.GPIO_Pin = GPIO_Pin_0;
	button_init.GPIO_Speed = GPIO_Low_Speed;
	button_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA,&button_init);


	SYSCFG_EXTILineConfig(GPIOD, EXTI_PinSource12);
	EXTI_InitTypeDef exti_init;
	exti_init.EXTI_Line = EXTI_Line12;
	exti_init.EXTI_LineCmd = ENABLE;
	exti_init.EXTI_Mode = EXTI_Mode_Interrupt;
	exti_init.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&exti_init);

	NVIC_SetPriority(EXTI15_10_IRQn,5);
	NVIC_EnableIRQ(EXTI15_10_IRQn);
}
