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
#define FRISBEE_INDEX 4
#define FRISBEE_TARGET_INDEX 5

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
unsigned int remaining_frisbee_moves = 0;
unsigned int shouldDeleteTarget = 0;    // 1 if the target should be deleted in the next interrupt
player_type last_thrower_team;
byte old_PORTB;
object objects[6];
byte cursor = 0;
game_mode mode = INACTIVE_MODE;

void tmr0_interrupt()
{
    TMR0 = TMR0_START;

    if (remaining_frisbee_moves)
    {
        objects[FRISBEE_INDEX].x = frisbee_steps[remaining_frisbee_moves - 1][0];
        objects[FRISBEE_INDEX].y = frisbee_steps[remaining_frisbee_moves - 1][1];
        remaining_frisbee_moves--;
        DisplayObject(&objects[FRISBEE_INDEX]);
    }
    else {
        if (shouldDeleteTarget)
        {
            ClearObject(&objects[FRISBEE_TARGET_INDEX]);
            shouldDeleteTarget = 0;
        }
        return;
    }

    for (int i = 0; i < 4; i++)
    {
        unsigned short conflict = 0,
            xChangeAmt = random_generator(1),
            yChangeAmt = random_generator(1),
            xChangeSign = random_generator(1),
            yChangeSign = random_generator(1),
            xCurrent = objects[i].x,
            yCurrent = objects[i].y,
            x = xCurrent + ((xChangeAmt - 1) * (xChangeSign ? 1 : -1)),
            y = yCurrent + ((yChangeAmt - 1) * (yChangeSign ? 1 : -1));

        if (objects[i].data.selected)
            continue;

        if (x < 1 || x > 16 || y < 1 || y > 4)
        {
            DisplayObject(&objects[i]);
            continue;
        }

        for (int j = 0; j < 5; j++)
        {
            if (objects[j].x == x && objects[j].y == y)
            {
                conflict = 1;
                break;
            }
        }

        if (conflict)
        {
            DisplayObject(&objects[i]);
            continue;
        }
        objects[i].x = x;
        objects[i].y = y;
        DisplayObject(&objects[i]);
    }

    // if there are no remaining moves, we will check who has the frisbee.
    // we will set the game mode to INACTIVE_MODE.
    // if team A has the frisbee, we will increment team A's score.
    // if team B has the frisbee, we will increment team B's score.
    // if no one has the frisbee, we will do nothing.
    if (!remaining_frisbee_moves)
    {
        for (int i = 0; i < 4; i++)
        {
            if (objects[i].x == objects[FRISBEE_INDEX].x &&
                objects[i].y == objects[FRISBEE_INDEX].y)
            {
                if (objects[i].data.type == TEAM_A_PLAYER)
                {
                    teamA_score++;
                }
                else if (objects[i].data.type == TEAM_B_PLAYER)
                {
                    teamB_score++;
                }
                objects[i].data.frisbee = 0;
                break;
            }
        }

        shouldDeleteTarget = 1;
        mode = INACTIVE_MODE;
    }
}

void rb0_interrupt()
{
    if (objects[cursor].x == objects[FRISBEE_INDEX].x &&
        objects[cursor].y == objects[FRISBEE_INDEX].y &&
        mode == INACTIVE_MODE)
    {
        last_thrower_team = objects[cursor].data.type;
        objects[cursor].data.frisbee = 0;
        mode = ACTIVE_MODE;
        remaining_frisbee_moves = compute_frisbee_target_and_route(objects[FRISBEE_INDEX].x, objects[FRISBEE_INDEX].y);
        // instead of initiating player moves here, we will calculate them when timer interrupt occurs

        // show target
        objects[FRISBEE_INDEX + 1].x = frisbee_steps[remaining_frisbee_moves - 1][0];
        objects[FRISBEE_INDEX + 1].y = frisbee_steps[remaining_frisbee_moves - 1][1];
        DisplayObject(&objects[FRISBEE_INDEX + 1]);
    }
}

void rb1_interrupt()
{
    if (mode == INACTIVE_MODE && objects[cursor].data.frisbee)
    {
        objects[cursor].data.selected = 0;
        cursor = (cursor + 1) % 4; // do not take frisbee into consideration
        objects[cursor].data.selected = 1;
        ClearObject(&objects[cursor]);
        DisplayObject(&objects[cursor]);
    }
}

void rb4_interrupt()
{
    // up
    ClearObject(&objects[cursor]);
    if (mode == ACTIVE_MODE && objects[cursor].data.selected && !objects[cursor].data.frisbee)
    {
        if (objects[cursor].y != 1) {
            objects[cursor].y--;
        }
    }
    DisplayObject(&objects[cursor]);
}

void rb5_interrupt()
{
    // right
    ClearObject(&objects[cursor]);
    if (mode == ACTIVE_MODE && objects[cursor].data.selected && !objects[cursor].data.frisbee)
    {
        if (objects[cursor].x != 16) {
            objects[cursor].x++;
        }
    }
    DisplayObject(&objects[cursor]);
}

void rb6_interrupt()
{
    // down
    ClearObject(&objects[cursor]);
    if (mode == ACTIVE_MODE && objects[cursor].data.selected && !objects[cursor].data.frisbee)
    {
        if (objects[cursor].y != 4) {
            objects[cursor].y++;
        }
    }
    DisplayObject(&objects[cursor]);
}

void rb7_interrupt()
{
    // left
    ClearObject(&objects[cursor]);
    if (mode == ACTIVE_MODE && objects[cursor].data.selected && !objects[cursor].data.frisbee)
    {
        if (objects[cursor].x != 1) {
            objects[cursor].x--;
        }
    }
    DisplayObject(&objects[cursor]);
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

void ConfigureTimers()
{
    // set the timer for 100ms
    T0CON = 0b101; // 1:64 prescaler
    TMR0 = TMR0_START;
    T0CONbits.TMR0ON = 1;

    // set TIMER3 for random number generation
    T3CON = 1;
    T3CONbits.TMR3ON = 1;
}

void InitGame()
{
    LCDAddSpecialCharacterFromObjectData(((object_data){0, 0, TEAM_A_PLAYER}), teamA_player);
    LCDAddSpecialCharacterFromObjectData(((object_data){0, 0, TEAM_B_PLAYER}), teamB_player);
    LCDAddSpecialCharacterFromObjectData(((object_data){1, 0, TEAM_A_PLAYER}), selected_teamA_player);
    LCDAddSpecialCharacterFromObjectData(((object_data){1, 0, TEAM_B_PLAYER}), selected_teamB_player);
    LCDAddSpecialCharacterFromObjectData(((object_data){1, 1, TEAM_A_PLAYER}), selected_teamA_player_with_frisbee);
    LCDAddSpecialCharacterFromObjectData(((object_data){1, 1, TEAM_B_PLAYER}), selected_teamB_player_with_frisbee);
    LCDAddSpecialCharacterFromObjectData(((object_data){0, 1, FRISBEE}), frisbee);
    LCDAddSpecialCharacterFromObjectData(((object_data){0, 1, FRISBEE_TARGET}), frisbee_target);
}

void main(void)
{
    ConfigurePorts();
    InitLCD();
    InitGame();
    ConfigureInterrupts();
    ConfigureTimers();
    objects[0] = (object){3, 2, {1,  0, TEAM_A_PLAYER}};
    objects[1] = (object){3, 3, {0, 0, TEAM_A_PLAYER}};
    objects[2] = (object){14, 2, {0,0,  TEAM_B_PLAYER}};
    objects[3] = (object){14, 3, {0,0,  TEAM_B_PLAYER}};
    objects[FRISBEE_INDEX] = (object){9, 2, {0, 1, FRISBEE}};
    // there is also a frisbee target, which is not shown initially

    for (int i = 0; i < 5; i++)
    {
        DisplayObject(&objects[i]);
    }
    while (1)
    {

    }
}
