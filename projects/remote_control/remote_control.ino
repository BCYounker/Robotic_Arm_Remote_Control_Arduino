#include <IRremote.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
int RECV_PIN = 10;

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
// Depending on your servo make, the pulse width min and max may vary, you 
// want these to be as small/large as possible without hitting the hard stop
// for max range. You'll have to tweak them as necessary to match the servos you
// have!
#define SERVOMIN  150 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // this is the 'maximum' pulse length count (out of 4096)


IRrecv irrecv(RECV_PIN);

decode_results results;

uint16_t pulselen_x=210;
uint16_t pulselen_y=250;
uint16_t pulselen_z=150;

void setup()
{
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
  
  pwm.begin();
  
  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates

  delay(10);
}

void loop() {
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    if (results.value == 0xFF18E7) //上的值
    {
        //first x axis from 210line to 440angle
      pulselen_x+=7;
      if(pulselen_x>=440)
        pulselen_x=440;
      if(pulselen_x<=210)
        pulselen_x=210;
      if (results.value == 0xFFFFFFFF)
      {
        pulselen_x+=20;
        if(pulselen_x>=440)
          pulselen_x=440;
        if(pulselen_x<=210)
          pulselen_x=210;Serial.println("good");
        irrecv.resume(); // Receive the next value
      }
    } else if (results.value == 0xFF4AB5) //下的值
    {
      pulselen_x-=20;
      if(pulselen_x>=440)
        pulselen_x=440;
      if(pulselen_x<=210)
        pulselen_x=210;
        irrecv.resume(); // Receive the next value
      if (results.value == 0xFFFFFFFF)
      {
        pulselen_x-=7;
        if(pulselen_x>=440)
          pulselen_x=440;
        if(pulselen_x<=210)
          pulselen_x=210;Serial.println("good");
        irrecv.resume(); // Receive the next value
      }
    }
    else if (results.value == 0xFF10EF) //左的值
    {
      //then y axis from 250angle to 510line
      pulselen_y+=20;
      if(pulselen_y>=510)
        pulselen_y=510;
      if(pulselen_y<=250)
        pulselen_y=250;
        irrecv.resume(); // Receive the next value
      if (&results.value == 0xFFFFFFFF)
      {
        pulselen_y+=20;
        if(pulselen_y>=510)
          pulselen_y=510;
        if(pulselen_y<=250)
          pulselen_y=250;Serial.println("good");
        irrecv.resume(); // Receive the next value
      }
    }
    else if (results.value == 0xFF5AA5) //右的值
    {
      pulselen_y-=8;
      if(pulselen_y>=510)
        pulselen_y=510;
      if(pulselen_y<=250)
        pulselen_y=250;
        irrecv.resume(); // Receive the next value
      if (results.value == 0xFFFFFFFF)
      {
        pulselen_y-=8;
        if(pulselen_y>=510)
          pulselen_y=510;
        if(pulselen_y<=250)
          pulselen_y=250;Serial.println("good");
        irrecv.resume(); // Receive the next value
      }
    }
    irrecv.resume(); // Receive the next value
  }if(pulselen_y>=510)
          pulselen_y=510;
        if(pulselen_y<=250)
          pulselen_y=250;
  pwm.setPWM(0, 0, pulselen_x);
  pwm.setPWM(1, 0, pulselen_x);
  pwm.setPWM(2, 0, pulselen_y);
  pwm.setPWM(3, 0, pulselen_z);
  delay(100);
}
