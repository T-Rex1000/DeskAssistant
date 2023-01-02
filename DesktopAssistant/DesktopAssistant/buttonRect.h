#ifndef BUTTON_RECT
#define BUTTON_RECT

#define CLICK   1
#define LONG	2
#define SLIP	3
#define PRESS   4

#define  UP		5
#define	DOWN	  6
#define	LEFT	  7
#define	RIGHT	8

#define SLIP_UP		15
#define	SLIP_DOWN	16
#define	SLIP_LEFT	17
#define	SLIP_RIGHT	18

int touch_check(struct ButtonRect button);
void DrawRect(struct ButtonRect button);
extern int aaa;

struct TOUCH
{
	short contouch;
	int x;
	int y;
};/*
extern struct TOUCH touch;
*/
struct ButtonRect
{
	int sx;
	int sy;
	int ex;
	int ey;
};
extern struct ButtonRect HOME_weather;
extern struct ButtonRect HOME_time;
extern struct ButtonRect HOME_photo; 
extern struct ButtonRect RETURN_button; 
extern struct ButtonRect Weather_location;
extern struct ButtonRect screen;

//typedef touch.x touch.press;
//typedef touch.y touch.release;

#endif // !BUTTON_RECT
