#include "common.h"

extern ingredient_status ingredients[4];
simulator_mode mode;
extern customer_status customers[3];
extern unsigned short money;
extern byte input_buffer[MAX_RESPONSE_LENGTH];
extern byte output_buffer[MAX_COMMAND_LENGTH];
extern byte *output_pointer = output_buffer;
extern unsigned short money;
// This function guarantees that first 3 bytes of output_buffer won't be changed if no customer is served.

void Serve()
{
        byte i, j, k;
        
        if (mode == END) return;
        
        for (i = 0; i < 3; i++)
        {
                if (!IsPresent(customers[i]) || customers[i].patience < 2 || customers[i].served)
                        continue;
                for (j = 0; j < 2; j++)
                {
                        switch (customers[i].ingredients[j])
                        {
                        case 'C':
                                goto next_customer;
                        case 'S':
                                goto next_customer;
                        case 'N':
                                output_buffer[3 + j] = 'N';
                                break;
                        default:
                                for (k = 0; k < 4; k++)
                                {
                                        if (ingredients[k] == customers[i].ingredients[j])
                                        {
                                                output_buffer[3 + j] = k;
                                                break;
                                        }
                                }
                                if (k == 4)
                                        goto next_customer; // ingredient not found
                        }
                }
                output_buffer[2] = customers[i].customer_id;
                break;
        next_customer:;
        }
        if (i == 3)
                return; // customer not found

        for (j = 0; j < 2; j++)
        {
                switch (customers[i].ingredients[j])
                {
                case 'C':
                case 'S':
                case 'N':
                        break;
                default:
                        ingredients[customers[i].ingredients[j]] = 'C';
                }
        }
        output_buffer[1] = 'C';
        output_buffer[5] = ':';
        customers[i].served = 1;
}

TASK(RESPONSE_TASK)
{
        while (1)
        {
                byte index = 1, i;
                WaitEvent(RESPONSE_EVENT_MASK);
                ClearEvent(RESPONSE_EVENT_MASK);
                switch (input_buffer[index++])
                {
                case 'G':
                        // GO
                        mode = ACTIVE;
                        SetRelAlarm(ALARM_TSK0, 30, 50);
                        break;
                case 'E':
                        // END
                        mode = END;
                        break;
                case 'R':
                        // STATUS
                        for (i = 0; i < 3; i++)
                        {
                                if (!(customers[i].customer_id == input_buffer[index])) {
                                    customers[i].served = 0;
                                }
                                customers[i].customer_id = input_buffer[index++];
                                customers[i].ingredients[0] = input_buffer[index++];
                                customers[i].ingredients[1] = input_buffer[index++];
                                customers[i].patience = input_buffer[index++];
                        }

                        for (i = 0; i < 4; i++)
                        {
                                ingredients[i] = input_buffer[index++];
                        }
                        money = *(unsigned short *)(input_buffer + (index++));
                        Serve();
                        break;
                case 'H':
                        SetEvent(HASH_TASK_ID, HASH_EVENT_MASK);
                        break;
                }
        }
}

/* End of File : tsk_task1.c */