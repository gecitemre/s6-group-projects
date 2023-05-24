/*
 * File:   the3.c
 * Author: e2521581
 *
 * Created on May 24, 2023, 10:17 AM
 */


#include <xc.h>
#include "lcd.h"
#include "the3.h"

/**
 * RB0 = throw
 *      - operate iff there is a player on the same cell that frisbee locates
 *        and the game is in INACTIVE_MODE
 *      - make the game mode ACTIVE_MODE
 *      - INT0
 * RB1 = switch player
 *      - no switch if inactive & the selected player holds the frisbee
 *      - order does not matter
 *      - INT1
 * RA3,RA4,RA5 = DISP2,DISP3,DISP4
 *      - DISP3 = '-'
 */

unsigned int teamA_score = 0;
unsigned int teamB_score = 0;

void initPorts() {
    TRISB = 255; // 11111111
}

void initInterrupts() {
}

void main(void) {
    return;
}
