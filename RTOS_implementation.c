#include<lpc214x.h>
#include<rtl.h>
#include<stdio.h>

#define THRE (1<<5)     // Transmit Holding Register Empty
#define RDR (1<<0)
#define NEW_LINE 0xA    // Character for new line .. analogus to '\n'
#define ENTER 0xD 

OS_TID tsk1,tsk2,tsk3;    
OS_RESULT result1,result2,result3;
int    cnt1,cnt2,cnt3,i;
unsigned char welcome[50] = "SCADA system for safe power plant operation    ";
unsigned char start[30] = "Start the turbine   ";
unsigned char t[30] = "Speed of turbine is ";
unsigned char r[30] = " rotations per second   ";
unsigned char war[50] = "DANGER!!! Maximum speed of turbine is reached   ";
unsigned int k;	
unsigned  int adc_value=0,temp_adc=0;
unsigned char z;
int temp;
unsigned int flag=0;
unsigned int a;
int var[10];
char val[10];
unsigned  char msg[]={"OUT OF SAFE ZONE"};
unsigned int j;
char c;

// Function declarations
void serial(void);
void lcd_init(void);
void cmd(unsigned int );
void lcd_init(void);
void hascii(int *);
void shut_down (void);
void data(unsigned int );
char U0Read(void);
void delay(unsigned  long int r1);


__task void speed_monitor (void); 		//declare function for job1
__task void temp_monitor (void); 		//declare function for job2
__task void shutdown (void); 		//declare function for job3


void delay(unsigned  long int r1)
{
  unsigned  long int r;
	for(r=0;r<r1;r++);
}

void U0Write(char data)
{
	while ( !(U0LSR & THRE ) );          // wait till the THR is empty
	                                    // now we can write to the Tx FIFO
	U0THR = data;
}

void serial()                           // Initialisation of uart 
{
    PINSEL0 = 0x00000005;
    U0LCR = 0x83;
    U0DLL = 0x61;
    U0LCR = 0x03;
    U0IER = 0x01;
}

char U0Read(void)
{
	while( !(U0LSR & RDR ) );           // wait till any data arrives into Rx FIFO
	return U0RBR;
}


void lcd_init()
{
    unsigned int i;
    unsigned  int c[]={0x30,0x20,0x28,0x01,0x06,0x0E,0x80};
    IODIR0=0x000000fc;
    IOCLR0=0x000000fc;
    for(i=0;i<7;i++)
    {
        cmd(c[i]);
        delay(10000);
    }
}
void cmd(unsigned int value)
{
    unsigned int y;
    y=value;
    y=y & 0xf0;
    IOCLR0=0x000000fc;
    IOCLR0=0X00000004;
    IOSET0=y;
    IOSET0=0x00000008;
    delay(10);
    IOCLR0=0x00000008;
    y=value;
    y=y & 0x0f;
    y=y<<4;
    IOCLR0=0x000000fc;
    IOCLR0=0X00000004;
    IOSET0=y;
    IOSET0=0x00000008;
    delay(10);
    IOCLR0=0x00000008;
}
void data(unsigned int dat)
{
    unsigned int y;
    y=dat;
    y=y & 0xf0;
    IOCLR0=0x000000fc;
    IOSET0=0X00000004;
    IOSET0=y;
    IOSET0=0x00000008;
    delay(10);
    IOCLR0=0x00000008;
    y=dat;
    y=y & 0x0f;
    y=y<<4;
    IOCLR0=0x000000fc;
    IOSET0=0X00000004;
    IOSET0=y;
    IOSET0=0x00000008;
    delay(10);
    IOCLR0=0x00000008;
}

void ADC(void)
{
		PINSEL1  = 0X00040000;       //AD0.4 pin is selected(P0.18)
    IO0DIR   = 0x000000FC;      //configure o/p lines for lcd
    lcd_init(); 	//Initialisation the lcd
    while(1)
    {
        //CONTROL register for ADC
        AD0CR = 0x09200010;                          //command register for ADC-AD0.4
        delay(100);
        while((temp_adc = AD0GDR) == 0x80000000);   //to check the interrupt bit
        adc_value = AD0GDR;                         //reading the ADC value
        adc_value >>=6;
        adc_value &= 0x000003ff;
        temp=adc_value & 0x00000f00;
        var[0]= temp>>8 ;
        temp=adc_value & 0x000000f0;
        var[1]= temp>>4;
        temp=adc_value & 0x0000000f;
        var[2]=temp;
        hascii(var);
        for(i=0;i<3;i++)
        {
            delay(32000);
            data(val[i]);
            delay(32000); delay(32000); delay(32000);
        }
        if((var[0]>=2)&&((var[1]>=0)|| (var[2]>=0)))    //Comparing input temperature with maximum temperature
        {
            if(var[1]>=0)
            {
                cmd(0xC0);                              // Moves the cursor to next line
                delay(10000);
                for(j=0;j<16;j++)
                {
                    data(msg[j]); 
									delay(1000000);
									delay(500000);
									delay(500000);
										// Displaying the warning message on LCD screen
										flag=1;
                    delay(500);
                }
            delay(65000);
            delay(65000);
            delay(65000);                               
            delay(65000);
            delay(65000);
            cmd(0x01);
            delay(65000);
            delay(65000);
            delay(65000);
            delay(65000);
            }
        }
    cmd(0x01); delay(120000); delay(120000);
    break;
    }
}

void hascii(int var[])
 {
    char asc[20]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    for (i=0;i<3;i++)
    val[i]=asc[var[i]];
}

int main (void) 
{

	PINSEL1=0x00000000;
  IODIR0=0x00ff4200;
	IOCLR0=0x00ff4200;
	serial();
	 for(i = 0;i<47;i++)
    {
        U0Write(welcome[i]);              
    }
    delay(10000000);
		U0Write(NEW_LINE);
	for(i = 0;i<20;i++)
    {
        U0Write(start[i]);              // Write it to Tx to send it back
    }

  os_sys_init (speed_monitor);		//initialize job1 
  while (1);
}


__task void speed_monitor (void)		// job1 function definition
 {	
   tsk1=os_tsk_self();
   os_tsk_prio_self (3);		// assign priority to job1 as 2
   tsk2= os_tsk_create (shutdown, 0);
		tsk3 = os_tsk_create(temp_monitor,0);//create job2
   //tsk3=	os_tsk_create (shutdown, 1);		//create job2
	 
	 
   while (1)
   {
		 a=0;
		 c=U0Read();
		 if(c == ENTER)
			 U0Write(NEW_LINE);
		 else if (c>'8')
		 {
			 for(i=0;i<48;i++)
			 {
				 U0Write(war[i]);
			 }
			 os_tsk_prio(tsk2,8);
			 
		 }
		 
		 else
		 {
			 for(i=0;i<20;i++)
			 {
				 U0Write(t[i]);
			 }
			 U0Write(c);
			 for(i=0;i<24;i++)
			 {
				 U0Write(r[i]);
			 }
			 os_tsk_prio(tsk3,4);
		 }
		
	 }
 }



__task void shutdown (void)		// job1 function definition
 {	
  
   while (1)
   {
   k=0x00800000;
		 PINSEL1=0x00000000;
  IODIR0=0x00ff4200;
	IOCLR0=0x00ff4200;
 	      for(i=0;i<8;i++)
    {
    
        IOSET0=0x00004200;                 // Buzzer turns on
        delay(1000000);
        IOCLR0=0x00004200;
        delay(1000000);                         
        IOSET0=k;		                    // LED's are turned OFF one by one
        delay(1000000);
        delay(1000000);
        k=k>>1;
			
    }
			os_tsk_delete(tsk1);
		os_tsk_delete(tsk3);
		  os_tsk_delete_self ();

   }
}
 

__task void temp_monitor (void)
{
	while(1)
	{
		ADC();
		if (flag ==1)
		{
			os_tsk_prio(tsk2,8);
		}
		a++;
		if(a>50)
		{
			os_tsk_prio_self(2);
		}
		
		
	
	}
	
}
