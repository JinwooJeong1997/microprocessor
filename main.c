/*
팀 프로젝트
digital clock
todo-list
login
timer => 
dual time - 미국/한국 시간 이런식 =? GMT+N
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

#include "lcd.h"

#define LED (*(volatile unsigned char *)0x8008)
#define LCD_DATA (*(volatile unsigned char *)0x8000)
#define LCD_CTL (*(volatile unsigned char *)0x8000)

volatile int cnt = 0, led=0;
unsigned char keydata = 0;
unsigned char key_old = 0;
unsigned int pw[4] = {1,2,3,4};
unsigned int input_pw[4] = {0,0,0,0};
unsigned int input_cnt = 0;




void port_init(void)
{ PORTA = 0x00;
DDRA = 0xff; //주소출력
PORTB = 0x00;
DDRB = 0x00;
PORTC = 0x00; //m103 output only
DDRC = 0x03; //주소 출력
PORTD = 0x00;
DDRD = 0x00;
PORTE = 0x00;
DDRE = 0x00;
PORTF = 0x00;
DDRF = 0x00;
PORTG = 0x00;
DDRG = 0x03; // Write, ale 신호
}

//
void login(){
    keydata = PINB & 0xff;
    if(keydata != key_old){ //처음 값이 들어올떄
        if(input_cnt < 4){
		    switch(keydata){
                case 0x01:
                
                input_pw[input_cnt++] = 0;
                break;
                case 0x02:
                input_pw[input_cnt++] = 1;
                break;
                case 0x04:
                input_pw[input_cnt++] = 2;
                break;
                case 0x08:
                input_pw[input_cnt++] = 3;
                break;
                case 0x10:
                input_pw[input_cnt++] = 4;
                break;
                case 0x20:
                input_pw[input_cnt++] = 5;
                break;
                case 0x40:
                input_pw[input_cnt++] = 6;
                break;
                case 0x80:
                input_pw[input_cnt++] = 7;   
                break;
                default :
                break;
            }
        }else{
            int num = 1;
            for(int i = 0; i < 4; i++){
                if(pw[i] != input_pw[i]){
                    num = 0;
                }
            }
            if(num != 1)
            {
                input_cnt = 0;
                for(int i = 0; i < 4; i++)
                {
                    input[i] = 0;
                }
            }

        }
    }
    key_old = keydata;
    
}

void lcdCTL(){
	lcd_gotoxy(1,1);
	lcd_putn3(led);
	//lcd_gotoxy(1,2);
    //lcd_putn4(keydata);
    lcd_gotoxy(6,2);
    lcd_putn4(input_pw);
}

ISR(TIMER0_COMP_vect){ 
    TCNT0 = 0x00;
    cnt++;
    if(cnt == 61){
         LED = 1 << (led % 8);
         cnt = 0;
         led++ ;
    }
}


void main(void){ 
    port_init();
    MCUCR = 0x80;
    TIMSK = 0b00000010;
    TCCR0 = 0b00001111; //CO mode
    TCNT0 = 0b00000000;
    OCR0 = 100;
    ASSR = 0x00;
    SREG = 0b10000000;
    lcdInit();
    lcdClear();
    while(1){
        lcdCTL();
        login();
	};
}
