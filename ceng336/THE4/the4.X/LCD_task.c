#include "common.h"
#include "LCD.h"

/**********************************************************************
 * ----------------------- Local Variables ----------------------------
 **********************************************************************/

extern char string_pool[2][16];

/**********************************************************************
 * ------------------------------ TASK0 -------------------------------
 * 
 * Writes various strings to LCD 
 * 
 **********************************************************************/
TASK(LCD_TASK)
{
    while(1) {
        WaitEvent(ALARM_EVENT);
        ClearEvent(ALARM_EVENT);

        SetEvent(LCD_TASK_ID, LCD_EVENT_MASK);  // see the TASK in LCD.c
    }
    TerminateTask();
}
