#include "common.h"
#include "pic_hash_compute.h"

extern byte input_buffer[MAX_RESPONSE_LENGTH];
extern byte output_buffer[MAX_COMMAND_LENGTH];
byte hash_ready = 0;
byte hash_output[MAX_COMMAND_LENGTH + 1];

TASK(HASH_TASK)
{
	while(mode != END) {
                byte hash_input[MAX_RESPONSE_LENGTH + 1];
                byte i;
                
                WaitEvent(HASH_EVENT_MASK);
                ClearEvent(HASH_EVENT_MASK);
                
                for (i = 0; i < MAX_RESPONSE_LENGTH; i++) {
                hash_input[i] = input_buffer[i];
                }
                hash_input[MAX_RESPONSE_LENGTH] = 0;
                
                compute_hash(hash_input, hash_output);
                hash_ready = 1;

	}
	TerminateTask();
}
