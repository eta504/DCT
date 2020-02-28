#include "STM32F407xx.h"
#include "PB_LCD_Drivers.h"
#include <stdio.h>

//Variable for wait functionality
uint32_t volatile ticks = 0;
//Variable to store ADC conversion
double ADC_result;

//Constructors
void initialise(void); 
void wait(int timeToWait);

//Used for the wait function
void SysTick_Handler (void) {
	ticks++;
}

//ADC interupt handler
void ADC_IRQHandler(void) {
	ADC_result = (ADC1->DR & 0xFFF);
	ADC1->CR2 |= ADC_CR2_SWSTART_Msk;
}

int main (void) {
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock/2);
	
	char buffer[16];
	char* str = "Voltage: ";
	
	initialise();
	PB_LCD_Init();
	PB_LCD_Clear();
	
	snprintf(buffer, 15, "%s", str);
	PB_LCD_WriteString(buffer, 15);
	ADC1->CR2 |= ADC_CR2_SWSTART_Msk;
	while (1) {
		PB_LCD_GoToXY(3, 1);
		snprintf(buffer, 5, "%f", ADC_result/1377);
		PB_LCD_WriteString(buffer, 5);
	}
	
}

//Waits for a specified time
void wait(int timeToWait) {
	uint32_t whenStarted = ticks;
	while (ticks - whenStarted < timeToWait);
}

void initialise(void) {
	//The ADC input uses PC4 (R78 on breadboard).
	//Enable GPIOC RCC and set the GPIOC mode to analogue
	RCC->AHB1ENR |= 1 << RCC_AHB1ENR_GPIOCEN_Pos;
	GPIOC->MODER |= 0x3 << GPIO_MODER_MODER4_Pos;
	
	//Enable ADC1
	RCC->APB2ENR |= 1 << RCC_APB2ENR_ADC1EN_Pos;
	//Enable discontinous conversion
	ADC1->CR1 |= 1 << ADC_CR1_DISCEN_Pos;
	//Enable EOC flag and ADON
	ADC1->CR2 |= 0x401;
	//Set ADC to perform one conversion at a time
	ADC1->SQR1 |= ADC_SQR1_L;
	//Set the input for the ADC to be converting to be 14 (PC4)
	ADC1->SQR3 |= 0xE;
	
	//ADC interupts
	ADC1->CR1 |= 1 << ADC_CR1_EOCIE_Pos;
	NVIC->ISER[0] |= 0x40000;
}
