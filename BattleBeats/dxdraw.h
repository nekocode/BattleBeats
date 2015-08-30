#include <Windows.h>
#include <tchar.h>
#include <d3d9.h>
#include <list>
#include <cmath>
#include "Bezier.h"
#include "textdraw.h"
#include "dxinput.h"
#include "dxImage.h"
#include "sleep.h"
#include "music.h"

#define GameWidth 640
#define GameHeight 400
#define btnTime 25

#define SCENE_INIT 0
#define SCENE_LOGO -1
#define SCENE_SELC_SONG -2
#define SCENE_SELC_SONG_OUT -3
#define SCENE_GAME 1
#define SCENE_GAME_OVER -4
#define SCENE_GAME_FIN -5

extern HWND hwnd;

//子弹类型:普通,圆周攻击,追踪,星星(round2为敌人子弹)
enum bulletType {b_common, b_kattack, b_line, b_enemy_1, b_star};
//弹道列表
struct BulletPath {
	bulletType bt;
	byte fft;
	byte fft_r;
	byte fft_g;
	byte fft_b;

	float nowx,nowy;	//当前坐标
	float vx,vy;		//当前速度
	float ax,ay;		//当前加速度

	byte color_a;		//子弹颜色
	byte color_r;
	byte color_g;
	byte color_b;
	float extra;		//额外信息
	float extra_2;		//额外信息2
	float extra_3;		//额外信息3

	dximg *img;
	BulletPath *front;	//上一路径
	BulletPath *next;	//下一路径
};

enum enemyType {e_stone, e_1, e_2, e_3, e_4, e_boss};	//
//敌人列表
struct Enemy {
	enemyType etype;
	int e_img_index;	//图片索引
	float hp;			//hp值
	bool inattack;		//是否被攻击中

	float nowx, nowy;	//当前坐标
	float rota;			//当前旋转弧度
	float vx,vy;		//当前速度
	float ax,ay;		//当前加速度

	float width, height;	//敌机宽高
	int path_type;			//敌机飞行路径type 0:正常模式 >0:bezier模式

	float time;			//时间控制
	float extra;		//额外信息
	bool fly_in;		//是否处于飞入屏幕过程

	dximg *img;
	Enemy *front;
	Enemy *next;
};

//加速运动辅助类
class AccelerateHelper {
public:
	//目标x,y
	int target_x, target_y;
	float now_x, now_y;
	float step;

	AccelerateHelper() {
		step = btnTime;
	}

	void setNowPos(int x, int y) {
		now_x = x;
		now_y = y;
		target_x = x;
		target_y = y;
	};

	void setTargetPos(int x, int y) {
		xis_finished = false;
		yis_finished = false;
		target_x = x; target_y = y;

		//获取x和y轴的路径长度
		float S_x = (target_x - now_x);
		float S_y = (target_y - now_y);

		//获取x和y轴的初速度
		v0_x = S_x * 2 / step;
		v0_y = S_y * 2 / step;

		//获取x和y轴的加速度
		if(S_x==0)
			a_x = 0;
		else
			a_x = - v0_x * v0_x / (S_x * 2);

		if(S_y==0)
			a_y = 0;
		else
			a_y = - v0_y * v0_y / (S_y * 2);
	}

	void nextPos() {
		//一秒钟的路程
		float s_x=0;
		float s_y=0;

		if(a_x<0) {
			if(v0_x>0) {
				s_x = v0_x + 0.5 * a_x;
				v0_x = v0_x + a_x;
			} else {
				s_x = 0;
				xis_finished = true;
			}
		}
		else {
			if(v0_x<0) {
				s_x = v0_x + 0.5 * a_x;
				v0_x = v0_x + a_x;
			} else {
				s_x = 0;
				xis_finished = true;
			}
		}

		if(a_y<0) {
			if(v0_y>0) {
				s_y = v0_y + 0.5 * a_y;
				v0_y = v0_y + a_y;
			} else {
				s_y = 0;
				yis_finished = true;
			}
		} else {
			if(v0_y<0) {
				s_y = v0_y + 0.5 * a_y;
				v0_y = v0_y + a_y;
			} else {
				s_y = 0;
				yis_finished = true;
			}
		}

		now_x = now_x+s_x;
		now_y = now_y+s_y;
	}

	boolean isFinished() {
		return (yis_finished && xis_finished);
	}
	
private:
	float a_x,a_y;
	float v0_x,v0_y;
	bool xis_finished, yis_finished;
};

//自定义函数声明
bool InitD3D();
void D3DRelease();
void Render();
void initGame();
void addEnemy(enemyType etype,float hp,float nowx,float nowy, float vx = 0.0f, float vy = 0.0f, float ax = 0.0f, float ay = 0.0f, int amp_track = 10);