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

#define ALARM_EVENT       0x80
#define RESPONSE_EVENT        0x40

/***********************************************************************
 * ----------------------------- Task ID -------------------------------
 **********************************************************************/
/* Info about the tasks:
 * TASK0: USART
 * TASK1: USART
 */

/* Priorities of the tasks */
#define TASK0_PRIO           8
#define TASK1_PRIO           7

#define ALARM_TSK0           0
#define byte unsigned char

enum {ALARM_EVENT_ID, RESPONSE_EVENT_ID};

typedef enum {FALSE = 0, TRUE = 1} bool;

typedef enum {IDLE, ACTIVE, END} simulator_mode;

typedef enum {MEAT = 'M', BREAD = 'B', POTATO = 'P', COOKING = 'C', SLOW_COOKING = 'S', NONE = 'N'} ingredient_status;
ingredient_status ingredients[4];

struct customer_status
{
    byte customer_id;
    ingredient_status ingredients[2];
    byte patience;
} customers[3];

uint16_t money;

bool is_present(customer_status customer) {
    return !(customer.customer_id == 0 && ingredients[0] == 'N' && ingredients[1] == 'N' && patience == 0);
}

bool is_food_judge(customer_status customer) {
    return customer.ingredients[0] == 'F' && customer.ingredients[1] == 'J';
}

#endif

/* End of File : common.h */
