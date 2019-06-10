/*
 * read_PWM_ICU.main.c
 *
 *  Created on: ??þ/??þ/????
 *      Author: Zizo Rashad
 */

#include"Types.h"
#include"registers.h"
#include"macros.h"
#include"DIO.h"
//#define F_CPU 8000000
//#include<util/delay.h>
#include"KEYPAD.h"
#include"LCD.h"
#include"TIMER0.h"
#define F_CPU 8000000


volatile u32 counter=0;
volatile u8 flag=0;
volatile u32 snap1,snap2,snap3;
volatile u32 f=100;

// overflow interrupt
#define __INTR_ATTRS used,externally_visible
void  __vector_9(void)__attribute__( (signal,__INTR_ATTRS) );
void __vector_9 (void)
{
	counter ++;
}

// icu interrupt
#define __INTR_ATTRS used,externally_visible
void  __vector_6(void)__attribute__( (signal,__INTR_ATTRS) );
void __vector_6 (void)
{
	switch (flag)
	{
	case 0:
		counter = 0;
		snap1 = ICR1;
		// falling edge icu
		CLR_BIT(TCCR1B,6);
		break;
	case 1:
		snap2 = (counter * 65536) + ICR1;
		// raising edge icu
		SET_BIT(TCCR1B,6);
		break;
	case 2:
		snap3 = (counter * 65536) + ICR1;
		// disable ICU
		CLR_BIT(TIMSK,5);
		break;
	}
	flag ++;
}

int main(void)
{
//	DIO_vidSetPinDirection(1,3,1);
	DIO_vidSetPinDirection(3,6,0);
	//___________ generate PWM using TIMER0________________//

//	f = TIMER0_u32FastPwm(2,50);
	f = TIMER0_u32PhasePwm(5,50);
/*	
	// fast pwm
	SET_BIT(TCCR0,3);
	SET_BIT(TCCR0,6);
	// clear on match
	CLR_BIT(TCCR0,4);
	SET_BIT(TCCR0,5);
	// prescaller 1024/256/64/8/1
	SET_BIT(TCCR0,0);
	CLR_BIT(TCCR0,1);
	SET_BIT(TCCR0,2);
	OCR0=128;
*/
	//____________ICU & TIMER1 counting time______________//
	// normal chanels disconecting
	CLR_BIT(TCCR1A,6);
	CLR_BIT(TCCR1A,7);
	// Normal mode
	CLR_BIT(TCCR1A,0);
	CLR_BIT(TCCR1A,1);
	CLR_BIT(TCCR1B,3);
	CLR_BIT(TCCR1B,4);
	// rising edge icu
	SET_BIT(TCCR1B,6);
	// no prescaller
	SET_BIT(TCCR1B,0);
	CLR_BIT(TCCR1B,1);
	CLR_BIT(TCCR1B,2);
	// enable ICU
	SET_BIT(TIMSK,5);
	//enable overflow timer 1
	SET_BIT(TIMSK,2);
	// global interrupt
	SET_BIT(SREG,7);

	LCD_vidInit();
	u32 duty,freq;
	while (1)
	{
		if (flag ==3)
		{
			duty = ( ((u32)snap2-snap1)*100 ) / ((u32)snap3-snap1);
			freq = 8000000 / ((u32)snap3-snap1) ;
			flag = 0;

			// print duty value
			LCD_vidSetPosition(1,1);
			LCD_vidWriteString((u8*) "Duty= ");
			LCD_vidWriteNumber(duty);
			// printing
			LCD_vidSetPosition(2,1);
			LCD_vidWriteString((u8*) "freq= ");
			LCD_vidWriteNumber(freq);

			//
			LCD_vidSetPosition(1,11);
			LCD_vidWriteString((u8*) "f=");
			LCD_vidWriteNumber(f);

			// raising edge icu
			SET_BIT(TCCR1B,6);
			// reset flag
			SET_BIT(TIFR,5);
			// Re-Enable icu interrupt
			SET_BIT(TIMSK,5);
		}
		//OCR0 = 128;
//		LCD_vidSetPosition(1,10);
//		LCD_vidWriteString((u8*)"yassin");

	}

	return 0;
}

