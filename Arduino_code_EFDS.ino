/*

The circuit:
ADXL335 (Accelerometer)
-----------------------
analog 0: accelerometer self test
analog 1: z-axis
analog 2: y-axis
analog 3: x-axis
GND: ground
+5V: vcc

HC-05 (Bluetooth Module)
------------------------
+3.3V: Vcc
GND: ground
Digital Pin 10(RX): TX
Digital Pin 11(TX): RX

Piezobuzzer
-----------
Digital Pin 9(PWM): +ve
GND: -ve

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Arduino Uno @ COM16---- via Arduino IDE

*/

#include <SoftwareSerial.h> 

// Accelerometer pin description
const int xpin = A3; // x-axis
const int ypin = A2; // y-axis
const int zpin = A1; // z-axis

float Xval, Yval, Zval; // acceleration in g's along each axis
float Anet, pitch, roll; // Net acceleration ( 1when stationary), pitch and roll convey orientation of accelerometer

unsigned long time = 0; // variable to compute time differences


int state = 0; // condition of subject ( 0 indicates stable and 5 indicates fallen )
int count =0;
int ALERT = 0; // fall, perhaps?

//below are the calibration values for the 3 axes, such that in stable condition the values are (1g, 0g, 0g)

float xZero = 505;
float yZero = 510;
float zZero = 525;

SoftwareSerial mySerial(10, 11); //RX, TX 

void setup()
{
  mySerial.begin(9600); // initialize the software serial communication
  pinMode(9, OUTPUT); // PWM output for piezobuzzer
  steptime = millis();
}

void loop()
{
  analogReference(EXTERNAL); // Enable Aref @ 3.3V
  // Compute acceleration values in g's
  Xval = (analogRead(xpin)- xZero)/102;
  Yval = (analogRead(ypin) - yZero)/102;
  Zval = (analogRead(zpin) - zZero)/102;
  Anet = sqrt(sq(Xval) + sq(Yval) + sq(Zval));

  // Pitch and roll caluculation
  pitch = atan(Xval/sqrt(pow(Yval,2) + pow(Zval,2)));
  roll = atan(Yval/sqrt(pow(Xval,2) + pow(Zval,2)));

  //convert radians into degrees
  pitch = pitch * (180.0/PI) - 90;
  roll = roll * (180.0/PI);

  // Serial transfer of required variables to Processing
  mySerial.print(pitch);
  mySerial.print(",");
  mySerial.print(roll);
  mySerial.print(",");
  mySerial.print(state);
  mySerial.print(",");
  mySerial.print(Anet);
  mySerial.print("\n");
  

  fall_detect(); // FALL DETECTION FUNCTION
  delay(20); // Delay between passing values
  }


void fall_detect()
{
  
  if((Anet<0.8)&&(state==0)&&((abs(pitch)<60)&&(abs(roll)<60))) // Initial drop in acc. due to free-fall like condition
    {
      state = 1;
      time = millis();
    }

    if(state == 1)
    {
        if(Anet>1) // Increase in acceleration due to impact of fall
        {
          state = 2;
          time = millis();
        }
    
    if(((millis()-time)/1000)>2) // max.permissible time between states 1 and 2
        state = 0;
    }

    if(state == 2)
      {
          if((abs(pitch)>60)||(abs(roll)>60)) //Senses a fall in any orientation by factoring pitch and roll
{
  state = 3;
  time = millis();
}
  if((millis()-time)>100) // max. permissible time between states 2 and 3
  state = 0;
}
  if(state == 3)
  { 
    if((abs(pitch)<60)&&(abs(roll)<60)) // Detects getting-up and prevents false alarm
    {
      count++;
      if(count>20)
        {
          state = 0;
          count = 0;
        }
    }
     if(((millis()-time)/1000)>3) // 3 second window after fall between states 3 and 4
    {
      state = 4;
      ALERT = 1;
      tone(9,250); // Fall, perhaps?
    }
  }
  
  if((state ==4)&&(ALERT==1))
  {
    if(((millis()-time)/1000)>20) // if patient is unresponsive for more than 20 secs, fall is confirmed-- necessary action taken via processing

    {
       state = 5;
       tone(9,4000); // Generate piezotone for distress
    }
    
  }
}
