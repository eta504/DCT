#include "STM32F407xx.h"
#include <stdio.h>

void initialiseLEDandButtonPorts(void){ 
  //RCC->AHB1ENR =  (RCC->AHB1ENR & 0xFFFFFFF6) | 0x9;
  //GPIOA->MODER = (GPIOA->MODER & 0xFFFFFFFC) ;
  //GPIOA->PUPDR = (GPIOA->PUPDR & 0xFFFFFFFC) | 0x01;
	//GPIOD->MODER = (GPIOD->MODER & 0x55FFFFFF) | 0x55000000;
	//GPIOD->PUPDR = (GPIOD->PUPDR & 0x00FFFFFF);
	//GPIOD->OTYPER = (GPIOD->OTYPER & 0xFFF0FFF);
	GPIOE->MODER = (GPIOE->MODER & 0xFFFFC0FF);
	GPIOE->PUPDR = (GPIOE->PUPDR & 0xFFFFD5FF) | 0x01;
}

int ButtonPressed(void){
	if ((GPIOE->IDR & GPIO_IDR_ID8_Msk )|( GPIOE->IDR & GPIO_IDR_ID9_Msk )|( GPIOE->IDR & GPIO_IDR_ID10_Msk)) {return 1;}
	else {return 0;}
}

int main (void) {
	 SystemCoreClockUpdate();
	 SysTick_Config(SystemCoreClock/2);
	 initialiseLEDandButtonPorts();
	// repeatedly poles button pressed to check for a change in state
	while (1){
			if (ButtonPressed()){
				if(GPIOE->IDR & GPIO_IDR_ID8_Msk)
					{}
				else if(GPIOE->IDR & GPIO_IDR_ID9_Msk)
					{}
				else if(GPIOE->IDR & GPIO_IDR_ID10_Msk)
					{}
			}
	}
}