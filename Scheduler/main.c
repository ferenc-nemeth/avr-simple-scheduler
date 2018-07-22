/**
 * @file	main.c
 * @author	Ferenc Nemeth
 * @date	18 Jul 2018
 * @brief	This is the main file with an example of three tasks.
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "OS.h"
#include "Task0.h"
#include "Task1.h"
#include "Task2.h"

/* Timer interrupt. The OS_TaskTimer() shall be here. */
ISR (TIMER1_OVF_vect)
{
	TCNT1 = 31250;
	OS_TaskTimer();
}

/* Main function. */
int16_t main(void)
{
	/* Initialize the 3 LEDS and turn them off as default value. */
    DDRD = (1 << DDD0) | (1 << DDD1) | (1 << DDD2);
	PORTD &= ~(1 << PORTD0) & ~(1 << PORTD1) & ~(1 << PORTD2);

	/* Set the timer interrupt to fire every 1 sec. */
	TCCR1B = (1 << CS12);
	TCNT1 = 31250;
	TIMSK1 = (1 << TOIE1);
	sei();

	/* Register the three blinker tasks. */
	/* Note: Task1_Blink is registered as suspended. It is never going to be called until someone changes the task's state manually. */
	/* The recommended default state is BLOCKED, the SUSPENDED is only for demonstration purpose. */
	OS_TaskCreate(&Task0_Blink, 1, BLOCKED);
	OS_TaskCreate(&Task1_Blink, 2, SUSPENDED);
	OS_TaskCreate(&Task2_Blink, 5, BLOCKED);

	/* The infinte loop, only the OS_TaskExecution() function shall be here. */
    while (1) 
    {
	OS_TaskExecution();
    }
	return 0;
}

