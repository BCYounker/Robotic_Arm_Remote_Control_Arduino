//拟定完整代码
//由于疑似uno算不过来目前难以实现
//希望能发现真正的原因
//

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <IRremote.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
 
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
 
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2
 
#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 
 
#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

#define TOUCH_SIG 8 //触摸感应器管脚8
#define TOUCH_SIG_HAND 9 //超声波触控控制爪的开合，管脚9
#define RECV_PIN 10 //红外接收器hx1838管脚10
IRrecv irrecv(RECV_PIN);

decode_results results;
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
//const int LFTPIN = 44; //left_control pin of check sensor
//const int RHTPIN = 46; //right_control pin of check sensor

int touch_mode_flag = 0; // Distinguishing the claw when to open or to close
int touch_mode_flag_hand = 0; // Distinguishing the claw when to open or to close

//显示一个心形
static const uint8_t PROGMEM Heart_16x16[] = {
  0x00,0x00,0x18,0x18,0x3C,0x3C,0x7E,0x7E,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0x7F,0xFE,0x3F,0xFC,0x1F,0xF8,0x0F,0xF0,0x07,0xE0,0x03,0xC0,0x00,0x00//未命名文件0
};
 
//"猛"
static const uint8_t PROGMEM Strong_16x16[] = {
  0x00,0x00,0x45,0xFC,0x28,0x08,0x10,0x10,0x28,0x20,0x4B,0xFE,0x88,0x20,0x08,0xA0,
  0x18,0x40,0x29,0xFC,0x49,0x54,0x89,0x54,0x09,0x54,0x09,0x54,0x57,0xFE,0x20,0x00//猛0
};

void setup() 
{ 
  pinMode(12, INPUT_PULLUP); //注意 Z 轴输入一定要上拉，不然电平不稳。
  pinMode(11, INPUT_PULLUP); //注意 Z 轴输入一定要上拉，不然电平不稳。
  //pinMode(46, INPUT_PULLUP);
  //pinMode(44, INPUT_PULLUP);
  pinMode(42, INPUT_PULLUP);
  Serial.begin(9600); 
  pwm.begin();
  
  irrecv.enableIRIn(); // Start the receiver
  pinMode(TOUCH_SIG, INPUT);       //设置8号端口为输入模式
  
  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64) 

  //delay(10);
} 
void loop() 
{ 
  boolean touch_stat = 0, touch_stat_hand = 0;
  //put your main code here, to run repeatedly:
  //Serial.print("\nrunning\nTouch Stat - ");
  touch_stat = get_touch();
  touch_stat_hand = get_touch_hand();
  //Serial.print("touch_stat=");
  //Serial.println(touch_stat);
  //Serial.print(mode_flag);
  if (touch_stat == 1)
  {
    Serial.println("touch_stat1");
    test_SSD1306();
    if (irrecv.decode(&results)) {
      Serial.println(results.value, HEX);
      switch (results.value)//通过按键切换模式
      {
        case 0xFF9867:
          mode_flag = 0;//欢迎模式
          break;
        case 0xFFA25D:
          mode_flag = 1;//摇杆模式
          break;
        case 0xFF629D:
          mode_flag = 2;//超声波测距模式
          break;
        case 0xFFE21D:
          mode_flag = 3;//遥控模式
          break;
      }
      irrecv.resume();//这句很重要，不然无法接收第二个值
    }
  }
  if (touch_stat == 0)
  {
    //Serial.println("touch_stat0");
    if (mode_flag == 0)
    {
      welcome_SSD1306();
    }
    if (mode_flag == 1)
    {
      mode1_SSD1306();
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
      /*
      //转动 取决于flag加系数与减系数还有flag幅值的比例
      //pulselen_z3=400;
      if (value_z3 > 530) flag += 11;
      if (value_z3 < 520)
      {
        pulselen_z3 = 300;
        flag -= 17;
      }
      //不转动
      if (value_z3 <= 530 && value_z3 >= 520) pulselen_z3 = 350;
      //转动极限
      if (flag >= 400)
      {
        pulselen_z3 = 350;//停止转动
        flag = 400;
      }
      if (flag <= 0)
      {
        pulselen_z3 = 350;
        flag = 0;
      }
      */
      pwm.setPWM(0, 0, pulselen_x);
      pwm.setPWM(1, 0, pulselen_x);
      pwm.setPWM(2, 0, pulselen_y);
      pwm.setPWM(3, 0, pulselen_z);
      pwm.setPWM(4, 0, pulselen_z1);
      pwm.setPWM(5, 0, pulselen_z2);
      //pwm.setPWM(6, 0, pulselen_z3);
      pwm.setPWM(7, 0, pulselen_z4);
   }
   if(mode_flag == 2)
   {
      mode2_SSD1306();
      
      //五线控制手的开合 
      //value_LFT = digitalRead(LFTPIN);
      //value_RHT = digitalRead(RHTPIN);
      value_RST = digitalRead(RSTPIN);
      
      if (value_RST == 1) 
      {
        duration_x0 = pulseIn(echoPin_z, HIGH);
        duration_y0 = pulseIn(echoPin_z, HIGH);
        duration_z0 = pulseIn(echoPin_z, HIGH);
        cm_x0 = microsecondsToCentimeters(duration_x0);
        cm_y0 = microsecondsToCentimeters(duration_y0);
        cm_z0 = microsecondsToCentimeters(duration_z0);
      }
      
      //手开合的极限
      if(pulselen_z4 >= 400) pulselen_z4 = 400;
      if(pulselen_z4 <= 180) pulselen_z4 = 180;
      //手的开合
      //if (value_LFT == 1) pulselen_z4 += 10;
      //if (value_LFT == 1 && value_RHT == 0) pulselen_z4 -= 10;
      //触控板控制手的开合
      if (touch_stat_hand == 1)
      {
        touch_mode_flag_hand ++;
        if (touch_mode_flag_hand == 4) touch_mode_flag_hand = 0;
      }
      if (touch_mode_flag_hand == 1) pulselen_z4 += 20;
      if (touch_mode_flag_hand == 3) pulselen_z4 -= 20;
      Serial.println(touch_mode_flag_hand);
      pwm.setPWM(7, 0, pulselen_z4);
      
      //multi-servo driving
      //first x axis from 210line to 440angle
      long duration_x, cm_x;

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
      if (cm_x >= 8 && cm_x <= 40){   
        pwm.setPWM(0, 0, pulselen_x);
        pwm.setPWM(1, 0, pulselen_x);
      }

      //then y axis from 250angle to 510line
      long duration_y, cm_y;

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
      long duration_z, cm_z;

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
      
      //串口监视器分别查看各维度测量的距离
      //Serial.println("x:");
      //Serial.println(cm_x);
      //Serial.println("y:");
      //Serial.println(cm_y);
      //Serial.println("z:");
      //Serial.println(cm_z);
   }
   if(mode_flag == 3)
   {
      mode3_SSD1306();
      if (irrecv.decode(&results)) 
      {
        //Serial.println(results.value, HEX);
        switch (results.value)
        {
          case 0xFF18E7://上的键值，两个x轴向y轴正方向转动
            pulselen_x += 20;
            if (pulselen_x >= 440) pulselen_x = 440;
            if (pulselen_x <= 210) pulselen_x = 210;
            pwm.setPWM(0, 0, pulselen_x);
            pwm.setPWM(1, 0, pulselen_x);
            break;
          case 0xFF4AB5://下的键值，两个x轴向y轴反方向转动
            pulselen_x -= 20;
            if (pulselen_x >= 440) pulselen_x = 440;
            if (pulselen_x <= 210) pulselen_x = 210;
            pwm.setPWM(0, 0, pulselen_x);
            pwm.setPWM(1, 0, pulselen_x);
            break;
          case 0xFF10EF://左的键值，y轴向内转动
            pulselen_y += 20;
            if (pulselen_y >= 570) pulselen_y = 570;
            if (pulselen_y <= 250) pulselen_y = 250;
            pwm.setPWM(2, 0, pulselen_y);
            break;
          case 0xFF5AA5://右的键值，y轴向外转动
            pulselen_y -= 20;
            if (pulselen_y >= 570) pulselen_y = 570;
            if (pulselen_y <= 250) pulselen_y = 250;
            pwm.setPWM(2, 0, pulselen_y);
            break;
          case 0xFF9867://0的键值,z轴向下转动
            pulselen_z += 20;
            if (pulselen_z >= 460) pulselen_z = 460;
            if (pulselen_z <= 140) pulselen_z = 140;
            pwm.setPWM(3, 0, pulselen_z);
            //Serial.print("键0:");
            //Serial.println(pulselen_z);
            break;  
          case 0xFFA857://8的键值,z轴向上转动
            pulselen_z -= 20;
            if (pulselen_z >= 460) pulselen_z = 460;
            if (pulselen_z <= 140) pulselen_z = 140;
            pwm.setPWM(3, 0, pulselen_z);
            //Serial.print("键8:");
            //Serial.println(pulselen_z);
            break;
          case 0xFFA25D://1的键值,z1轴顺时针转动，y轴向自己伸展
            pulselen_z1 += 20;
            if (pulselen_z1 >= 590) pulselen_z1 = 590;
            if (pulselen_z1 <= 130) pulselen_z1 = 130;
            pwm.setPWM(4, 0, pulselen_z1);
            break;  
          case 0xFFE21D://3的键值,z1轴逆时针转动
            pulselen_z1 -= 20;
            if (pulselen_z1 >= 590) pulselen_z1 = 590;
            if (pulselen_z1 <= 130) pulselen_z1 = 130;
            pwm.setPWM(4, 0, pulselen_z1);
            break;
          case 0xFF22DD://4的键值,z2轴时钟型顺时针向上抬动
            pulselen_z2 -= 20;
            if (pulselen_z2 >= 600) pulselen_z2 = 600;
            if (pulselen_z2 <= 100) pulselen_z2 = 100;
            pwm.setPWM(5, 0, pulselen_z2);
            //Serial.print("键4:");
            //Serial.println(pulselen_z2);
            break;  
          case 0xFFC23D://6的键值,z2轴时钟型逆时针向上抬动
            pulselen_z2 += 20;
            if (pulselen_z2 >= 600) pulselen_z2 = 600;
            if (pulselen_z2 <= 100) pulselen_z2 = 100;
            pwm.setPWM(5, 0, pulselen_z2);
            //Serial.print("键6:");
            //Serial.println(pulselen_z2);
            break;
          /*
            //360度舵机无法像原来那样驱动
            case 0xFFE01F://7的键值,z3轴向转动
            pulselen_z3+=10;
            if(pulselen_z3>=370) pulselen_z3=370;
            if(pulselen_z3<=140) pulselen_z3=140;
            pwm.setPWM(6, 0, pulselen_z3);
            break;  
          case 0xFF906F://9的键值,z3轴向转动
            pulselen_z3-=10;
            if(pulselen_z3>=370) pulselen_z3=370;
            if(pulselen_z3<=140) pulselen_z3=140;
            pwm.setPWM(6, 0, pulselen_z3);
            break;
          */
          case 0xFF6897://*的键值,爪子夹紧
            if (pulselen_z4 >= 400) pulselen_z4 = 400;
            if (pulselen_z4 <= 180) pulselen_z4 = 180;
            pwm.setPWM(7, 0, 400);
            //Serial.print("键*:");
            //Serial.println(pulselen_z4);
            break;  
          case 0xFFB04F://#的键值,爪子张大
            if (pulselen_z4 >= 400) pulselen_z4 = 400;
            if (pulselen_z4 <= 180) pulselen_z4 = 180;
            pwm.setPWM(7, 0, 180);
            //Serial.print("键#:");
            //Serial.println(pulselen_z4);
            break;
          case 0xFFFFFFFF:
            break;
        }
        irrecv.resume();
      }
    }
  }
  //delay(100); 
} 

void test_SSD1306(void)
{    
  //13.显示英文 数字
  display.clearDisplay();   // clears the screen and buffer
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Please enter a mode");
  display.println("number!");
  /*display.setTextColor(BLACK, WHITE); // 'inverted' text
  display.println(3.141592);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.print("0x"); display.println(0xDEADBEEF, HEX);*/
  display.display();
}
void welcome_SSD1306(void)
{
  //Serial.println("good2");
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Welcome!");
  display.display();
  //Serial.println("good3");
}
void mode1_SSD1306(void)
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Mode 1");
  display.setTextSize(2);
  display.println("Joystick_Control");
  display.display();;
}
void mode2_SSD1306(void)
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Mode 2");
  display.setTextSize(2);
  display.println("Ultrasonic_Control");
  display.display();
}
void mode3_SSD1306(void)
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Mode 3");
  display.setTextSize(2);
  display.println("Remote_Control");
  display.display();
}
//把时间根据声速换算成长度
long microsecondsToCentimeters(long microseconds)
{ 
   return microseconds / 29 / 2;
}
