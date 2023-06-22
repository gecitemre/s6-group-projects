#ifndef COMMON_H
#define COMMON_H

#include "device.h"

/***********************************************************************
 * ------------------------ Timer settings -----------------------------
 **********************************************************************/
#define _10MHZ	63320
#define _16MHZ	61768
#define _20MHZ	60768
#define _32MHZ	57768
#define _40MHZ 	55768

/***********************************************************************
 * ----------------------------- Events --------------------------------
 **********************************************************************/

enum {ALARM_EVENT, RESPONSE_EVENT, COMMAND_EVENT}; // Event IDs
#define VALUE(ID) (1 << ID)
#define ALARM_TSK0 0

/***********************************************************************
 * --------------------------- Game Logic ------------------------------
 **********************************************************************/
#define byte unsigned char

typedef enum {IDLE, ACTIVE, END} simulator_mode;
simulator_mode mode;

typedef enum {MEAT = 'M', BREAD = 'B', POTATO = 'P', COOKING = 'C', SLOW_COOKING = 'S', NONE = 'N'} ingredient_status;

ingredient_status ingredients[4];

typedef struct
{
    byte customer_id;
    ingredient_status ingredients[2];
    byte patience;
} customer_status;

customer_status customers[3];

unsigned short money;

byte IsPresent(customer_status customer) {
    return !(customer.customer_id == 0 && ingredients[0] == 'N' && ingredients[1] == 'N' && customer.patience == 0);
}

byte IsFoodJudge(customer_status customer) {
    return customer.ingredients[0] == 'F';
}

#define MAX_RESPONSE_LENGTH 21
#define MAX_COMMAND_LENGTH 18
byte input_buffer[MAX_RESPONSE_LENGTH];
byte *input_pointer = input_buffer;
byte output_buffer[MAX_COMMAND_LENGTH] = {'$', 'W', ':'};
byte *output_pointer = output_buffer;

#endif

/* End of File : common.h */
