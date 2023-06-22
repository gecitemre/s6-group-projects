#include "common.h"

extern byte* output_pointer, output_buffer[MAX_COMMAND_LENGTH];

TASK(COMMAND_TASK) 
{
	SetRelAlarm(ALARM_TSK0, 20 * 90, 50);

	while(1) {
        WaitEvent(ALARM_EVENT_MASK);
        ClearEvent(ALARM_EVENT_MASK);
        TXREG = *output_pointer++;
        if (*output_pointer == ':') {
            output_pointer = output_buffer;
            WaitEvent(COMMAND_EVENT_MASK);
            ClearEvent(COMMAND_EVENT_MASK);
        }
	}
	TerminateTask();
}

/* End of File : tsk_task1.c */