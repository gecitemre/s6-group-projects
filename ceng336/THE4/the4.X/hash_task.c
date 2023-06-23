#include "common.h"
#include "pic_hash_compute.h"

extern byte input_buffer[MAX_RESPONSE_LENGTH];
extern byte output_buffer[MAX_COMMAND_LENGTH];

TASK(HASH_TASK)
{
	while(mode != END) {
        byte i;
        byte inp[22], out[19];
        byte i = 0;
        
        WaitEvent(HASH_EVENT_MASK);
        ClearEvent(HASH_EVENT_MASK);
        
        for (i = 0; i < MAX_RESPONSE_LENGTH; i++) {
            inp[i] = input_buffer[i];
        }
        inp[MAX_RESPONSE_LENGTH] = 0;
        
        compute_hash(inp, out);
        
        for (i = 0; i < MAX_COMMAND_LENGTH; i++) {
            output_buffer[i] = out[i];
        }
	}
	TerminateTask();
}
