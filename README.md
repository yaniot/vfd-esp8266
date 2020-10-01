# vfd-esp8266
本工程在原作者基础上修改，适应esp8266，按照接线接线即可
屏幕：08-MD-006INKM
主控：esp8266

感谢原作者开源奉献：https : //github.com/hexesdesu/ESP32VFDClock
功能：
一键配网
NTP网络时钟
源码作为学习使用，纯wifi获取到的时钟信息，自己可以添加DS3231时钟模块作为辅助。
clk = 14; // CK
cs = 4; // CS
R st = 1；// RS
en = 3; // EN 0
