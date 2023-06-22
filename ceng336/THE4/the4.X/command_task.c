#include "common.h"

extern byte* output_pointer, output_buffer[MAX_COMMAND_LENGTH];

TASK(COMMAND_TASK) 
{
	SetRelAlarm(ALARM_TSK0, 20 * 90, 50);

	while(1) {
        WaitEvent(VALUE(ALARM_EVENT));
        ClearEvent(VALUE(ALARM_EVENT));
        TXREG = *output_pointer++;
        if (*output_pointer == ':') {
            output_pointer = output_buffer;
            WaitEvent(VALUE(COMMAND_EVENT));
            ClearEvent(VALUE(COMMAND_EVENT));
        }
	}
	TerminateTask();
}

/* End of File : tsk_task1.c */