/*
 * File:   the3.c
 * Author: e2521581 & e2449015
 *
 * Created on May 24, 2023, 10:17 AM
 */

#include <xc.h>
#include "pragmas.h"
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

void __interrupt(high_priority) ISR()
{
}

inline void InitPorts()
{
    TRISB = 255; // 11111111
}

inline void InitInterrupts()
{
}

inline void InitGame()
{
    LCDAddSpecialCharacter(TEAM_A_PLAYER_SCI, teamA_player);
    LCDAddSpecialCharacter(TEAM_B_PLAYER_SCI, teamB_player);
    LCDAddSpecialCharacter(SELECTED_TEAM_A_PLAYER_SCI, selected_teamA_player);
    LCDAddSpecialCharacter(SELECTED_TEAM_B_PLAYER_SCI, selected_teamB_player);
    LCDAddSpecialCharacter(SELECTED_TEAM_A_PLAYER_WITH_FRISBEE_SCI, selected_teamA_player_with_frisbee);
    LCDAddSpecialCharacter(SELECTED_TEAM_B_PLAYER_WITH_FRISBEE_SCI, selected_teamB_player_with_frisbee);
    LCDAddSpecialCharacter(FRISBEE_SCI, frisbee);
    LCDAddSpecialCharacter(FRISBEE_TARGET_SCI, frisbee_target);
    LCDGoto(3, 2);
    LCDDat(TEAM_A_PLAYER_SCI);
    LCDGoto(3, 3);
    LCDDat(TEAM_A_PLAYER_SCI);
    LCDGoto(14, 2);
    LCDDat(TEAM_B_PLAYER_SCI);
    LCDGoto(14, 3);
    LCDDat(TEAM_B_PLAYER_SCI);
    LCDGoto(9, 2);
    LCDDat(FRISBEE_SCI);
}

void main(void)
{
    InitLCD();
    InitGame();

    while (1)
        ;
}
