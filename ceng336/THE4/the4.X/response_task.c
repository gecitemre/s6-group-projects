#include "common.h"

extern byte output_buffer[MAX_COMMAND_LENGTH], input_buffer[MAX_RESPONSE_LENGTH];
extern unsigned short money;
extern simulator_mode mode;
extern ingredient_status ingredients[4];

// This function guarantees that first 3 bytes of output_buffer won't be changed if no customer is served.
void Serve() {
        byte i = 0;
        for (i = 0; i < 3; i++) {
                if (customers[i].patience < 2) continue;
                byte l = 3;
                for (byte j = 0; j < 2; j++) {
                        switch (customers[i].ingredients[j]) {
                                case 'C':
                                        goto next_customer;
                                case 'S':
                                        goto next_customer;
                                case 'N':
                                        output_buffer[l++] = 'N';
                                        break;
                                default:
                                        byte k;
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
                WaitEvent(VALUE(RESPONSE_EVENT));
                ClearEvent(VALUE(RESPONSE_EVENT));
                byte *input_pointer = input_buffer;
                switch (*++input_pointer)
                {
                case 'G':
                        // GO
                        mode = ACTIVE;
                        SetEvent(COMMAND_TASK, VALUE(COMMAND_EVENT));
                        break;
                case 'E':
                        // END
                        break;
                case 'R':
                        // STATUS
                        for (byte i = 0; i < 3; i++)
                        {
                                customers[i].customer_id = *++input_pointer;
                                customers[i].ingredients[0] = *++input_pointer;
                                customers[i].ingredients[1] = *++input_pointer;
                                customers[i].patience = *++input_pointer;
                        }

                        for (byte i = 0; i < 4; i++)
                        {
                                ingredients[i] = *++input_pointer;
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