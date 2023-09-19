#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>


#define TOUCH_SIG 8 //触摸感应器管脚8
#define TOUCH_SIG_HAND 9 //超声波触控控制爪的开合，管脚9


int mode_flag = 0;//模式状态变量

//获取状态
boolean get_touch() {
  boolean touch_stat = 0;
  touch_stat = digitalRead(TOUCH_SIG);//读入状态
  return touch_stat;
}

boolean get_touch_hand() {
  boolean touch_stat_hand = 0;
  touch_stat_hand = digitalRead(TOUCH_SIG_HAND);//读入状态
  return touch_stat_hand;
}

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
// Depending on your servo make, the pulse width min and max may vary, you 
// want these to be as small/large as possible without hitting the hard stop
// for max range. You'll have to tweak them as necessary to match the servos you
// have!
#define SERVOMIN  150 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // this is the 'maximum' pulse length count (out of 4096)

//在这里VRx(x)连的是模拟端口0，VRy(y)连的是模拟端口1，SW(z)连接到了数字端口7
int value_x = 0; 
int value_y = 0; 
int value_z = 0; 
int value_z1 = 0; 
int value_left = 1;
int value_right = 1;
int value_LFT = 1;
int value_RHT = 1;
int value_RST = 1;
int value_left_hand = 1;
int value_right_hand = 1;
int flag = 300;
long duration_x0 = 0, cm_x0 = 0;
long duration_y0 = 0, cm_y0 = 0;
long duration_z0 = 0, cm_z0 = 0;

uint16_t pulselen_x = 250;
uint16_t pulselen_y = 300;
uint16_t pulselen_z = 150;
uint16_t pulselen_z1 = 360;
uint16_t pulselen_z2 = 300;
uint16_t pulselen_z3 = 350;
uint16_t pulselen_z4 = 300;

//超声波部分的定义
const int echoPin_x = 2; // Echo Pin of Ultrasonic Sensor
const int pingPin_x = 3; // Trigger Pin of Ultrasonic Sensor
const int echoPin_y = 4; // Echo Pin of Ultrasonic Sensor
const int pingPin_y = 5; // Trigger Pin of Ultrasonic Sensor
const int echoPin_z = 6; // Echo Pin of Ultrasonic Sensor
const int pingPin_z = 7; // Trigger Pin of Ultrasonic Sensor
const int RSTPIN = 42; //reset pin of check sensor


int touch_mode_flag = 0; // Distinguishing the claw when to open or to close
int touch_mode_flag_hand = 0; // Distinguishing the claw when to open or to close


void setup() 
{ 
  pinMode(12, INPUT_PULLUP); //注意 Z 轴输入一定要上拉，不然电平不稳。
  pinMode(11, INPUT_PULLUP); //注意 Z 轴输入一定要上拉，不然电平不稳。
  //pinMode(46, INPUT_PULLUP);
  //pinMode(44, INPUT_PULLUP);
  pinMode(42, INPUT_PULLUP);
  Serial.begin(9600); 
  pwm.begin();
 
  pinMode(TOUCH_SIG, INPUT);       //设置8号端口为输入模式
  
  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates
  //delay(10);
} 
void loop() 
{ 
  boolean touch_stat = 0, touch_stat_hand = 0;
  //put your main code here, to run repeatedly:
  //Serial.print("\nrunning\nTouch Stat - ");
  touch_stat = get_touch();
  touch_stat_hand = get_touch_hand();

      value_x = analogRead(A0); 
      value_y = analogRead(A1); 
      value_z = analogRead(A3); 
      value_z1 = analogRead(A2); 
      //遥杆控制z2
      value_left = digitalRead(11); 
      value_right = digitalRead(12);
      //五线控制手的开合 
      //value_LFT = digitalRead(LFTPIN);
      //value_RHT = digitalRead(RHTPIN);

      //x轴的移动 中间值做了调整，网上是512 实测x轴524 y轴511
      if (value_x > 530) pulselen_x += 10;
      if (value_x < 520) pulselen_x -= 10;
      //x轴的极限
      if (pulselen_x >= 440) pulselen_x = 440;
      if (pulselen_x <= 210) pulselen_x = 210;

      //y轴的移动
      if (value_y > 530) pulselen_y += 10;
      if (value_y < 500) pulselen_y -= 10;
      //y轴的极限
      if (pulselen_y >= 510) pulselen_y = 510;
      if (pulselen_y <= 250) pulselen_y = 250;

      //z轴移动
      if (value_z > 530) pulselen_z += 10;
      if (value_z < 500) pulselen_z -= 10;
      //z轴极限
      if (pulselen_z >= SERVOMAX) pulselen_z = SERVOMAX;
      if (pulselen_z <= SERVOMIN) pulselen_z = SERVOMIN;

      //z1轴移动
      if (value_z1 > 530) pulselen_z1 += 10;
      if (value_z1 < 500) pulselen_z1 -= 10;
      //z1轴极限
      if (pulselen_z1 >= 590) pulselen_z1 = 590;
      if (pulselen_z1 <= 130) pulselen_z1 = 130;

      //z2的转动
      if (value_left == 0 && value_right == 1) pulselen_z2 += 10;
      if (value_left == 1 && value_right == 0) pulselen_z2 -= 10;
      //z2的范围，100到600
      if (pulselen_z2 >= 600) pulselen_z2 = 600;
      if (pulselen_z2 <= 100) pulselen_z2 = 100;

      //手的开合
      //if (value_LFT == 0 && value_RHT == 1) pulselen_z4 += 10;
      //if (value_LFT == 1 && value_RHT == 0) pulselen_z4 -= 10;
      //z2的范围，100到600
      if (pulselen_z4 >= 400) pulselen_z4 = 400;
      if (pulselen_z4 <= 180) pulselen_z4 = 180;
      
      if (touch_stat_hand == 1)
      {
        touch_mode_flag_hand ++;
        if (touch_mode_flag_hand == 4) touch_mode_flag_hand = 0;
      }
      if (touch_mode_flag_hand == 1) pulselen_z4 += 20;
      if (touch_mode_flag_hand == 3) pulselen_z4 -= 20;

      pwm.setPWM(7, 0, pulselen_z4);
      Serial.println(touch_mode_flag_hand);
      pwm.setPWM(0, 0, pulselen_x);
      pwm.setPWM(1, 0, pulselen_x);
      pwm.setPWM(2, 0, pulselen_y);
      pwm.setPWM(3, 0, pulselen_z);
      pwm.setPWM(4, 0, pulselen_z1);
      pwm.setPWM(5, 0, pulselen_z2);
      //pwm.setPWM(6, 0, pulselen_z3);
      pwm.setPWM(7, 0, pulselen_z4);
   
  //delay(100); 
} 
