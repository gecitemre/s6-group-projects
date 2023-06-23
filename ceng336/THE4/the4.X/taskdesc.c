#include "common.h"

/**********************************************************************
 * --------------------- COUNTER & ALARM DEFINITION -------------------
 **********************************************************************/
Counter Counter_list[] = 
{
       /*******************************************************************
        * -------------------------- First counter ------------------------
        *******************************************************************/
    {
        {
            200,                                /* maxAllowedValue        */
            10,                                /* ticksPerBase           */
            100                                 /* minCycle               */
        },
        0,                                    /* CounterValue           */
        0                                     /* Nbr of Tick for 1 CPT  */
    }
};

Counter Counter_kernel = 
{
    {
        65535,                              /* maxAllowedValue        */
        1,                              /* ticksPerBase           */
        0                               /* minCycle               */
    },
    0,                                    /* CounterValue           */
    0                                     /* Nbr of Tick for 1 CPT  */
};

AlarmObject Alarm_list[] = 
{
   /*******************************************************************
    * -------------------------- First task ---------------------------
    *******************************************************************/
    {
        OFF,                                  /* State                   */
        0,                                    /* AlarmValue              */
        0,                                    /* Cycle                   */
        &Counter_kernel,                      /* ptrCounter              */
        COMMAND_TASK_ID,
        ALARM_EVENT_MASK,
        0                                     /* CallBack                */
    },
};

#define _ALARMNUMBER_          sizeof(Alarm_list)/sizeof(AlarmObject)
#define _COUNTERNUMBER_        sizeof(Counter_list)/sizeof(Counter)
unsigned char ALARMNUMBER    = _ALARMNUMBER_;
unsigned char COUNTERNUMBER  = _COUNTERNUMBER_;
unsigned long global_counter = 0;

/**********************************************************************
 * --------------------- COUNTER & ALARM DEFINITION -------------------
 **********************************************************************/
Resource Resource_list[] = 
{
    {
        10,                                /* priority           */
        0,                                /* Task prio          */
        0,                                /* lock               */
    }
};

#define _RESOURCENUMBER_       sizeof(Resource_list)/sizeof(Resource)
unsigned char RESOURCENUMBER = _RESOURCENUMBER_;

/**********************************************************************
 * ----------------------- TASK & STACK DEFINITION --------------------
 **********************************************************************/
#define DEFAULT_STACK_SIZE      256

DeclareTask(RESPONSE_TASK);
DeclareTask(COMMAND_TASK);
DeclareTask(HASH_TASK);
DeclareTask(LCD_TASK);


#define LCD_TASK_PRIORITY 15
#define COMMAND_TASK_PRIORITY 12
#define RESPONSE_TASK_PRIORITY 9
#define HASH_TASK_PRIORITY 6

// to avoid any C18 map error : regroup the stacks into blocks
// of 256 bytes (except the last one).
#pragma		udata      STACK_A   
volatile unsigned char stack0[DEFAULT_STACK_SIZE];
#pragma		udata      STACK_B   
volatile unsigned char stack1[DEFAULT_STACK_SIZE];
#pragma udata STACK_C
volatile unsigned char stack2[DEFAULT_STACK_SIZE];
#pragma udata STACK_D
volatile unsigned char stack3[DEFAULT_STACK_SIZE];
#pragma		udata

/**********************************************************************
 * ---------------------- TASK DESCRIPTOR SECTION ---------------------
 **********************************************************************/
#pragma		romdata		DESC_ROM
const rom unsigned int descromarea;
/**********************************************************************
 * -----------------------------  task 0 ------------------------------
 **********************************************************************/
rom_desc_tsk rom_desc_task0 = {
    RESPONSE_TASK_PRIORITY,                       /* prioinit from 0 to 15       */
    stack0,                           /* stack address (16 bits)     */
    RESPONSE_TASK,                            /* start address (16 bits)     */
    READY,                            /* state at init phase         */
    RESPONSE_TASK_ID,                         /* id_tsk from 0 to 15         */
    sizeof(stack0)                    /* stack size    (16 bits)     */
};

/**********************************************************************
 * -----------------------------  task 1 ------------------------------
 **********************************************************************/
rom_desc_tsk rom_desc_task1 = {
    COMMAND_TASK_PRIORITY,                       /* prioinit from 0 to 15       */
    stack1,                           /* stack address (16 bits)     */
    COMMAND_TASK,                            /* start address (16 bits)     */
    READY,                            /* state at init phase         */
    COMMAND_TASK_ID,                         /* id_tsk from 0 to 15         */
    sizeof(stack1)                    /* stack size    (16 bits)     */
};

/**********************************************************************
 * -----------------------------  task 2 ------------------------------
 **********************************************************************/
rom_desc_tsk rom_desc_task2 = {
    HASH_TASK_PRIORITY,                       /* prioinit from 0 to 15       */
    stack2,                           /* stack address (16 bits)     */
    HASH_TASK,                            /* start address (16 bits)     */
    READY,                            /* state at init phase         */
    HASH_TASK_ID,                         /* id_tsk from 0 to 15         */
    sizeof(stack2)                    /* stack size    (16 bits)     */
};

rom_desc_tsk rom_desc_task3 = {
    LCD_TASK_PRIORITY,                       /* prioinit from 0 to 15       */
    stack3,                           /* stack address (16 bits)     */
    LCD_TASK,                            /* start address (16 bits)     */
    READY,                            /* state at init phase         */
    LCD_TASK_ID,                         /* id_tsk from 0 to 15         */
    sizeof(stack3)                    /* stack size    (16 bits)     */
};

/**********************************************************************
 * --------------------- END TASK DESCRIPTOR SECTION ------------------
 **********************************************************************/
rom_desc_tsk end = {
    0x00,                              /* prioinit from 0 to 15       */
    0x0000,                            /* stack address (16 bits)     */
    0x0000,                            /* start address (16 bits)     */
    0x00,                              /* state at init phase         */
    0x00,                              /* id_tsk from 0 to 15         */
    0x0000                             /* stack size    (16 bits)     */
};

volatile rom unsigned int * taskdesc_addr = (&(descromarea)+1);
  
/* End of File : taskdesc.c */
