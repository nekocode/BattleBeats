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

//�ӵ�����:��ͨ,Բ�ܹ���,׷��,����(round2Ϊ�����ӵ�)
enum bulletType {b_common, b_kattack, b_line, b_enemy_1, b_star};
//�����б�
struct BulletPath {
	bulletType bt;
	byte fft;
	byte fft_r;
	byte fft_g;
	byte fft_b;

	float nowx,nowy;	//��ǰ����
	float vx,vy;		//��ǰ�ٶ�
	float ax,ay;		//��ǰ���ٶ�

	byte color_a;		//�ӵ���ɫ
	byte color_r;
	byte color_g;
	byte color_b;
	float extra;		//������Ϣ
	float extra_2;		//������Ϣ2
	float extra_3;		//������Ϣ3

	dximg *img;
	BulletPath *front;	//��һ·��
	BulletPath *next;	//��һ·��
};

enum enemyType {e_stone, e_1, e_2, e_3, e_4, e_boss};	//
//�����б�
struct Enemy {
	enemyType etype;
	int e_img_index;	//ͼƬ����
	float hp;			//hpֵ
	bool inattack;		//�Ƿ񱻹�����

	float nowx, nowy;	//��ǰ����
	float rota;			//��ǰ��ת����
	float vx,vy;		//��ǰ�ٶ�
	float ax,ay;		//��ǰ���ٶ�

	float width, height;	//�л����
	int path_type;			//�л�����·��type 0:����ģʽ >0:bezierģʽ

	float time;			//ʱ�����
	float extra;		//������Ϣ
	bool fly_in;		//�Ƿ��ڷ�����Ļ����

	dximg *img;
	Enemy *front;
	Enemy *next;
};

//�����˶�������
class AccelerateHelper {
public:
	//Ŀ��x,y
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

		//��ȡx��y���·������
		float S_x = (target_x - now_x);
		float S_y = (target_y - now_y);

		//��ȡx��y��ĳ��ٶ�
		v0_x = S_x * 2 / step;
		v0_y = S_y * 2 / step;

		//��ȡx��y��ļ��ٶ�
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
		//һ���ӵ�·��
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

//�Զ��庯������
bool InitD3D();
void D3DRelease();
void Render();
void initGame();
void addEnemy(enemyType etype,float hp,float nowx,float nowy, float vx = 0.0f, float vy = 0.0f, float ax = 0.0f, float ay = 0.0f, int amp_track = 10);