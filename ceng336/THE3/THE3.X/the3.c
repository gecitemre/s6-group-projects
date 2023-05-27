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


void MovePlayer(unsigned index, unsigned x, unsigned y)
{
    ClearObject(&objects[index]);
    objects[index].x = x;
    objects[index].y = y;
    DisplayObject(&objects[index]);
}

unsigned DetermineScoreDisplay(unsigned score)
{
    return display_num_array[score % 10];
}

void SwitchDisplay()
{
    displayMode = (displayMode + 1) % 3;

    if (displayMode == DISP2)
    {
        LATD = DetermineScoreDisplay(teamA_score);
        LATA = 0b00001000;
    }
    else if (displayMode == DISP3)
    {
        LATD = display_dash;
        LATA = 0b00010000;
    }
    else if (displayMode == DISP4)
    {
        LATD = DetermineScoreDisplay(teamB_score);
        LATA = 0b00100000;
    }
}
void TMR0Interrupt()
{
    TMR0 = TMR0_START;
            

    if (!remaining_frisbee_moves) return;
    ClearObject(&objects[FRISBEE_INDEX]);
    DisplayObject(&objects[cursor]);
    objects[FRISBEE_INDEX].x = frisbee_steps[remaining_frisbee_moves - 1][0];
    objects[FRISBEE_INDEX].y = frisbee_steps[remaining_frisbee_moves - 1][1];
    remaining_frisbee_moves--;
    DisplayObject(&objects[FRISBEE_INDEX]);

    // if there are no remaining moves, we will check who has the frisbee.
    // we will set the game mode to INACTIVE_MODE.
    // if team A has the frisbee, we will increment team A's score.
    // if team B has the frisbee, we will increment team B's score.
    // if no one has the frisbee, we will do nothing.
    if (!remaining_frisbee_moves)
    {
        unsigned int frisbeeCaught = 0;

        ClearObject(&frisbee_target_object);
        
        for (unsigned i = 0; i < 4; i++)
        {
            if (Collision(&objects[FRISBEE_INDEX], &objects[i]))
            {
                frisbeeCaught = 1;
                
                if (objects[i].data.type == TEAM_A_PLAYER)
                {
                    teamA_score++;
                }
                else if (objects[i].data.type == TEAM_B_PLAYER)
                {
                    teamB_score++;
                }
                
                ClearObject(&objects[FRISBEE_INDEX]);
                ClearObject(&objects[i]);
                objects[i].data.frisbee = 1;
                DisplayObject(&objects[i]);
                break;
            }
        }
        
        if (frisbeeCaught) {
            mode = INACTIVE_MODE;
            right_to_throw = objects[cursor].data.type;
        }
        else {
            right_to_throw = !objects[cursor].data.type;
            DisplayObject(&objects[FRISBEE_INDEX]);
        }
    }
    
    for (unsigned i = 0; i < 4; i++)
    {
        if (objects[i].data.selected)
        {
            continue;
        }
        unsigned movement = Random(9);
        unsigned k = movement;
#define horizontal (k % 3 - 1 + objects[i].x)
#define vertical (k / 3 - 1 + objects[i].y)

        while(1) {
            for (unsigned j = 0; j < 5; j++)
            {
                if (i == j) continue;
                if (horizontal == objects[j].x && vertical == objects[j].y ||
                        (horizontal < 1 || horizontal > 16) || (vertical < 1 || vertical > 4))
                {
                    goto try_different_movement;
                }
            }
            break;
try_different_movement:
            k = (k + 1) % 9;
            if (k == movement) {
                k = 4;
                break;
            }
        }
        
        MovePlayer(i, horizontal, vertical);
    }
}

void RB0Interrupt()
{
    if (Collision(&objects[cursor], &objects[FRISBEE_INDEX]) &&
        mode == INACTIVE_MODE && right_to_throw == objects[cursor].data.type)
    {
        ClearObject(&objects[cursor]);
        objects[cursor].data.frisbee = 0;
        DisplayObject(&objects[cursor]);
        mode = ACTIVE_MODE;
        remaining_frisbee_moves = ComputeFrisbeeTargetAndRoute(objects[FRISBEE_INDEX].x, objects[FRISBEE_INDEX].y);
        // instead of initiating player moves here, we will calculate them when timer interrupt occurs

        // show target
        frisbee_target_object.x = frisbee_steps[0][0];
        frisbee_target_object.y = frisbee_steps[0][1];
        DisplayObject(&frisbee_target_object);
    }
}

void SelectObject(unsigned index)
{
    ClearObject(&objects[cursor]);
    objects[cursor].data.selected = 0;
    DisplayObject(&objects[cursor]);
    cursor = index;
    ClearObject(&objects[cursor]);
    objects[cursor].data.selected = 1;
    DisplayObject(&objects[cursor]);
}

void RB1Interrupt()
{
    if ((mode == ACTIVE_MODE || first_round) && !objects[cursor].data.frisbee)
    {
        SelectObject((cursor + 1) % 4);
    }
}
void RB4Interrupt()
{
    // up
    if (objects[cursor].y != 1)
    {
        MoveCursorPlayer(objects[cursor].x, objects[cursor].y - 1);
    }
}

void RB5Interrupt()
{
    // right
    if (objects[cursor].x != 16)
    {
        MoveCursorPlayer(objects[cursor].x + 1, objects[cursor].y);
    }
}

void RB6Interrupt()
{
    // down
    if (objects[cursor].y != 4)
    {
        MoveCursorPlayer(objects[cursor].x, objects[cursor].y + 1);
    }
}

void RB7Interrupt()
{
    // left
    if (objects[cursor].x != 1)
    {
        MoveCursorPlayer(objects[cursor].x - 1, objects[cursor].y);
    }
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
            RB0Interrupt();
        }
        INTCONbits.INT0IF = 0;
    }
    if (INTCON3bits.INT1IF)
    {
        byte new_PORTB = PORTB;
        WREG = ~new_PORTB & old_PORTB;
        PORTBbits_t falling_edge = *(PORTBbits_t *)(void *)&WREG;
        old_PORTB = new_PORTB;
        if (falling_edge.RB1)
        {
            RB1Interrupt();
        }
        INTCON3bits.INT1IF = 0;
    }
    if (INTCONbits.RBIF) {
        byte new_PORTB = PORTB;
        WREG = ~new_PORTB & old_PORTB;
        PORTBbits_t falling_edge = *(PORTBbits_t *)(void *)&WREG;
        old_PORTB = new_PORTB;
        if (falling_edge.RB4)
        {
            RB4Interrupt();
        }
        if (falling_edge.RB5)
        {
            RB5Interrupt();
        }
        if (falling_edge.RB6)
        {
            RB6Interrupt();
        }
        if (falling_edge.RB7)
        {
            RB7Interrupt();
        }
        INTCONbits.RBIF = 0;
    }
    if (INTCONbits.TMR0IF)
    {
        TMR0Interrupt();
        INTCONbits.TMR0IF = 0;
    }
}

void ConfigurePorts()
{
    // TRISB = 0b11111111; (default)
    TRISA = 0;
}

void ConfigureInterrupts()
{
    INTCONbits.GIE = 1;
    INTCONbits.RBIE = 1;
    INTCONbits.TMR0IE = 1;
    INTCONbits.INT0IE = 1;
    INTCON3bits.INT1IE = 1;
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



void InitADC()
{
    ADCON0bits.ADON = 1;
    ADCON1 = 0b1110;
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
    objects[4] = (object){9, 2, {0, 1, FRISBEE}};
    for (unsigned i = 0; i < 5; i++)
    {
        DisplayObject(&objects[i]);
    }

    while (1)
    {
        SwitchDisplay();
    }
}
