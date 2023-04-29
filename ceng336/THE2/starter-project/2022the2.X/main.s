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
#define TIMER_START 15536 ; 100ms
#define TIMER_START_LOW 0xb0
#define TIMER_START_HIGH 0x3c
    
; GLOBAL SYMBOLS
; You need to add your variables here if you want to debug them.
GLOBAL counter1, post_timer

; Define space for the variables in RAM
PSECT udata_acs
counter1:
  DS 1
post_timer: ; increase every 100ms
    DS 1

PSECT CODE
org 0x0000
  goto main



org 0x0008
interrupt_service_routine: ; only timer0 for the moment
  call timer0_isr
  retfie
timer0_isr:
  bcf INTCON, INTCON_TMR0IF_POSITION
  movlw TIMER_START_LOW
  movwf TMR0L
  movlw TIMER_START_HIGH
  movwf TMR0H
  incf post_timer
  return

main:
configure_timer: ; this is a label for clearity
  movlw 0b10000000 ; enable timer0, 1:2 prescaler, 131.072 ms 0 -> 65,536
  movwf T0CON
  movlw 0b10100000
  movwf INTCON
  movlw TIMER_START_LOW
  movwf TMR0L
  movlw TIMER_START_HIGH
  movwf TMR0H


  
main_loop:
  goto main_loop

