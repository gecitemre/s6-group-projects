#include "common.h"

extern byte* output_pointer, output_buffer[MAX_COMMAND_LENGTH];

// send command output
TASK(COMMAND_TASK) 
{
    WaitEvent(COMMAND_EVENT_MASK);
    ClearEvent(COMMAND_EVENT_MASK);
    SetRelAlarm(ALARM_TSK0, 30, 50);
	while(1) {
        WaitEvent(ALARM_EVENT_MASK);
        ClearEvent(ALARM_EVENT_MASK);
        TXREG = *output_pointer;
        if (*output_pointer == ':') {
            // reset output buffer to $W:
            output_buffer[1] = 'W';
            output_buffer[2] = ':';
            output_pointer = output_buffer;
            WaitEvent(COMMAND_EVENT_MASK);
            ClearEvent(COMMAND_EVENT_MASK);
        }
        output_pointer++;
	}
	TerminateTask();
}

/* End of File : tsk_task1.c */