/* ------------------------------------------
	Ka Yi Cheng
	ECS642 Lab 5
	KL28Z Version
				
    Start at green RGB LED on
    After first press B1, VR1 and VR2 are measured, and the red RGB LED on
    Check with the condition, if true, the system will control the shield LED
    if false, the system will go to error state   
  -------------------------------------------- */
#include <stdbool.h>
#include <stdint.h>
#include <MKL28Z7.h>
#include "..\inc\SysTick.h"
#include "..\inc\button.h"
#include "..\inc\rgb.h"
#include "..\inc\led.h"
#include "..\inc\adc.h"

/* --------------------------------------------
  Set the maximum and minimum on/off time
*----------------------------------------------------------------------------*/
#define maxTime 4000    
#define minTime 500     

/*----------------------------------------------------------------------------
  task1pollB1
  
  This task polls button B1. 
*----------------------------------------------------------------------------*/
int b1State;               
bool pressedB1_ev;        

void initPollB1Task() {
    b1State = BOPEN;       
    pressedB1_ev = false;   
}

void pollB1Task() {
    static int b1BounceCount = 0; 
    if (b1BounceCount > 0) b1BounceCount--;
    switch (b1State) {
        case BOPEN:
            if (isPressed(B1)) {        
                b1State = BCLOSED;  
                pressedB1_ev = true;  
            }
            break;
        case BCLOSED:
            if (!isPressed(B1)) {        
                b1State = BBOUNCE;   
                b1BounceCount = BOUNCEDELAY; 
            }
            break;
        case BBOUNCE:
            if (isPressed(B1)) {         
                b1State = BCLOSED;   
            } else if (b1BounceCount == 0) {  
                b1State = BOPEN;    
            }
            break;
    }
}

/*----------------------------------------------------------------------------
   Global variables
*----------------------------------------------------------------------------*/
// ADC value for potentiometer VR1 and VR2
volatile uint16_t vr1in;  
volatile uint16_t vr2in; 

// Minimum and maximum calibration values for VR1 and VR2
uint16_t vr1min, vr1max;      
uint16_t vr2min, vr2max;    

bool isSystemActive = false;    
bool calibration = true;    
int counter = 0;       

/*----------------------------------------------------------------------------
   Task: task2MeasureVR

   This task used to manage system state based on the button actions 
*----------------------------------------------------------------------------*/
void task2MeasureVR() {
    if (pressedB1_ev) {  
        if (calibration) {
            // First button press: Record minimum ADC values for VR1 and VR2
            vr1min = MeasureVR(VR1);
            vr2min = MeasureVR(VR2);
            calibration = false; 

            // And change the red RGB LED to be on
            setRGB(GREEN, RGB_OFF);
            setRGB(BLUE, RGB_OFF);
            setRGB(RED, RGB_ON);
        } else {
            // Second button press: Record maximum ADC values for VR1 and VR2
            vr1max = MeasureVR(VR1);
            vr2max = MeasureVR(VR2);

            // Check the condition
            if (vr1max > vr1min && vr2max > vr2min) {
                isSystemActive = true; // Switch system to active mode

                // And change the green RGB LED to be on
                setRGB(RED, RGB_OFF);
                setRGB(BLUE, RGB_OFF);
                setRGB(GREEN, RGB_ON);
            } else {
                // Error State: Flash blue RGB LED and shield LEDs
                setRGB(RED, RGB_OFF);   
                setRGB(GREEN, RGB_OFF); 
                while (true) {
                    setRGB(BLUE, RGB_ON);
                    waitSysTickCounter(500); // Wait for 0.5 seconds (on)
									
                    setRGB(BLUE, RGB_OFF);
                    ledOnOff(LED1, LED_OFF);
                    ledOnOff(LED2, LED_OFF);
                    ledOnOff(LED3, LED_OFF);
                    ledOnOff(LED4, LED_OFF);
                    ledOnOff(LED5, LED_OFF);
                    waitSysTickCounter(500); // Wait for 0.5 seconds (off)
                }
            }
        }
        pressedB1_ev = false;
    }
}

/*----------------------------------------------------------------------------
   Task: ledManagerTaskTask

   This task used to control the shield LEDs based on potentiometer values
*----------------------------------------------------------------------------*/
void ledManagerTask() {
    if (isSystemActive) {  
				// Read ADC value for VR1 and VR2
        vr1in = MeasureVR(VR1);
        vr2in = MeasureVR(VR2); 

        // Calculate LED on/off durations based on ADC values
        uint32_t ledOnTime = minTime + (vr1in * (maxTime - minTime)) / 0xFFFF;
        uint32_t ledOffTime = minTime + (vr2in * (maxTime - minTime)) / 0xFFFF;

        // Manage the LED flashing cycle
        if (counter < ledOnTime / 10) { // LEDs on phase
            ledOnOff(LED1, LED_ON);
            ledOnOff(LED2, LED_ON);
            ledOnOff(LED3, LED_ON);
            ledOnOff(LED4, LED_ON);
            ledOnOff(LED5, LED_ON);
        } else if (counter < (ledOnTime + ledOffTime) / 10) { // LEDs off phase
            ledOnOff(LED1, LED_OFF);
            ledOnOff(LED2, LED_OFF);
            ledOnOff(LED3, LED_OFF);
            ledOnOff(LED4, LED_OFF);
            ledOnOff(LED5, LED_OFF);
        } else {
            counter = 0; // Reset cycle counter after a full cycle
        }
        counter++;  // Increment the cycle counter every 10 ms
    } else {
        // Ensure all LEDs are off when the system is inactive
        ledOnOff(LED1, LED_OFF);
        ledOnOff(LED2, LED_OFF);
        ledOnOff(LED3, LED_OFF);
        ledOnOff(LED4, LED_OFF);
        ledOnOff(LED5, LED_OFF);
    }
}

// Main function to execute the system
int main(void) {
	
    // Initialise peripherals
    configureButtons(B1, false);  // Configure button B1 for polling
    configureLEDs();              // Configure shield LEDs
    configureRGB();                // Configure the 3-colour LED on the FRDM board 
	
    // Initialise and calibrate ADC
    initADC(); 
	
    // initialse SysTick every 1 ms
    Init_SysTick(1000);         
	
    // Initialise tasks and cycle counter
    initPollB1Task();          
    waitSysTickCounter(10);      
    setRGB(GREEN, RGB_ON); 	

    while (1) {           // this runs forever
        pollB1Task();           // Generate signals for a simulated button
        task2MeasureVR();      
        ledManagerTask();              
        waitSysTickCounter(10);    // cycle every 10 ms 
    }
}
