#include "common.h"

char rcv_value;
ingredient_status ingredients[4];
typedef enum {IDLE, ACTIVE, END} simulator_mode;
customer_status customers[3];
unsigned short money;
byte input_buffer[MAX_RESPONSE_LENGTH];
byte *input_pointer = input_buffer;
byte output_buffer[MAX_COMMAND_LENGTH] = {'$', 'W', ':'};
byte *output_pointer = output_buffer;

byte IsPresent(customer_status customer) {
    return !(customer.customer_id == 0 && ingredients[0] == 'N' && ingredients[1] == 'N' && customer.patience == 0);
}

byte IsFoodJudge(customer_status customer) {
    return customer.ingredients[0] == 'F';
}

/**********************************************************************
 * Function you want to call when an IT occurs.
 **********************************************************************/
  extern void AddOneTick(void);
/*extern void MyOwnISR(void); */
  void InterruptVectorL(void);
  void InterruptVectorH(void);

/**********************************************************************
 * General interrupt vector. Do not modify.
 **********************************************************************/
#pragma code IT_vector_low=0x18
void Interrupt_low_vec(void)
{
   _asm goto InterruptVectorL  _endasm
}
#pragma code

#pragma code IT_vector_high=0x08
void Interrupt_high_vec(void)
{
   _asm goto InterruptVectorH  _endasm
}
#pragma code

/**********************************************************************
 * General ISR router. Complete the function core with the if or switch
 * case you need to jump to the function dedicated to the occuring IT.
 * .tmpdata and MATH_DATA are saved automaticaly with C18 v3.
 **********************************************************************/
char hello = 0;
#pragma	code _INTERRUPT_VECTORL = 0x003000
#pragma interruptlow InterruptVectorL
void InterruptVectorL(void)
{
	EnterISR();
	
	if (INTCONbits.TMR0IF == 1)
		AddOneTick();
	/* Here are the other interrupts you would desire to manage */
	if (PIR1bits.TXIF == 1) {
        if(hello > 0){
            hello--;
            TXREG = rcv_value-hello;
        }
	}
	if (PIR1bits.RCIF == 1) {
        *input_pointer = RCREG;
        if (*input_pointer == ':') {
            input_pointer = input_buffer;
            SetEvent(RESPONSE_TASK_ID, RESPONSE_EVENT_MASK);
        }
        input_pointer++;
		PIR1bits.RCIF = 0;	// clear RC1IF flag
	}
        if (RCSTAbits.OERR)
        {
          RCSTAbits.CREN = 0;
          RCSTAbits.CREN = 1;
        }
	LeaveISR();
}
#pragma	code

/* BE CARREFULL : ONLY BSR, WREG AND STATUS REGISTERS ARE SAVED  */
/* DO NOT CALL ANY FUNCTION AND USE PLEASE VERY SIMPLE CODE LIKE */
/* VARIABLE OR FLAG SETTINGS. CHECK THE ASM CODE PRODUCED BY C18 */
/* IN THE LST FILE.                                              */
#pragma	code _INTERRUPT_VECTORH = 0x003300
#pragma interrupt InterruptVectorH nosave=FSR0, TBLPTRL, TBLPTRH, TBLPTRU, TABLAT, PCLATH, PCLATU, PROD, section(".tmpdata"), section("MATH_DATA")
void InterruptVectorH(void)
{
  if (INTCONbits.INT0IF == 1)
    INTCONbits.INT0IF = 0;
}
#pragma	code



extern void _startup (void);
#pragma code _RESET_INTERRUPT_VECTOR = 0x003400
void _reset (void)
{
    _asm goto _startup _endasm
}
#pragma code


/* End of file : int.c */
