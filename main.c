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


//상태
#define STAT_INIT 0
#define STAT_LOGIN 1
#define STAT_TIME 2
#define STAT_DTIME 3

volatile int cnt = 0, led=0;
unsigned char keydata = 0;
unsigned char key_old = 0;
unsigned int pw = 0;
unsigned int input_pw = 0;
unsigned int input_cnt = 0;


unsigned int status = 0;

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
                input_pw *= 10;
                input_pw += 0;
                input_cnt ++;
                break;
                case 0x02:
                input_pw *= 10;
                input_pw += 1;
                input_cnt ++;
                break;
                case 0x04:
                input_pw *= 10;
                input_pw += 2;
                input_cnt ++;
                break;
                case 0x08:
                input_pw *= 10;
                input_pw += 3;
                input_cnt ++;
                break;
                case 0x10:
                input_pw *= 10;
                input_pw += 4;
                input_cnt ++;
                break;
                case 0x20:
                input_pw *= 10;
                input_pw += 5;
                input_cnt ++;
                break;
                case 0x40:
                input_pw *= 10;
                input_pw += 6;
                input_cnt ++;
                break;
                case 0x80:
                input_pw *= 10;
                input_pw += 7;   
                input_cnt ++;
                break;
                default :
                break;
            }
            _delay_ms(500);
            
        }else{
                    
            if(input_pw != pw){
                input_cnt = 0;
                input_pw = 0;
            }
            else{
                status = STAT_TIME;
            }

        }
    }
    key_old = keydata;
    
}

void lcdCTL(int stat){
	//lcd_gotoxy(1,1);
	//lcd_putn3(led);
    switch(stat){
        case STAT_LOGIN:
        lcd_gotoxy(1,1);
        lcd_putss("input password");
        lcd_gotoxy(1,2);
        lcd_putn4(input_pw);
        lcd_gotoxy(6,2);
        lcd_putn4(pw);
        break;
        case STAT_TIME:
        lcd_gotoxy(1,1);
        lcd_putn4(led);
        lcd_gotoxy(1,2);
        lcd_putss("login complete");
            break;
        default :
            break;
    }
	//lcd_gotoxy(1,2);
    //lcd_putn4(keydata);
    //lcd_gotoxy(6,2);
    //lcd_putn4(input_pw);
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

void manager(unsigned int stat){

    lcdCTL(stat);
    switch(stat){
        case STAT_LOGIN:
        login();
        break;
        case STAT_TIME:
        break;

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
    status = STAT_LOGIN;

    while(1){
        manager(status);
	};
}
