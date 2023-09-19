#include <IRremote.h>
#include <Wire.h>
#include <string.h>
#include <Adafruit_PWMServoDriver.h>
int RECV_PIN = 10;

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
// Depending on your servo make, the pulse width min and max may vary, you 
// want these to be as small/large as possible without hitting the hard stop
// for max range. You'll have to tweak them as necessary to match the servos you
// have!
#define SERVOMIN  150 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // this is the 'maximum' pulse length count (out of 4096)
boolean stat[12]={0,0,0,0,0,0,0,0,0,0,0,0};//引入状态变量数组，通过遥控决定是维持状态还是更改状态
uint16_t pulselen_x=210;
uint16_t pulselen_y=250;
uint16_t pulselen_z=140;
uint16_t pulselen_z1=130;
uint16_t pulselen_z2=300;
uint16_t pulselen_z3=140;
uint16_t pulselen_z4=140;
IRrecv irrecv(RECV_PIN);
decode_results results;

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
    switch(results.value){
      case 0xFF18E7://上的键值，两个x轴向y轴正方向转动
        stat[0]=1;
        break;
      case 0xFF4AB5://下的键值，两个x轴向y轴反方向转动
        stat[1]=1;
        break;
      case 0xFF10EF://左的键值，y轴向内转动
        stat[2]=1;
        break;
      case 0xFF5AA5://右的键值，y轴向外转动
        stat[3]=1;
        break;
      case 0xFF9867://0的键值,z轴向下转动
        stat[4]=1;
        break;  
      case 0xFFA857://8的键值,z轴向上转动
        stat[5]=1;
        break;
      case 0xFFA25D://1的键值,z1轴顺时针转动，y轴向自己伸展
        stat[6]=1;
        break;  
      case 0xFFE21D://3的键值,z1轴逆时针转动
        stat[7]=1;
        break;
      case 0xFF22DD://4的键值,z2轴时钟型顺时针向上抬动
        stat[8]=1;
        break;  
      case 0xFFC23D://6的键值,z2轴时钟型逆时针向上抬动
        stat[9]=1;
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
        stat[10]=1;
        break;  
      case 0xFFB04F://#的键值,爪子张大
        stat[11]=1;
        break;
      case 0xFFFFFFFF:
        break;
      default: 
        memset(stat,0,sizeof(stat));
        Serial.println("good");
    }
    irrecv.resume();
    if(stat[0]==1)
    {
      pulselen_x+=10;
      if(pulselen_x>=440) pulselen_x=440;
      if(pulselen_x<=210) pulselen_x=210;
      pwm.setPWM(0, 0, pulselen_x);
      pwm.setPWM(1, 0, pulselen_x);
    }
    if(stat[1]==1)
    {
      pulselen_x-=10;
        if(pulselen_x>=440) pulselen_x=440;
        if(pulselen_x<=210) pulselen_x=210;
        pwm.setPWM(0, 0, pulselen_x);
        pwm.setPWM(1, 0, pulselen_x);
    }
    if(stat[2]==1)
    {
      pulselen_y+=10;
        if(pulselen_y>=570) pulselen_y=570;
        if(pulselen_y<=250) pulselen_y=250;
        pwm.setPWM(2, 0, pulselen_y);
    }
    if(stat[3]==1)
    {
      pulselen_y-=10;
        if(pulselen_y>=570) pulselen_y=570;
        if(pulselen_y<=250) pulselen_y=250;
        pwm.setPWM(2, 0, pulselen_y);
    }
    if(stat[4]==1)
    {
      pulselen_z+=10;
        if(pulselen_z>=460) pulselen_z=460;
        if(pulselen_z<=140) pulselen_z=140;
        pwm.setPWM(3, 0, pulselen_z);
        //Serial.print("键0:");
        //Serial.println(pulselen_z);
    }
    if(stat[5]==1)
    {
      pulselen_z-=10;
        if(pulselen_z>=460) pulselen_z=460;
        if(pulselen_z<=140) pulselen_z=140;
        pwm.setPWM(3, 0, pulselen_z);
        //Serial.print("键8:");
        //Serial.println(pulselen_z);
    }
    if(stat[6]==1)
    {
      pulselen_z1+=10;
        if(pulselen_z1>=590) pulselen_z1=590;
        if(pulselen_z1<=130) pulselen_z1=130;
        pwm.setPWM(4, 0, pulselen_z1);
    }
    if(stat[7]==1)
    {
      pulselen_z1-=10;
        if(pulselen_z1>=590) pulselen_z1=590;
        if(pulselen_z1<=130) pulselen_z1=130;
        pwm.setPWM(4, 0, pulselen_z1);
    }
    if(stat[8]==1)
    {
      pulselen_z2-=10;
        if(pulselen_z2>=600) pulselen_z2=600;
        if(pulselen_z2<=100) pulselen_z2=100;
        pwm.setPWM(5, 0, pulselen_z2);
        //Serial.print("键4:");
        //Serial.println(pulselen_z2);
    }
    if(stat[9]==1)
    {
      pulselen_z2+=10;
        if(pulselen_z2>=600) pulselen_z2=600;
        if(pulselen_z2<=100) pulselen_z2=100;
        pwm.setPWM(5, 0, pulselen_z2);
        //Serial.print("键6:");
        //Serial.println(pulselen_z2);
    }
    if(stat[10]==1)
    {
      pulselen_z4+=10;
        if(pulselen_z4>=360) pulselen_z4=360;
        if(pulselen_z4<=180) pulselen_z4=180;
        pwm.setPWM(7, 0, pulselen_z4);
        //Serial.print("键*:");
        //Serial.println(pulselen_z4);
    }
    if(stat[11]==1)
    {
      pulselen_z4-=10;
        if(pulselen_z4>=360) pulselen_z4=360;
        if(pulselen_z4<=180) pulselen_z4=180;
        pwm.setPWM(7, 0, pulselen_z4);
        //Serial.print("键#:");
        //Serial.println(pulselen_z4);
    }
  }
  delay(100);
}
