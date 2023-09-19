//最基础的切换程序（通过遥控切换）
//操作方法：触摸感应器被触发为1时才能更改模式
//通过按下1 2 3 0选择模式，其中0模式是欢迎界面
//计划在各模式的分支中加入他们的操作代码
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

#define TOUCH_SIG 13 //触摸感应器管脚13
#define RECV_PIN 10 //红外接收器hx1838管脚10
IRrecv irrecv(RECV_PIN);

decode_results results;
int mode_flag=0;//模式状态变量

//获取状态
boolean get_touch(){
  boolean touch_stat=0;
  touch_stat=digitalRead(TOUCH_SIG);//读入状态
  return touch_stat;
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
int value_theta = 0; 
int value_left = 1;
int value_right = 1;
int flag = 300;
uint16_t pulselen_x=210;
uint16_t pulselen_y=250;
uint16_t pulselen_theta=350;
uint16_t pulselen_z=375;
uint16_t pulselen_hand=300;

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

void setup() { 
 pinMode(12, INPUT_PULLUP); //注意 Z 轴输入一定要上拉，不然电平不稳。
 pinMode(11, INPUT_PULLUP); //注意 Z 轴输入一定要上拉，不然电平不稳。
 Serial.begin(9600); 
 pwm.begin();
  
  irrecv.enableIRIn(); // Start the receiver
  pinMode(TOUCH_SIG,INPUT);       //设置13号端口为输入模式
  
  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64) 

  delay(10);
} 
void loop() { 
  boolean touch_stat;
  // put your main code here, to run repeatedly:
  //Serial.print("\nrunning\nTouch Stat - ");
  touch_stat=get_touch();
  Serial.print(mode_flag);
  if(touch_stat==1)
    {
      test_SSD1306();
      if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    switch(results.value)//通过按键切换模式
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
    irrecv.resume();
  }
    }
   if(touch_stat == 0)
   {
    if(mode_flag == 0)
      {
        welcome_SSD1306();
      }
    if(mode_flag==1)
     {
      mode1_SSD1306();
     }
     if(mode_flag == 2)
     {
      mode2_SSD1306();
     }
     if(mode_flag == 3)
     {
      mode3_SSD1306();
     }
   }
 delay(100); 
} 

void test_SSD1306(void){
    
  //13.显示英文 数字
  display.clearDisplay();   // clears the screen and buffer
  /*display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);*/
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
  Serial.println("good2");
  display.clearDisplay();
  //display.setTextSize(1);
  //display.setTextColor(WHITE);
  //display.setCursor(0,0);
  display.println("Welcome!");
  display.display();
  Serial.println("good3");
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
  display.setCursor(0,0);
  display.println("Mode 3");
  display.setTextSize(2);
  display.println("Remote_Control");
  display.display();
}
