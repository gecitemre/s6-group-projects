#include "common.h"

/**********************************************************************
 * ----------------------- GLOBAL VARIABLES ---------------------------
 **********************************************************************/
extern byte input_buffer[21];
extern uint16_t money;
/**********************************************************************
 * ----------------------- LOCAL FUNCTIONS ----------------------------
 **********************************************************************/

/**********************************************************************
 * ------------------------------ TASK1 -------------------------------
 *
 * Basic echo function test. Typing a single char should reply with the same char,
 * then some chars that alphabetically precede this char. You will need buffers to echo strings.
 *
 **********************************************************************/

TASK(TASK1)
{
        while (1)
        {
                WaitEvent(RESPONSE_EVENT);
                ClearEvent(RESPONSE_EVENT);
                byte *current_byte = input_buffer;
                switch (*++current_byte)
                {
                case 'G':
                        // GO
                        break;
                case 'E':
                        // END
                        break;
                case 'R':
                        // STATUS
                        for (byte i = 0; i < 3; i++)
                        {
                                customers[i].customer_id = *++current_byte;
                                customers[i].ingredients[0] = *++current_byte;
                                customers[i].ingredients[1] = *++current_byte;
                                customers[i].patience = *++current_byte;
                        }
                        for (byte i = 0; i < 4; i++)
                        {
                                ingredients[i] = *++current_byte;
                        }
                        money = *(uint16_t*)(++current_byte);
                        break;
                case 'H':
                        // HASH
                        break;
                }
        }
        TerminateTask();
}

/* End of File : tsk_task1.c */