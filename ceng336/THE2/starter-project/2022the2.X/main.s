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
#define BEAT_DURATION_DEFAULT 10
#define BAR_LENGTH_DEFAULT 4
    
; GLOBAL SYMBOLS
; You need to add your variables here if you want to debug them.
GLOBAL counter1, time_ds

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
beat_duration_ds:
    DS 1
pause:
    DS 1
bar_length:
    DS 1

PSECT CODE
org 0x0000
  goto main



org 0x0008
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
  movff beat_duration_ds, time_ds
  bcf INTCON, 2 
  return

rb_interrupt:
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
    incf bar_length ; TODO: this value should not pass 8
    return
paused_rb5:
    incf beat_duration_ds ; TODO: this value should not pass 10
    return

rb6_pressed:
    ;Decrease button. Affects the speed level if paused, bar length if running.
    movf pause
    bnz paused_rb6
    decf bar_length ; TODO: this value should not pass 2
    return
paused_rb6:
    decf beat_duration_ds ; TODO: this value should not pass 2
    return

rb7_pressed:
    ;Reset button. Affects the speed level if paused, bar length if running.
    movf pause
    bnz paused_rb7
    movlw BEAT_DURATION_DEFAULT
    movwf beat_duration_ds
paused_rb7:
    movlw BAR_LENGTH_DEFAULT
    movwf bar_length
    return

main:
  call timer0_interrupt ; reset timer to start value
  movlw BEAT_DURATION_DEFAULT
  movwf beat_duration_ds
  movwf time_ds
  movlw 0
  movwf pause
  movlw BAR_LENGTH_DEFAULT
  movwf bar_length
  
; configure_timer
  movlw 0b10000000 ; enable timer0, 1:2 prescaler, 131.072 ms 0 -> 65,536
  movwf T0CON
  movlw 0b10101000
  movwf INTCON
  movlw TIMER_START_LOW
  movwf TMR0L
  movlw TIMER_START_HIGH
  movwf TMR0H


main_loop:
  goto main_loop

