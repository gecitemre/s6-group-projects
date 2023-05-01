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
#define TIMER_START 15536 ; 100ms (65536 - 50000)
#define TIMER_START_LOW 0xb0
#define TIMER_START_HIGH 0x3c
#define BEAT_DURATION_DEFAULT 500
#define BEAT_DURATION_HIGH 1000 ; when speed = 1
#define BEAT_DURATION_LOW 200   ; when speed = 9
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
GLOBAL counter1, time_ds, wreg_tmp, status_tmp, new_portb, last_portb
GLOBAL beat_duration_ds, pause, bar_length
GLOBAL current_display
GLOBAL main_loop_inc

; Define space for the variables in RAM
PSECT udata_acs
counter1:
  DS 1
time_ds: ; time in deciseconds counting backwards, decrease every 100ms
    DS 1
wreg_tmp:
    DS 1
status_tmp:
    DS 1
new_portb:
    DS 1
last_portb:
    DS 1
beat_duration_ds: ; beat duration in ms.
    DS 1          ; beat_duration_ds = 1100 - (speed * 100)
pause:            ; non-zero if paused, zero if paused
    DS 1
bar_length:
    DS 1
current_display:  ; [0, 3]: RA[0, 3]
    DS 1	  ; e.g if current_display = 1 => RA1 is lit
main_loop_inc:    ; to keep track of main_loop
    DS 1          ; will be useful in switching displays

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
    retfie 1
timer0_interrupt:
    movlw TIMER_START_LOW
    movwf TMR0L
    movlw TIMER_START_HIGH
    movwf TMR0H
    dcfsnz time_ds
    call beat_duration_reached
    bcf INTCON, 2 
    return

beat_duration_reached:
    ; TO BE IMPLEMENTED
    movff beat_duration_ds, time_ds
    return

rb_interrupt: ; click handler
    movff PORTB, new_portb
    comf new_portb, W
    andwf last_portb, W
    movff new_portb, last_portb
    btfsc WREG, 4
    call rb4_pressed
    btfsc WREG, 5
    call rb5_pressed
    btfsc WREG, 6
    call rb6_pressed
    btfsc WREG, 7
    call rb7_pressed
    bcf INTCON, 0
    return
rb4_pressed:
    comf pause
    return

rb5_pressed:
    ;Increase button. Affects the speed level if paused, bar length if running.
    movf pause
    bnz paused_rb5
    
    movff bar_length, WREG
    cpfseq BAR_LENGTH_HIGH  ; if bar_length is max  
    incf bar_length
    return                  ; then return
paused_rb5:
    movff beat_duration_ds, WREG
    cpfseq BEAT_DURATION_LOW ; if beat duration is min, i.e. speed is max
    call increase_beat_duration
    return                   ; then return    
increase_beat_duration:
    movlw 100
    addwf beat_duration_ds
    return

rb6_pressed:
    ;Decrease button. Affects the speed level if paused, bar length if running.
    movf pause
    bnz paused_rb6
    cpfseq BAR_LENGTH_LOW   ; if bar_length is min  
    decf bar_length
    return                  ; then return
paused_rb6:
    movff beat_duration_ds, WREG
    cpfseq BEAT_DURATION_HIGH ; if beat duration is max, i.e. speed is min
    call decrease_beat_duration
    return                    ; then return    
decrease_beat_duration:
    movlw 100
    subwf beat_duration_ds
    return

rb7_pressed:
    ;Reset button. Affects the speed level if paused, bar length if running.
    movf pause
    bnz paused_rb7
    movlw BEAT_DURATION_DEFAULT
    movwf beat_duration_ds
    return
paused_rb7:
    movlw BAR_LENGTH_DEFAULT
    movwf bar_length
    return

;  -----------
; | MAIN CODE |
;  -----------
    
main:
    call init
    goto main_loop
init:
    call timer0_interrupt ; reset timer to start value

  ; configure_timer
    movlw 0b10000000 ; enable timer0, 1:2 prescaler, 131.072 ms 0 -> 65,536
    movwf T0CON
    movlw 0b10101000
    movwf INTCON
    movlw TIMER_START_LOW
    movwf TMR0L
    movlw TIMER_START_HIGH
    movwf TMR0H

    ; init values for metronome
    movlw BEAT_DURATION_DEFAULT
    movwf beat_duration_ds
    movwf time_ds

    movlw 0b11111111 ; enable pause, any value except 0 will do
    movwf pause
    movlw BAR_LENGTH_DEFAULT
    movwf bar_length
    
    clrf main_loop_inc
    incf main_loop_inc

    return


main_loop:
    ; POST LOOP MODIFICATIONS
    incf main_loop_inc
    clrf WREG
    subwf main_loop_inc, 0
    bz switch_display    ; if main_loop_inc = 0 then switch display
    
    goto main_loop

    
switch_display:
    clrf WREG
    cpfseq pause
    
    ; if paused
    goto switch_paused_display
    
    ; else
    
    goto switch_continuing_display
    
    switch_paused_display:
	movff current_display, WREG
	sublw 3
	negf WREG
	movwf current_display
	return
    
    switch_continuing_display:
	incf current_display   ; first increment current_display
	movlw 4
	cpfslt current_display ; if current_display < 4 skip
	clrf current_display   ; else clear it
	return    
    
show_RA0:
    movlw 0b00000001
    movwf RA

    clrf WREG
    cpfseq pause
    
    ; if paused
    
    goto show_paused_RA0
    
    ; else
    
    goto show_continuing_RA0
    
    ; show P
    show_paused_RA0:
	movlw DISPLAY_P
	movwf RD
	return
	
    ; show nothing
    show_continuing_RA0:
	clrf RD
	return
    
show_RA1:
    movlw 0b00000010
    movwf RA

    clrf WREG
    cpfseq pause
    
    ; if paused
    
    goto show_paused_RA1
    
    ; else
    
    goto show_continuing_RA1
    
    ; TODO: show current beat
    show_paused_RA1:
	return
	
    ; show nothing
    show_continuing_RA1:
	clrf RD
	return
    
show_RA2:
    movlw 0b00000100
    movwf RA

    clrf WREG
    cpfseq pause
    
    ; if paused
    
    goto show_paused_RA2
    
    ; else
    
    goto show_continuing_RA2
    
    ; show -
    show_paused_RA2:
	movlw DISPLAY_DASH
	movwf RD
	return
	
    ; show nothing
    show_continuing_RA2:
	clrf RD
	return
    
show_RA3:
    movlw 0b00001000
    movwf RA

    clrf WREG
    cpfseq pause
    
    ; if paused
    
    goto show_paused_RA3
    
    ; else
    
    goto show_continuing_RA3
    
    show_paused_RA0:
	; switch case
	movlw 1000
	sublw beat_duration_ds
	bz speed_1
	
	movlw 900
	sublw beat_duration_ds
	bz speed_2
	
	movlw 800
	sublw beat_duration_ds
	bz speed_3
	
	movlw 700
	sublw beat_duration_ds
	bz speed_4
	
	movlw 600
	sublw beat_duration_ds
	bz speed_5
	
	movlw 500
	sublw beat_duration_ds
	bz speed_6
	
	movlw 400
	sublw beat_duration_ds
	bz speed_7
	
	movlw 300
	sublw beat_duration_ds
	bz speed_8
	
	movlw 200
	sublw beat_duration_ds
	bz speed_9
	
	speed_1:
	    movlw DISPLAY_1
	    movwf RD
	    return
	    
	speed_2:
	    movlw DISPLAY_2
	    movwf RD
	    return
	    
	speed_3:
	    movlw DISPLAY_3
	    movwf RD
	    return
	    
	speed_4:
	    movlw DISPLAY_4
	    movwf RD
	    return
	    
	speed_5:
	    movlw DISPLAY_5
	    movwf RD
	    return
	    
	speed_6:
	    movlw DISPLAY_6
	    movwf RD
	    return
	    
	speed_7:
	    movlw DISPLAY_7
	    movwf RD
	    return
	    
	speed_8:
	    movlw DISPLAY_8
	    movwf RD
	    return
	    
	speed_9:
	    movlw DISPLAY_9
	    movwf RD
	    return

    show_continuing_RA0:	
	; switch case
	movlw 2
	sublw bar_length
	bz bar_length_2
	
	movlw 3
	sublw bar_length
	bz bar_length_3
	
	movlw 4
	sublw bar_length
	bz bar_length_4
	
	movlw 5
	sublw bar_length
	bz bar_length_5
	
	movlw 6
	sublw bar_length
	bz bar_length_6
	
	movlw 7
	sublw bar_length
	bz bar_length_7
	
	movlw 8
	sublw bar_length
	bz bar_length_8
	
	bar_length_2:
	    movlw DISPLAY_2
	    movwf RD
	    return
	    
	bar_length_3:
	    movlw DISPLAY_3
	    movwf RD
	    return
	    
	bar_length_4:
	    movlw DISPLAY_4
	    movwf RD
	    return
	    
	bar_length_5:
	    movlw DISPLAY_5
	    movwf RD
	    return
	    
	bar_length_6:
	    movlw DISPLAY_6
	    movwf RD
	    return
	    
	bar_length_7:
	    movlw DISPLAY_7
	    movwf RD
	    return
	    
	bar_length_8:
	    movlw DISPLAY_8
	    movwf RD
	    return
    return
    