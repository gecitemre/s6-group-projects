#include "common.h"

extern byte* output_pointer, output_buffer[MAX_COMMAND_LENGTH];
extern ingredient_status ingredients[4];

// send command output
TASK(COMMAND_TASK) 
{
	while(mode != END) {
        byte i = 4, toss_index = 0;
        WaitEvent(ALARM_EVENT_MASK);
        ClearEvent(ALARM_EVENT_MASK);
        while (i--) {
            if (ingredients[i] == 'N') break;
            if (ingredients[i] != 'C' && ingredients[i] != 'S') toss_index = i;
        }
        if (i == -1 && output_buffer[1] == 'W') {
            // TOSS
            output_buffer[1] = 'T';
            output_buffer[2] = i;
            output_buffer[3] = ':';
        }
        while (mode != END) {
            TXREG = *output_pointer;
            if (*output_pointer == ':' && mode != END) {
                // reset output buffer to $W:
                output_buffer[1] = 'W';
                output_buffer[2] = ':';
                output_pointer = output_buffer;
                break;
            }
            output_pointer++;
            WaitEvent(COMMAND_EVENT_MASK);
            ClearEvent(COMMAND_EVENT_MASK);
        }
	}
	TerminateTask();
}

/* End of File : tsk_task1.c */