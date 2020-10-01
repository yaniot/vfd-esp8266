//本工程在原作者基础上修改，适配esp8266，按照引脚接线即可。
//屏幕：08-MD-006INKM
//主控：esp8266
//修改：@颜七岁
//感谢原作者开源奉献：https://github.com/hexesdesu/ESP32VFDClock
//源码作为学习使用，纯wifi获取到的时钟信息，自己可以添加DS3231时钟模块作为辅助。
//开源作品。禁止商业！！！
//esp8266引脚接线，本工程主要为GPIO口为主（引脚图见附件）：
//vfd屏幕  //esp8266引脚
//din   = 12; // DA
//clk   = 14; // CK
//cs    = 4; // CS
//R   st = 1; // RS
//en    = 3; // EN 0

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>  
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>               // NTPClient库文件
#include <TimeLib.h>                 // Arduino时间库


#define NTP_OFFSET 8 * 3600 // + 8h，北京时间
#define NTP_INTERVAL 8* 60 * 60 * 1000 // 12 * 60 * 60 * 1000 12小时更新
#define NTP_ADDRESS  "cn.ntp.org.cn"




WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS,NTP_OFFSET,NTP_INTERVAL);
// 'cn.ntp.org.cn' is used (default server) with +1 hour offset (3600 seconds) 60 seconds (60000 milliseconds) update interval


//适用于esp8266的引脚
uint8_t din   = 12; // DA
uint8_t clk   = 14; // CK
uint8_t cs    = 4; // CS
uint8_t Reset = 1; // RS
uint8_t en    = 3; // EN 0

char *str_time = "00:00:00";
String format_time = "00:00:00";




//vfd时钟驱动部分
void write_6302(unsigned char w_data)
{
  unsigned char i;
  for (i = 0; i < 8; i++)
  {
    digitalWrite(clk, LOW);
    if ( (w_data & 0x01) == 0x01)
    {
      digitalWrite(din, HIGH);
    }
    else
    {
      digitalWrite(din, LOW);
    }
    w_data >>= 1;
    digitalWrite(clk, HIGH);
  }
}

void VFD_cmd(unsigned char command)
{
  digitalWrite(cs, LOW);
  write_6302(command);
  digitalWrite(cs, HIGH);
  delayMicroseconds(5);
}

void S1201_show(void)
{
  digitalWrite(cs, LOW);//开始传输
  write_6302(0xe8);     //地址寄存器起始位置
  digitalWrite(cs, HIGH); //停止传输

}

void VFD_init()
{
  //SET HOW MANY digtal numbers
  digitalWrite(cs, LOW);
  write_6302(0xe0);
  delayMicroseconds(5);
  write_6302(0x07);//8 digtal
  digitalWrite(cs, HIGH);
  delayMicroseconds(5);

  //set bright
  digitalWrite(cs, LOW);
  write_6302(0xe4);
  delayMicroseconds(5);
  
  //write_6302(0x33);//esp32用这个
  write_6302(0xff);//8266用这个
  digitalWrite(cs, HIGH);
  delayMicroseconds(5);

}

/******************************
  在指定位置打印一个字符(用户自定义,所有CG-ROM中的)
  x:0~11;chr:要显示的字符编码
*******************************/
void S1201_WriteOneChar(unsigned char x, unsigned char chr)
{
  digitalWrite(cs, LOW);  //开始传输
  write_6302(0x20 + x); //地址寄存器起始位置
  write_6302(chr + 0x30);
  digitalWrite(cs, HIGH); //停止传输
  S1201_show();

}
/******************************
  在指定位置打印字符串
  (仅适用于英文,标点,数字)
  x:0~11;str:要显示的字符串
*******************************/
void S1201_WriteStr(unsigned char x, char *str)
{
  digitalWrite(cs, LOW);  //开始传输
  write_6302(0x20 + x); //地址寄存器起始位置
  while (*str)
  {
    write_6302(*str); //ascii与对应字符表转换
    str++;
  }
  digitalWrite(cs, HIGH); //停止传输
  S1201_show();

}
//vfd驱动部分结束


void setup(){
  WiFiManager wifiManager;
   Serial.begin(115200);  
    wifiManager.autoConnect("VFDClock");
    // wifiManager.autoConnect("AutoConnectAP", "12345678");

    Serial.println(""); 
    Serial.print("ESP8266 Connected to ");
    Serial.println(WiFi.SSID());              // WiFi名称
    Serial.print("IP address:\t");
    Serial.println(WiFi.localIP());           // IP


    
    //vfd引脚
  pinMode(en, OUTPUT);
  pinMode(clk, OUTPUT);
  pinMode(din, OUTPUT);
  pinMode(cs, OUTPUT);
  pinMode(Reset, OUTPUT);
  digitalWrite(en, HIGH);
  digitalWrite(Reset, LOW);
  delayMicroseconds(5);
  digitalWrite(Reset, HIGH);
  VFD_init(); 
//vfd引脚结束
 


  S1201_WriteStr(0, "        ");
  delay(200);
  S1201_WriteStr(0, "       W");
  delay(200);
  S1201_WriteStr(0, "      We");
  delay(200);
  S1201_WriteStr(0, "     Wel");
  delay(200);
  S1201_WriteStr(0, "    Welc");
  delay(200);
  S1201_WriteStr(0, "   Welco");
  delay(200);
  S1201_WriteStr(0, "  Welcom");
  delay(200);
  S1201_WriteStr(0, " Welcome");
  delay(200);
  S1201_WriteStr(0, "Welcome ");
  delay(200);
  S1201_WriteStr(0, "Welcome ");
  delay(200);
  S1201_WriteStr(0, "Welcome ");
  delay(200);
  S1201_WriteStr(0, "elcome ");
  delay(200);
  S1201_WriteStr(0, "lcome  ");
  delay(200);
  S1201_WriteStr(0, "come   ");
  delay(200);
  S1201_WriteStr(0, "ome    ");
  delay(200);
  S1201_WriteStr(0, "me     ");
  delay(200);
  S1201_WriteStr(0, "e      ");
  delay(200);
  S1201_WriteStr(0, "        ");
  delay(200);

      timeClient.begin();
    // 建立WiFiManager对象
    

    


}


void clock_loop() {
  static uint32_t time_out = 0; // 刷新时间控制
  uint32_t now_mill = millis(); // 当前的系统时间
  
  // 如果系统计时器溢出
  if(now_mill >= time_out) 
  {
    // 设定下次刷新的时间点（1s以后再次刷新）
    time_out = now_mill + 1000;
    // 更新TNP时间
    timeClient.update();

    
    
    // 显示时间
  format_time = timeClient.getFormattedTime();
  char *str_time = &format_time[0]; 
  S1201_WriteStr(0, str_time);
  
  }
  delay(1000);
}


void loop()
{
 clock_loop();
}
