#include "common.h"

extern byte hash_ready;
extern byte* output_pointer, output_buffer[MAX_COMMAND_LENGTH], hash_output[MAX_COMMAND_LENGTH + 1];
extern ingredient_status ingredients[4];
extern byte food_judge_served;

/**
 * @brief The task responsible for sending commands to the robot.
 */
TASK(COMMAND_TASK) {
    byte mode;
	while(mode != END) {
        byte i = 4, toss_index = 0;

        WaitEvent(ALARM_EVENT_MASK);
        ClearEvent(ALARM_EVENT_MASK);
        if (hash_ready) {
            output_pointer = hash_output;
            hash_ready = 0;
            mode = 0;
        }
        else {
            mode = 1;
            output_pointer = output_buffer;
            while (i--) {
                if (ingredients[i] == 'N') break;
                if (ingredients[i] != 'C' && ingredients[i] != 'S') toss_index = i;
            }
            if (i == -1 && output_buffer[1] == 'W') {
                // TOSS
                output_buffer[1] = 'T';
                output_buffer[2] = toss_index;
                output_buffer[3] = ':';
            }
        }

        while (mode != END) {
            TXREG = *output_pointer;
            if (*output_pointer == ':' && mode != END) {
                // reset output buffer to $W:
                output_buffer[1] = 'W';
                output_buffer[2] = ':';
                if (mode == 0) {
                    food_judge_served = 0;
                }
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