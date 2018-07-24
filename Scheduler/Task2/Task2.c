/**
 * @file    Task2.c
 * @author  Ferenc Nemeth
 * @date    21 Jul 2018
 * @brief   Demo task for blinking a LED.
 */ 
 
#include "Task2.h"

/**
 * @brief   Changes the state of a pin.
 * @param   void
 * @return  void
 */
void Task2_Blink(void)
{
    PORTD ^= (1 << PORTD2);
}