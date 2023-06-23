#include "common.h"

/**********************************************************************
 * Globals
 **********************************************************************/
ingredient_status ingredients[4] = {NONE, NONE, NONE, NONE};
customer_status customers[3];
unsigned short money;
byte input_buffer[MAX_RESPONSE_LENGTH];
byte *input_pointer = input_buffer;
byte output_buffer[MAX_COMMAND_LENGTH] = {'$', 'W', ':'};
byte *output_pointer = output_buffer;
