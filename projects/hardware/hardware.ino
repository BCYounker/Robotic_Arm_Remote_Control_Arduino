#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

/*
x:
Echo 2
Trig 3
y:
Echo 4
Trig 5
z:
Echo 6
Trig 7
*/
  
// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();


// Depending on your servo make, the pulse width min and max may vary, you 
// want these to be as small/large as possible without hitting the hard stop
// for max range. You'll have to tweak them as necessary to match the servos you
// have!
#define SERVOMIN  150 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // this is the 'maximum' pulse length count (out of 4096)
#define TOUCH_SIG 9 //触摸感应器管脚9
#define VOICE_SIG 12 //声音感应器管脚12

// our servo # counter
uint8_t servonum = 0;

const int echoPin_x = 2; // Echo Pin of Ultrasonic Sensor
const int pingPin_x = 3; // Trigger Pin of Ultrasonic Sensor
const int echoPin_y = 4; // Echo Pin of Ultrasonic Sensor
const int pingPin_y = 5; // Trigger Pin of Ultrasonic Sensor
const int echoPin_z = 6; // Echo Pin of Ultrasonic Sensor
const int pingPin_z = 7; // Trigger Pin of Ultrasonic Sensor
int touch_mode_flag = 0; // Distinguishing the claw when to open or to close

uint16_t pulselen_hand=300;

long duration_x0 = 0, cm_x0 = 0;
long duration_y0 = 0, cm_y0 = 0;
long duration_z0 = 0, cm_z0 = 0;

//获取状态
boolean get_touch(){
  boolean touch_stat=0;
  touch_stat=digitalRead(TOUCH_SIG);//读入状态
  return touch_stat;
}
boolean get_voice(){
  boolean voice_stat = 0;
  voice_stat = digitalRead(VOICE_SIG);
  return voice_stat;
}

void setup() {
   Serial.begin(9600); // Starting Serial Terminal
   
   pwm.begin();
  pinMode(TOUCH_SIG,INPUT);       //设置9号端口为输入模式
  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates

  delay(10);
}

void loop() {
boolean touch_stat, voice_stat;
  // put your main code here, to run repeatedly:
  //Serial.print("\nrunning\nTouch Stat - ");
  touch_stat=get_touch();
  voice_stat=get_voice();
  if(touch_stat == 1)
  {
    touch_mode_flag ++;
    if(touch_mode_flag == 4)
      touch_mode_flag = 0;
  }
  if(touch_mode_flag == 1)
    pulselen_hand+=20;
  if(touch_mode_flag == 3)
    pulselen_hand-=20;
  if(pulselen_hand>=400)//手开合的极限
    pulselen_hand=400;
  if(pulselen_hand<=200)
    pulselen_hand=200;
  pwm.setPWM(7, 0, pulselen_hand);
//multi-servo driving
  //first x axis from 210line to 440angle
  long duration_x,cm_x;
  pinMode(pingPin_x, OUTPUT);
  digitalWrite(pingPin_x, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin_x, HIGH);
  delayMicroseconds(10);
  digitalWrite(pingPin_x, LOW);
  pinMode(echoPin_x, INPUT);
  duration_x = pulseIn(echoPin_x, HIGH);
  cm_x = microsecondsToCentimeters(duration_x);
  uint16_t pulselen_x = (cm_x - cm_x0) * 7 + 210;
  //x轴俩舵机0和1的cm_x范围，可修改
  if(cm_x >=8 && cm_x <= 40){   
    pwm.setPWM(0, 0, pulselen_x);
    pwm.setPWM(1, 0, pulselen_x);
  }
  //then y axis from 250angle to 510line
  long duration_y,cm_y;
  pinMode(pingPin_y, OUTPUT);
  digitalWrite(pingPin_y, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin_y, HIGH);
  delayMicroseconds(10);
  digitalWrite(pingPin_y, LOW);
  pinMode(echoPin_y, INPUT);
  duration_y = pulseIn(echoPin_y, HIGH);
  cm_y = microsecondsToCentimeters(duration_y);
  uint16_t pulselen_y = (cm_y - cm_y0) * 8 + 250;
  //y轴舵机2的cm_y范围，可修改
  if(cm_y >=16 && cm_y <= 52) {
    pwm.setPWM(2, 0, pulselen_y);
  }
  //last z axis from 150 upper to 600 bottom
  long duration_z,cm_z;
  pinMode(pingPin_z, OUTPUT);
  digitalWrite(pingPin_z, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin_z, HIGH);
  delayMicroseconds(10);
  digitalWrite(pingPin_z, LOW);
  pinMode(echoPin_z, INPUT);
  duration_z = pulseIn(echoPin_z, HIGH);
  
  cm_z = microsecondsToCentimeters(duration_z);
  uint16_t pulselen_z = (cm_z - cm_z0) * 15 + 150;
  //z轴舵机3的cm_z范围，可修改
  if(cm_z >= 3 && cm_z <= 60) {
     pwm.setPWM(3, 0, pulselen_z);
  }

  if(voice_stat == 1)
  {
    duration_x0 = pulseIn(echoPin_z, HIGH);
    duration_y0 = pulseIn(echoPin_z, HIGH);
    duration_z0 = pulseIn(echoPin_z, HIGH);
    cm_x0 = microsecondsToCentimeters(duration_x0);
    cm_y0 = microsecondsToCentimeters(duration_y0);
    cm_z0 = microsecondsToCentimeters(duration_z0);
  }
//串口监视器分别查看各维度测量的距离
  Serial.println("x:");
  Serial.println(cm_x);
  Serial.println("y:");
  Serial.println(cm_y);
  Serial.println("z:");
  Serial.println(cm_z);
  
  delay(100);
}

long microsecondsToCentimeters(long microseconds) {
   return microseconds / 29 / 2;
}

// you can use this function if you'd like to set the pulse length in seconds
// e.g. setServoPulse(0, 0.001) is a ~1 millisecond pulse width. its not precise!
void setServoPulse(uint8_t n, double pulse) {
  double pulselength;
  
  pulselength = 1000000;   // 1,000,000 us per second
  pulselength /= 60;   // 60 Hz
  Serial.print(pulselength); Serial.println(" us per period"); 
  pulselength /= 4096;  // 12 bits of resolution
  Serial.print(pulselength); Serial.println(" us per bit"); 
  pulse *= 1000000;  // convert to us
  pulse /= pulselength;
  Serial.println(pulse);
  pwm.setPWM(n, 0, pulse);
}
