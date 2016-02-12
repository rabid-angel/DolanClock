

//ASSIGNMENT

//Try and have your clocks ready to present next Wednesday.
//You can use any colors you want, but the colors distinguish the minutes and we should be able to tell what time it is to the nearest 5 minutes.
//Similarly, the hour should be indicated with a pulse sequence, but you can choose what that sequence should be, and whether to use 12 or 24-hour time.
//Use S1 to select between running, setting minutes, and setting hours; use S2 to actually set the value.
//Discuss how you want to indicate when in set mode- turn off the LED not being set, perhaps (or something else).
//
//Group E
//

#include "msp.h"

//////////////////////////////////////////////Global Variables

//int color = 0;
int mode = 0; //Automatic mode by default
int counter = 0;
int milliSecondsCounter = 0;
short secondsCounter = 0;
int minutesCounter = 0;
short hoursCounter = 0;
//short greenIntensity = 0;
//short blueIntensity = 0;
//short redIntensity = 0;
short radix = 0;
unsigned short m=0;

//unsigned short greenArray[5] = {128, 127, 84, 43, 0};  //Off, Lowest dim, Middle val, Half bright, Full on
//unsigned short blueArray[4] = {128, 127, 64, 0};       //Off, Lowest Dim, Half on, Full on
//unsigned short redArray[3] = {128, 127, 0};            //Off, Lowest Dim, Full on

unsigned short hour[5] = {0,0,0,0,0};
unsigned short colors[6] = {4,6,2,3,1,5};
unsigned short minute[2] = {0,0};

//////////////////////////////////////////////CONFIGURATION JUNK

void selectPortFunction (int port, int line, int sel0, int sel1) {
	//This function allows you to select correct registers for specified Ports and Lines

	if (port == 1) {                            //Setting Port 1//
		if (P1SEL0 & BIT(line) != sel0) {
			if (P1SEL1 & BIT(line) != sel1) {
				P1SELC |= BIT(line);                //Switching Both P1 Registers
			} else {
				P1SEL0 ^= BIT(line);                //Switching SEL0 P1 Register
			}
		} else {
			if (P1SEL1 & BIT(line) != sel1) {
				P1SEL1 ^= BIT(line);                //Switching SEL1 P1 Register
			}
		}
	} else {                                   //Setting Port 2//
		if (P2SEL0 & BIT(line) != sel0) {
			if (P2SEL1 & BIT(line) != sel1) {
				P2SELC |= BIT(line);                //Switching Both P1 Registers
			} else {
				P2SEL0 ^= BIT(line);                //Switching SEL0 P2 Register
			}
		} else {
			if (P2SEL1 & BIT(line) != sel1) {
				P2SEL1 ^= BIT(line);                //Switching SEL1 P2 Register
			}
		}
	}
}

void initializeLED (void) {
	P2DIR |= BIT0;                  // Configure P2.0 as output
	P2DIR |= BIT1;                  // Configure P2.1 as output
	P2DIR |= BIT2;				   // Configure P2.2 as output
	P1DIR |= BIT0;

	selectPortFunction(2,0,0,0);
	selectPortFunction(2,1,0,0);
	selectPortFunction(2,2,0,0);

	P2OUT &=~ (BIT2 | BIT1 | BIT0);
}

void initializePushButton (int line) {

	selectPortFunction(1,line,0,0);

	P1DIR &=~ BIT(line);
	P1REN |= BIT(line);
	P1OUT |= BIT(line);
}

void configureTimer (void) {
	TA0CTL=0x0100;
	TA0CCTL0=0x2000;
	TA0CCR0=0x0080; // or TA0CCR0=128
	TA0CCTL1=0x2010;      //CCTL1
	TA0CCR1=0x0040; //RED LED
	TA0CCTL2=0x2010;      //CCTL2
	TA0CCR2=0x0040; //GREEN LED
	TA0CCTL3=0x2010;      //CCTL3
	TA0CCR3=0x0040; //BLUE LED
	TA0CCTL4=0x2010;      //CCTL4
	TA0CCR4=0x0040; //RED LED1
	TA0CTL=0x0116;

}

void setClockFrequency (void) {
	CSKEY = 0x695A;
	CSCTL1 = 0x00000233;
	CSCLKEN = 0x0000800F;
	CSKEY = 0xA596;
}

//////////////////////////////////////////////CONFIGURATION JUNK FINISHED

//////////////////////////////////////////////VVV REAL METHODS VVV

void updateHourLights (void) {
	hour[4]=hoursCounter%2;
	hour[3]=(((int) (hoursCounter/2))%2);
	hour[2]=(((int) (hoursCounter/4))%2);
	hour[1]=(((int) (hoursCounter/8))%2);
	hour[0]=(((int) (hoursCounter/16))%2);
}

void updateHour (void) {
	////////////////////////Hours
	hoursCounter++;
	if (hoursCounter == 24) {
		hoursCounter = 0;
		//updateHourLights();
	} else {
		//updateHourLights();
	}
	updateHourLights();
}

void updateMinute (void) {
	////////////////////////Minutes
	minutesCounter++;
	//minute[1]=((int) ((minutesCounter%10)*(6/10)));
	//minute[1]=(((int) (minutesCounter*(36/60)))%6);
	minute[0]=(((int) (minutesCounter/10))%6);
	if (minute[0]==0){minute[1]=minute[1]+1;}
	//minute[0]=(((int) (minutesCounter*(36/60)))%6);

	//minute[2]=(((int) (minutesCounter/10))%6);
	//minute[3]=(((int) (minutesCounter*(36/60)))%6);
	//minute[4]=(((int) (minutesCounter/10))%6);

	if (minutesCounter == 60) {
		minutesCounter = 0;
		updateHour();
	}
}

void updateSecond (void) {
	secondsCounter++;
	if (secondsCounter == 60){
		secondsCounter=0;
		updateMinute();
	}
}



void portOneInterrupt(void){
	unsigned short iflag = P1IV;

	if(!(P1IN & BIT4)){
		if(mode == 0){
			mode = 1;
			P1OUT&=~BIT0;
		} else if(mode == 1) {
			mode = 2;
		} else if(mode == 2){
			mode = 0;
		}
	}

	if(!(P1IN & BIT1)){
/*		if(mode == 1){
			int i;
			for(i=0;i<60;i++){
				updateSecond();
			}*/
			updateMinute();
		//}
/*		else if(mode == 2){
			updateHour();
		}*/
	}
}


void timerA0Interrupt (void) {                   //Loop set to fire every millisecond

	//Catching the interrupts
	unsigned short intv = TA0IV; //Table on page 607

	if (intv == 0x02 &&mode!=1) {                         //0x02 is interrupt on compare 1
		P2OUT|=BIT0; // Turn Red LED on
	} else if (intv == 0x04 &&mode!=1) {              //0x04 is interrupt on compare 2
		P2OUT|=BIT1; // Turn green LED on
	}else if (intv == 0x06 &&mode!=1) {               //0x06 is interrupt on compare 3
		P2OUT|=BIT2; // Turn blue LED on
	}else if (intv == 0x08 &&mode!=2){
		P1OUT|=BIT0; //Turn Red hour LED on
	}else if (intv==0x0E) {                 //0x0E is overflow interrupt (you reached top of peak)
		P2OUT&=~(BIT0 | BIT1 | BIT2); // Turn lights off
		if(radix>4||!(hour[radix])){P1OUT&=~BIT0;}//leave the red led on when hour[radix] is 0.
		milliSecondsCounter++;
	}

	if(m==2){
				TA0CCR1=129;
				TA0CCR2=129;
				TA0CCR3=129;
			//	TA0CCR1=(((int) (colors[minute[1]]/4)+1)%2)*129;
			//					TA0CCR2=(((int) (colors[minute[1]]/2)+1)%2)*129;
			//					TA0CCR3=((colors[minute[1]]+1)%2)*129;
			}else if(m==1){
				TA0CCR1=(((int) (colors[minute[0]]/4)+1)%2)*129;//0
				TA0CCR2=(((int) (colors[minute[0]]/2)+1)%2)*129;//129
				TA0CCR3=((colors[minute[0]]+1)%2)*129;//129
			}else{
				TA0CCR1=(((int) (colors[minute[1]]/4)+1)%2)*129;
				TA0CCR2=(((int) (colors[minute[1]]/2)+1)%2)*129;
				TA0CCR3=((colors[minute[1]]+1)%2)*129;
				/*does this:
				if(((int) (colors[minute[m-1]]/4))%2){TA0CCR1=0;}else{TA0CCR1=129;}
				if(((int) (colors[minute[m-1]]/2))%2){TA0CCR2=0;}else{TA0CCR2=129;}
				if(colors[minute[m-1]]%2){TA0CCR3=0;}else{TA0CCR3=129;}*/
			}

	if(milliSecondsCounter==800){TA0CCR4=129;}
	//controls hour seq
	if(milliSecondsCounter==1000){
		radix++;


		m++;
		if(m==3){m=0;}

		//for red LED
		if(radix==7){radix=0;}
		if(radix<5){TA0CCR4=128;}

		milliSecondsCounter = 0;                   //Reset the timer
		updateSecond();
		}

}



void main (void) {
	WDTCTL = WDTPW | WDTHOLD;

	initializeLED();
	initializePushButton(1);
	initializePushButton(4);
	setClockFrequency();
	configureTimer();
	updateMinute();

	P1IE = (BIT1 | BIT4);
	P1IES |= (BIT1 | BIT4);
	NVIC_EnableIRQ(PORT1_IRQn);
	NVIC_EnableIRQ(TA0_N_IRQn);
	P1OUT|=BIT0;
	for(;;)
	{
		// Delay
	}
}
