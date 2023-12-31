#include <Wire.h>
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
 
static const uint8_t PROGMEM Welcome_16x16[] ={
0//0x00,0x80,0x00,0x80,0xFC,0x80,0x04,0xFC,0x05,0x04,0x49,0x08,0x2A,0x40,0x14,0x40,
/*0x10,0x40,0x28,0xA0,0x24,0xA0,0x45,0x10,0x81,0x10,0x02,0x08,0x04,0x04,0x08,0x02,//欢0
0x00,0x00,0x20,0x80,0x13,0x3C,0x12,0x24,0x02,0x24,0x02,0x24,0xF2,0x24,0x12,0x24,
0x12,0x24,0x12,0xB4,0x13,0x28,0x12,0x20,0x10,0x20,0x28,0x20,0x47,0xFE,0x00,0x00,//迎1
0x01,0x00,0x01,0x00,0x01,0x00,0x7F,0xFC,0x01,0x00,0x11,0x10,0x09,0x10,0x09,0x20,
0xFF,0xFE,0x03,0x80,0x05,0x40,0x09,0x20,0x31,0x18,0xC1,0x06,0x01,0x00,0x01,0x00,//来2
0x00,0x04,0xFF,0x84,0x08,0x04,0x10,0x24,0x22,0x24,0x41,0x24,0xFF,0xA4,0x08,0xA4,
0x08,0x24,0x08,0x24,0x7F,0x24,0x08,0x24,0x08,0x04,0x0F,0x84,0xF8,0x14,0x40,0x08,//到3
0x00,0x00,0x7F,0xFC,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x3F,0xF8,
0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0xFF,0xFE,0x00,0x00,//王4
0x02,0x00,0x02,0x08,0x3F,0xD0,0x02,0x20,0x02,0x40,0xFF,0xFE,0x01,0x00,0x02,0x00,
0x0F,0xF0,0x18,0x10,0x28,0x10,0x4F,0xF0,0x88,0x10,0x08,0x10,0x0F,0xF0,0x08,0x10,//者5
0x08,0x20,0x08,0x20,0xFF,0xFE,0x08,0x20,0x00,0x00,0x7F,0xFE,0x40,0x02,0x81,0x04,
0x01,0x00,0x7F,0xFC,0x03,0x80,0x05,0x40,0x09,0x20,0x31,0x18,0xC1,0x06,0x01,0x00,//荣6
0x10,0x00,0x11,0xDC,0x90,0x44,0x55,0x54,0x58,0xCC,0x11,0x54,0xFC,0x00,0x28,0x48,
0x28,0xFE,0x29,0x90,0x2A,0xFC,0x28,0x90,0x2A,0xFC,0x4C,0x90,0x48,0xFE,0x80,0x80//耀7
*/}; 
 
static const uint8_t PROGMEM Author_16x16[] ={
0x10,0x10,0x08,0x20,0x04,0x40,0x3F,0xF8,0x21,0x08,0x21,0x08,0x3F,0xF8,0x21,0x08,
0x21,0x08,0x3F,0xF8,0x01,0x00,0x01,0x00,0xFF,0xFE,0x01,0x00,0x01,0x00,0x01,0x00,//单0
0x00,0x40,0x10,0x40,0x10,0x40,0x10,0x40,0x10,0x40,0x1F,0xFC,0x10,0x00,0x10,0x00,
0x10,0x00,0x1F,0xE0,0x10,0x20,0x10,0x20,0x10,0x20,0x20,0x20,0x20,0x20,0x40,0x20,//片1
0x10,0x00,0x11,0xF0,0x11,0x10,0x11,0x10,0xFD,0x10,0x11,0x10,0x31,0x10,0x39,0x10,
0x55,0x10,0x55,0x10,0x91,0x10,0x11,0x12,0x11,0x12,0x12,0x12,0x12,0x0E,0x14,0x00,//机2
0x08,0x20,0x08,0x20,0xFF,0xFE,0x08,0x20,0x00,0x10,0x00,0xF8,0x3F,0x00,0x11,0x10,
0x08,0x20,0x01,0x00,0x7F,0xFC,0x05,0x40,0x09,0x20,0x31,0x18,0xC1,0x06,0x01,0x00,//菜3
0x01,0x00,0x02,0x00,0x1F,0xF0,0x10,0x10,0x12,0x10,0x11,0x10,0x11,0x50,0x10,0x20,
0x10,0x00,0x1F,0xFC,0x00,0x04,0x00,0x04,0x7F,0xE4,0x00,0x04,0x00,0x28,0x00,0x10,//鸟4
}; 
 
void setup()   {                
  Serial.begin(115200);
  delay(500);
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64) 
}
 
void loop() {
  test_SSD1306();
}
 
void test_SSD1306(void){
    //1.检测全屏显示(看看有没有大面积坏点)
  /*display.fillScreen(WHITE);
  display.display();
  delay(2000);*/
 /*
  //2.画点 点坐标(10,10)
  display.clearDisplay();   // clears the screen and buffer
  display.drawPixel(10, 10, WHITE);
  display.display();
  delay(2000);
 
  //3. 画线 从(0,0)到(50,50)
  display.clearDisplay();   // clears the screen and buffer
  display.drawLine(0, 0,50,50, WHITE);
  display.display();
  delay(2000);
 
  //4.画空心矩形  左上角坐标(x0,y0)  右下角坐标(x1,y1)
  display.clearDisplay();   // clears the screen and buffer
  display.drawRect(0,0,128,64,WHITE);
  display.display();
  delay(2000);
 
  //5.来画个实心矩形
  display.clearDisplay();   // clears the screen and buffer
  display.fillRect(0,0,64,64,WHITE);
  display.display();
  delay(2000);
 
  //6.画空心圆
  display.clearDisplay();   // clears the screen and buffer
  display.drawCircle(20,20,20,WHITE);
  display.display();
  delay(2000);
  
  //7.画实心圆
  display.clearDisplay();   // clears the screen and buffer
  display.fillCircle(20,20,20,WHITE);
  display.display();
  delay(2000);
 
  //8.画空心三角形
  display.clearDisplay();   // clears the screen and buffer
  display.drawTriangle(20,0,0,20,40,20,WHITE);
  display.display();
  delay(2000);
 
  //9.画实心三角形
  display.clearDisplay();   // clears the screen and buffer
  display.fillTriangle(20,0,0,20,40,20,WHITE);
  display.display();
  delay(2000);
 
  //10.画空心圆角矩形
  display.clearDisplay();   // clears the screen and buffer
  display.drawRoundRect(0,0,40,40,5,WHITE);
  display.display();
  delay(2000);
 
  //11.画实心圆角矩形
  display.clearDisplay();   // clears the screen and buffer
  display.fillRoundRect(0,0,40,40,5,WHITE);
  display.display();
  delay(2000);
 
  //12.画心形(楼主自己用取模软件画的)
  display.clearDisplay();   // clears the screen and buffer
  display.drawBitmap(16,16,Heart_16x16,16,16,WHITE);
  display.display();
  delay(2000);
 */
  //13.显示英文 数字
  display.clearDisplay();   // clears the screen and buffer
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Hello, Arduino!");
  display.setTextColor(BLACK, WHITE); // 'inverted' text
  display.println(3.141592);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.print("0x"); display.println(0xDEADBEEF, HEX);
  display.display();
  delay(2000);
 /*
  //14.显示单个文字 有木有发现就是调用drawBitmap
  display.clearDisplay();   // clears the screen and buffer
  display.drawBitmap(16,16,Strong_16x16,16,16,WHITE);
  display.display();
  delay(2000);*/
  
}
