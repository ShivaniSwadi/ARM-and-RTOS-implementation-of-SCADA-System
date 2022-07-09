#include <rtl.h>
#include <lpc214x.h>
#include <Stdio.h>
int counter1;
int counter2;
unsigned int i,j;
unsigned  char msg[]={"OUT OF SAFE ZONE"};
unsigned  int adc_value=0,temp_adc=0;
unsigned char z;
int temp;
int var[10];
char val[10];
 void delay(unsigned  long int r1)
{
  	unsigned  long int r;
	for(r=0;r<r1;r++);
}
void shut_down (void)
{
	unsigned int k=0x00800000;
	PINSEL0=0x00000000;
	IODIR0=0x00ff0200;                      // making p0.16 to p0.23 as output lines
	IOCLR0=0x00ff0000;                      // LED's are turned ON means all the sectors are ON
	for(i=0;i<8;i++)
    {
    
        IOSET0=0x00000200;                 // Buzzer turns on
        delay(10000000);
        IOCLR0=0x00000200;
        delay(1000);                         
        IOSET0=k;		                    // LED's are turned OFF one by one
       // delay(10000000);
        delay(10000000);
        k=k>>1;
    }
}

void hascii(int var[])
 {
    char asc[20]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    for (i=0;i<3;i++)
    val[i]=asc[var[i]];
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



void temp_monitor(void)
{
	PINSEL1  = 0X00040000;       //AD0.4 pin is selected(P0.18)
    IO0DIR   = 0x00FF00FC;      //configure o/p lines for lcd
    lcd_init();                 //Initialisation the lcd
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
                    data(msg[j]);                       // Displaying the warning message on LCD screen 
                    delay(500);
                }
            delay(65000);
            delay(65000);
            delay(65000);
            shut_down();                                // This function sets down the entire system
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




__task void job1 (void);
__task void job2 (void);

__task void job1 (void) {
  os_tsk_create (job2, 0); 
  /* Create task 2 and mark it as ready */
	
  while (1) {                /* loop forever */
    //counter1++;  		/* update the counter */
			
      temp_monitor(); 
	}
}

__task void job2 (void) {
  while (1) {         
	/* loop forever */
    //counter2++; 
	/* update the counter */
  }
}

void main (void) {
	
  os_sys_init (job1);  
	/* Initialize RTX Kernel and start task 1 */
	
	PINSEL0=0x00000000;
	IODIR0=0x00ff02fc;                      // making p0.16 to p0.23 as output lines
	IOCLR0=0x00ff0000;
  for (;;)
	;
	
	
}
