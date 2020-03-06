#include "STM32F407xx.h"
#include "PB_LCD_Drivers.h"
#include <stdio.h>

//Variable for wait functionality
uint32_t volatile ticks = 0;
//Variable to store ADC conversion
double ADC_result;
double resistance;
double voltage;
double current;

int mode = 1;
char* str;
char buffer[16];

//Constructors
void initialise(void); 
void wait(int timeToWait);
int ButtonPressed(void);

//Used for the wait function
void SysTick_Handler (void) {
	ticks++;
}

//ADC interupt handler
void ADC_IRQHandler(void) {
	ADC_result = (ADC1->DR & 0xFFF);
	ADC1->CR2 |= ADC_CR2_SWSTART_Msk;
}

void EXTI9_5_IRQHandler(void) {
	
	if ((GPIOE->IDR & GPIO_IDR_ID8_Msk)) {
		EXTI->PR = EXTI_PR_PR8;
		mode = 1;
	}
	else if ((GPIOE->IDR & GPIO_IDR_ID9_Msk)) {
		EXTI->PR = EXTI_PR_PR9;
		mode = 2;
	}
}

void EXTI15_10_IRQHandler(void) {
	EXTI->PR = EXTI_PR_PR10;
	mode = 3;
}

int main (void) {
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock/2);
	
	initialise();
	PB_LCD_Init();
	PB_LCD_Clear();

	
	
	while (1) {
		while (mode == 1) {
			PB_LCD_Clear();
			str = "Voltmeter: ";
			snprintf(buffer, 15, "%s", str);
			PB_LCD_WriteString(buffer, 15);
			ADC1->CR2 |= ADC_CR2_SWSTART_Msk;
			while (mode == 1) {
				PB_LCD_GoToXY(3, 1);
				voltage = ADC_result/1377;
				snprintf(buffer, 8, "%.2f %s", voltage, "V");
				PB_LCD_WriteString(buffer, 8);
				wait(1);
			}
		}
		while (mode == 2) {
			PB_LCD_Clear();
			str = "Ammeter: ";
			snprintf(buffer, 15, "%s", str);
			PB_LCD_WriteString(buffer, 15);
			while (mode == 2){
				voltage = ADC_result/1377;
				current = voltage/10;
				PB_LCD_GoToXY(3, 1);
				snprintf(buffer, 10, "%.1f %s", current*1000, "mA");
				PB_LCD_WriteString(buffer, 10);
				wait(1);
			}
		}
		while (mode == 3) {
			PB_LCD_Clear();
			str = "Resistance: ";
			snprintf(buffer, 15, "%s", str);
			PB_LCD_WriteString(buffer, 15);
			ADC1->CR2 |= ADC_CR2_SWSTART_Msk;
			while (mode == 3) {
				PB_LCD_GoToXY(3, 1);
				voltage = ADC_result/1377;
				resistance = (510000*(voltage/3))/(1-(voltage/3));
				snprintf(buffer, 10, "%.3g %s", resistance, "ohms");
				PB_LCD_WriteString(buffer, 10);
				wait(1);
			}
		}
	}
}
	





//Waits for a specified time
void wait(int timeToWait) {
	uint32_t whenStarted = ticks;
	while (ticks - whenStarted < timeToWait);
}

int ButtonPressed(void){
	if ((GPIOE->IDR & GPIO_IDR_ID8_Msk )|( GPIOE->IDR & GPIO_IDR_ID9_Msk )|( GPIOE->IDR & GPIO_IDR_ID10_Msk)) {return 1;}
	else {return 0;}
}

void initialise(void) {
	//The ADC input uses PC4 (R78 on breadboard).
	//Enable GPIOC RCC and set the GPIOC mode to analogue
	RCC->AHB1ENR |= 1 << RCC_AHB1ENR_GPIOCEN_Pos;
	RCC->AHB1ENR |= 1 << RCC_AHB1ENR_GPIOEEN_Pos;
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
	
	//Set buttons to inputs, and enable pull down resistors
	GPIOE->MODER = (GPIOE->MODER & 0xFFC0FFFF);
	GPIOE->PUPDR |= 0x2A0000;
	//Enable interupts
	RCC->APB2ENR |= 1 << RCC_APB2ENR_SYSCFGEN_Pos;
	SYSCFG->EXTICR[2] |= 0x444;
	EXTI->IMR |= 0x700;
	EXTI->RTSR |= 0x700;
	
	NVIC->ISER[0] |= 0x800000;
	NVIC->ISER[1] |= 0x100;
	
	NVIC_EnableIRQ(EXTI15_10_IRQn);
	NVIC_EnableIRQ(EXTI9_5_IRQn);
	
	
}
