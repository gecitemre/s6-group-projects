#include "common.h"

extern ingredient_status ingredients[4];
extern typedef enum {IDLE, ACTIVE, END} simulator_mode;
extern customer_status customers[3];
extern unsigned short money;
extern byte input_buffer[MAX_RESPONSE_LENGTH];
extern byte output_buffer[MAX_COMMAND_LENGTH];
extern byte *output_pointer = output_buffer;
// This function guarantees that first 3 bytes of output_buffer won't be changed if no customer is served.
void Serve() {
        byte i = 0, l, j, k;
        for (i = 0; i < 3; i++) {
                if (customers[i].patience < 2) continue;
                l = 3;
                for (j = 0; j < 2; j++) {
                        switch (customers[i].ingredients[j]) {
                                case 'C':
                                        goto next_customer;
                                case 'S':
                                        goto next_customer;
                                case 'N':
                                        output_buffer[l++] = 'N';
                                        break;
                                default:
                                        for (k = 0; k < 4; k++) {
                                                if (ingredients[k] == customers[i].ingredients[j]) output_buffer[l++] = k;
                                                break;
                                        }
                                        if (k == 4) goto next_customer; // ingredient not found
                        }
                }
                output_buffer[2] = customers[i].customer_id;
                break;
                next_customer:;
        }
        if (i != 3) // customer found, send cook command
                output_buffer[1] = 'C';
}

TASK(RESPONSE_TASK)
{
        while (1)
        {
                WaitEvent(RESPONSE_EVENT_MASK);
                ClearEvent(RESPONSE_EVENT_MASK);
                byte index = 0;
                switch (input_buffer[++index])
                {
                case 'G':
                        // GO
                        simulator_mode = ACTIVE;
                        SetEvent(COMMAND_EVENT, COMMAND_EVENT_MASK);
                        break;
                case 'E':
                        // END
                        break;
                case 'R':
                        // STATUS
                        for (byte i = 0; i < 3; i++)
                        {
                                customers[i].customer_id = input_buffer[++index];
                                customers[i].ingredients[0] = input_buffer[++index];
                                customers[i].ingredients[1] = input_buffer[++index];
                                customers[i].patience = input_buffer[++index];
                        }

                        for (byte i = 0; i < 4; i++)
                        {
                                ingredients[i] = input_buffer[++index];
                        }
                        money = *(unsigned short*)(++input_pointer);
                        input_pointer = input_buffer;
                        Serve();
                        break;
                case 'H':
                        // HASH
                        break;
                }
        }
}

/* End of File : tsk_task1.c */