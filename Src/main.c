#include <16F877A.h>
#fuses NOWDT,NOLVP,NOPROTECT,XT
#use delay(clock=4000000)
#byte  INTCON=0x0b
#byte  PIR1=0x0c
#byte  PIR2=0x0d
#byte  PIE1=0x8c
#byte  PIE2=0x8d
#byte  TMR1H=0x0f
#byte  TMR1L=0x0e
#byte  T1CON=0x10
#byte  porta=0x05
#byte  portb=0x06
#byte  portc=0x07
#byte  trisa=0x85
#byte  trisb=0x86
#byte  trisc=0x87
int status;
int count;
int iter=0;
//The T1 is configured for T1=15535
//fosc=4Mhz
//With no prescaler
//So that each interrupt generates a 0.05 sec delay
void T1_config(void)
{
   TMR1L=0xaf;//Set up T1 register
   TMR1H=0x3c;
   T1CON&=~(1<<0);
   T1CON&=~(1<<1);
   T1CON&=~(1<<2);
   T1CON&=~(1<<3);
   T1CON&=~(1<<4);
   T1CON&=~(1<<5);//Setting up t1 con register while T1 is still off
}
void T1_on(void)
{
  //T1 is set off by setting bit 0 of T1 register
   T1CON|=1<<0;
}
void T1_off(void)
{
  //T1 is set off by resetting bit 0 of T1 register
   T1CON&=~(1<<0);
    INTCON&=~(1<<7);
   INTCON&=~(1<<6);
}
void T1_setflag(void)
{
  //setting up the t1timer flag
   PIR1|=1<<0;
}
void T1_clearflag(void)
{
  //resetting up the t1timer flag
   PIR1&=~(1<<0);
}
void Interrupt_Config(void)
{
  //configuring global interrupt,Peripheral interrupt and enbling the interrupt for t1 peripheral 
   T1_config();
   INTCON|=1<<7;
   INTCON|=1<<6;
   INTCON|=1<<5;
   PIE1|=1<<0;
   
}
void Delay(int seconds)
{
   Interrupt_Config();
   T1_clearflag();
   count=seconds*20;
   status=0;
   T1_on();
   while(1)
   {
     //loop executes till the ISR handles the no of interrupts raised
      if(status)
      {
         break;
      }
   }
}
//interrupt handler(ccs)
#INT_TIMER1
//The preprosseor keyword here is fixed
void delay_isr(void)
{
   if(PIR1&(1<<0))
   {
      iter++;
      if(iter==count)
      {
         T1_off();
         iter=0;
         status=1;
      }
      else
      {
         TMR1L=0xaf;
         TMR1H=0x3c;
         T1_clearflag();
         status=0;
      }
   }
}
//!void Delay(int sec)

void main()
{
trisb=0x00;
portb|=0x02;

   while(TRUE)
   {
      Delay(1);
      portb^=0x02;      
   }

}
