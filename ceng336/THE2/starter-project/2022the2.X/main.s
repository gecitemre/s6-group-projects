PROCESSOR    18F4620

#include <xc.inc>

; CONFIGURATION (DO NOT EDIT)
CONFIG OSC = HSPLL      ; Oscillator Selection bits (HS oscillator, PLL enabled (Clock Frequency = 4 x FOSC1))
CONFIG FCMEN = OFF      ; Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
CONFIG IESO = OFF       ; Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)
; CONFIG2L
CONFIG PWRT = ON        ; Power-up Timer Enable bit (PWRT enabled)
CONFIG BOREN = OFF      ; Brown-out Reset Enable bits (Brown-out Reset disabled in hardware and software)
CONFIG BORV = 3         ; Brown Out Reset Voltage bits (Minimum setting)
; CONFIG2H
CONFIG WDT = OFF        ; Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
; CONFIG3H
CONFIG PBADEN = OFF     ; PORTB A/D Enable bit (PORTB<4:0> pins are configured as digital I/O on Reset)
CONFIG LPT1OSC = OFF    ; Low-Power Timer1 Oscillator Enable bit (Timer1 configured for higher power operation)
CONFIG MCLRE = ON       ; MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)
; CONFIG4L
CONFIG LVP = OFF        ; Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
CONFIG XINST = OFF      ; Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

; timer macros
#define TIMER1_START 40536 ; 50ms (65536 - 25000)
#define TIMER1_START_LOW 0x58
#define TIMER1_START_HIGH 0x9e
#define TIMER0_START 15536 ; 100ms (65536 - 55000)
#define TIMER0_START_LOW 0xb0
#define TIMER0_START_HIGH 0x3c
#define BEAT_DURATION_DEFAULT 5
#define BEAT_DURATION_HIGH 10 ; when speed = 1
#define BEAT_DURATION_LOW 2   ; when speed = 9
#define BAR_LENGTH_DEFAULT 4
#define BAR_LENGTH_HIGH 8
#define BAR_LENGTH_LOW  2
    
; VALUES FOR 7-SEGMENT DISPLAY PINS
;                   -gfedcba
#define DISPLAY_1 0b00000110
#define DISPLAY_2 0b01011011
#define DISPLAY_3 0b01001111
#define DISPLAY_4 0b01100110
#define DISPLAY_5 0b01101101
#define DISPLAY_6 0b01111101
#define DISPLAY_7 0b00000111
#define DISPLAY_8 0b01111111
#define DISPLAY_9 0b01101111
#define DISPLAY_0 0b00111111
#define DISPLAY_P 0b01110011
#define DISPLAY_DASH 0b01000000

    
; GLOBAL SYMBOLS
; You need to add your variables here if you want to debug them.
GLOBAL time_ds, new_portb, last_portb
GLOBAL beat_duration_ds, pause, bar_length
GLOBAL current_display
GLOBAL main_loop_inc
GLOBAL current_beat_num
GLOBAL rc0_light, rc1_light
GLOBAL WREG_tmp

; Define space for the variables in RAM
PSECT udata_acs
time_ds: ; time in deciseconds counting backwards, decrease every 100ms
    DS 1
new_portb:
    DS 1
last_portb:
    DS 1
beat_duration_ds: ; beat duration in ds.
    DS 1          ; beat_duration_ds = 11 - (speed)
pause:            ; -1 (all ones) if paused, zero if not paused
    DS 1
bar_length:
    DS 1
current_display:  
    DS 1
main_loop_inc:    ; to keep track of main_loop
    DS 1          ; will be useful in switching displays
current_beat_num:
    DS 1
rc0_light:        ; 1 if RC0 is turned on, -1 if RC0 is turned off
    DS 1
rc1_light:        ; 1 if RC1 is turned on, -1 if RC1 is turned off
    DS 1
WREG_tmp:   
    DS 1

PSECT CODE
org 0x0000
goto main

org 0x0008
goto interrupt_service_routine
  
interrupt_service_routine:
    btfsc INTCON, 2
    call timer0_interrupt
    btfsc INTCON, 0
    call rb_interrupt
    btfsc PIR1, 0
    call timer1_interrupt
    retfie 1

timer1_interrupt:
    movlw TIMER1_START_LOW
    movwf TMR1L
    movlw TIMER1_START_HIGH
    movwf TMR1H
    bcf PORTC, 1
    bcf PORTC, 0
    bcf PIR1, 0
    movlw 0b11111111 ; -1
    setf rc0_light
    setf rc1_light
    clrf LATC
    clrf T1CON
    return

timer0_interrupt:
    movlw TIMER0_START_LOW
    movwf TMR0L
    movlw TIMER0_START_HIGH
    movwf TMR0H
    bcf INTCON, 2 
    
    ; if paused, turn the lights off, decrease time and do nothing else
    
    clrf WREG
    subwf pause, 0
    bnz quit_interrupt

    decrease_time_ds:
        movlw 0b10000000 ; enable timer1, 1:2 prescaler, 131.072 ms 0 -> 65,536
        movwf T1CON
        movlw TIMER1_START_LOW
        movwf TMR1L
        movlw TIMER1_START_HIGH
        movwf TMR1H
        dcfsnz time_ds
        call beat_duration_reached
        return
	
    quit_interrupt:
    setf rc0_light
    setf rc1_light
    clrf LATC
	return

beat_duration_reached:
    movff bar_length, WREG
    cpfslt current_beat_num  ; if bar_length > current_beat_num then call not_on_the_beat
    goto on_the_beat   
    goto not_on_the_beat
    
    not_on_the_beat:
        incf current_beat_num
    
        movff beat_duration_ds, time_ds
	
        clrf WREG
        subwf pause, 0
        bz turn_rc0_on

        return
	
    on_the_beat:
        movlw 1
        movwf current_beat_num
        
        movff beat_duration_ds, time_ds
	
        clrf WREG
        subwf pause, 0
        bz turn_both_rcs_on
	
        return
	
    turn_rc0_on:
	; turn rc0 on
	movlw 1
	movwf rc0_light

        movlw 0b00000001 ; 1
        movwf LATC
	return
	
    turn_both_rcs_on:
	call turn_rc0_on
	
	movlw 1
	movwf rc1_light
	
	movlw 0b00000011
	movwf LATC
	
	return
	


rb_interrupt: ; click handler
    movff PORTB, new_portb
    comf new_portb, W
    andwf last_portb, W
    movff new_portb, last_portb
    
    movwf WREG_tmp
    
    btfsc WREG_tmp, 4
    call rb4_pressed
    btfsc WREG_tmp, 5
    call rb5_pressed
    btfsc WREG_tmp, 6
    call rb6_pressed
    btfsc WREG_tmp, 7
    call rb7_pressed
    bcf INTCON, 0
    return
rb4_pressed:
    comf pause
    call switch_display
    return

rb5_pressed:
    ;Increase button. Affects the speed level if paused, bar length if running.
    movf pause
    bnz paused_rb5
    
    movlw BAR_LENGTH_HIGH
    cpfseq bar_length       ; if bar_length is max  
    incf bar_length
    return                  ; then return
paused_rb5:
    movlw BEAT_DURATION_LOW
    cpfseq beat_duration_ds  ; if beat duration is min, i.e. speed is max
    decf beat_duration_ds
    return                   ; then return  

rb6_pressed:
    ;Decrease button. Affects the speed level if paused, bar length if running.
    movf pause
    bnz paused_rb6
    
    movlw BAR_LENGTH_LOW
    cpfseq bar_length       ; if bar_length is min  
    decf bar_length
    return                  ; then return
paused_rb6:
    movlw BEAT_DURATION_HIGH
    cpfseq beat_duration_ds   ; if beat duration is max, i.e. speed is min
    incf beat_duration_ds
    return                    ; then return

rb7_pressed:
    ;Reset button. Affects the speed level if paused, bar length if running.
    movf pause
    bnz paused_rb7
    movlw BAR_LENGTH_DEFAULT
    movwf bar_length
    return
paused_rb7:
    movlw BEAT_DURATION_DEFAULT
    movwf beat_duration_ds
    return

;  -----------
; | MAIN CODE |
;  -----------
    
main:
    call init
    goto main_loop
init:
    ; call timer0_interrupt ; reset timer to start value

    ; configure_timer
    call initialise_timer

    ; init values for metronome
    movlw BEAT_DURATION_DEFAULT
    movwf beat_duration_ds
    movwf time_ds
    
    movlw 1
    movwf current_beat_num

    setf pause

    movlw BAR_LENGTH_DEFAULT
    movwf bar_length
    
    clrf main_loop_inc
    incf main_loop_inc
    
    clrf current_display
    incf current_display

    clrf TRISA
    clrf TRISC
    clrf TRISD
    movlw 0b11110000
    movwf TRISB
    
    movlw 0b11111111
    movwf rc0_light
    movwf rc1_light

    call show_paused_RA0
    
    clrf LATC
    
    return

initialise_timer:
    movlw 0b10000000 ; enable timer0, 1:2 prescaler, 131.072 ms 0 -> 65,536
    movwf T0CON
    movlw 0b10101000
    movwf INTCON
    movlw TIMER0_START_LOW
    movwf TMR0L
    movlw TIMER0_START_HIGH
    movwf TMR0H
    return

main_loop:
    ; POST LOOP MODIFICATIONS
    incf main_loop_inc
    clrf WREG
    cpfseq main_loop_inc
    goto main_loop
    call switch_display    ; if main_loop_inc = 0 then switch display
    
    goto main_loop

    
switch_display:
    clrf WREG
    cpfseq pause
    
    ; if paused
    goto switch_paused_display
    
    ; else
    
    goto switch_continuing_display
    
    switch_paused_display:
	movlw 0b00001000
	cpfseq current_display
	goto turn_DIS4_on   ; if current_display != 0b1000; then turn DIS4 on
	goto turn_DIS1_on   ; if current_display == 0b1000; then turn DIS1 on
    
    switch_continuing_display:
	movlw 0b00001000
	cpfslt current_display
	goto turn_DIS1_on   ; if current_display == 0b1000; then turn DIS1 on 
	movlw 2		    ; else, current_display *= 2
	mulwf current_display
	movff PRODL, current_display
	
	movlw 0b00001000
	cpfslt current_display
	goto turn_DIS4_on
	
	movlw 0b00000100
	cpfslt current_display
	goto turn_DIS3_on
	
	movlw 0b00000010
	cpfslt current_display
	goto turn_DIS2_on
	
	movlw 0b00000001
	cpfslt current_display
	goto turn_DIS1_on
	
	return    
    
show_RA0:
    clrf WREG
    cpfseq pause
    
    ; if paused
    
    goto show_paused_RA0
    
    ; else
    
    goto show_continuing_RA0
    
    ; show P
    show_paused_RA0:
	movlw DISPLAY_P
	movwf LATD ; RD
	return
	
    ; show nothing
    show_continuing_RA0:
	clrf LATD ; RD
	return
    
show_RA1:
    clrf WREG
    cpfseq pause
    
    ; if paused
    
    goto show_paused_RA1
    
    ; else
    
    goto show_continuing_RA1
	
    ; show nothing
    show_paused_RA1:
	clrf LATD ; RD
	return
	
    show_continuing_RA1:
	; switch case
	movlw 8
	subwf current_beat_num, 0
	bz display_8
	
	movlw 7
	subwf current_beat_num, 0
	bz display_7
	
	movlw 6
	subwf current_beat_num, 0
	bz display_6
	
	movlw 5
	subwf current_beat_num, 0
	bz display_5
	
	movlw 4
	subwf current_beat_num, 0
	bz display_4
	
	movlw 3
	subwf current_beat_num, 0
	bz display_3
	
	movlw 2
	subwf current_beat_num, 0
	bz display_2

    movlw 1
    subwf current_beat_num, 0
    bz display_1

    return
    
show_RA2:
    clrf WREG
    cpfseq pause
    
    ; if paused
    
    goto show_paused_RA2
    
    ; else
    
    goto show_continuing_RA2
    
    ; show nothing
    show_paused_RA2:
	clrf LATD ; RD
	return

    ; show -
    show_continuing_RA2:
	movlw DISPLAY_DASH
	movwf LATD ; RD
	return
	
    
show_RA3:
    clrf WREG
    cpfseq pause
    
    ; if paused
    
    goto show_paused_RA3
    
    ; else
    
    goto show_continuing_RA3
    
    show_paused_RA3:
	; switch case
	movlw 10
	subwf beat_duration_ds, 0
	bz display_1
	
	movlw 9
	subwf beat_duration_ds, 0
	bz display_2
	
	movlw 8
	subwf beat_duration_ds, 0
	bz display_3
	
	movlw 7
	subwf beat_duration_ds, 0
	bz display_4
	
	movlw 6
	subwf beat_duration_ds, 0
	bz display_5
	
	movlw 5
	subwf beat_duration_ds, 0
	bz display_6
	
	movlw 4
	subwf beat_duration_ds, 0
	bz display_7
	
	movlw 3
	subwf beat_duration_ds, 0
	bz display_8
	
	movlw 2
	subwf beat_duration_ds, 0
	bz display_9

    show_continuing_RA3:	
	; switch case
	movlw 2
	subwf bar_length, 0
	bz display_2
	
	movlw 3
	subwf bar_length, 0
	bz display_3
	
	movlw 4
	subwf bar_length, 0
	bz display_4
	
	movlw 5
	subwf bar_length, 0
	bz display_5
	
	movlw 6
	subwf bar_length, 0
	bz display_6
	
	movlw 7
	subwf bar_length, 0
	bz display_7
	
	movlw 8
	subwf bar_length, 0
	bz display_8
    return
    
    
    
; GENERIC DISPLAY FUNCTIONS
    
display_0:
    movlw DISPLAY_0
    movwf LATD ; RD
    return
    
display_1:
    movlw DISPLAY_1
    movwf LATD ; RD
    return
    
display_2:
    movlw DISPLAY_2
    movwf LATD ; RD
    return
    
display_3:
    movlw DISPLAY_3
    movwf LATD ; RD
    return
    
display_4:
    movlw DISPLAY_4
    movwf LATD ; RD
    return
    
display_5:
    movlw DISPLAY_5
    movwf LATD ; RD
    return
    
display_6:
    movlw DISPLAY_6
    movwf LATD ; RD
    return
    
display_7:
    movlw DISPLAY_7
    movwf LATD ; RD
    return
    
display_8:
    movlw DISPLAY_8
    movwf LATD ; RD
    return
    
display_9:
    movlw DISPLAY_9
    movwf LATD ; RD
    return
    
display_p:
    movlw DISPLAY_P
    movwf LATD ; RD
    return
    
display_dash:
    movlw DISPLAY_DASH
    movwf LATD ; RD
    return
    
    
turn_DIS1_on:
    movlw 0b00000001
    movwf LATA
    movwf current_display
    
    call show_RA0
    
    return
    
turn_DIS2_on:
    movlw 0b00000010
    movwf LATA
    movwf current_display
    
    call show_RA1
    
    return
    
turn_DIS3_on:
    movlw 0b00000100
    movwf LATA
    movwf current_display
    
    call show_RA2
    
    return
    
turn_DIS4_on:
    movlw 0b00001000
    movwf LATA
    movwf current_display
    
    call show_RA3
    
    return