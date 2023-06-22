#include "common.h"

extern byte* output_pointer, output_buffer[MAX_COMMAND_LENGTH];

// send command output
TASK(COMMAND_TASK) 
{
	while(1) {
        while (1) {
            TXREG = *output_pointer;
            if (*output_pointer == ':') {
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
        WaitEvent(ALARM_EVENT_MASK);
        ClearEvent(ALARM_EVENT_MASK);
	}
	TerminateTask();
}

/* End of File : tsk_task1.c */