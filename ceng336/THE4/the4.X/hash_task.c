#include "common.h"
#include "pic_hash_compute.h"

extern byte input_buffer[MAX_RESPONSE_LENGTH];
extern byte output_buffer[MAX_COMMAND_LENGTH];
byte hash_ready = 0;
byte hash_output[MAX_COMMAND_LENGTH];

/**
 * @brief The task responsible for computing the hash.
 */
TASK(HASH_TASK) {
	while(mode != END) {
                byte hash_input[MAX_RESPONSE_LENGTH + 1];
                byte i;
                
                WaitEvent(HASH_EVENT_MASK);
                ClearEvent(HASH_EVENT_MASK);
                
                for (i = 0; i < MAX_RESPONSE_LENGTH; i++) {
                    if (input_buffer[i+2] == ':') break;
                    hash_input[i] = input_buffer[i+2];
                }
                hash_input[i] = 0;
                
                compute_hash(hash_input, hash_output + 2);
                hash_output[0] = '$';
                hash_output[1] = 'H';
                hash_output[18] = ':';
                hash_ready = 1;

	}
    
	TerminateTask();
}
