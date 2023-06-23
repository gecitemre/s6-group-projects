#ifndef COMMON_H
#define COMMON_H

#include "device.h"
typedef unsigned char byte;

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

enum {ALARM_EVENT, RESPONSE_EVENT, COMMAND_EVENT, LCD_EVENT}; // Event IDs
enum {RESPONSE_TASK_ID, COMMAND_TASK_ID, LCD_TASK_ID}; // Task IDs
#define ALARM_EVENT_MASK 0x01
#define RESPONSE_EVENT_MASK 0x02
#define COMMAND_EVENT_MASK 0x04
#define LCD_EVENT_MASK 0x40

#define ALARM_TSK0 0

/***********************************************************************
 * --------------------------- Game Logic ------------------------------
 **********************************************************************/

typedef enum {MEAT = 'M', BREAD = 'B', POTATO = 'P', COOKING = 'C', SLOW_COOKING = 'S', NONE = 'N'} ingredient_status;

typedef enum {IDLE, ACTIVE, END} simulator_mode;

typedef struct
{
    byte customer_id;
    ingredient_status ingredients[2];
    byte patience;
    byte served;
} customer_status;

#define MAX_RESPONSE_LENGTH 21
#define MAX_COMMAND_LENGTH 18

extern simulator_mode mode;

#endif

/* End of File : common.h */
