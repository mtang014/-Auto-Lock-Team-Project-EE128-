/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* Mini-Project Code
*Created by Jiahao Wang and Michael Tang
*ROT_DIR controls CW/CCW
*ROT_SPD controls 22.5 or 180 degrees per second
*/

#include "fsl_device_registers.h"					//.h file for FRDM commands
#include <stdint.h>									//.h file for ADC commands

unsigned int a, k;
unsigned long delay;								//Variables for motor delays
unsigned long i;									//Variable for Switch and Motor delay
unsigned long SW_1=0x00;							//Variable for Switch 1
unsigned long SW_2=0x00;							//Variable for Switch 2
unsigned long SW_3=0x00;							//Variable for Switch 3
unsigned long SW_4=0x00;							//Variable for Switch 4
unsigned long SW_5=0x00;							//Variable for Switch 5

void software_delay(unsigned long delay)			/*Function for motor delay*/
{
	while (delay > 0) delay--;						//Decrement delay until delay is no longer over 0
}

void main(void)
{
	SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK; 				/*Enable Port A Clock Gate Control*/
	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK; 				/*Enable Port B Clock Gate Control*/
	SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK; 				/*Enable Port C Clock Gate Control*/
	SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK; 				/*Enable Port E Clock Gate Control*/

	/*GPIO configurations*/
	PORTA_PCR4 	= 0x100; 					//NMI to GPIO

	PORTB_PCR2 	= 0x100;					//Configures PB2 for GPIO
	PORTB_PCR3 	= 0x100;					//Configures PB3 for GPIO
	PORTB_PCR9 	= 0x100;					//Configures PB9 for GPIO
	PORTB_PCR10 = 0x100;					//Configures PB10 for GPIO
	PORTB_PCR11 = 0x100;					//Configures PB11 for GPIO
	PORTB_PCR21 = 0x100; 					//Blue LED configured as GPIO
	PORTB_PCR22 = 0x100; 					//Red LED configured as GPIO

	PORTC_PCR0 = 0x100;						//Configures PC0 for GPIO
	PORTC_PCR1 = 0x100;						//Configures PC1 for GPIO
	PORTC_PCR2 = 0x100;						//Configures PC2 for GPIO
	PORTC_PCR3 = 0x100;						//Configures PC3 for GPIO
	PORTC_PCR4 = 0x100;						//Configures PC4 for GPIO
	PORTC_PCR5 = 0x100;						//Configures PC5 for GPIO

	PORTE_PCR26 = 0x100; 					//Green LED configured as GPIO

	/*Inputs and Outputs*/
	GPIOB_PDDR |= (0 << 2);					//Set PB2 for input
	GPIOB_PDDR |= (0 << 3);					//Set PB3 for input
	GPIOB_PDDR |= (0 << 9);					//Set PB9 for input
	GPIOB_PDDR |= (0 << 10);				//Set PB10 for input
	GPIOB_PDDR |= (0 << 11);				//Set PB11 for input
	GPIOB_PDDR |= (1 << 21); 				//Blue LED as Output
	GPIOB_PDDR |= (1 << 22); 				//Red LED as Output

	GPIOC_PDDR |= (1 << 0);					//Set PC0 for output
	GPIOC_PDDR |= (1 << 1);					//Set PC1 for output
	GPIOC_PDDR |= (1 << 2);					//Set PC2 for output
	GPIOC_PDDR |= (1 << 3);					//Set PC3 for output
	GPIOC_PDDR |= (1 << 4);					//Set PC4 for output
	GPIOC_PDDR |= (1 << 5);					//Set PC5 for output

	GPIOE_PDDR |= (1 << 26); 				//Green LED as Output

	GPIOC_PDOR = 0x00000000; 				/*initialize port C to 0*/

	/*Turn off RGB LEDs*/
	GPIOB_PDOR |= (1 << 21); 				//Blue LED
	GPIOB_PDOR |= (1 << 22); 				//Red LED
	GPIOE_PDOR |= (1 << 26); 				//Green LED

	for (;;)
	{
		/*Switches to Pins Configuration*/
		uint32_t  input = 0;						//Unique Integer for DIP switch
		input = GPIOB_PDIR & 0xFFFFC; 				//Reads and stores pins 2 to 20 on PORT B
		//for (i = 0; i < 100000; i++); 			//S/W Delay

		SW_1 = input & 0x4;							//Store the pin 2 of the Port B
		SW_2 = input & 0x8;							//Store the pin 3 of the Port B
		SW_3 = input & 0x200;						//Store the pin 9 of Port B
		SW_4 = input & 0x400;						//store the pin 10 of port b
		SW_5 = input & 0x800;						//store the pin 11 of port B

		/*Rotation Speed*/
		delay = 7000;								//Approx. 7.88 degrees/second speed

		/*Default Starting Lock*/
		/*None of the switches are pressed and door lock should not move!*/
		/*To ease confusion: 0 is pressed, not 0 is not pressed (i.e. 1)*/
		if((SW_1 != 0) && (SW_2 != 0) && (SW_3 != 0) && (SW_4 != 0))
		{
			if(SW_5 == 0) /*Reset Mode*/
			{
				GPIOB_PDOR = (1 << 22);             //Turn on blue LED
				software_delay(delay);				//Wait Delay
				a = 0;								//Set a to 0
				  for(a = 0; a <= 10; a++)			//Secure the lock back to 'lock' position(clockwise)
				  {
					GPIOC_PDOR = 0x3A;				//0011 1010 ENA/ENB and PTC1 and PTC3
					software_delay(delay);			//Delay(speed)
					GPIOC_PDOR = 0x39;				//0011 1001 ENA/ENB and PTC0 and PTC3
					software_delay(delay);			//Delay(speed)
					GPIOC_PDOR = 0x35;				//0011 0101 ENA/ENB and PTC0 and PTC2
					software_delay(delay);			//Delay(speed)
					GPIOC_PDOR = 0x36;				//0011 0110 ENA/ENB and PTC1 and PTC2
					software_delay(delay);			//Delay(speed)
				  }
				a = 0;								//Set a back to 0 for repeated performance
				k = 0;								//Set k back to 0 for repeated performance
				GPIOB_PDOR = (1 << 21) | (1 << 22); //Turn off blue LED
				software_delay(delay);				//Wait Delay
			}
			else
			{
				 /*All of the switches are not pressed, so lock does nothing and stays in locked position*/
			}
		}

		/*When switch(s) pressed*/
		else
		{
		   if((SW_1 == 0) && (SW_2 == 0) && (SW_3 != 0) && (SW_4 == 0))		/*For the correct password (1101)*/
		   {
			   GPIOE_PDOR = (0 << 26);              	//Turn on green LED
			   software_delay(delay);					//Delay(speed)
			   /*Auto Re-Lock*/
			   if(k >= 20)								//When 'k' finishes an unlock
			   {
				  if(a >= 100)							//'k' finishes an unlock, now to determine if 'a' finishes a re-lock
				  {
					   /*'a finished a re-lock. Nothing happens until reset.'*/
					  GPIOE_PDOR = (1 << 26); 			//Turn off green LED
					   software_delay(delay);			//Delay(speed)
					  /*End of Auto Re-Lock*/
				  }
				  else{									//'a' did not complete a re-lock!
					  software_delay(delay+9000000);	//Approx. 4 to 5 seconds of pause
					  software_delay(delay);			//Approx. 4 to 5 seconds of pause
					  for(a = 0; a <= 100; a++)			//Ends unlock and begins re-lock(clockwise)
					  {
						GPIOC_PDOR = 0x3A;				//0011 1010 ENA/ENB and PTC1 and PTC3
						software_delay(delay);			//Delay(speed)
						GPIOC_PDOR = 0x39;				//0011 1001 ENA/ENB and PTC0 and PTC3
						software_delay(delay);			//Delay(speed)
						GPIOC_PDOR = 0x35;				//0011 0101 ENA/ENB and PTC0 and PTC2
						software_delay(delay);			//Delay(speed)
						GPIOC_PDOR = 0x36;				//0011 0110 ENA/ENB and PTC1 and PTC2
						software_delay(delay);			//Delay(speed)
					  }
				  }
			   }
			   else{			/*Standard Unlock*/

				for(k = 0; k <= 20; k++)			//CounterClockwise(unlock)
				{
					GPIOC_PDOR = 0x36;				//0011 0110 ENA/ENB and PTC1 and PTC2
					software_delay(delay);			//Delay(speed)
					GPIOC_PDOR = 0x35;				//0011 0101 ENA/ENB and PTC0 and PTC2
					software_delay(delay);			//Delay(speed)
					GPIOC_PDOR = 0x39;				//0011 1001 ENA/ENB and PTC0 and PTC3
					software_delay(delay);			//Delay(speed)
					GPIOC_PDOR = 0x3A;				//0011 1010 ENA/ENB and PTC1 and PTC3
					software_delay(delay);			//Delay(speed)
				}
			   }
		   }
		   else		/*For the wrong password*/
		   {
			   	/*Lock remains locked. Will do nothing except blink red until otherwise.*/
			    GPIOB_PDOR = (1 << 21);              //Turn on red LED
			   	software_delay(delay);				 //Wait Delay
			   	GPIOB_PDOR = (1 << 22) | (1 << 21);  //Turn off red LED
			    //GPIOB_PDOR = (1 << 21);
			   	software_delay(delay);				 //Wait Delay
		   }
		}
	}
}

/*END OF CODE*/
