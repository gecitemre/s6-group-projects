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

// YOU CAN CHANGE THE FUNCTION RETURN TYPES, ARGUMENTS, FUNCTION NAMES, VARIABLE NAMES, ANYTHING YOU WANT

typedef unsigned char byte;     // define byte here for readability and sanity.    
    
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

// function declarations
unsigned short compute_frisbee_target_and_route(unsigned short current_fisbee_x_position, unsigned short current_fisbee_y_position);   // a simple implementation is given below
unsigned short random_generator(unsigned short modulo); // YOU SHOULD IMPLEMENT THIS FUNCTION ON YOUR OWN

unsigned short compute_frisbee_target_and_route(unsigned short current_fisbee_x_position, unsigned short current_fisbee_y_position) {
    
   // YOU CAN DO ANY NECESSARY CHANGES IN THIS FUNCTION //
    
    unsigned short x_step_size, y_step_size;    // hold the number of cells to walk in x and y dimensions, respectively
    unsigned short number_of_steps;             // hold the maximum number of steps to walk
    unsigned short target_x, target_y;          // hold the target <x,y> coordinates of the frisbee
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // compute target <x,y> position for the frisbee
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    
    while(1) {  // loop until finding a valid position
        
        target_x = random_generator(16) + 1; // find a random integer in [0, 15] + 1
        target_y = random_generator(4) + 1;  // find a random integer in [0, 3] + 1
        
        // how many cells are there in x-dimension (horizontal) between the target and current positions of the frisbee
        if (target_x < current_fisbee_x_position)
            x_step_size = current_fisbee_x_position - target_x;
        else
            x_step_size = target_x - current_fisbee_x_position;
        
        // how many cells are there in y-dimension (vertical) between the target and current positions of the frisbee
        if (target_y < current_fisbee_y_position)
            y_step_size = current_fisbee_y_position - target_y;
        else
            y_step_size = target_y - current_fisbee_y_position;
        
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
    unsigned short x = current_fisbee_x_position;
    if (target_x < current_fisbee_x_position) {
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
    unsigned short y = current_fisbee_y_position;
    if (target_y < current_fisbee_y_position) {
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

unsigned short random_generator(unsigned short modulo) {
    
    // THE IMPLEMENTATION TOTALLY BELONGS TO YOU. You can follow the steps given in the3.pdf
    // You can define different rotation amounts in bits for different types of needs for random integer
    
}

#ifdef	__cplusplus
}
#endif

#endif	/* THE3_H */