/* 
 * File:   Definitions.h
 * Author: Merve Asiler
 *
 * Created on April 17, 2023, 6:52 PM
 */

#ifndef THE3_H
#define	THE3_H

#ifdef	__cplusplus
extern "C" {
#endif
#include <xc.h>
#include "lcd.h"

#define FRISBEE_INDEX 4
typedef enum {
    TEAM_A_PLAYER=0, TEAM_B_PLAYER=1
} player_type;

// typedef enum {
//     FALSE = 0, TRUE = 255
// } boolean; // 255 because ~0 = 255 (not FALSE = TRUE)

typedef enum {
    FRISBEE, FRISBEE_TARGET
} frisbee_type;

typedef struct {
    unsigned selected : 1;
    unsigned frisbee : 1;
    unsigned type : 1;
} object_data;

#define LCDAddSpecialCharacterFromObjectData(data, character) LCDAddSpecialCharacter(*((byte*)(void*)&(data)), character)

typedef struct {
    byte x, y;
    object_data data;
} object;

typedef enum {
    INACTIVE_MODE, ACTIVE_MODE
} game_mode;

byte teamA_player[] = {
                  0b10001,
                  0b10101,
                  0b01010,
                  0b00100,
                  0b00100,
                  0b00100,
                  0b01010,
                  0b01010
                };              // teamA_player NOT holding the frisbee, NOT indicated by the cursor

byte teamB_player[] = {
                  0b10001,
                  0b10101,
                  0b01010,
                  0b00100,
                  0b01110,
                  0b11111,
                  0b01010,
                  0b01010
                };              // teamB_player NOT holding the frisbee, NOT indicated by the cursor

byte selected_teamA_player[] = {
                  0b10001,
                  0b10101,
                  0b01010,
                  0b00100,
                  0b00100,
                  0b00100,
                  0b01010,
                  0b11111
                };              // teamA_player, NOT holding the frisbee, indicated by the cursor

byte selected_teamB_player[] = {
                  0b10001,
                  0b10101,
                  0b01010,
                  0b00100,
                  0b01110,
                  0b11111,
                  0b01010,
                  0b11111
                };              // teamB_player, NOT holding the frisbee, indicated by the cursor


byte selected_teamA_player_with_frisbee[] = {
                  0b11111,
                  0b10101,
                  0b01010,
                  0b00100,
                  0b00100,
                  0b00100,
                  0b01010,
                  0b11111
                };              // teamA_player, holding the frisbee, indicated by the cursor

byte selected_teamB_player_with_frisbee[] = {
                  0b11111,
                  0b10101,
                  0b01010,
                  0b00100,
                  0b01110,
                  0b11111,
                  0b01010,
                  0b11111
                };              // teamB_player, holding the frisbee, indicated by the cursor

byte frisbee[] = {
                  0b01110,
                  0b11111,
                  0b11111,
                  0b11111,
                  0b01110,
                  0b00000,
                  0b00000,
                  0b00000
                };              // the frisbee itself

byte frisbee_target[] = {
                  0b01110,
                  0b10001,
                  0b10001,
                  0b10001,
                  0b01110,
                  0b00000,
                  0b00000,
                  0b00000
                };              // the character indicating the target position of the frisbee
                                // you should blink this character 



unsigned short frisbee_steps[15][2];                    // maximum 15 steps in x (horizontal) and y (vertical) directions

unsigned int first_round = 1;
unsigned int remaining_frisbee_moves = 0;
byte old_PORTB;
object objects[5];

// function declarations
unsigned short ComputeFrisbeeTargetAndRoute(unsigned short current_frisbee_x_position, unsigned short current_frisbee_y_position);   // a simple implementation is given below
unsigned short Random(unsigned short modulo); // YOU SHOULD IMPLEMENT THIS FUNCTION ON YOUR OWN

void ClearObject(object* c)
{
    LCDGoto(c->x, c->y);
    LCDDat(' ');
}

void DisplayObject(object* c)
{
    LCDGoto(c->x, c->y);
    LCDDat(*(unsigned*)(void*)&(c->data));
}

unsigned short ComputeFrisbeeTargetAndRoute(unsigned short current_frisbee_x_position, unsigned short current_frisbee_y_position) {
    
   // YOU CAN DO ANY NECESSARY CHANGES IN THIS FUNCTION //
    
    unsigned short x_step_size, y_step_size;    // hold the number of cells to walk in x and y dimensions, respectively
    unsigned short number_of_steps;             // hold the maximum number of steps to walk
    unsigned short target_x, target_y;          // hold the target <x,y> coordinates of the frisbee
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // compute target <x,y> position for the frisbee
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    
    while(1) {  // loop until finding a valid position
        unsigned int collidesWithAPlayer = 0;
        
        target_x = Random(16) + 1; // find a random integer in [0, 15] + 1
        target_y = Random(4) + 1;  // find a random integer in [0, 3] + 1
        
        // if the target collides with a player, recompute
        for (int i = 0; i < 4; i++)
        {
            if (objects[i].x == target_x && objects[i].y == target_y)
            {
                collidesWithAPlayer = 1;
                break;
            }
        }
        
        if (collidesWithAPlayer) continue;
        
        // how many cells are there in x-dimension (horizontal) between the target and current positions of the frisbee
        if (target_x < current_frisbee_x_position)
            x_step_size = current_frisbee_x_position - target_x;
        else
            x_step_size = target_x - current_frisbee_x_position;
        
        // how many cells are there in y-dimension (vertical) between the target and current positions of the frisbee
        if (target_y < current_frisbee_y_position)
            y_step_size = current_frisbee_y_position - target_y;
        else
            y_step_size = target_y - current_frisbee_y_position;
        
        // a close target cell is not preferred much, so change the target if it is very close
        if (x_step_size <= 2 && y_step_size <= 2)
            continue;
        
        // total number of steps can be as many as the maximum of {x_step_size, y_step_size}
        if (x_step_size > y_step_size)
            number_of_steps = x_step_size;
        else
            number_of_steps = y_step_size;
        
        break;
    }
    
    
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // compute a route for the frisbee to reach the target step-by-step
    // note that each step corresponds to a 1-cell-movement in x and y directions simultaneously
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    
    // steps in x direction
    unsigned short x = current_frisbee_x_position;
    if (target_x < current_frisbee_x_position) {
        for (unsigned short i = 0; i < x_step_size; i++) {
            x = x - 1;                  // move in -x direction
            frisbee_steps[i][0] = x;    // the x-position at i^th step
        }  
    }
    else {
        for (unsigned short i = 0; i < x_step_size; i++) {
            x = x + 1;                  // move in +x direction
            frisbee_steps[i][0] = x;    // the x-position at i^th step
        } 
    }
    for (unsigned short i = x_step_size; i < number_of_steps; i++)
        frisbee_steps[i][0] = x;        // fill the rest of the steps as "no movement" in x direction 
    
    // steps in y direction
    unsigned short y = current_frisbee_y_position;
    if (target_y < current_frisbee_y_position) {
        for (unsigned short i = 0; i < y_step_size; i++) {
            y = y - 1;                  // move in -y direction
            frisbee_steps[i][1] = y;    // the y-position at i^th step
        }  
    }
    else {
        for (unsigned short i = 0; i < y_step_size; i++) {
            y = y + 1;                  // move in +y direction
            frisbee_steps[i][1] = y;    // the y-position at i^th step
        } 
    }
    for (unsigned short i = y_step_size; i < number_of_steps; i++)
        frisbee_steps[i][1] = y;        // fill the rest of the steps as "no movement" in y direction
    
    return number_of_steps;
}

unsigned short Random(unsigned short modulo) {
    unsigned randomTimerVal = TMR3L;

    TMR3L = TMR3L >> 2;

    return randomTimerVal % modulo;
}

object frisbee_target_object = (object){1,1, {0,1,FRISBEE_TARGET}};
byte cursor = 0;

game_mode mode = ACTIVE_MODE;

unsigned Collision(object *obj1, object *obj2)
{
    return (obj1->x == obj2->x) && (obj1->y == obj2->y);
}

player_type right_to_throw = TEAM_A_PLAYER;
void MoveCursorPlayer(byte x, byte y)
{
    for (unsigned i = 0; i < 4; i++)
    {
        if ((i != cursor) && objects[i].x == x && objects[i].y == y)
        {
            return;
        }
    }
    ClearObject(&objects[cursor]);
    objects[cursor].x = x;
    objects[cursor].y = y;
    if (objects[cursor].data.frisbee)
    {
        objects[cursor].data.frisbee = 0;
        DisplayObject(&objects[FRISBEE_INDEX]);
        mode = ACTIVE_MODE;
    }
    else if (Collision(&objects[cursor], &objects[FRISBEE_INDEX]))
    {
        first_round = 0;
        objects[cursor].data.frisbee = 1;
        if (mode == ACTIVE_MODE && !remaining_frisbee_moves) mode = INACTIVE_MODE;
    }
    DisplayObject(&objects[cursor]);
}


#ifdef	__cplusplus
}
#endif

#endif	/* THE3_H */
