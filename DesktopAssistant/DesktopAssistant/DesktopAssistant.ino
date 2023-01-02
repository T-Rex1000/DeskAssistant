#include "included.h"

uint8_t RUN_MODE=1;
uint8_t RUN_MODE_OLD=1;
uint8_t RUN_MODE_LAST=1;

unsigned long press_time=0,release_time=0;
uint16_t ftx=0,fty=0;
uint16_t ltx=0, lty=0;

unsigned int Click_time=130;

uint8_t first_display=0;
uint8_t home_display=0;

bool updata_weather=1;
bool bilibili_updata=1;

bool Start_time=1;
bool Start_weather=1;
bool first_show_weather=1;
uint8_t showed=1;

//bool fresh_time=1;
uint8_t Start=10;

char sta_ssid[32] = {0};
char sta_password[64] = {0};
String ssid;
String password;


uint32_t targetTime;
int ss=0,mm=0,hh=0;
int day=0;
float sx = 0, sy = 1, mx = 1, my = 0, hx = -1, hy = 0;    //H, M, S x & y multipliers
float sdeg=0, mdeg=0, hdeg=0;
uint16_t osx=120, osy=120, omx=120, omy=120, ohx=120, ohy=120;  //H, M, S x & y coords
uint16_t x0=0, x1=0, yy0=0, yy1=0;
bool initial = 1;


int weather_code;
const char* weatherhost="api.seniverse.com";//心知天气服务器地址
String now_address="",now_time="",now_temperature="";//用来存储报文得到的字符串
String reqUserKey = "SWYPj9sCERdkMT9ow";   // 私钥
String weather_location ="zhuhai";         // 城市
String reqUnit = "c";                      // 摄氏/华氏
uint8_t forecast_code[3],forecast_high[3],forecast_low[3];
String forecast_upday;
int location=0;


String biliID="476974306";


String HOME_background;
String FILE_JPEG[10];
File fsUploadFile;

void setup() 
{
    int color[6]={TFT_RED,TFT_ORANGE,TFT_YELLOW,TFT_GREEN,TFT_BLUE,TFT_BLACK};
    int i=0,j=0;
    Init();
    //SPIFFS.format();
    Serial.println(WiFi.SSID());
    Serial.println(WiFi.localIP());
    touch_calibrate();
    tft.fillScreen(TFT_BLACK);
    timeClient.update();
    hh=timeClient.getHours();
    mm=timeClient.getMinutes();
    ss=timeClient.getSeconds();
    bool update_bool=1;
    /*
    for(j=0;j<6;j++)
    {
      for(i=1;i<11;i++)
      {
          tft.fillCircle(120, 150, 100-j*10+i, color[j]);
          timeClient.update();
          if(update_bool&&display_time(showtime_x,showtime_y,showtime_size))
          {
              update_bool=0;
              display_week(showdate_x,showdate_y,showdate_size);
          }
          delay(1);
      }
    }
    for(i=0;i<150;i++)
    {
        timeClient.update();
        if(update_bool&&display_time(showtime_x,showtime_y,0))
        {
            update_bool=0;
            display_week(showdate_x,showdate_y,showdate_size);
        }
        delay(5);
    }
    */
    Init_ticker();
    while (j<40&&WiFi.status() != WL_CONNECTED) 
    { 
        for(i=1;i<11;i++)
        {
            if(j<6)
            {
                tft.fillCircle(120, 150, 100-j*10+i, color[j]);
            }
            delay(10);
        }        
        j++;                      
    }
    tft.fillScreen(TFT_BLACK);
    if((first_display==HOME))
    {
        //tft.fillScreen(TFT_BLACK);
        tft.loadFont(NotoSansBold36);
        tft.fillRect(showtime_x,showtime_y,showtime_size*3.1,showtime_size,TFT_BLACK);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.drawString(" - -:- -",showtime_x,showtime_y);
        tft.unloadFont();
    }
}

#define input_number    1
#define input_abc       2
#define input_ABC       3
#define input_symbol    4

bool analyse_touch_input(int sx,int sy,int ex,int ey)
{
    if ((ltx > sx && ltx<ex && lty>sy && lty < ey))
    {
        return true;
    }
    return false;
}


//char number_c[10] = { 0,1,2,3,4,5,6,7,8,9 };
String input_method()
{
    String str;
    uint8_t shift= input_number; 
    int input_x=10,input_y=35;
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(input_x, input_y, 4);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(1);
    bool pressed = 0;
    bool oldpressed = 0;
    uint16_t tx,ty;
    int x = 4, y=120;
    int input_line_time=0;
    for (int i = 0; i < 4; i++)
    {
        x=4;
        for (int j = 0; j < 4; j++)
        {
            tft.fillRect(x, y, 55, 45,tft.color565(127,127,127));
            if (i < 3 && j < 3)
            {
                tft.setTextColor(TFT_WHITE, tft.color565(127,127,127));
                tft.drawNumber(i*3+j+1,x+20,y+10, 4);
                //Serial.print("!");
            }
            x += 59;
        }
        y+=50;
    }
    tft.setTextColor(TFT_WHITE, tft.color565(127,127,127));
    tft.drawChar(5,5,'<',TFT_WHITE,TFT_BLACK,2);
    tft.drawNumber(0,63+20,270+10, 4);
    uint8_t backspace_label[2][16]=
    {
      {0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xFE,0x08,0x02,0x11,0x12,0x20,0xA2,0x40,0x42},
      {0x20,0xA2,0x11,0x12,0x08,0x02,0x07,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}
    },
    enter_label[2][16]=
    {
      {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x04,0x00,0x04},
      {0x00,0x04,0x10,0x04,0x20,0x04,0x7F,0xFC,0x20,0x00,0x10,0x00,0x00,0x00,0x00,0x00}
    },
    space_label[2][16]=
    {
      {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
      {0x00,0x00,0x40,0x02,0x40,0x02,0x7F,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}
    };
    lcd_display_chinese(193, 127, 32,backspace_label[0],TFT_WHITE,-1,1);
    lcd_display_chinese(193, 277, 32,enter_label[0],TFT_WHITE,-1,1);
    lcd_display_chinese(133, 277, 32,space_label[0],TFT_WHITE,-1,1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    while (1)
    {
        input_x=tft.getCursorX();
        if(input_line_time<15)
        {
          input_line_time++;
          tft.drawLine(input_x,input_y,input_x,input_y+20,TFT_WHITE);
        }
        else if(input_line_time<30)
        {
          input_line_time++;
          tft.drawLine(input_x,input_y,input_x,input_y+20,TFT_BLACK);
        }
        else
        {
          input_line_time=0;
        }
        if (oldpressed)
        {
            ltx = tx;
            lty = ty;
        }
        pressed = tft.getTouch(&tx, &ty);
        if (shift == input_number)
        {
            if (!pressed && oldpressed)
            {
                tft.drawLine(input_x,input_y,input_x,input_y+20,TFT_BLACK);
                //tft.fillRect(10,15,219,40,TFT_GREEN); 
                if(str.length()<50)
                {
                    if (analyse_touch_input(4, 120, 59, 165))//1
                    {
                        str += "1";
                    }
                    else if (analyse_touch_input(63, 120, 118, 165))//2
                    {
                        str += "2";
                    }
                    else if (analyse_touch_input(122, 120, 177, 165))//3
                    {
                        str += "3";
                    }
                    else if (analyse_touch_input(4, 170, 59, 215))//4
                    {
                        str += "4";
                    }
                    else if (analyse_touch_input(63, 170, 118, 215))//5
                    {
                        str += "5";
                    }
                    else if (analyse_touch_input(122, 170, 177, 215))//6
                    {
                        str += "6";
                    }
                    else if (analyse_touch_input(4, 220, 59, 265))//7
                    {
                        str += "7";
                    }
                    else if (analyse_touch_input(63, 220, 118, 265))//8
                    {
                        str += "8";
                    }
                    else if (analyse_touch_input(122, 220, 177, 265))//9
                    {
                        str += "9";
                    }
                    else if (analyse_touch_input(63, 270, 118, 315))//0
                    {
                        str += "0";
                    }
                    else if (analyse_touch_input(122, 270, 177, 315))//spade
                    {
                        str += " ";
                    }
                }
                if (analyse_touch_input(181, 120, 236, 165))//back
                {
                    str = str.substring(0, str.length() - 1);
                    tft.fillRect(input_x-30,input_y,32,30,TFT_BLACK);
                }
                else if (analyse_touch_input(181, 170, 236, 215))//abc
                {
                    //shift = input_abc;
                }
                else if (analyse_touch_input(181, 220, 236, 265))//ABC
                {
                    //shift = input_ABC;
                }
                else if (analyse_touch_input(4, 270, 59, 315))//ABC
                {
                    //shift = input_symbol;
                }
                else if (analyse_touch_input(181, 270, 236, 315))//Enter
                {
                    tx = ty = ltx = lty = 0;
                    return str;
                }
                else if (analyse_touch_input(0, 0, 50, 40))
                {
                    tx = ty = ltx = lty = 0;
                    str.clear();
                    return str;
                }
                tft.setCursor(10, 35, 4);
                tft.print(str);
                input_x=tft.getCursorX();
                input_y=tft.getCursorY();
            }
        }
        oldpressed = pressed;
    }
}
//uint16_t ltx = 0, lty = 0;
#define scroll_distancy 30
bool analyse_touch(struct ButtonRect button,int gesture)
{
    //Serial.println(press_time);
    //Serial.println(release_time);
    if(release_time>15)
    {
      if(ftx>button.sx&&ftx<button.ex&&fty>button.sy&&fty<button.ey)
      {
          if((ltx>button.sx&&ltx<button.ex&&lty>button.sy&&lty<button.ey)&&(abs(ltx-ftx)<10&&abs(lty-fty)<10))
          {
                  Serial.println("press_time:"+String(press_time));
              if(press_time<10)
              {
                  if(gesture==CLICK)
                  {
                      ltx=0;
                      lty=0;
                      press_time=0;
                      return true;
                  }
                  return false;
              }
              else
              {   
                  if(gesture==LONG)
                  {
                      ltx=0;
                      lty=0;
                      press_time=0;
                      return true;
                  }
                  return false;
              }
          }
          else
          {
              int condition=SLIP;;
              if((int)(ltx-ftx)>scroll_distancy&&abs((int)(fty-lty))<scroll_distancy)
              {
                  
                  Serial.println("condition:"+String(condition));
                  if(gesture==RIGHT)
                  {
                      ltx=0;
                      lty=0;
                      press_time=0;
                      return true;
                  }
                  return false;
              }
              else if((int)(ftx-ltx)>scroll_distancy&&abs((int)(fty-lty))<scroll_distancy)
              {
                  if(gesture==LEFT)
                  {
                      ltx=0;
                      lty=0;
                      press_time=0;
                      return true;
                  }
                  return false;
              }
              else if((int)(lty-fty)>scroll_distancy&&abs((int)(ftx-ltx))<scroll_distancy)
              {
                  if(gesture==DOWN)
                  {
                      ltx=0;
                      lty=0;
                      press_time=0;
                      return true;
                  }
                  return false;
              }
              else if((int)(fty-lty)>scroll_distancy&&abs((int)(ftx-ltx))<scroll_distancy)
              {
                  if(gesture==UP)
                  {
                      ltx=0;
                      lty=0;
                      press_time=0;
                      return true;
                  }
                  return false;
              }
              
              if(gesture==SLIP)
              {
                  ltx=0;
                  lty=0;
                  press_time=0;
                  return true;
              }
              return false;
          }
      }
      press_time=0;
    }
    return false;
}

int analyse_touch(int sx,int sy,int ex,int ey)
{
    //Serial.println(press_time);
    //Serial.println(release_time);
    if(release_time>15)
    {
      if(ftx>sx&&ftx<ex&&fty>sy&&fty<ey)
      {
          Serial.println("("+String(abs(ltx-ftx))+","+String(abs(lty-fty))+")");
          if((ltx>sx&&ltx<ex&&lty>sy&&lty<ey)&&(abs(ltx-ftx)<10&&abs(lty-fty)<10))
          {
              if(press_time<10)
              {
                  ltx=0;
                  lty=0;
                  press_time=0;
                  return CLICK;
              }
              else
              {
                  ltx=0;
                  lty=0;
                  press_time=0;
                  return LONG;
              }
          }
          else
          {
              press_time=0;
              int condition=SLIP;;
              if((int)(ltx-ftx)>scroll_distancy&&abs((int)(fty-lty))<scroll_distancy)
              {
                  condition= RIGHT;
              }
              else if((int)(ftx-ltx)>scroll_distancy&&abs((int)(fty-lty))<scroll_distancy)
              {
                  condition= LEFT;
              }
              else if((int)(lty-fty)>scroll_distancy&&abs((int)(ftx-ltx))<scroll_distancy)
              {
                  condition= DOWN;
              }
              else if((int)(fty-lty)>scroll_distancy&&abs((int)(ftx-ltx))<scroll_distancy)
              {
                  condition= UP;
              }
              Serial.println("condition:"+String(condition));
              return condition;
          }
      }
      press_time=0;
    }
    return 0;
}
bool device_mode=1;
uint8_t menu_page=1;
void loop()
{   
    //if(shift==0&&checkwifi&&!CheckWiFi()&&!connectwifi()) shift=1;//>>
    uint16_t tx = 0, ty = 0;
    bool pressed=0;
    static bool oldpressed=0;
    RUN_MODE_OLD=RUN_MODE;
    //timeClient.update();
    pressed=tft.getTouch(&tx, &ty);
    if(pressed)
    {
      Serial.println(String(tx)+","+String(ty));
    }
    if(pressed)
    {
        oldpressed=pressed;
        release_time=0;
        if(!press_time)
        {
            ftx=tx;
            fty=ty;
        }
        press_time++;
        ltx=tx;
        lty=ty;
    }
    else if(press_time)
    {
        release_time++;
        if(release_time>50)
        {
          release_time=0;
          press_time=0;
        }
        //if(release_time>1) oldpressed=pressed;
    }
    else
    {
        release_time=0;
    }
    switch(RUN_MODE)
    {
        case HOME:
        {
            if(display_time(showtime_x,showtime_y,showtime_size,showed))
                display_week(showdate_x,showdate_y,showdate_size);
            if(updata_weather||Start_weather)
            {
                weather_code=Updata_weather();
                if(first_show_weather) display_date(showdate_x,showdate_y,showdate_size);
                if(weather_code>=0) display_date(showdate_x,showdate_y,showdate_size);
                display_weather(showweather_x,showweather_y,showweather_size,weather_code,1);
            }
            if(home_display==PHOTO)
            {
                if(showed)
                {
                    showed=0;
                    drawJpeg(HOME_background.c_str(),HOME_photo.sx , HOME_photo.sy);
                }
                if(!pressed&&analyse_touch(HOME_photo,CLICK))
                {
                    RUN_MODE=PHOTO;
                    break;
                }
            }
            else if(home_display==BILIBILI)
            {
                if(pressed||(release_time>0&&release_time<50))bilibili_updata=0;
                if(bilibili_updata)
                {
                    bilibili_updata=0;
                    BiliID();
                }
                if(!pressed&&analyse_touch(HOME_photo,CLICK))
                {
                    RUN_MODE=BILIBILI;
                    break;
                }
                if(showed)
                {
                    showed=0;
                    drawJpeg("/system/theme/home_bilibili.jpg",HOME_photo.sx , HOME_photo.sy);
                }
            }
            RUN_MODE_LAST=RUN_MODE;
            if(!pressed&&analyse_touch(screen,LEFT))
            {
                menu_page=1;
                RUN_MODE=MORE;
                break;
            }
            if(!pressed&&analyse_touch(HOME_weather,CLICK))
            {
                RUN_MODE=WEATHER;
                break;
            }
            if(!pressed&&analyse_touch(100,40,190,90)==CLICK)
            {
                RUN_MODE=CLOCK;
                break;
            }
            if(!pressed&&analyse_touch(190,40,239,90)==CLICK)
            {
                RUN_MODE=CALENDAR;
                break;
            }
        }
        break;
        case PHOTO:
        {
            static uint8_t photo_show=0;
            static bool delecting=0;
            if(!pressed&&analyse_touch(screen,RIGHT))
            {
                if(delecting) 
                {
                    delecting=0;
                    tft.fillScreen(TFT_BLACK);
                    showed=1;
                }
                else
                {
                    if(RUN_MODE==RUN_MODE_LAST)RUN_MODE=HOME;
                    else RUN_MODE=RUN_MODE_LAST;
                }
                break;
            }
            if(!pressed&&analyse_touch(screen,UP))
            {
                if(!FILE_JPEG[photo_show+1].equals("end")) 
                {
                  photo_show++;
                  tft.fillScreen(TFT_BLACK);
                }
                showed=1;
                break;
            }
            if(!pressed&&analyse_touch(screen,DOWN))
            {
                if(photo_show) 
                {
                  photo_show--;
                  tft.fillScreen(TFT_BLACK);
                }
                
                showed=1;
                break;
            }
            if(!pressed&&analyse_touch(screen,LONG))
            {
                File photo_data=SPIFFS.open(HOME_bgpath,"w");
                photo_data.println(FILE_JPEG[photo_show]);
                photo_data.close();
                RUN_MODE=HOME;
                HOME_background=FILE_JPEG[photo_show];
                break;
            }
            if(!FILE_JPEG[0].equals("end")&&!pressed&&analyse_touch(screen,LEFT))
            {
                tft.fillRect(180,0,60,320,TFT_RED);
                uint8_t del[2][16]=
                {
                    {0x00,0x00,0x01,0xC0,0x01,0xC0,0x3F,0xFE,0x00,0x00,0x00,0x00,0x1F,0xFC,0x12,0x24},
                    {0x12,0x24,0x12,0x24,0x12,0x24,0x0A,0x28,0x09,0x48,0x09,0x48,0x09,0x48,0x0F,0xF8}
                };
                lcd_display_chinese(195,150,32,del[0],TFT_WHITE,TFT_RED,1);
                delecting=1;
            }
            if(!pressed&&analyse_touch(180,0,240,320)==CLICK)
            {
                delecting=0;
                tft.fillScreen(TFT_BLACK);
                SPIFFS.remove(FILE_JPEG[photo_show]);
                listFiles();
                if(photo_show)photo_show--;
                //HOME_background=FILE_JPEG[photo_show];
                Serial.println(FILE_JPEG[photo_show]);
                showed=1;
            }
            if(delecting&&!pressed&&analyse_touch(0,0,180,320)==CLICK)
            {
                delecting=0;
                tft.fillScreen(TFT_BLACK);
                showed=1;
            }
            
            if(showed)
            {
                if(FILE_JPEG[0].equals("end")) 
                {
                    uint8_t imple[2][16]=
                    {
                        {0x02,0x00,0x01,0x00,0x7F,0xFE,0x40,0x02,0x88,0x24,0x10,0x10,0x20,0x08,0x00,0x00},
                        {0x1F,0xF0,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x7F,0xFC,0x00,0x00}
                    };
                    lcd_display_chinese(100,80,32,imple[0],TFT_WHITE,TFT_BLACK,1);
                }
                else
                    drawJpeg((const char*)FILE_JPEG[photo_show].c_str(),0,0);
                showed=0;
            }
        }
        break;
        case CLOCK:
        {
            uint8_t size=showdate_size;
            if(!pressed&&analyse_touch(screen,RIGHT))
            {
                if(RUN_MODE==RUN_MODE_LAST)RUN_MODE=HOME;
                else RUN_MODE=RUN_MODE_LAST;
                break;
            }
            if(showed)
            {
                showed=0;
                //tft.setTextColor(TFT_WHITE, TFT_GREY);  // Adding a background colour erases previous text automatically
                // Draw clock face
                tft.fillCircle(120, 120, 118, TFT_WHITE);//外环
                tft.fillCircle(120, 120, 110, TFT_BLACK);
                // Draw 12 lines
                for(int i = 0; i<360; i+= 30)
                {
                  sx = cos((i-90)*0.0174532925);
                  sy = sin((i-90)*0.0174532925);
                  x0 = sx*114+120;
                  yy0 = sy*114+120;
                  x1 = sx*100+120;
                  yy1 = sy*100+120;
              
                  tft.drawLine(x0, yy0, x1, yy1, TFT_WHITE);//时刻度
                }
              
                // Draw 60 dots
                for(int i = 0; i<360; i+= 6) {
                  sx = cos((i-90)*0.0174532925);
                  sy = sin((i-90)*0.0174532925);
                  x0 = sx*102+120;
                  yy0 = sy*102+120;
                  // Draw minute markers
                  tft.drawPixel(x0, yy0, TFT_WHITE);
                  
                  // Draw main quadrant dots
                  if(i==0 || i==180) tft.fillCircle(x0, yy0, 2, TFT_WHITE);
                  if(i==90 || i==270) tft.fillCircle(x0, yy0, 2, TFT_WHITE);
                }
              
                tft.fillCircle(120, 121, 3, TFT_BLACK);
              
                // Draw text at position 120,260 using fonts 4
                // Only font numbers 2,4,6,7 are valid. Font 6 only contains characters [space] 0 1 2 3 4 5 6 7 8 9 : . - a p m
                // Font 7 is a 7 segment font and only contains characters [space] 0 1 2 3 4 5 6 7 8 9 : .
                //tft.drawCentreString("Time flies",120,260,4);
            }
            
            if(pressed||(release_time>0&&release_time<50));
            else
            {
                if(timeClient.update()==1) initial=1;
                if(ss==timeClient.getSeconds())
                    break;
                if(hh!=timeClient.getHours())
                    tft.fillRect(48,260,size*10,size*2,TFT_BLACK);
                else if(mm!=timeClient.getMinutes())
                    tft.fillRect(101,260,size*6.2,size*2,TFT_BLACK);
                
                tft.fillRect(153,260,size*3,size*2,TFT_BLACK);
                //Serial.println("old:"+String(hh)+":"+String(mm)+":"+String(ss));
                //Serial.println("new:"+String((int)timeClient.getHours())+":"+String((int)timeClient.getMinutes())+":"+String((int)timeClient.getSeconds()));
                hh=timeClient.getHours();
                mm=timeClient.getMinutes();
                ss=timeClient.getSeconds();
                //Serial.println("new:"+String(hh)+":"+String(mm)+":"+String(ss));
                tft.loadFont(NotoSansBold36);
                //tft.fillRect(80,260,size*6,size,TFT_GREEN);
                tft.setTextColor(TFT_WHITE, TFT_BLACK);
                String HH=String(hh);
                String MM=String(mm);
                String SS=String(ss);
                if(hh<=9)HH="0"+String(hh);
                if(mm<=9)MM="0"+String(mm);
                if(ss<=9)SS="0"+String(ss);
                
                tft.drawString(HH+":"+MM+":"+SS,48,260);
                
                tft.unloadFont();
                //delay(20);
                sdeg = ss*6;                  // 0-59 -> 0-354
                mdeg = mm*6+sdeg*0.01666667;  // 0-59 -> 0-360 - includes seconds
                hdeg = hh*30+mdeg*0.0833333;  // 0-11 -> 0-360 - includes minutes and seconds
                hx = cos((hdeg-90)*0.0174532925);    
                hy = sin((hdeg-90)*0.0174532925);
                mx = cos((mdeg-90)*0.0174532925);
                my = sin((mdeg-90)*0.0174532925);
                sx = cos((sdeg-90)*0.0174532925);    
                sy = sin((sdeg-90)*0.0174532925);
            
                if (ss==0 || initial) 
                {
                    initial = 0;
                    // Erase hour and minute hand positions every minute
                    tft.drawLine(ohx, ohy, 120, 121, TFT_BLACK);
                    ohx = hx*62+121;    
                    ohy = hy*62+121;
                    tft.drawLine(omx, omy, 120, 121, TFT_BLACK);
                    omx = mx*84+120;    
                    omy = my*84+121;
                }
            
                  // Redraw new hand positions, hour and minute hands not erased here to avoid flicker
                tft.drawLine(osx, osy, 120, 121, TFT_BLACK);
                osx = sx*90+121;
                osy = sy*90+121;
                tft.drawLine(osx, osy, 120, 121, TFT_RED);
                tft.drawLine(ohx, ohy, 120, 121, TFT_WHITE);
                tft.drawLine(omx, omy, 120, 121, TFT_WHITE);
                tft.drawLine(osx, osy, 120, 121, TFT_RED);
            
                tft.fillCircle(120, 121, 3, TFT_RED);
            }
            
        }
        break;
        case CALENDAR:
        {
            if(!pressed&&analyse_touch(screen,RIGHT))
            {
                if(RUN_MODE!=RUN_MODE_LAST)RUN_MODE=RUN_MODE_LAST;
                else RUN_MODE=HOME;
                break;
            }
            if(pressed||(release_time>0&&release_time<50));
            else
            {
                if(timeClient.update()==1) 
                {
                  showed=1;
                  Updata_weather();
                }
            }
            day=timeClient.getDay();
            if(showed)
            {
                draw_calendar();
                showed=0;
            }
        }
        break;
        case CALCULATOR:
        {
            Calculator();
            RUN_MODE=MORE;
        }
        break;
        case ABOUT:
        {
            if(!pressed&&analyse_touch(screen,RIGHT))
            {
                RUN_MODE=SETTING;
                break;
            }
            if(showed)
            {
                drawJpeg("/system/app_hostbg/aboutbg.jpg", 0, 0);
                showed=0;
            }
        }
        break;
        case THEME:
        {
            uint8_t old_first_display=first_display;
            uint8_t old_home_display=home_display;
            static int sx=0,sy=0,ex=0,ey=0;
            int osx,osy,oex,oey;
            osx=sx;osy=sy;oex=ex;oey=ey;
            static int sx1=0,sy1=0,ex1=0,ey1=0;
            int osx1,osy1,oex1,oey1;
            osx1=sx1;osy1=sy1;oex1=ex1;oey1=ey1;
            
            if(!pressed&&analyse_touch(15,85,80,150)==CLICK)
            {
                sx=10;sy=85;ex=80;ey=150;
                first_display=HOME;
            }
            if(!pressed&&analyse_touch(80,85,160,150)==CLICK)
            {
                sx=85;sy=85;ex=155;ey=150;
                first_display=PHOTO;
            }
            if(!pressed&&analyse_touch(160,85,225,150)==CLICK)
            {
                sx=160;sy=85;ex=225;ey=150;
                first_display=BILIBILI;
            }
            if(!pressed&&analyse_touch(15,150,80,215)==CLICK)
            {
                sx=10;sy=150;ex=80;ey=215;
                first_display=CLOCK;
            }
            if(!pressed&&analyse_touch(80,150,160,215)==CLICK)
            {
                sx=85;sy=150;ex=155;ey=215;
                first_display=WEATHER;
            }
            if(!pressed&&analyse_touch(160,150,225,215)==CLICK)
            {
                sx=160;sy=150;ex=225;ey=215;
                first_display=CALENDAR;
            }

            
            if(!pressed&&analyse_touch(15,250,80,310)==CLICK)
            {
                sx1=15;sy1=250;ex1=80;ey1=310;
                home_display=PHOTO;
            }
            if(!pressed&&analyse_touch(80,250,160,310)==CLICK)
            {
                sx1=80;sy1=250;ex1=160;ey1=310;
                home_display=BILIBILI;
            }
            if(showed)
            {
                drawJpeg("/system/app_hostbg/themebg.jpg", 0, 0);
                showed=0;
                if(first_display==HOME)
                {
                    sx=10;sy=85;ex=80;ey=150;
                }
                else if(first_display==PHOTO)
                {
                    sx=85;sy=85;ex=155;ey=150;
                }
                else if(first_display==BILIBILI)
                {
                    sx=160;sy=85;ex=225;ey=150;
                }
                else if(first_display==CLOCK)
                {
                    sx=10;sy=150;ex=80;ey=215;
                }
                else if(first_display==WEATHER)
                {
                    sx=85;sy=150;ex=155;ey=215;
                }
                else if(first_display==CALENDAR)
                {
                    sx=160;sy=150;ex=225;ey=215;
                }
                osx=sx;osy=sy;oex=ex;oey=ey;
                DrawRect(sx,sy,ex,ey,tft.color565(0,162,232));
    
                if(home_display==PHOTO)
                {
                    sx1=10;sy1=250;ex1=80;ey1=310;
                }
                else if(home_display==BILIBILI)
                {
                    sx1=85;sy1=250;ex1=155;ey1=310;
                }
                osx1=sx1;osy1=sy1;oex1=ex1;oey1=ey1;
                DrawRect(sx1,sy1,ex1,ey1,tft.color565(0,162,232));
                
            }
            if(old_first_display!=first_display)
            {
                DrawRect(osx,osy,oex,oey,TFT_BLACK);
                DrawRect(sx,sy,ex,ey,tft.color565(0,162,232));
            }
            if(old_home_display!=home_display)
            {
                DrawRect(osx1,osy1,oex1,oey1,TFT_BLACK);
                DrawRect(sx1,sy1,ex1,ey1,tft.color565(0,162,232));
            }
            if(!pressed&&analyse_touch(screen,RIGHT))
            {
                RUN_MODE=SETTING;
                if(SPIFFS.exists(THEME_DATA)) Serial.println("exist");
                File dataFile = SPIFFS.open(THEME_DATA, "w");
                if(first_display<10)dataFile.print("0"+String(first_display));
                else dataFile.print(String(first_display));
                if(home_display<10)dataFile.println("0"+String(home_display));
                else dataFile.println(String(home_display));
                dataFile.close();
                Serial.println(String(first_display)+String(home_display));
            }
        }
        break;
        case SETTING:
        {
            if(!pressed&&analyse_touch(screen,RIGHT))
            {
                RUN_MODE=RUN_MODE_LAST;
                break;
            }
            if(!pressed&&analyse_touch(20,120,100,185)==CLICK)
            {
                RUN_MODE=CONNECT;
                break;
            }
            if(!pressed&&analyse_touch(130,120,210,185)==CLICK)
            {
                RUN_MODE=THEME;
                break;
            }
            if(!pressed&&analyse_touch(20,200,100,265)==CLICK)
            {
                RUN_MODE=ABOUT;
                break;
            }
            if(showed)
            {
                showed=0;
                drawJpeg("/system/app_hostbg/setting.jpg",0,0);
            }
        }
        break;
        case MORE:
        {
            RUN_MODE_LAST=RUN_MODE;
            if(!pressed&&analyse_touch(15,5,95,75)==CLICK)//1
            {
                if(menu_page==1) RUN_MODE=SETTING;
                else if(menu_page==2) RUN_MODE=WEATHER;
                break; 
            }
            if(!pressed&&analyse_touch(135,5,215,75)==CLICK)//2
            {
                if(menu_page==1) RUN_MODE=FILEMANAGE;
                if(menu_page==2) {painting();showed=1;}
                break;
            }
            if(!pressed&&analyse_touch(15,85,95,155)==CLICK)//3
            {
                if(menu_page==1) RUN_MODE=CLOCK;
                break;
            }
            if(!pressed&&analyse_touch(135,85,215,155)==CLICK)//4
            {
                if(menu_page==1) RUN_MODE=BILIBILI;
                break;
            }
            if(!pressed&&analyse_touch(15,165,95,235)==CLICK)//5
            {
                if(menu_page==1) RUN_MODE=CALENDAR;
                break;
            }
            if(!pressed&&analyse_touch(135,165,215,235)==CLICK)//6
            {
                if(menu_page==1) RUN_MODE=CALCULATOR;
                break;
            }
            if(!pressed&&analyse_touch(15,245,95,315)==CLICK)//7
            {
                if(menu_page==1) RUN_MODE=UPLOAD;
                break;
            }
            if(!pressed&&analyse_touch(135,245,215,315)==CLICK)//8
            {
                if(menu_page==1) RUN_MODE=PHOTO;
                break;
            }
            if(!pressed&&analyse_touch(screen,RIGHT))
            {
                RUN_MODE=HOME;
                break;
            }
            
            if(!pressed&&analyse_touch(screen,UP))
            {
                showed=1;
                menu_page++;
                if(menu_page>MAX_menu_page)menu_page=MAX_menu_page;
                break;
            }
            
            if(!pressed&&analyse_touch(screen,DOWN))
            {
                showed=1;
                menu_page--;
                if(menu_page<1)menu_page=1;
                break;
            }
            if(showed)
            {
                showed=0;
                tft.fillScreen(TFT_BLACK);
                draw_moremenu();
                
                //DrawRect(15,10,95,90);
                //DrawRect(135,10,215,90);
                //DrawRect(15,100,95,180);
            }
        }
        break;
        case BILIBILI:
        {
            if(!pressed&&analyse_touch(RETURN_button,CLICK))//if((touch_check(RETURN_button)==CLICK))
            {
                if(RUN_MODE==RUN_MODE_LAST)RUN_MODE=HOME;
                else RUN_MODE=RUN_MODE_LAST;
            }
            if(!pressed&&analyse_touch(screen,RIGHT))
            {
                if(RUN_MODE==RUN_MODE_LAST)RUN_MODE=HOME;
                else RUN_MODE=RUN_MODE_LAST;
            }
            if(!pressed&&analyse_touch(80,0,160,80)==CLICK)
            {
                String temp=input_method();
                if(temp.length()>0) 
                {
                  biliID=temp;
                  File dataFile = SPIFFS.open(BILI_DATA, "w");
                  dataFile.println(biliID);      
                  dataFile.close();
                }
                //set_biliID();
                showed=1;
            }
            tft.unloadFont();
            tft.loadFont(NotoSansBold15);
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
            tft.drawCentreString("UID: "+biliID,120,75,4);
            tft.unloadFont();
            if(showed)
            {
                showed=0;
                drawJpeg("/system/app_hostbg/bilibili.jpg",0,0);
                return_label(return_x,return_y,return_size,TFT_WHITE);
                //if(!BiliID()&&WiFi.status() == WL_CONNECTED)showed=1;
            }
            if(pressed||(release_time>0&&release_time<50))bilibili_updata=0;
            if(bilibili_updata)
            {
                BiliID();
                bilibili_updata;
            }
        }
        break;
        case FILEMANAGE:
        {
            if(!pressed&&analyse_touch(RETURN_button,CLICK))//if((touch_check(RETURN_button)==CLICK))
            {
                RUN_MODE=RUN_MODE_LAST;
            }
            if(!pressed&&analyse_touch(screen,RIGHT))
            {
                RUN_MODE=RUN_MODE_LAST;
            }
            /*
            if(!pressed&&analyse_touch(screen,UP))
            {
                
            }
            if(!pressed&&analyse_touch(screen,DOWN))
            {
                if(page>0)page--;
            }
            */
            if(showed)
            {
                //Dir dir = SPIFFS.openDir(folder_name);
                showed=0;
                return_label(return_x,return_y,return_size,TFT_WHITE);
            }
        }
        break;
        case UPLOAD:
        {
              photo_upload_server.handleClient();
              if(showed)
              {
                  showed=0;
                  drawJpeg("/system/app_hostbg/homebg.jpg",0,0);
                  
                  return_label(return_x,return_y,return_size,TFT_WHITE);
                  
                  tft.unloadFont();
                  //tft.loadFont(NotoSansBold15);
                  tft.setTextColor(TFT_WHITE, TFT_BLACK);
                  String localip=WiFi.localIP().toString();
                  tft.drawCentreString(localip,120,185,4);
                  tft.unloadFont();
                  
                  photo_upload_server.on("/system/fileup/upload.html",   // 如果客户端通过upload页面
                                    HTTP_POST,        // 向服务器发送文件(请求方法POST)
                                    respondOK,        // 则回复状态码 200 给客户端
                                    handleFileUpload);// 并且运行处理文件上传函数
                  
                  photo_upload_server.onNotFound(handleUploadRequest);
                
                  photo_upload_server.begin();
              }
              
              if(!pressed&&analyse_touch(screen,RIGHT))
              {
                  RUN_MODE=RUN_MODE_LAST;
                  photo_upload_server.close();
              }
              if(oldpressed&&!pressed&&analyse_touch(RETURN_button,CLICK))
              {
                  RUN_MODE=RUN_MODE_LAST;
                  photo_upload_server.close();
              }
        }
        break;
        case WEATHER:
        {
            //DrawRect(RETURN_button);
            //Updata_weather();
            static int oldcode=-1;
            if(pressed||(release_time>0&&release_time<50));
            else 
            {
              if(oldcode==-1||showed)weather_code=Updata_weather();
              if((weather_code!=oldcode)&&(weather_code>=0))
              {
                  showed=1;
                  Serial.print(oldcode);
                  Serial.print(",");
                  Serial.println(weather_code);
                  if(oldcode!=-1)oldcode=weather_code;
              }
            }
            if(oldpressed&&!pressed&&analyse_touch(Weather_location,CLICK))//if((touch_check(Weather_location)==CLICK))
            {
                RUN_MODE=LOCATION;
            }
            if(oldpressed&&!pressed&&analyse_touch(RETURN_button,CLICK))//if((touch_check(RETURN_button)==CLICK))
            {
                if(RUN_MODE==RUN_MODE_LAST)RUN_MODE=HOME;
                else RUN_MODE=RUN_MODE_LAST;
            }
            if(!pressed&&analyse_touch(screen,RIGHT))
            {
                RUN_MODE=RUN_MODE_LAST;
            }
            if(showed)
            {
                showed=0;
                tft.fillScreen(TFT_BLACK);
                return_label(return_x,return_y,return_size,TFT_WHITE);
                for(int i=0;i<3;i++)
                {
                    display_weather(60,95+80*i,showweather_size,forecast_code[i],0);
                    tft.loadFont(NotoSansBold15);
                    //tft.fillRect(x,y,size*6,size,TFT_BLACK);
                    tft.setTextColor(TFT_ORANGE, TFT_BLACK);
                    tft.drawString(String(forecast_high[i]),110,83+80*i);
                    tft.setTextColor(TFT_SKYBLUE, TFT_BLACK);
                    tft.drawString(String(forecast_low[i]),110,103+80*i);
                    
                    tft.unloadFont();
                    tft.loadFont(NotoSansBold36);
                    tft.setTextColor(TFT_WHITE, TFT_BLACK);
                    tft.drawString(String(atoi(forecast_upday.substring(8,10).c_str())+i),187,85+80*i);
                    tft.unloadFont();

                    //DrawRect(Weather_location);
                }
                lcd_display_chinese(155,17,32,select_location[location],TFT_WHITE,-1,1);
                lcd_display_chinese(155+32,17,32,select_location[location+2],TFT_WHITE,-1,1);
                
            }
            if(oldcode==-1&&weather_code>=0)showed=1;
            {
                Serial.print(oldcode);
                Serial.print(",");
                Serial.println(weather_code);
                oldcode=weather_code;
            }
        }
        break;
        case LOCATION:
        {
            struct ButtonRect LOCATION_button[4] =
            {
                {10,80,110,140},
                {130,80,230,140},
                {10,155,110,215},
                {130,155,230,215}
            };
            if(!pressed&&analyse_touch(RETURN_button,CLICK))//if((touch_check(RETURN_button)==CLICK))
            {
                RUN_MODE=WEATHER;
            }
            if(!pressed&&analyse_touch(screen,RIGHT))
            {
                RUN_MODE=WEATHER;
            }
            for(int i=0;i<4;i++)
            {
                if(!pressed&&analyse_touch(LOCATION_button[i],CLICK)&&location!=i*4)
                {
                    location=i*4;
                    Set_weather_location();
                    if(SPIFFS.exists(LOCATION_DATA)) Serial.println("exist");
                    File dataFile = SPIFFS.open(LOCATION_DATA, "w");
                    if(location<10)dataFile.println("0"+String(location));
                    else dataFile.println(String(location));
                    dataFile.close();
                    float j=0;
                    tft.drawCircle(120, 270, 20, TFT_WHITE);
                    int x=20*cos(j),y=20*sin(j);
                    while(Updata_weather()!=-1)
                    {
                        pressed=tft.getTouch(&tx, &ty);
                        if(!pressed&&analyse_touch(RETURN_button,CLICK))//if((touch_check(RETURN_button)==CLICK))
                        {
                            Serial.println("break");
                            RUN_MODE=WEATHER;
                            break;
                        }
                        oldpressed=pressed;
                        j+=0.1;
                        tft.fillCircle(120+x, 270+y,3, TFT_BLACK);
                        x=20*cos(j);
                        y=20*sin(j);
                        tft.fillCircle(120+x, 270+y,3, TFT_WHITE);
                        tft.drawCircle(120, 270, 20, TFT_WHITE);
                        //delay(1);
                    }
                    tft.fillCircle(120, 270, 25, TFT_BLACK);
                    showed=1;
                    break;
                }
            }
           
            if(showed)
            {
                showed=0;
                return_label(return_x,return_y,return_size,TFT_WHITE);
                for(int i=0;i<4;i++)
                {
                    if(i*4==location) tft.fillRect(LOCATION_button[i].sx,LOCATION_button[i].sy,LOCATION_button[i].ex-LOCATION_button[i].sx,LOCATION_button[i].ey-LOCATION_button[i].sy,tft.color565(0, 162, 232));
                    else tft.fillRect(LOCATION_button[i].sx,LOCATION_button[i].sy,LOCATION_button[i].ex-LOCATION_button[i].sx,LOCATION_button[i].ey-LOCATION_button[i].sy,tft.color565(127, 127, 127));
                    //DrawRect(LOCATION_button[i]);
                    lcd_display_chinese(LOCATION_button[i].sx+18,LOCATION_button[i].sy+13,32,select_location[i*4],TFT_WHITE,-1,1);
                    lcd_display_chinese(LOCATION_button[i].sx+35+18,LOCATION_button[i].sy+13,32,select_location[i*4+2],TFT_WHITE,-1,1);
                }
            }
        }
        break;
        case CONNECT:
        {
            if(!pressed&&analyse_touch(screen,RIGHT))
            {
                RUN_MODE=SETTING;
            }
            if(showed)
            {
                handleSetAP();
                RUN_MODE=CONNECTING;
                showed=0;
            }
        }
        break;
        case CONNECTING:
        {
            if(!pressed&&analyse_touch(screen,RIGHT))
            {
                connect_server.close();
                WiFi.mode(WIFI_STA);
                RUN_MODE=SETTING;
                if (SPIFFS.exists(WIFI_DATA))
                {
                    Serial.print(WIFI_DATA);
                    Serial.println(" FOUND.");
                    
                    File dataFile = SPIFFS.open(WIFI_DATA, "r");
                    char temp;
                    ssid.clear();
                    password.clear();
                    while(1)
                    {
                        //Serial.print((char)dataFile.read());
                        temp=(char)dataFile.read();
                        if(temp=='\r') break;
                        ssid+=temp;
                    }
                    dataFile.read();
                    Serial.println(ssid);
                    while(1)
                    {
                        temp=(char)dataFile.read();
                        if(temp=='\r') break;
                        password+=temp;
                    }
                    dataFile.close();
                    Serial.println(password);
                    delay(100);
                    WiFi.begin(ssid,password);
                    delay(100);
                    //httpserver();
                }
                else
                {
                    Serial.print(WIFI_DATA);
                    Serial.println("not  FOUND.");
                }
            }
            connect_server.handleClient();
            dnsServer.processNextRequest();
            if(showed)
            {
                drawJpeg("/system/app_hostbg/setwifi.jpg",0,0);
                tft.unloadFont();
                //tft.loadFont(NotoSansBold15);
                tft.setTextColor(TFT_WHITE, TFT_BLACK);
                tft.drawCentreString(AP_NAME,120,115,4);
                String localip="192.168.0.123";//WiFi.localIP().toString();
                tft.drawCentreString(localip,120,200,4);
                tft.unloadFont();
                
                showed=0;
            }
        }
        break;
    }
    //if(RUN_MODE>3) RUN_MODE=1;
    if(RUN_MODE_OLD!=RUN_MODE)
    {
        //RUN_MODE_LAST=RUN_MODE_OLD;
        Start=3;
        Start_weather=1;
        tft.fillScreen(TFT_BLACK);
        showed=1;
        start_Ticker.attach(1,End_Start);
        first_show_weather=1;
    }
    //ESP.wdtFeed();
}
//*****************************************Painting*************************************
void painting()
{
    uint16_t red=0,green=0,blue=0;
    bool pressed=0,oldpressed;
    uint16_t tx=0,ty=0;
    uint16_t color=0;
    bool drawed=0;
    bool erase=0;
    tft.fillScreen(TFT_WHITE);
    tft.drawChar(8,10,'<',TFT_BLACK,TFT_WHITE,4);
    for(int i=0;i<153;i++)
    {
        tft.drawLine(i+43,0,i+43,15,tft.color565((uint16_t)i*1.66,0,0));
    }
    for(int i=0;i<153;i++)
    {
        tft.drawLine(i+43,15,i+43,30,tft.color565(0,(uint16_t)i*1.66,0));
    }
    for(int i=0;i<153;i++)
    {
        tft.drawLine(i+43,30,i+43,45,tft.color565(0,0,(uint16_t)i*1.66));
    }
    tft.fillRect(207,5,20,5,tft.color565(239,228,176));
    tft.fillRect(206,11,22,25,tft.color565(255,174,201));
    while(1)
    {
        if(oldpressed)
        {
            ltx=tx;
            lty=ty;
        }
        pressed = tft.getTouch(&tx, &ty);
        if(pressed&&ty>55)
        {
            if(erase)
            {
                tft.fillCircle(tx,ty,10,TFT_WHITE);
                if(oldpressed) 
                {
                    for(int i=0;i<7;i++)
                    {
                        tft.drawLine(tx-i,ty,ltx-i,lty,TFT_WHITE);
                        tft.drawLine(tx+i,ty,ltx+i,lty,TFT_WHITE);
                        tft.drawLine(tx,ty-i,ltx,lty-i,TFT_WHITE);
                        tft.drawLine(tx,ty+i,ltx,lty+i,TFT_WHITE);
                    }
                }
            }
            else
            {
                //tft.drawPixel(tx,ty,color);
                tft.fillCircle(tx,ty,3,color);
                if(oldpressed) 
                {
                    for(int i=0;i<4;i++)
                    {
                        tft.drawLine(tx-i,ty,ltx-i,lty,color);
                        tft.drawLine(tx+i,ty,ltx+i,lty,color);
                        tft.drawLine(tx,ty-i,ltx,lty-i,color);
                        tft.drawLine(tx,ty+i,ltx,lty+i,color);
                    }
                }
                drawed=1;
            }
        }
        else if(pressed)
        {
            if(tx>43&&tx<196)
            {
                if(ty>0&&ty<15) red=(uint16_t)((tx-43)*1.66);
                if(ty>15&&ty<30)green=(uint16_t)((tx-43)*1.66);
                if(ty>30&&ty<45)blue=(uint16_t)((tx-43)*1.66);
                color=tft.color565(red,green,blue);
            }
        }
        else if(oldpressed&&!pressed)
        {
            if(lty>0&&lty<45)
            {
                if(ltx>0&&ltx<43)
                {
                    if(!drawed)
                    {
                        ltx=lty=0;
                        return;
                    }
                    else
                    {
                        drawed=0;
                        tft.fillRect(0,45,239,274,TFT_WHITE);
                    }
                }
                if(ltx>194&&ltx<240)
                {
                    if(erase)
                    {
                        erase=0;
                        tft.drawRect(196,1,43,43,TFT_WHITE);
                    }
                    else 
                    {
                        erase=1;
                        tft.drawRect(196,1,43,43,TFT_VIOLET);
                    }
                }
            }
        }
        oldpressed=pressed;
    }
}
//****************************************CALENDER**************************************
#define day_dx 34
#define day_dy 35
void draw_calendar()
{
    int i=0,j=0;
    uint8_t fday=0,tday,today=atoi(weatherNow.getLastUpdate().substring(8,10).c_str());
    uint8_t mday=30,moon=atoi(weatherNow.getLastUpdate().substring(5,7).c_str());
    int year=atoi(weatherNow.getLastUpdate().substring(0,4).c_str());
    uint8_t countday=1;
    drawJpeg("/system/app_hostbg/datebg.jpg",0,0);
    switch(moon)
    {
        case 4:
        case 6:
        case 9:
        case 11:
        {
            mday=30;
            break;
        }
        case 2:
        {
            if((!(year%4)&&(year%100))||!(year%400))
                mday=29;
            else
                mday=28;
            break;
        }
        default:
        {
            mday=31;
            break;
        }
    }
    fday=(day+7-(today%7)+1)%7;
    
    
    tft.loadFont(NotoSansBold36);
    tft.setTextColor(TFT_BLACK, tft.color565(239,228,176));
    tft.drawCentreString(forecast_upday.substring(0,10).c_str(),120,13,4);
    tft.unloadFont();
    tft.loadFont(NotoSansBold15);
    tft.setTextColor(TFT_BLACK, tft.color565(239,228,176));
    for(i=0;i<6;i++)
    {
      if(i==0) j=fday;
      else j=0;
      for(;j<7;j++)
      {
        //;
        if(countday==today)
        {
            tft.setTextColor(TFT_RED, tft.color565(239,228,176));
            tft.drawCentreString(String(countday),18+day_dx*j,100+i*day_dy,2);
            tft.setTextColor(TFT_BLACK, tft.color565(239,228,176));
        }
        else
            tft.drawCentreString(String(countday),18+day_dx*j,100+i*day_dy,2);
        countday++;
        if(countday>mday)
        {
          return;
        }
      }
    }
}

//****************************************Calculator************************************
void Calculator()
{
    String str;
    uint8_t shift= input_number; 
    int input_x=10,input_y=10;
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(input_x, input_y, 4);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(1);
    bool pressed = 0;
    bool oldpressed = 0;
    uint16_t tx,ty;
    int x = 4, y=120;
    int input_line_time=0;
    int firstend=0,secondstart=0;
    bool first_input=1;
    for (int i = 0; i < 4; i++)
    {
        x=4;
        for (int j = 0; j < 4; j++)
        {
            tft.fillRect(x, y, 55, 45,tft.color565(127,127,127));
            if (i < 3 && j < 3)
            {
                tft.setTextColor(TFT_WHITE, tft.color565(127,127,127));
                tft.drawNumber(i*3+j+1,x+20,y+10, 4);
                //Serial.print("!");
            }
            x += 59;
        }
        y+=50;
    }
    char cal[4]={'+','-','X','/'};
    x=4;
    y=70;
    for(int i=0;i<4;i++)
    {
        tft.fillRect(x, y, 55, 45,tft.color565(127,127,127));
        tft.drawChar(x+20,y+15,cal[i],TFT_WHITE,tft.color565(127,127,127),2);
        x += 59;
    }
    tft.drawChar(181+20,170+15,'C',TFT_WHITE,tft.color565(127,127,127),2);
    tft.drawChar(181+20,220+15,'^',TFT_WHITE,tft.color565(127,127,127),2);
    tft.drawChar(122+20,270+15,'.',TFT_WHITE,tft.color565(127,127,127),2);
    tft.drawChar(181+20,270+15,'=',TFT_WHITE,tft.color565(127,127,127),2);
    tft.setTextColor(TFT_WHITE, tft.color565(127,127,127));
    tft.drawChar(4+20,270+15,'<',TFT_WHITE,tft.color565(127,127,127),2);
    tft.drawNumber(0,63+20,270+15, 4);
    uint8_t backspace_label[2][16]=
    {
      {0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xFE,0x08,0x02,0x11,0x12,0x20,0xA2,0x40,0x42},
      {0x20,0xA2,0x11,0x12,0x08,0x02,0x07,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}
    };
    lcd_display_chinese(193, 127, 32,backspace_label[0],TFT_WHITE,-1,1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    while (1)
    {
        input_x=tft.getCursorX();
        if(input_line_time<15)
        {
          input_line_time++;
          tft.drawLine(input_x,input_y,input_x,input_y+20,TFT_WHITE);
        }
        else if(input_line_time<30)
        {
          input_line_time++;
          tft.drawLine(input_x,input_y,input_x,input_y+20,TFT_BLACK);
        }
        else
        {
          input_line_time=0;
        }
        if (oldpressed)
        {
            ltx = tx;
            lty = ty;
        }
        pressed = tft.getTouch(&tx, &ty);
        if (shift == input_number)
        {
            if (!pressed && oldpressed)
            {
                tft.drawLine(input_x,input_y,input_x,input_y+20,TFT_BLACK);
                //tft.fillRect(10,15,219,40,TFT_GREEN); 
                if(str.length()<16)
                {
                    bool input_add=0;
                    if (analyse_touch_input(4, 120, 59, 165))//1
                    {
                        if(first_input) 
                        {
                            tft.fillRect(0,10,239,30,TFT_BLACK);
                            str.clear();
                            first_input=0;
                        }
                        str += "1";
                    }
                    else if (analyse_touch_input(63, 120, 118, 165))//2
                    {
                        if(first_input) 
                        {
                            tft.fillRect(0,10,239,30,TFT_BLACK);
                            str.clear();
                            first_input=0;
                        }
                        str += "2";
                    }
                    else if (analyse_touch_input(122, 120, 177, 165))//3
                    {
                        if(first_input) 
                        {
                            tft.fillRect(0,10,239,30,TFT_BLACK);
                            str.clear();
                            first_input=0;
                        }
                        str += "3";
                    }
                    else if (analyse_touch_input(4, 170, 59, 215))//4
                    {
                        if(first_input) 
                        {
                            tft.fillRect(0,10,239,30,TFT_BLACK);
                            str.clear();
                            first_input=0;
                        }
                        str += "4";
                    }
                    else if (analyse_touch_input(63, 170, 118, 215))//5
                    {
                        if(first_input) 
                        {
                            tft.fillRect(0,10,239,30,TFT_BLACK);
                            str.clear();
                            first_input=0;
                        }
                        str += "5";
                    }
                    else if (analyse_touch_input(122, 170, 177, 215))//6
                    {
                        if(first_input) 
                        {
                            tft.fillRect(0,10,239,30,TFT_BLACK);
                            str.clear();
                            first_input=0;
                        }
                        str += "6";
                    }
                    else if (analyse_touch_input(4, 220, 59, 265))//7
                    {
                        if(first_input) 
                        {
                            tft.fillRect(0,10,239,30,TFT_BLACK);
                            str.clear();
                            first_input=0;
                        }
                        str += "7";
                    }
                    else if (analyse_touch_input(63, 220, 118, 265))//8
                    {
                        if(first_input) 
                        {
                            tft.fillRect(0,10,239,30,TFT_BLACK);
                            str.clear();
                            first_input=0;
                        }
                        str += "8";
                    }
                    else if (analyse_touch_input(122, 220, 177, 265))//9
                    {
                        if(first_input) 
                        {
                            tft.fillRect(0,10,239,30,TFT_BLACK);
                            str.clear();
                            first_input=0;
                        }
                        str += "9";
                    }
                    else if (analyse_touch_input(63, 270, 118, 315))//0
                    {
                        if(first_input) 
                        {
                            tft.fillRect(0,10,239,30,TFT_BLACK);
                            str.clear();
                            first_input=0;
                        }
                        str += "0";
                    }
                    else if (analyse_touch_input(122, 270, 177, 315))//.
                    {
                        if(first_input) 
                        {
                            tft.fillRect(0,10,239,30,TFT_BLACK);
                            str.clear();
                            first_input=0;
                        }
                        str += ".";
                    }
                    else if (analyse_touch_input(4, 70, 59, 115))//+
                    {
                        first_input=0;
                        str += "+";
                    }
                    else if (analyse_touch_input(63, 70, 118, 115))//-
                    {
                        first_input=0;
                        str += "-";
                    }
                    else if (analyse_touch_input(122, 70, 177, 115))//*
                    {
                        first_input=0;
                        str += "*";
                    }
                    else if (analyse_touch_input(181, 70, 236, 115))//*
                    {
                        first_input=0;
                        str += "/";
                    }
                    else if (analyse_touch_input(181, 220, 236, 265))//^
                     {
                        first_input=0;
                        str += "^";
                    }
                }
                if (analyse_touch_input(181, 120, 236, 165))//back
                {
                    if(first_input) 
                    {
                        tft.fillRect(0,10,239,30,TFT_BLACK);
                        str.clear();
                        first_input=0;
                    }
                    str = str.substring(0, str.length() - 1);
                    tft.fillRect(input_x-30,input_y,32,30,TFT_BLACK);
                }
                if (analyse_touch_input(181, 270, 236, 315))//Enter
                {
                    first_input=1;
                    tft.fillRect(0,10,239,30,TFT_BLACK);
                    char *strc=(char*)str.c_str();
                    int len=0;
                    while((strc[len]>='0'&&strc[len]<='9')||strc[len]=='.')
                    {
                        len++;
                    }
                    double first=atof(str.substring(0,len).c_str());
                    double second=atof(str.substring(len+1,str.length()).c_str());
                    Serial.print(first);
                    Serial.print(strc[len]);
                    Serial.println(second);
                    switch(strc[len])
                    {
                        case '+':
                        {
                            str=String(first+second);
                        }
                        break;
                        case '-':
                        {
                            str=String(first-second);
                        }
                        break;
                        case '*':
                        {
                            str=String(first*second);
                        }
                        break;
                        case '/':
                        {
                            if(second!=0)
                                str=String(first/second);
                            else
                                str="error";
                        }
                        break;
                        case '^':
                        {
                            str=String((float)pow(first,second));
                        }
                        break;
                    }
                }
                else if (analyse_touch_input(181, 170, 236, 215))
                {
                    tft.fillRect(0,10,239,30,TFT_BLACK);
                    str.clear();
                }
                else if (analyse_touch_input(4, 270, 59, 315))
                {
                    tx = ty = ltx = lty = 0;
                    return ;
                }
                Serial.println(str);
                tft.setCursor(10, 10, 4);
                if(str.length()<=16) tft.print(str);
                else  
                {
                  tft.print("error");
                  str.clear();
                }
                input_x=tft.getCursorX();
                input_y=tft.getCursorY();
            }
        }
        oldpressed = pressed;
    }
}
//****************************************MORE******************************************
#define MENU_SXL 15
#define MENU_SXR 135
#define MENU_SY 5
#define MENU_DY 80
//#define MENU_W 230
//#define MENU_H 30


void draw_moremenu()
{
    int y=MENU_SY,size=showweather_size;
    if(menu_page==1)
    {
        drawJpeg("/system/app_logo/setting.jpg", MENU_SXL, y);
        drawJpeg("/system/app_logo/filemanage.jpg", MENU_SXR, y);
        y+=MENU_DY;
        drawJpeg("/system/app_logo/clock.jpg", MENU_SXL, y);
        drawJpeg("/system/app_logo/bilibili.jpg", MENU_SXR, y);
        y+=MENU_DY;
        drawJpeg("/system/app_logo/calendar.jpg", MENU_SXL, y);
        drawJpeg("/system/app_logo/calculator.jpg", MENU_SXR, y);
        y+=MENU_DY;
        drawJpeg("/system/app_logo/upload.jpg", MENU_SXL, y);
        drawJpeg("/system/app_logo/photo.jpg", MENU_SXR, y);
    }
    else if(menu_page==2)
    {
        tft.fillCircle(MENU_SXL+45,y+35,size,TFT_ORANGE);
        tft.fillCircle(MENU_SXL+size*0.5+45,y+size*0.5+35,size,TFT_WHITE);
        tft.fillCircle(MENU_SXL-size*0.66+45,y+size*0.75+35,size*0.75,TFT_WHITE);
        tft.fillRect(MENU_SXL-size*1.5+45,y+size*1.25+35,size*3,size*0.5,TFT_BLACK);
        drawJpeg("/system/app_logo/paint.jpg", MENU_SXR, y);
    }
    /*
    y+=MENU_DY;
    drawJpeg("/system/app_logo/bilibili.jpg", MENU_SX, y);
    y+=MENU_DY;
    */
}
//****************************************file_upload***********************************
void handleFileUpload()
{
    HTTPUpload& upload = photo_upload_server.upload();
    
    if(upload.status == UPLOAD_FILE_START){                     // 如果上传状态为UPLOAD_FILE_START
      
      String filename = upload.filename;                        // 建立字符串变量用于存放上传文件名
      if(!filename.startsWith("/")) 
      {
        if(filename.endsWith(".jpg")) filename = "/data/img/" + filename;  // 为上传文件名前加上"/"
        if(filename.endsWith(".txt")) filename = "/data/text/" + filename; 
        if(filename.endsWith(".mp3")) filename = "/data/music/" + filename; 
        if(filename.endsWith(".mp4")) filename = "/data/video/" + filename; 
      }
      else
      {
        if(filename.endsWith(".jpg")) filename = "/data/img"+ filename;
        if(filename.endsWith(".txt")) filename = "/data/text" + filename; 
        if(filename.endsWith(".mp3")) filename = "/data/music" + filename; 
        if(filename.endsWith(".mp4")) filename = "/data/video" + filename; 
      }
      Serial.println("File Name: " + filename);                 // 通过串口监视器输出上传文件的名称
      
      fsUploadFile = SPIFFS.open(filename, "w");            // 在SPIFFS中建立文件用于写入用户上传的文件数据
      
    } else if(upload.status == UPLOAD_FILE_WRITE){          // 如果上传状态为UPLOAD_FILE_WRITE      
      
      if(fsUploadFile)
        fsUploadFile.write(upload.buf, upload.currentSize); // 向SPIFFS文件写入浏览器发来的文件数据
        
    } else if(upload.status == UPLOAD_FILE_END){            // 如果上传状态为UPLOAD_FILE_END 
      if(fsUploadFile) {                                    // 如果文件成功建立
        fsUploadFile.close();                               // 将文件关闭
        listFiles();
        Serial.println(" Size: "+ upload.totalSize);        // 通过串口监视器输出文件大小
        photo_upload_server.sendHeader("Location","/system/fileup/finish.html");  // 将浏览器跳转到/success.html（成功上传页面）
        photo_upload_server.send(303);                               // 发送相应代码303（重定向到新页面） 
      } else {                                              // 如果文件未能成功建立
        Serial.println("File upload failed");               // 通过串口监视器输出报错信息
        photo_upload_server.send(500, "text/plain", "500: couldn't create file"); // 向浏览器发送相应代码500（服务器错误）
      }    
    }
}
void respondOK(){
  photo_upload_server.send(200);
}
void handleUploadRequest(){
                              
  // 获取用户请求网址信息
  String webAddress = photo_upload_server.uri();
  
  // 通过handleFileRead函数处处理用户访问
  bool fileReadOK = handleFileRead(webAddress);

  // 如果在SPIFFS无法找到用户访问的资源，则回复404 (Not Found)
  if (!fileReadOK){                                                 
    photo_upload_server.send(404, "text/plain", "404 Not Found"); 
  }
}
bool handleFileRead(String path) {            //处理浏览器HTTP访问

  if (path.endsWith("/")) {                   // 如果访问地址以"/"为结尾
    path = "/system/fileup/index.html";                     // 则将访问地址修改为/index.html便于SPIFFS访问
  } 
  
  String contentType = getContentType(path);  // 获取文件类型
  
  if (SPIFFS.exists(path)) {                     // 如果访问的文件可以在SPIFFS中找到
    File file = SPIFFS.open(path, "r");          // 则尝试打开该文件
    photo_upload_server.streamFile(file, contentType);// 并且将该文件返回给浏览器
    file.close();                                // 并且关闭文件
    return true;                                 // 返回true
  }
  return false;                                  // 如果文件未找到，则返回false
}

//****************************************Bilibili**************************************
bool BiliID()
{
    int fans=-1,likes=-1,play=-1,follow=-1;
    static int ofans=-1,olikes=-1,oplay=-1,ofollow=-1;
    
    
    FansInfo fansInfo(biliID);
    UpInfo upInfo(biliID);  
    
    if(fansInfo.update()&&upInfo.update())
    {
        fans=fansInfo.getFansNumber();
        likes=upInfo.getLikes();
        play=upInfo.getPlay();
        follow=fansInfo.getFollowing();
        
        if(fans!=ofans||likes!=olikes||play!=oplay||follow!=ofollow)
        {
            tft.fillRect(25,175,70,15,TFT_BLACK);
            tft.fillRect(140,175,70,15,TFT_BLACK);
            tft.fillRect(25,268,70,15,TFT_BLACK);
            tft.fillRect(140,268,70,15,TFT_BLACK);
            ofans=fans;
            olikes=likes;
            oplay=play;
            ofollow=follow;
        }
        
        tft.unloadFont();
        tft.loadFont(NotoSansBold15);
        tft.setTextColor(tft.color565(255, 106, 155), TFT_BLACK);
        tft.drawCentreString(String(fansInfo.getFansNumber()),61,175,4);
        tft.drawCentreString(String(upInfo.getLikes()),172,175,4);
        tft.drawCentreString(String(upInfo.getPlay()),61,268,4);
        tft.drawCentreString(String(fansInfo.getFollowing()),172,268,4);
        tft.unloadFont();
        return true;
    } 
    else // 更新失败
    {     
        Serial.println("Update Fail...");  
        Serial.print("Server Response: ");       
        Serial.println(fansInfo.getServerCode()); 
        return false;
    }
}
//****************************************label****************************************
void wifi_label(int x,int y,int size,uint16_t color)
{
    uint8_t wifi[2][16]={
      {0x00,0x00,0x03,0xC0,0x1C,0x38,0x20,0x04,0x43,0xC2,0x0C,0x30,0x10,0x08,0x03,0xC0},
      {0x04,0x20,0x01,0x80,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}
    };
    lcd_display_chinese(x,y,size,wifi[0],color,-1,1);
}
void error_label(int x,int y,int size,uint16_t color)
{
    uint8_t error[2][16]={
      {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x10,0x04,0x20},
      {0x02,0x40,0x01,0x80,0x01,0x80,0x02,0x40,0x04,0x20,0x08,0x10,0x00,0x00,0x00,0x00}
    };
    lcd_display_chinese(x,y,size,error[0],TFT_RED,-1,1);
}
void connect_error_label(int x,int y,int size,uint16_t color)
{
    wifi_label(x,y,size,color);
    error_label(x,y-size*0.1,size,TFT_RED);
}
void return_label(int x,int y,int size,uint16_t color)
{
    /*
    uint8_t return_[2][16]={
      {0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x30,0x00,0x40,0x01,0x80,0x06,0x00,0x08,0x00},
{0x30,0x00,0x08,0x00,0x06,0x00,0x01,0x80,0x00,0x40,0x00,0x30,0x00,0x08,0x00,0x00}
    };
    lcd_display_chinese(x,y,size,return_[0],color,-1,1);
    */
    tft.drawLine(x,y+size/2,x+size,y,color);
    tft.drawLine(x,y+size/2,x+size,y+size,color);
}
//****************************************TIME*******************************************
bool display_time(int x,int y,int size,bool refresh)
{
    timeClient.update();
    if(size==0) return 0;
    if(mm==timeClient.getMinutes()&&!refresh) return 0;
    hh=timeClient.getHours();
    mm=timeClient.getMinutes();
    ss=timeClient.getSeconds();
    day=timeClient.getDay();
    tft.loadFont(NotoSansBold36);
    tft.fillRect(x,y,size*3.1,size,TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    
    if(hh>9&&mm>9)
        tft.drawString(String(hh)+":"+String(mm),x,y);
    else if(hh<=9&&mm>9)
        tft.drawString(String(hh)+":"+String(mm),x+13,y);
    else if(hh<=9&&mm<=9)
        tft.drawString(String(hh)+":0"+String(mm),x+13,y);
    else
        tft.drawString(String(hh)+":0"+String(mm),x,y);
    
    tft.unloadFont();
    return true;
}
//****************************************DATE*******************************************

void display_week(int x,int y,int size)
{
    //if(day==timeClient.getDay())return;
    day=timeClient.getDay();
    switch(day)
    {
      case 1:
      {
          uint8_t week[2][16]={
          {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0xFE},
          {0x7F,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"一",0*/
          };
          lcd_display_chinese(x+size*6,showtime_y+8,32,week[0],TFT_BLACK,TFT_YELLOW,1);
      }
      break;
      case 2:
      {
          uint8_t week[2][16]={
          {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xF8,0x00,0x00,0x00,0x00,0x00,0x00},
          {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0xFE,0x7F,0xFE,0x00,0x00,0x00,0x00}/*"二",0*/
          };
          lcd_display_chinese(x+size*6,showtime_y+8,32,week[0],TFT_WHITE,TFT_RED,1);
      }
      break;
      case 3:
      {
          uint8_t week[2][16]={
          {0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
          {0x07,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xFC,0x00,0x00,0x00,0x00}/*"三",0*/
          };
          lcd_display_chinese(x+size*6,showtime_y+8,32,week[0],TFT_BLACK,TFT_CYAN,1);
      }
      break;
      case 4:
      {
        uint8_t week[2][16]={
          {0x00,0x00,0x00,0x00,0x3F,0xFC,0x22,0x44,0x22,0x44,0x22,0x44,0x22,0x44,0x26,0x44},
          {0x24,0x44,0x2C,0x7C,0x38,0x04,0x20,0x04,0x20,0x04,0x3F,0xFC,0x20,0x04,0x00,0x00}/*"四",0*/
          };
          lcd_display_chinese(x+size*6,showtime_y+8,32,week[0],TFT_BLACK,TFT_GREEN,1);
      }
      break;
      case 5:
      {
          uint8_t week[2][16]={
          {0x00,0x00,0x00,0x00,0x3F,0xFC,0x3F,0xFC,0x01,0x00,0x03,0x00,0x03,0x00,0x1F,0xF0},
          {0x02,0x10,0x02,0x10,0x02,0x10,0x02,0x10,0x06,0x10,0x7F,0xFE,0x40,0x00,0x00,0x00}/*"五",0*/
          };
          lcd_display_chinese(x+size*6,showtime_y+8,32,week[0],TFT_BLACK,TFT_SKYBLUE,1);
      }
      break;
      case 6:
      {
          uint8_t week[2][16]={
          {0x00,0x00,0x00,0x00,0x03,0x00,0x01,0x80,0x00,0x80,0x7F,0xFE,0x7F,0xFE,0x00,0x00},
          {0x04,0x20,0x06,0x30,0x0C,0x30,0x08,0x18,0x18,0x0C,0x30,0x04,0x20,0x04,0x00,0x00}/*"六",0*/
          };
          lcd_display_chinese(x+size*6,showtime_y+8,32,week[0],TFT_BLACK,TFT_ORANGE,1);
      }
      break;
      case 0:
      {
          uint8_t week[2][16]={
          {0x00,0x00,0x1F,0xF8,0x1F,0xF8,0x18,0x08,0x18,0x08,0x18,0x08,0x18,0x08,0x1F,0xF8},
          {0x18,0x08,0x18,0x08,0x18,0x08,0x18,0x08,0x1F,0xF8,0x1F,0xF8,0x18,0x08,0x00,0x00}/*"日",0*/
          };
          lcd_display_chinese(x+size*6,showtime_y+8,32,week[0],TFT_BLACK,TFT_WHITE,1);
          //lcd_display_chinese(0,0,32,week[0],TFT_GREEN,TFT_RED,1);
      }
      break;
    }
}
void display_date(int x,int y,int size)
{
    //Serial.println("time");
    //unsigned long epochTime;
    //struct tm *ptm=gmtime((time_t*)&epochTime);
    //unsigned int monthDay=ptm->tm_mday;
    //unsigned int currentMonth=ptm->tm_mon+1;
    //unsigned int currentyear=ptm->tm_year-100;
        //Serial.println(stoi(weatherNow.getLastUpdate().substring(1,3)));
        tft.fillRect(x,y,size*6,size,TFT_BLACK);
        tft.loadFont(NotoSansBold15);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        //tft.drawString(String(currentyear-11474)+"-"+String(currentMonth-1)+"-"+String(monthDay-9),x,y);
        //tft.drawString(String(currentyear)+"-"+String(currentMonth)+"-"+String(monthDay),x,y);
        tft.drawString(weatherNow.getLastUpdate().substring(0,10),x,y);
        
        tft.unloadFont();
    
}
//****************************************weather****************************************
void Set_weather_location()
{
    switch(location)
    {
        case SHANTOU:   weather_location="shantou";break;
        case ZHUHAI:    weather_location="zhuhai";break;
        case GUANGZHOU: weather_location="guangzhou";break;
      case SHENZHEN:    weather_location="shenzhen";break;
    }
    weatherNow.config(reqUserKey, weather_location, reqUnit);
    forecast.config(reqUserKey, weather_location, reqUnit);
}
int Updata_weather()
{
    if(weatherNow.update()&&forecast.update())// 更新天气信息
    {  
        //Serial.println(code);
        weather_code=weatherNow.getWeatherCode();
        for(int i=0;i<3;i++)
        {
            forecast_code[i]=forecast.getDayCode(i);
            forecast_high[i]=forecast.getHigh(i);
            forecast_low[i]=forecast.getLow(i);
        }
        forecast_upday=forecast.getLastUpdate();
        //Serial.println(code);
        Start_weather=0;
        return weather_code;
    }
    Start_weather=1;
    return -1;
}
void display_weather(int x,int y,int size,int code,bool num)
{
     uint8_t C[2][16]={{0x00,0x00,0x20,0x00,0x50,0x00,0x50,0xF0,0x23,0x0C,0x02,0x04,0x06,0x00,0x04,0x00},
{0x04,0x00,0x04,0x00,0x04,0x04,0x06,0x04,0x03,0x0C,0x01,0xF8,0x00,0x00,0x00,0x00}};
    static bool error_bool=1;
    static int8_t old_code=-1;
    //if(code==old_code) error_bool=1;
    
    if(!error_bool&&code<0) 
    {
        if(first_show_weather) 
        {
          code=old_code;
          first_show_weather=0;
        }
        else 
        {
          return;
        }
    }
    old_code=code;
    //Serial.print("weather_code:");Serial.println(code);
    if(code>=0) tft.fillRect(x-size-10,y-size,size*3.1,size*4,TFT_BLACK);
    updata_weather=0;
    if(code==0||code==1)//晴
    {
        tft.fillCircle(x,y,size,TFT_ORANGE);
    }
    else if(code>=4&&code<=6)//多云
    {
        tft.fillCircle(x,y,size,TFT_ORANGE);
        tft.fillCircle(x+size*0.5,y+size*0.5,size,TFT_WHITE);
        tft.fillCircle(x-size*0.66,y+size*0.75,size*0.75,TFT_WHITE);
        tft.fillRect(x-size*1.5,y+size*1.25,size*3,size*0.5,TFT_BLACK);
    }
    else if(code==7||code==8||code==9)//阴
    {
      
        tft.fillCircle(x+size*0.5,y+size*0.5,size,TFT_WHITE);
        tft.fillCircle(x-size*0.66,y+size*0.75,size*0.75,TFT_WHITE);
        tft.fillRect(x-size*1.5,y+size*1.25,size*3,size*0.5,TFT_BLACK);
    }
    else if(code==10||(code>=13&&code<=19))//雨
    {
        uint8_t rain[2][16]={{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x22,0x22,0x66,0x66,0xEE,0xEE},
        {0xCC,0xCC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}};
        tft.fillCircle(x+size*0.5,y,size,TFT_WHITE);
        tft.fillCircle(x-size*0.66,y+size*0.25,size*0.75,TFT_WHITE);
        tft.fillRect(x-size*1.5,y+size*0.75,size*3,size*0.5,TFT_BLACK);
        lcd_display_chinese(x-size*0.7,y+size*0.50,32,rain[0],TFT_SKYBLUE,-1,1);
        
    }
    else if(code==11||code==12)//雷
    {
        tft.fillCircle(x+size*0.5,y,size,TFT_WHITE);
        tft.fillCircle(x-size*0.66,y+size*0.25,size*0.75,TFT_WHITE);
        tft.fillRect(x-size*1.5,y+size*0.75,size*3,size*0.5,TFT_BLACK);
        uint8_t flash[2][16]={
            {0x00,0x00,0x03,0xF8,0x03,0xF0,0x07,0xF0,0x07,0xE0,0x0F,0xFC,0x1F,0xF8,0x03,0xF0},
            {0x03,0xE0,0x07,0xC0,0x07,0x80,0x0F,0x00,0x0C,0x00,0x18,0x00,0x10,0x00,0x00,0x00}
        };
        lcd_display_chinese(x-size*0.8,y,32,flash[0],TFT_ORANGE,-1,1);
    }
    else if(code<0&&error_bool)
    {
        error_bool=0;
        connect_error_label(x-size*0.5,y-size*0.5,32,TFT_WHITE);
    }
    else if(code>=0)
    {
        uint8_t unknow[2][16]={
            {0x00,0x00,0x07,0xC0,0x0F,0xE0,0x1C,0x70,0x18,0x30,0x18,0x30,0x00,0x70,0x01,0xE0},
            {0x01,0xC0,0x01,0x80,0x01,0x80,0x01,0x80,0x00,0x00,0x01,0x80,0x01,0x80,0x00,0x00}
        };
        lcd_display_chinese(x-size*0.5,y-size*0.5,32,unknow[0],TFT_RED,-1,1);
    }
    if(num)
    {
        tft.loadFont(NotoSansBold15);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.drawString(String(weatherNow.getDegree()),x-size,y+size*1.6);
        lcd_display_chinese(x+size*0.15,y+size*1.55,16,C[0],TFT_WHITE,TFT_BLACK,1);
        tft.unloadFont();
    }
  /*
    uint8_t error[2][16]={
      {0x07,0xC0,0x0F,0xE0,0x1F,0xF0,0x1C,0x70,0x1C,0x70,0x00,0xE0,0x01,0xC0,0x03,0x80},
      {0x03,0x80,0x03,0x80,0x03,0x80,0x00,0x00,0x00,0x00,0x03,0x80,0x03,0x80,0x03,0x80}
    };
    //lcd_display_chinese(x-size*0.5,y-size*0.5,32,error[0],TFT_RED,TFT_BLACK,1);
    */
  
}
//****************************************init*******************************************
#define SD_CS 16;

void Init()
{
    Serial.begin(115200);
    SPIFFS.begin();
    pinMode(5,OUTPUT);
    pinMode(9,OUTPUT);
    digitalWrite(5,HIGH);
    digitalWrite(9,HIGH);
    tft.init();
    tft.fillScreen(TFT_BLACK);
    WiFi.mode(WIFI_STA);
    /*
    if (!WiFi.config(local_IP, gateway, subnet)) 
    {
        Serial.println("Failed to Config ESP8266 IP"); 
    }
    */
    if (SPIFFS.exists(WIFI_DATA))
    {
        Serial.print(WIFI_DATA);
        Serial.println(" FOUND.");
        
        File dataFile = SPIFFS.open(WIFI_DATA, "r");
        char temp;
        while(1)
        {
            //Serial.print((char)dataFile.read());
            temp=(char)dataFile.read();
            if(temp=='\r') break;
            ssid+=temp;
        }
        dataFile.read();
        Serial.println(ssid);
        while(1)
        {
            temp=(char)dataFile.read();
            if(temp=='\r') break;
            password+=temp;
        }
        Serial.println(password);
        delay(100);
        WiFi.begin(ssid,password);
        dataFile.close();
        //delay(100);
        //httpserver();
    }
    else
    {
        Serial.print(WIFI_DATA);
        Serial.println("not  FOUND.");
    }
     
    listFiles();
    char temp;
    HOME_background.clear();
    if(SPIFFS.exists(HOME_bgpath))
    {
        File photo_data=SPIFFS.open(HOME_bgpath,"r");
        while(1)
        {
            temp=(char)photo_data.read();
            if(temp=='\r') break;
            HOME_background+=temp;
        }
        photo_data.close();
    }
    else
    {
        File photo_data=SPIFFS.open(HOME_bgpath,"w");
        photo_data.println("/data/img/homebg.jpg");
        HOME_background="/data/img/homebg.jpg";
        photo_data.close();
    }
    //SD.begin();
    
    timeClient.begin();
    timeClient.setTimeOffset(28800);
    weatherNow.config(reqUserKey, weather_location, reqUnit);
    forecast.config(reqUserKey, weather_location, reqUnit);
    if(SPIFFS.exists(LOCATION_DATA))
    {
        File dataFile = SPIFFS.open(LOCATION_DATA, "r");
        char temp;
        location=0;
        for(int i=1;i>=0;i--)
        {
            temp=(char)dataFile.read();
            location+=(temp-48)*pow(10,i); 
            Serial.println(temp);
        }  
        dataFile.close();
        Serial.println("Location:"+String(location));
    }
    else
    {
        Serial.println("Location:"+String(location));
        File dataFile = SPIFFS.open(LOCATION_DATA, "w");
        dataFile.println("00");      
        dataFile.close();
    }

    if(SPIFFS.exists(BILI_DATA))
    {
        File dataFile = SPIFFS.open(BILI_DATA, "r");
        biliID.clear();
        char temp;
        while(1)
        {
            //Serial.print((char)dataFile.read());
            temp=(char)dataFile.read();
            if(temp=='\r') break;
            biliID+=temp;
        }   
        dataFile.close();
        Serial.println(biliID); 
    }
    else 
    {
        File dataFile = SPIFFS.open(BILI_DATA, "w");
        dataFile.println("00000000");      
        dataFile.close();
    }
        
    
    if(SPIFFS.exists(THEME_DATA))
    {
        File dataFile = SPIFFS.open(THEME_DATA, "r");
        char temp;
        first_display=0;
        home_display=0;
        for(int i=1;i>=0;i--)
        {
            temp=(char)dataFile.read();
            first_display+=(temp-48)*pow(10,i); 
            Serial.println(temp);
        }
        for(int i=1;i>=0;i--)
        {
            temp=(char)dataFile.read();
            home_display+=(temp-48)*pow(10,i); 
            Serial.println(temp);
        }
        dataFile.close();
        Serial.println("first_display:"+String(first_display));
    }
    else
    {
        Serial.println("first_display:"+String(first_display));
        File dataFile = SPIFFS.open(THEME_DATA, "w");
        dataFile.println("0107");      
        dataFile.close();
    }
    RUN_MODE=first_display;
    RUN_MODE_OLD=first_display;
    RUN_MODE_LAST=first_display;
    Set_weather_location();
}
//****************************************ticker*****************************************

//void Updata_Time()        {updata_time=1;}
void Updata_Weather()     {updata_weather=1;}
void End_Start()          {Start--;if(Start<1)start_Ticker.detach();}
void BilibiliUpdata()     {bilibili_updata=1;}

void Init_ticker()
{
    WiFiconnectTicker.attach(5,CheckWiFiBool);
    //updata_timeTicker.attach(30,Updata_Time);
    updata_weatherTicker.attach(3600,Updata_Weather);
    start_Ticker.attach(1,End_Start);
    bilibiliTicker.attach_ms(500,BilibiliUpdata);
}
//****************************************Init_touch**************************************

void touch_calibrate()
{
  uint16_t calData[5];
  uint8_t calDataOK = 0;

  // check file system exists
  /*
  if (!SPIFFS.begin()) {
    Serial.println("Formating file system");
    SPIFFS.format();
    SPIFFS.begin();
  }
  */
  // check if calibration file exists and size is correct
  if (SPIFFS.exists(CALIBRATION_FILE)) {
    if (false)
    {
      // Delete if we want to re-calibrate
      SPIFFS.remove(CALIBRATION_FILE);
      Serial.println("remove");
    }
    else
    {
      File f = SPIFFS.open(CALIBRATION_FILE, "r");
      if (f) {
        if (f.readBytes((char *)calData, 14) == 14)
          calDataOK = 1;
        f.close();
      }
    }
  }

  if (calDataOK && !false) 
  {
    // calibration data valid
    tft.setTouch(calData);
  } 
  else 
  {
    // data not valid so recalibrate
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(20, 0);
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.println("Touch corners as indicated");

    tft.setTextFont(1);
    tft.println();

    if (false) 
    {
      tft.setTextColor(TFT_RED, TFT_BLACK);
      tft.println("Set REPEAT_CAL to false to stop this running again!");
    }

    tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println("Calibration complete!");

    // store data
    File f = SPIFFS.open(CALIBRATION_FILE, "w");
    if (f) 
    {
      f.write((const unsigned char *)calData, 14);
      f.close();
    }
  }
}

//****************************************connect****************************************
bool checkwifi=0;
void CheckWiFiBool()
{
    checkwifi=1;
}
bool CheckWiFi(int wait=30)
{
  checkwifi=0;
  int i=0;
  while (i<wait&&WiFi.status() != WL_CONNECTED) 
  {
    delay(100);
    Serial.print(i++); Serial.print(' ');
  }
  if(i<wait)return true;
  else return false;
}
bool connectwifi(int wait=30)
{
  if (SPIFFS.exists(WIFI_DATA))
  {
    Serial.print(WIFI_DATA);
    Serial.println(" FOUND.");
    
    File dataFile = SPIFFS.open(WIFI_DATA, "r");
    char temp;
    ssid.clear();
    password.clear();
    while(1)
    {
        temp=(char)dataFile.read();
        if(temp=='\r') break;
        ssid+=temp;
    }
    dataFile.read();//消除\n
    Serial.println(ssid);
    while(1)
    {
        temp=(char)dataFile.read();
        if(temp=='\r') break;
        password+=temp;
    }
    Serial.println(password);
    dataFile.close();
    delay(100);
    WiFi.begin(ssid,password);
    if(CheckWiFi(wait)) return true;
    else return false;
  } 
  else 
  {
    Serial.print(WIFI_DATA);
    Serial.print(" NOT FOUND.");
    return false;
  }
}
//**********************************************setAP******************************************
void handleSetAP()
{
    //esp8266_server.close();
    initSoftAP();
    initWebServer();
    initDNS();
    Serial.print("ap111");
}

void handleRoot()
{
  // 如果在SPIFFS无法找到用户访问的资源，则回复404 (Not Found)
  if (!handleFileRead_ap(connect_server.uri())){  
    Serial.println("404");                                               
    connect_server.send(404, "text/plain", "404 Not Found"); 
  }
}

bool handleFileRead_ap(String path) {            //处理浏览器HTTP访问
  //Serial.print("g:  ");
  //Serial.println(path);
  if(path.endsWith("204")||path.endsWith("/"))
  {
    path = "/system/wifi/setap.html";
  }
  
  String contentType = getContentType(path);  // 获取文件类型
  
  if (SPIFFS.exists(path)) // 如果访问的文件可以在SPIFFS中找到
  {
    File file = SPIFFS.open(path, "r");          // 则尝试打开该文件
    Serial.println("exists");
    connect_server.streamFile(file, contentType);// 并且将该文件返回给浏览器
    file.close();                                // 并且关闭文件
    return true;                                 // 返回true
  }
  Serial.println("not found");
  return false;                                  // 如果文件未找到，则返回false
}
//int ip1,ip2,ip3,ip4;
void handleRootPost() {//Post回调函数
  Serial.println("handleRootPost");
  if (connect_server.hasArg("ssid")) {//判断是否有账号参数
    Serial.print("got ssid:");
    strcpy(sta_ssid, connect_server.arg("ssid").c_str());//将账号参数拷贝到sta_ssid中
    Serial.println(sta_ssid);
  } else {//没有参数
    Serial.println("error, not found ssid");
    connect_server.send(200, "text/html", "<meta charset='UTF-8'>error, not found ssid");//返回错误页面
    return;
  }
  //密码与账号同理
  if (connect_server.hasArg("password")) {
    Serial.print("got password:");
    strcpy(sta_password, connect_server.arg("password").c_str());
    Serial.println(sta_password);
  } else {
    //Serial.println("error, not found password");
    connect_server.send(200, "text/html", "<meta charset='UTF-8'>error, not found password");
    return;
  }
  //返回保存成功页面
  delay(100);
    File dataFile = SPIFFS.open(WIFI_DATA, "w");
    dataFile.println(sta_ssid);       
    dataFile.println(sta_password);       
    dataFile.close();
  connect_server.send(200, "text/html", "<meta charset='UTF-8'>已保存");
  delay(1000);
  WiFi.mode(WIFI_STA);
  WiFi.begin(sta_ssid,sta_password);
  if(CheckWiFi(70))
  {
    connect_server.close();
    //httpserver();
    //shift=0;
  }
  else
  {
    //shift=1;
  }
}
void initSoftAP(void){//初始化AP模式
  IPAddress local_IP(192, 168, 0, 123);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(local_IP, local_IP, IPAddress(255, 255, 255, 0));
  if(WiFi.softAP(AP_NAME)){
    Serial.println("ESP8266 SoftAP is right");
  }
}

void initWebServer(void){//初始化WebServer
  //server.on("/",handleRoot);
  //上面那行必须以下面这种格式去写否则无法强制门户
  connect_server.on("/", HTTP_GET, handleRoot);//设置主页回调函数

  connect_server.onNotFound(handleRoot);//设置无法响应的http请求的回调函数
  connect_server.on("/", HTTP_POST, handleRootPost);//设置Post请求回调函数/clean
  connect_server.on("/clean", HTTP_POST, ResetData);
  
  connect_server.begin();//启动WebServer
  Serial.println("WebServer started!");
}

void initDNS(void){//初始化DNS服务器
  IPAddress local_IP(192, 168, 0, 123);
  if(dnsServer.start(53, "*", local_IP)){//判断将所有地址映射到esp8266的ip上是否成功
    Serial.println("start dnsserver success.");
  }
  else Serial.println("start dnsserver failed.");
}

void ResetData()
{
    SPIFFS.remove(WIFI_DATA);
    connect_server.send(200, "text/html", "<meta charset='UTF-8'>重置成功");
    handleSetAP();
}

String getContentType(String filename)
{
  if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

void lcd_display_chinese_32(uint16_t x, uint16_t y, const uint8_t *p, uint16_t color,int32_t bgcolor)
{
    int i,j,k,sx,sy;
    for(i=0;i<32;i++)
    {
        for(j=0;j<4;j++)
        {
            for(k=7;k>=0;k--)
            {
                sx=j*8+(7-k)+x;
                sy=i+y;
                if((*((p+i*4+j))>>k) & 0x01)
                  tft.drawPixel(sx,sy,color);
                else if(bgcolor>=0)
                  tft.drawPixel(sx,sy,bgcolor);
            }
        }
    }
}
void lcd_display_chinese(uint16_t x, uint16_t y, uint8_t size, const uint8_t *p, uint16_t color,int32_t bgcolor,int number)
{
    int i, j, k,l; 
    uint8 temp, temp1, temp2;
    const uint8 *p_data;
    int nx,ny;
    
    temp1 = number;
    temp2 = size/16;//映射比例
    while(temp1--)
    {
        for(i=0;i<size;i++)//y
        {
            for(l=0;l<2;l++)
            {
                p_data=p+2*(i/temp2)+l;
                for(j=0;j<size/2;j++)
                {   
                    nx=l*size/2+j;
                    if(7-j/temp2<8)temp = (*p_data>>(7-j/temp2)) & 0x01;
                    else temp = (*p_data>>(7-j/temp2+1)) & 0x01;
                    if(temp) tft.drawPixel(x+nx,y+i,color);
                    else if(bgcolor>=0) tft.drawPixel(x+nx,y+i,bgcolor);
                }
            }
        }
    }
}
void DrawRect(int sx,int sy,int ex,int ey,uint16_t color)
{
    tft.drawRect(sx,sy,ex-sx,ey-sy,color);
}
void DrawRect(struct ButtonRect button,uint16_t color)
{
    tft.drawRect(button.sx,button.sy,button.ex-button.sx,button.ey-button.sy,color);
}
