//include
#ifndef INCLUDE
#define INCLUDE


#include "FS.h"
#include <ESP8266WiFi.h>        // ������ʹ�� ESP8266WiFi��
//#include <ESP8266WiFiMulti.h>   //  ESP8266WiFiMulti��
#include <ESP8266WebServer.h>   //  ESP8266WebServer��
#include <HttpClient.h>//HTTP��
#include <DNSServer.h>
#include <ArduinoJson.h>        //  ArduinoJson��
#include <Ticker.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "NotoSansBold15.h"
#include "NotoSansBold36.h"
#include "font_zh.h"
#include <ESP8266_Seniverse.h>
#include "ESP8266_BiliBili.h"
#include "buttonRect.h"
#include <JPEGDecoder.h>
#include <SD.h>

//#include <String.h>

//define

//#define POWERPIN D3

#define AP_NAME             "DeskAssistant"
#define WIFI_DATA           "/system/wifi/wifi.txt"
#define CALIBRATION_FILE    "/system/touch/TouchCalData1"
#define LOCATION_DATA		"/system/location/location.txt"
#define THEME_DATA			"/system/theme/themedata.txt"
#define BILI_DATA			"/system/bilidata/UID.txt"
#define HOME_bgpath "/system/theme/home_picture.txt"
//*******************************************MODE*******************************************

#define HOME        1
#define CONNECT     2
#define CONNECTING  3
#define INPUT       4
#define CALCULATOR  5
#define SETTING     6
#define PHOTO       7
#define CALENDAR    8
#define SCHEDULE    9
#define MORE        10
#define WEATHER     11
#define LOCATION    12
#define BILIBILI    13
#define FILEMANAGE  14
#define THEME		15
#define ABOUT		16
#define UPLOAD		17
#define CLOCK		18

#define MAX_menu_page  2

#define DISP_X 1
#define DISP_Y 10
#define DISP_W 238
#define DISP_H 50
#define DISP_TSIZE 3
#define DISP_TCOLOR TFT_CYAN
//*****************************************SHOW_LOCATION********************************
#define showtime_x			99
#define showtime_y			40
#define showtime_size		30

#define showdate_x			107
#define showdate_y			73
#define showdate_size		15

#define showweather_x		50
#define showweather_y		50
#define showweather_size	20

#define return_x			22
#define return_y			22
#define return_size			15
//*********************************************button***************************************
#define Weatherb			1
#define Returnb				2
#define Locationb			3
#define SHANTOUb			4
#define ZHUHAIb				5
#define GUANGZHOUb			6
#define SHENZHENb			7
//claim
void Init();
void Init_ticker();
bool connectwifi(int);
bool CheckWiFi(int);
void CheckWiFiBool();
bool CheckWiFi(int);
bool connectwifi(int);
void initSoftAP(void);
void touch_calibrate();
void lcd_display_chinese(uint16_t, uint16_t, uint8_t, const uint8_t*, uint16_t, int32_t, int);
void handleSetAP();
bool BiliID();
int Updata_weather();
bool display_time(int x, int y, int size, bool refresh=0);
void display_date(int x, int y, int size); 
void display_weather(int x, int y, int size, int code,bool s);
void wifi_label(int x, int y, int size, uint16_t color);
void error_label(int x, int y, int size, uint16_t color);
void connect_error_label(int x, int y, int size, uint16_t color);
void return_label(int x, int y, int size, uint16_t color);
void display_week(int x, int y, int size);
void End_Start();
void draw_moremenu();
void DrawRect(int sx, int sy, int ex, int ey, uint16_t color = TFT_GREEN);
void DrawRect(struct ButtonRect button, uint16_t color = TFT_GREEN);
//ticker
Ticker LEDflashTicker;
Ticker WiFiconnectTicker;
Ticker bilibiliTicker;
Ticker start_Ticker;
Ticker updata_weatherTicker;


//object
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite menu_scroll = TFT_eSprite(&tft);

ESP8266WebServer connect_server(80);//AP����
ESP8266WebServer photo_upload_server(80);
DNSServer dnsServer;//����dnsServerʵ��
/*
IPAddress local_IP(192, 168, 0, 123); // ����ESP8266-NodeMCU�������IP
IPAddress gateway(192, 168, 0, 1);    // ��������IP��ͨ������IP��WiFI·��IP��
IPAddress subnet(255, 255, 255, 0);   // ������������
IPAddress dns(192,168,0,1);           // ���þ�����DNS��IP��ͨ��������DNS��IP��WiFI·��IP��
*/

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "ntp.sjtu.edu.cn", 3600, 60000);//NTP��������ַ
WeatherNow weatherNow;
Forecast forecast;

//location
#define SHANTOU			0
#define ZHUHAI			4
#define GUANGZHOU		8
#define SHENZHEN		12
extern uint8_t select_location[16][16] =
{
	{0x00,0x40,0x20,0x40,0x10,0x40,0x10,0x40,0x84,0x44,0x44,0x44,0x44,0x44,0x14,0x44},
	{0x14,0x44,0x24,0x44,0xE4,0x44,0x24,0x44,0x24,0x44,0x27,0xFC,0x20,0x04,0x00,0x00},/*"��",0*/

	{0x00,0x80,0x00,0x80,0x08,0x80,0x04,0x80,0x24,0x80,0x10,0x80,0x10,0x80,0x00,0x80},
	{0xFF,0xFE,0x01,0x00,0x01,0x40,0x02,0x20,0x04,0x10,0x08,0x08,0x30,0x04,0xC0,0x04},/*"ͷ",1*/

	{0x00,0x20,0x01,0x20,0xFD,0x20,0x11,0xFC,0x11,0x20,0x12,0x20,0x7C,0x20,0x13,0xFE},
	{0x10,0x70,0x10,0xA8,0x10,0xA8,0x1D,0x24,0xE1,0x24,0x42,0x22,0x00,0x20,0x00,0x20},/*"��",2*/

	{0x01,0x00,0x21,0x00,0x11,0xFC,0x12,0x00,0x85,0xF8,0x41,0x08,0x49,0x48,0x09,0x28},
	{0x17,0xFE,0x11,0x08,0xE2,0x48,0x22,0x28,0x23,0xFC,0x20,0x08,0x20,0x50,0x00,0x20},/*"��",3*/

	{0x01,0x00,0x00,0x80,0x00,0x80,0x3F,0xFC,0x20,0x00,0x20,0x00,0x20,0x00,0x20,0x00},
	{0x20,0x00,0x20,0x00,0x20,0x00,0x20,0x00,0x20,0x00,0x40,0x00,0x40,0x00,0x80,0x00},/*"��",4*/

	{0x10,0x04,0x10,0x84,0x10,0x84,0x10,0x84,0x10,0x84,0x54,0xA4,0x52,0x94,0x52,0x94},
	{0x90,0x84,0x10,0x84,0x10,0x84,0x10,0x84,0x20,0x84,0x20,0x84,0x40,0x04,0x80,0x04},/*"��",5*/

	{0x00,0x00,0x27,0xFC,0x14,0x04,0x14,0xA4,0x81,0x10,0x42,0x08,0x40,0x40,0x10,0x40},
	{0x17,0xFC,0x20,0x40,0xE0,0xE0,0x21,0x50,0x22,0x48,0x2C,0x46,0x20,0x40,0x00,0x40},/*"��",6*/

	{0x11,0x04,0x11,0x24,0x11,0x24,0x11,0x24,0x11,0x24,0xFD,0x24,0x11,0x24,0x11,0x24},
	{0x11,0x24,0x11,0x24,0x11,0x24,0x1D,0x24,0xE1,0x24,0x42,0x24,0x02,0x04,0x04,0x04}/*"��",7*/

};





#endif // !INCLUDE
