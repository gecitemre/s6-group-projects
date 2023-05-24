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

#define TMR0_START 61629

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
byte old_PORTB;
object objects[5];
byte cursor = 0;

void tmr0_interrupt()
{
    TMR0 = TMR0_START;
}

void rb0_interrupt()
{
    // TODO
}

void rb1_interrupt()
{
    // TODO
}

void rb4_interrupt()
{
    // TODO
}

void rb5_interrupt()
{
    // TODO
}

void rb6_interrupt()
{
    // TODO
}

void rb7_interrupt()
{
    // TODO
}

void __interrupt(high_priority) ISR()
{
    if (INTCONbits.INT0IF)
    {
        byte new_PORTB = PORTB;
        WREG = ~new_PORTB & old_PORTB;
        PORTBbits_t falling_edge = *(PORTBbits_t *)(void *)&WREG;
        old_PORTB = new_PORTB;
        if (falling_edge.RB0)
        {
            rb0_interrupt();
        }
        if (falling_edge.RB1)
        {
            rb1_interrupt();
        }
        INTCONbits.INT0IF = 0;
    }
    if (INTCONbits.RBIF) {
        byte new_PORTB = PORTB;
        WREG = ~new_PORTB & old_PORTB;
        PORTBbits_t falling_edge = *(PORTBbits_t *)(void *)&WREG;
        old_PORTB = new_PORTB;
        if (falling_edge.RB4)
        {
            rb4_interrupt();
        }
        if (falling_edge.RB5)
        {
            rb5_interrupt();
        }
        if (falling_edge.RB6)
        {
            rb6_interrupt();
        }
        if (falling_edge.RB7)
        {
            rb7_interrupt();
        }
        INTCONbits.RBIF = 0;
    }
    if (INTCONbits.TMR0IF)
    {
        tmr0_interrupt();
        INTCONbits.TMR0IF = 0;
    }
}

void ConfigurePorts()
{
    // TRISB = 0b11111111; (default)
}

void ConfigureInterrupts()
{
    INTCONbits.GIE = 1;
    INTCONbits.RBIE = 1;
    INTCONbits.TMR0IE = 1;
    INTCONbits.INT0IE = 1;
}

void ConfigureTimer()
{
    // set the timer for 100ms
    T0CON = 0b101; // 1:64 prescaler
    TMR0 = TMR0_START;
    T0CONbits.TMR0ON = 1;
}

void InitGame()
{
    LCDAddSpecialCharacter(TEAM_A_PLAYER_SCI, teamA_player);
    LCDAddSpecialCharacter(TEAM_B_PLAYER_SCI, teamB_player);
    LCDAddSpecialCharacter(SELECTED_TEAM_A_PLAYER_SCI, selected_teamA_player);
    LCDAddSpecialCharacter(SELECTED_TEAM_B_PLAYER_SCI, selected_teamB_player);
    LCDAddSpecialCharacter(SELECTED_TEAM_A_PLAYER_WITH_FRISBEE_SCI, selected_teamA_player_with_frisbee);
    LCDAddSpecialCharacter(SELECTED_TEAM_B_PLAYER_WITH_FRISBEE_SCI, selected_teamB_player_with_frisbee);
    LCDAddSpecialCharacter(FRISBEE_SCI, frisbee);
    LCDAddSpecialCharacter(FRISBEE_TARGET_SCI, frisbee_target);
}

void main(void)
{
    ConfigurePorts();
    InitLCD();
    InitGame();
    ConfigureInterrupts();
    ConfigureTimer();
    objects[0] = (object){3, 2, PLAYER, .data.player = {TEAM_A, 1}};
    objects[1] = (object){3, 3, PLAYER, .data.player = {TEAM_A, 0}};
    objects[2] = (object){14, 2, PLAYER, .data.player = {TEAM_B, 0}};
    objects[3] = (object){14, 3, PLAYER, .data.player = {TEAM_B, 0}};
    objects[4] = (object){9, 2, FRISBEE};
    for (int i = 0; i < 5; i++)
    {
        DisplayObject(&objects[i]);
    }
    while (1)
    {
    }
}
