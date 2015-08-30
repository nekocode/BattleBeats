#include "dxdraw.h"
#include <ctime>
//#define valBuffer 195
using namespace std;

void setupViewAndProjMat();
void control();
void addBullet(bulletType bt,float nowx,float nowy,float vx,float vy,float ax,float ay,float extra=0.0f,float extra_2=0.0f,float extra_3=0.0f);
void addBomb(float nowx, float nowy, int bomb_max_size=50, float speed=2.0f);
void UIRender();
void graphicRender();
void backgroundRender();
void updataNowThemeColor();
void bulletRender();
void enemyRender();
void drawBeat(int fft, float x, float y, byte r, byte g, byte b, float ratio_scal = 1.0f);
void clearGameList();
void createEmeny();

LPDIRECT3D9 g_pD3D = NULL;              //Direct3D����
IDirect3DDevice9* g_pd3dD;              //Direct3D�豸����

dximg img_logo;
dximg img_gameover;
dximg img_gamefin;
dximg img_player;
dximg img_enemy[4];
dximg img_stone[3];
dximg img_boss;
dximg img_bullet;
dximg img_star;
dximg img_beat;
dximg img_button;
dximg img_progress;

//�ӵ��б�
BulletPath *bpnow,*bphead;
//�л��б�
Enemy *emynow,*emyhead;

//��ť�б�
std::vector<AccelerateHelper*> vector_btn;
int btn_selected;

extern float hz;
extern std::vector<MusicInfo*> vector_music;
extern int nowplaying_position;		//��ǰ���ŵĸ���
extern unsigned int song_pos;		//��ǰ���Ÿ���λ��
extern int amp_left[NUM_FFT+1];
extern int amp_right[NUM_FFT+1];
extern unsigned int nowplaying_song_length;


mytime time_attack;
mytime k_attack;
mytime time_inattack;
mytime me_time_inattack;
mytime time_target;

#define MAX_HP 100.0f
#define MAX_KATTACK 10.0f
#define MAX_HIDE 200.0f

//�Ƿ�����״̬
bool me_is_hide = false;
//��������ֵ
float point_hide = 200.0f;
//KAttackֵ
float point_kattack = 1.0f;
bool kattacking = false;
//meHP
float point_hp = 100.0;
bool me_inattack = false;
int attack_interval = 40;

float point_score = 0.0f;

bool is_gameover = false;
bool is_gamefin = false;

extern int scene;
//extern bool addnew;

float now_theme_color_r = 255.0f;
float now_theme_color_g = 255.0f;
float now_theme_color_b = 255.0f;

//==================================
//  ����:Render()
//  Ŀ��:��Ⱦͼ�β���ʾ
//
void Render() {
	if(NULL == g_pd3dD)
		return;

	g_pd3dD->Clear(0,NULL,D3DCLEAR_TARGET && D3DCLEAR_ZBUFFER,0x00000000,1.0f,0);

	if(SUCCEEDED(g_pd3dD->BeginScene())) {
		updataNowThemeColor();

		int fft = amp_left[2];
		if(fft>70) {
			fft = (128 - fft)/6;
			attack_interval = fft;
		} else
			attack_interval = 40;


		if(scene>0 && !is_gameover) {
			control();
		}

		graphicRender();
		//�����ں�̨������Ⱦͼ��
		g_pd3dD->EndScene();
	}


	//���ں�̨������Ƶ�ͼ���ύ��ǰ̨������ʾ
	g_pd3dD->Present(NULL,NULL,NULL,NULL);
}


//==================================
//  ����:control()
//  Ŀ��:���̿���
//
void control() {
	bool up,down,left,right,slow,attack,kattack,hide;
	int speed_ud=4, speed_lr=5;
	static int old_ud=0,old_lr=0;

	up = KeyIsDown(DIK_W);
	down = KeyIsDown(DIK_S);
	left = KeyIsDown(DIK_A);
	right = KeyIsDown(DIK_D);
	slow = KeyIsDown(DIK_SPACE);
	attack = KeyIsDown(DIK_J);
	kattack = KeyIsDown(DIK_K);
	hide = KeyIsDown(DIK_L);

	//�ж��Ƿ��¼��ټ�
	if(slow)
	{
		speed_ud = 2;
		speed_lr = 2;
	}

	//�жϷ����Ϻͷ����¼�
	if(up&&down)
	{
		if(old_ud==1)
		{
			if(img_player.nowy+img_player.height<GameHeight)
				img_player.setpos(img_player.nowx,img_player.nowy+speed_ud);
		}
		else if(old_ud==2)
		{
			if(img_player.nowy>0)
				img_player.setpos(img_player.nowx,img_player.nowy-speed_ud);
		}
	}
	else
	{
		if(up)
		{
			if(img_player.nowy>0)
				img_player.setpos(img_player.nowx,img_player.nowy-speed_ud);
			old_ud = 1;
		}
		else if(down)
		{
			if(img_player.nowy+img_player.height<GameHeight)
				img_player.setpos(img_player.nowx,img_player.nowy+speed_ud);
			old_ud = 2;
		}
	}

	//�жϷ�����ͷ����Ҽ�
	if(left&&right)
	{
		if(old_lr==1)
		{
			if(img_player.nowx+img_player.width/2<GameWidth) {
				img_player.setpos(img_player.nowx+speed_lr,img_player.nowy);
			}
		}
		else if(old_lr==2)
		{
			if(img_player.nowx+img_player.width/2>0) {
				img_player.setpos(img_player.nowx-speed_lr,img_player.nowy);
			}
		}
	}
	else
	{
		if(left)
		{
			if(img_player.nowx+img_player.width/2>0) {
				img_player.setpos(img_player.nowx-speed_lr,img_player.nowy);
			}
			old_lr = 1;
		}
		else if(right)
		{
			if(img_player.nowx+img_player.width/2<GameWidth) {
				img_player.setpos(img_player.nowx+speed_lr,img_player.nowy);
			}
			old_lr = 2;
		}
	}

	//�ж��Ƿ��¹�����,40ms�ж�һ��
	if(time_attack.sleep(attack_interval))
	{
		if(attack)
		{
			int launch_x = img_player.nowx+img_player.width/2-img_bullet.width/2;
			int launch_y = img_player.nowy-img_bullet.height+8;

			addBullet(b_common, launch_x-15, launch_y+17, 0.0f, -10.0f, 0.0f, 0.0f, 0.0f);
			addBullet(b_common, launch_x+15, launch_y+17, 0.0f, -10.0f, 0.0f, 0.0f, 1.0f);

			addBullet(b_common, launch_x-5, launch_y, 0.0f, -7.0f, 0.0f, 0.0f, 1.0f);
			addBullet(b_common, launch_x+5, launch_y, 0.0f, -7.0f, 0.0f, 0.0f, 0.0f);
		}
	}

	//����Բ�ܹ�����
	if(kattack&&!kattacking&&(point_kattack>=1.0f))
	{
		kattacking = true;
		point_kattack-=1.0f;
	}
	if(kattacking&&k_attack.sleep(35))
	{
		static double i = 0;
		float s = sin(i)*14,c = cos(i)*14;
		MyMusic::playAudio(4);
		addBullet(b_kattack, img_player.nowx+img_player.width/2-img_bullet.width/2, img_player.nowy-img_bullet.height+25, -s, -c, 0.0f, 0.0f, 2.0f, 1.0f, 0.0f);
		addBullet(b_kattack, img_player.nowx+img_player.width/2-img_bullet.width/2, img_player.nowy-img_bullet.height+25, s,  c, 0.0f, 0.0f, 2.0f, 1.0f, 1.0f);
		addBullet(b_kattack, img_player.nowx+img_player.width/2-img_bullet.width/2, img_player.nowy-img_bullet.height+25, -c,  s, 0.0f, 0.0f, 2.0f, 1.0f, 0.0f);
		addBullet(b_kattack, img_player.nowx+img_player.width/2-img_bullet.width/2, img_player.nowy-img_bullet.height+25, c, -s, 0.0f, 0.0f, 2.0f, 1.0f, 1.0f);
		i += 6.28318530/80;
		if(i>6.28318530) {
			i = 0;
			kattacking = false;
		}
	}

	//���������
	if(hide) {
		if(point_hide>=1.0f) {
			me_is_hide = true;
			point_hide-=1.0f;
		}
		else
			me_is_hide = false;
	}
	else
		me_is_hide = false;
}




#define BTN_WIDTH 245
#define BTN_HEIGHT 76
#define BTN_SCREEN_CENTER_Y 162		//400/2-76/2
#define BTN_NORMAL_X 395			//640-245
#define PROGRESS_WIDTH_TIME 600.0f
#define PROGRESS_HEIGHT_NORAML 1.0f
#define PROGRESS_WIDTH_NORAML 80.0f
mytime timer_interval_keycheck_up;
mytime timer_interval_keycheck_down;
mytime timer_interval_qucikdrag;

//==================================
//  ����:UIRender()
//  Ŀ��:������Ⱦ
//
void UIRender() {
	switch (scene) {
	case SCENE_SELC_SONG:
	case SCENE_SELC_SONG_OUT: {
		bool need_refresh = false;
		bool up_isdown = false;
		bool down_isdown = false;
		static bool up_isdown_old = false;
		static bool down_isdown_old = false;
		static bool quick_drag = false;
		static int old_btn_selected = 0;

		if(scene == SCENE_SELC_SONG) {
			up_isdown = KeyIsDown(DIK_UP);
			down_isdown = KeyIsDown(DIK_DOWN);
		}

		//��ס��ť����2�����������϶�ģʽ
		if(up_isdown && !down_isdown && up_isdown_old) {
			if(timer_interval_keycheck_up.sleep(120))
				quick_drag = true;
		} else
			timer_interval_keycheck_up.clear();

		if(!up_isdown && down_isdown && down_isdown_old) {
			if(timer_interval_keycheck_down.sleep(120))
				quick_drag = true;
		} else
			timer_interval_keycheck_down.clear();

		if(quick_drag && timer_interval_qucikdrag.sleep(60)) {
			if(up_isdown) {
				if(btn_selected != 0)
					btn_selected--;
				need_refresh = true;
			} else if(down_isdown) {
				if(btn_selected != vector_btn.size()-1)
					btn_selected++;
				need_refresh = true;
			} else {
				quick_drag = false;
				timer_interval_qucikdrag.clear();
			}
		}

		if(up_isdown && !down_isdown && !up_isdown_old) {
			if(btn_selected != 0)
				btn_selected--;
			need_refresh = true;
		} else if (!up_isdown && down_isdown && !down_isdown_old) {
			if(btn_selected != vector_btn.size()-1)
				btn_selected++;
			need_refresh = true;
		}
		up_isdown_old = up_isdown;
		down_isdown_old = down_isdown;

		if(need_refresh) {
			MyMusic::playAudio(1);

			vector_btn[0]->setTargetPos(BTN_NORMAL_X, BTN_SCREEN_CENTER_Y - btn_selected*img_button.height);
			need_refresh = false;
		}

		if(old_btn_selected != btn_selected && !up_isdown && !down_isdown) {
			MyMusic::playMusic(btn_selected);
			old_btn_selected = btn_selected;
		}


		//����btn�б��λ��
		vector_btn[0]->nextPos();
		int btn0_nowx = vector_btn[0]->now_x;
		int btn0_nowy = vector_btn[0]->now_y;
		for(int i=1; i<vector_btn.size(); i++)
			vector_btn[i]->setNowPos(btn0_nowx, btn0_nowy + i*img_button.height);

		//��Ⱦbtn
		for(int i=0; i<vector_btn.size(); i++) {
			img_button.setpos(vector_btn[i]->now_x, vector_btn[i]->now_y);
			if(i != btn_selected) {
				if(i%2==0)
					img_button.drawAddColor(255,49,49,49);
				else
					img_button.drawAddColor(255,56,56,56);
			} else {
				img_button.drawAddColor(255,223,120,51);
			}

			//��Ⱦ��ť�ϵ�����
			if(wcslen(vector_music[i]->artist) != 0) {
				drawtext(vector_music[i]->title, btn0_nowx + 20 , vector_btn[i]->now_y + 20, 25, 25);
				drawtext(vector_music[i]->artist, btn0_nowx + 20 , vector_btn[i]->now_y + 50, 15, 15);
			} else {
				drawtext(vector_music[i]->title, btn0_nowx + 20 , vector_btn[i]->now_y + 20, 25, 25);
				drawtext(L"--", btn0_nowx + 20 , vector_btn[i]->now_y + 50, 15, 15);
			}
		}
		break;
		}

	case SCENE_GAME: {
		drawtext(L"��Ϸ����", 20 , 5, 15, 15);
		drawtext(L"����ֵ", 20 , 305, 15, 15);
		drawtext(L"ɢ������", 20 , 335, 15, 15);
		drawtext(L"��������", 20 , 365, 15, 15);

		drawtext(L"SCORE:", 480 , 375, 20, 20);
		wchar_t str_scroe[100];
		wsprintf(str_scroe, L"%d", int(point_score));
		drawtext(str_scroe, 540 , 355, 40, 40);

		//��Ⱦ����������
		img_progress.setpos(20, 5);
		img_progress.setWH(PROGRESS_WIDTH_TIME, PROGRESS_HEIGHT_NORAML);
		img_progress.drawAddColor(100, 255, 255, 255);

		float progress = float(song_pos) / nowplaying_song_length;
		img_progress.setpos(20, 4);
		img_progress.setWH(PROGRESS_WIDTH_TIME * progress + 1, PROGRESS_HEIGHT_NORAML+1);
		img_progress.drawAddColor(200, 255, 255, 255);

		//��ȾHP������
		img_progress.setpos(20, 305);
		img_progress.setWH(PROGRESS_WIDTH_NORAML, PROGRESS_HEIGHT_NORAML);
		img_progress.drawAddColor(100, 255, 255, 255);

		if(point_hp >= 0.0f) {
			progress = point_hp / MAX_HP;
			img_progress.setpos(20, 304);
			img_progress.setWH(PROGRESS_WIDTH_NORAML * progress, PROGRESS_HEIGHT_NORAML+1);
			img_progress.drawAddColor(200, 255, 255, 255);
		}

		//��Ⱦɢ������������
		img_progress.setpos(20, 335);
		img_progress.setWH(PROGRESS_WIDTH_NORAML, PROGRESS_HEIGHT_NORAML);
		img_progress.drawAddColor(100, 255, 255, 255);

		if(point_kattack >= 1.0f) {
			progress = point_kattack / MAX_KATTACK;
			img_progress.setpos(20, 334);
			img_progress.setWH(PROGRESS_WIDTH_NORAML * progress, PROGRESS_HEIGHT_NORAML+1);
			img_progress.drawAddColor(200, 255, 255, 255);
		}

		//��Ⱦ��������������
		img_progress.setpos(20, 365);
		img_progress.setWH(PROGRESS_WIDTH_NORAML, PROGRESS_HEIGHT_NORAML);
		img_progress.drawAddColor(100, 255, 255, 255);

		if(point_hide >= 1.0f) {
			progress = point_hide / MAX_HIDE;
			img_progress.setpos(20, 364);
			img_progress.setWH(PROGRESS_WIDTH_NORAML * progress, PROGRESS_HEIGHT_NORAML+1);
			img_progress.drawAddColor(200, 255, 255, 255);
		}

		break;
		}
	}
}


float gameover_alpha = 0.0f;
int gameover_progress = 0;
float gamefin_alpha = 0.0f;
int gamefin_progress = 0;
int tmp_score = 0;
wchar_t str_tmp_scroe[100];;
//==================================
//  ����:graphicRender()
//  Ŀ��:ͼ����Ⱦ
//
void graphicRender() {
	backgroundRender();

	if(scene == SCENE_GAME) {
		createEmeny();

		if(!is_gameover && !is_gamefin) {
			//��Ⱦ�ɻ�
			if(!me_is_hide) {
				if(me_inattack) {
					img_player.drawAddColor(255,255,0,0);

					if(me_time_inattack.sleep(50.0f))
						me_inattack = false;
				} else
					img_player.draw();
			} else {
				img_player.drawAddColor(100,0,0,0);
				me_inattack = false;
			}
		}

		//��Ⱦ�л�
		enemyRender();

		//��Ⱦ��Ļ
		bulletRender();
	}

	//��ȾUI
	UIRender();

	
	if(is_gameover) {
		img_gameover.draw(gameover_alpha);

		if(gameover_progress == 0) {
			gameover_alpha += 5.0f;
			if(gameover_alpha >= 255.0f) {
				gameover_alpha = 255.0f;
				scene = SCENE_SELC_SONG;
				gameover_progress++;
			}
		} else if(gameover_progress == 180){
			gameover_alpha -= 5.0f;
			if(gameover_alpha <= 0.0f) {
				vector_btn[0]->setTargetPos(395, vector_btn[0]->now_y);
				is_gameover = false;
			}
		} else {
			gameover_progress++;
		}
		
	} else if(is_gamefin) {
		img_gamefin.draw(gamefin_alpha);
		drawtext(str_tmp_scroe, 390 , 190, 40, 40, D3DCOLOR_ARGB(int(gamefin_alpha),255,255,255));

		if(gamefin_progress == 0) {
			gamefin_alpha += 5.0f;
			if(gamefin_alpha >= 255.0f) {
				gamefin_alpha = 255.0f;
				scene = SCENE_SELC_SONG;
				gamefin_progress++;
			}
		} else if(gamefin_progress == 1){
			tmp_score+=10;
			if(tmp_score>int(point_score)) {
				tmp_score = int(point_score);
				gamefin_progress++;
			}
			wsprintf(str_tmp_scroe, L"%d", tmp_score);
			drawtext(str_tmp_scroe, 390 , 190, 40, 40);
		} else if(gamefin_progress == 180){
			gamefin_alpha -= 5.0f;

			if(gamefin_alpha <= 0.0f) {
				MyMusic::playMusic(btn_selected);
				vector_btn[0]->setTargetPos(395, vector_btn[0]->now_y);
				is_gamefin = false;
			}
		} else {
			gamefin_progress++;
		}
	}
}

//==================================
//  ����:backgroundRender()
//  Ŀ��:������Ⱦ
//
void backgroundRender() {
	switch(scene) {
	case SCENE_LOGO: {
		static float alpha = 0.0f;
		static int progress = 0;
		
		img_logo.draw(alpha);
		if(progress == 0) {
			alpha+=10.0f;
			if(alpha >= 255.0f) {
				alpha = 255.0f;
				
				progress++;
			}
		} else if(progress == 180){
			alpha-=10.0f;
			if(alpha <= 0.0f) {
				MyMusic::playMusic(0);
				vector_btn[0]->setNowPos(GameWidth, vector_btn[0]->now_y);
				vector_btn[0]->setTargetPos(395, vector_btn[0]->now_y);
				scene = SCENE_SELC_SONG;
			}
		} else {
			progress++;
		}

		break;
	}
	case SCENE_SELC_SONG:
		for(int i=0; i<NUM_FFT; i++) {
			int nowx, nowy;

			float scale = amp_left[i] * 10;

			img_beat.setWH(scale, scale);
			img_beat.setpos(i*10.0f - img_beat.width/2.0f, 200.0f - img_beat.height/2.0f);
			img_beat.drawAddColor(amp_left[i]*0.5f, now_theme_color_r, now_theme_color_g, now_theme_color_b, true);

			img_beat.setWH(scale, scale);
			img_beat.setpos((640.0f-i*10.0f) - img_beat.width/2.0f, 200.0f - img_beat.height/2.0f);
			img_beat.drawAddColor(amp_right[i]*0.5f, now_theme_color_b, now_theme_color_g, now_theme_color_r, true);
		}
		break;
	}
}


void updataNowThemeColor() {
	now_theme_color_r += abs((amp_right[10]-64)/6.0f)*(rand()%5-2);
	now_theme_color_g += abs((amp_right[20]-64)/6.0f)*(rand()%5-2);
	now_theme_color_b += abs((amp_right[30]-64)/6.0f)*(rand()%5-2);

	if(now_theme_color_r<80.0f)
		now_theme_color_r = 80.0f;
	else if(now_theme_color_r>255.0f)
		now_theme_color_r = 255.0f;

	if(now_theme_color_g<80.0f)
		now_theme_color_g = 100.0f;
	else if(now_theme_color_g>255.0f)
		now_theme_color_g = 255.0f;

	if(now_theme_color_b<80.0f)
		now_theme_color_b = 80.0f;
	else if(now_theme_color_b>255.0f)
		now_theme_color_b = 255.0f;
}


//==================================
//  ����:addBullet()
//  Ŀ��:���ӵ���
//
void addBullet(bulletType bt,float nowx,float nowy,float vx,float vy,float ax,float ay,float extra,float extra_2, float extra_3) {
	static short int fft_num = -1;
	if(fft_num++ > NUM_FFT - 20)
		fft_num = 0;

	BulletPath *temp = new BulletPath;

	temp->bt = bt;
	temp->fft_r = now_theme_color_r;
	temp->fft_g = now_theme_color_g;
	temp->fft_b = now_theme_color_b;

	temp->nowx = nowx;
	temp->nowy = nowy;
	temp->vx = vx;
	temp->vy = vy;
	temp->ax = ax;
	temp->ay = ay;
	temp->extra = extra;
	temp->extra_2 = extra_2;
	temp->extra_3 = extra_3;
	temp->next= 0;
	
	switch(bt) {
	case b_common:
		temp->img = &img_bullet;
		temp->color_a = 100;
		temp->color_r = 255;
		temp->color_g = 255;
		temp->color_b = 255;

		if(extra == 0.0f) {
			temp->fft = fft_num;
			//temp->vy *= (amp_left[temp->fft]/90 + 1);
		} else {
			temp->fft = (fft_num + 50)%NUM_FFT;
			temp->fft_r = now_theme_color_b;
			temp->fft_g = now_theme_color_r;
			temp->fft_b = now_theme_color_g;
			//temp->vy *= (amp_right[temp->fft]/90 + 1);
		}
		break;

	case b_enemy_1:
		temp->img = &img_bullet;
		temp->color_a = 200;
		temp->color_r = 255;
		temp->color_g = 255;
		temp->color_b = 255;
		temp->extra = D3DX_PI - atan(vx/vy);
		break;

	case b_star:
		temp->img = &img_star;
		temp->color_a = 150;
		temp->color_r = rand()%246+10;
		temp->color_g = rand()%246+10;
		temp->color_b = rand()%246+10;
		break;

	case b_kattack:
		temp->img = &img_bullet;
		temp->color_a = 100;
		temp->color_r = now_theme_color_r;
		temp->color_g = now_theme_color_b;
		temp->color_b = now_theme_color_g;
		temp->extra = D3DX_PI - atan(vx/vy);
		temp->extra_2 = 1;			//����һ��

		if(extra_3 == 0.0f) {
			temp->fft = fft_num;
		} else {
			temp->fft = (fft_num + 50)%NUM_FFT;
			temp->fft_r = now_theme_color_b;
			temp->fft_g = now_theme_color_r;
			temp->fft_b = now_theme_color_g;
		}
		break;
	}

	if(bphead) {
		temp->front = bpnow;
		bpnow->next = temp;
		bpnow = temp;
	} else {
		temp->front = 0;
		bphead = temp;
	}
	bpnow = temp;
}


//==================================
//  ����:addStar()
//  Ŀ��:����img_star
//
void addStar(float nowx, float nowy, float angle = 60.0f, int anglenum = 3) {
	//extra���ڼ�¼�����������ӳ�
	float i;
	for(i=0.0f;i<=6.28318530;i+=6.28318530/angle)
	{
		int num=rand()%anglenum+1;
		for(int i2=0;i2<num;i2++)
		{
			float v=(rand()%10+1)/5.0f;
			float extra=rand()%60+10;
			addBullet(b_star,nowx,nowy,-sin((float)i)*v,-cos((float)i)*v,0.0f,0.0f,extra);
		}
	}
}


//==================================
//  ����:TargetIt(BulletPath *b)
//  Ŀ��:�ض���img_star����
//
void TargetIt(BulletPath *b) {
	float nowx=b->nowx+img_star.width/2, nowy=b->nowy+img_star.height/2;
	float cx=img_player.nowx+img_player.width/2, cy=img_player.nowy+img_player.height/2;
	float tmp=5.0f/(sqrt(pow(nowx-cx,2)+pow(nowy-cy,2)));
	b->vx = (cx-nowx)*tmp*fabs(b->extra);
	b->vy = (cy-nowy)*tmp*fabs(b->extra);
	b->extra-=0.1f;
}


//==================================
//  ����:addTargetBullet()
//  Ŀ��:������img_playerΪĿ����ӵ�
//
void addTargetBullet(bulletType bt,float nowx,float nowy,float v) {
	float cx_p = img_player.nowx+img_player.width/2, cy_p = img_player.nowy+img_player.height/2+2;
	float cx_b = nowx+2, cy_b = nowy+4;
	float tmp=v/sqrt(pow(cx_b-cx_p,2)+pow(cy_b-cy_p,2));
	float vx = (cx_p-cx_b)*tmp;
	float vy = (cy_p-cy_b)*tmp;

	addBullet(bt,nowx,nowy,vx,vy,0.0f,0.0f);
}


void drawBeat(int fft, float x, float y, byte r, byte g, byte b, float ratio_scal){
	if(fft > 60) {
		float scale = (fft/64.0f)+1.1f;
		if(fft > 95)
			scale = (fft/8.0f)+1.0f;
		else if(fft > 85)
			scale = (fft/16.0f)+1.0f;
		else if(fft > 75)
			scale = (fft/32.0f)+1.0f;

		img_beat.setWH(20.0f * scale * ratio_scal, 20.0f * scale * ratio_scal);
		img_beat.setpos(x - img_beat.width/2.0f, y-img_beat.height/2.0f);
		img_beat.drawAddColor(30 + fft, r, g, b, true);
	}
}


//==================================
//  ����:bulletRender()
//  Ŀ��:��Ļ��Ⱦ
//
void bulletRender() {
	BulletPath *nowp = bphead;
	dximg *b;

	//����Ļû�е���ʱɾ�����ез��ĵ���
	/*
	if(!emyhead&&nowp)
	{
		while(nowp)
		{
			if(nowp->bt==b_enemy_1)
			{
				float cx = nowp->nowx+img_bullet.width/2;
				float cy = nowp->nowy+img_bullet.width/2;

				//�б�ͷ��
				if(bphead==nowp)
				{
					if(nowp->next)
					{
						nowp->next->front = 0;
						bphead = nowp->next;
					}
					else
					{
						bphead = 0;
						delete nowp;
						addStar(cx,cy,10,1);
						break;
					}
				}
				//�б��в�
				else if(nowp->front&&nowp->next)
				{
					nowp->front->next = nowp->next;
					nowp->next->front = nowp->front;
				}
				//�б�β��
				else
				{
					bpnow = nowp->front;
					nowp->front->next = 0;
					delete nowp;
					addStar(cx,cy,10,1);
					break;
				}

				BulletPath *tempp = nowp;
				nowp = nowp->next;
				if(tempp)
					delete tempp;

				addStar(cx,cy,10,1);
				continue;
			}
			nowp = nowp->next;
		}
	}*/


	nowp = bphead;
	//��Ļ��Ⱦ
	while(nowp) {
		b = nowp->img;

		nowp->vx += nowp->ax;
		nowp->vy += nowp->ay;
		nowp->nowx += nowp->vx;
		nowp->nowy += nowp->vy;

		//�Ƿ񳬳���Ļ
		bool isover = ((nowp->nowx+b->width<0)||(nowp->nowx>GameWidth)||(nowp->nowy+b->height<0)||(nowp->nowy>GameHeight));

		//�Ƿ���ײ
		bool ishit = false;
		float cx = nowp->nowx+b->width/2;
		float cy = nowp->nowy+b->height/2;

		switch(nowp->bt) {
		case b_common:
		case b_kattack:
		case b_line: {
			Enemy *tmp = emyhead;
			while(tmp)
			{
				if((cx > tmp->nowx) && (cx < tmp->nowx + tmp->width) && (cy > tmp->nowy) && (cy < tmp->nowy + tmp->height))
				{
					
					img_beat.setWH(50.0f * ((rand()%20)+10)/10.0f, 50.0f * ((rand()%20)+10)/10.0f);
					img_beat.setpos(nowp->nowx - (img_beat.width - b->width)/2, nowp->nowy-(img_beat.height - b->height)/2);
					img_beat.drawAddColor(100, nowp->fft_r, nowp->fft_g, nowp->fft_b, true);

					int offset_x = cx - tmp->nowx;
					int offset_y = cy - tmp->nowy;
					if(tmp->img->map[offset_x][offset_y] == true) {
						tmp->inattack = true;
						if(nowp->bt==b_common)
							tmp->hp -= 2;
						else
							tmp->hp -= 10;
						ishit = true;
					}
				}
				else
					tmp->inattack = false;
				tmp=tmp->next;
			}
			break;
		}

		case b_enemy_1:
			if(!me_is_hide)
			{
				if((cx > img_player.nowx) && (cx < img_player.nowx+img_player.width) && (cy > img_player.nowy) && (cy < img_player.nowy+img_player.height))
				{
					int offset_x = cx - img_player.nowx;
					int offset_y = cy - img_player.nowy;
					if(img_player.map[offset_x][offset_y] == true) {
						point_hp-=10;
						me_inattack = true;
						ishit = true;
					}
				}
			}
			break;

		case b_star:
			if(nowp->extra>0.0f)
				nowp->extra--;
			else {
				if((cx>img_player.nowx)&&(cx<img_player.nowx+img_player.width)&&(cy>img_player.nowy)&&(cy<img_player.nowy+img_player.height))
				{
					switch(rand()%3) {
					case 0:
						point_hp+=0.005;
						if(point_hp>MAX_HP)
							point_hp = MAX_HP;
						point_score += 0.1;
						break;
					case 1:
						point_kattack+=0.002;
						if(point_kattack>MAX_KATTACK)
							point_kattack = MAX_KATTACK;
						point_score += 0.1;
						break;
					case 2:
						point_hide+=0.08;
						if(point_hide>MAX_HIDE)
							point_hide = MAX_HIDE;
						point_score += 0.1;
						break;
					}
					ishit = true;
				}
				TargetIt(nowp);
			}

			isover = false;
			break;
		}

		//���������Ļ��Χ������
		if(isover||ishit)
		{
			//�����ӵ�����
			if(isover&&!ishit&&nowp->extra_2&&nowp->bt==b_kattack)
			{
				nowp->extra_2--;
				if((nowp->nowx+img_bullet.width<0)||(nowp->nowx>GameWidth))
					nowp->vx = -nowp->vx;
				if((nowp->nowy+img_bullet.height<0)||(nowp->nowy>GameHeight))
					nowp->vy = -nowp->vy;
				nowp->extra = D3DX_PI - atan(nowp->vx/nowp->vy);

				nowp = nowp->next;
				continue;
			}

			//�б�ͷ��
			if(bphead==nowp)
			{
				if(nowp->next)
				{
					nowp->next->front = 0;
					bphead = nowp->next;
				}
				else
				{
					bphead = 0;
					delete nowp;
					break;
				}
			}
			//�б��в�
			else if(nowp->front&&nowp->next)
			{
				nowp->front->next = nowp->next;
				nowp->next->front = nowp->front;
			}
			//�б�β��
			else
			{
				bpnow = nowp->front;
				nowp->front->next = 0;
				delete nowp;
				break;
			}

			BulletPath *tempp = nowp;
			nowp = nowp->next;
			if(tempp)
				delete tempp;
			continue;
		}

		if(nowp->bt==b_common) {
			//�����ٶ�
			if(amp_left[20] > 60)
				nowp->nowy -= ((amp_left[20]-60) * 10.0f/50.0f );

			float f1 = nowp->nowx - (img_beat.width - b->width)/2.0f;
			float f2 = nowp->nowy-(img_beat.height - b->height)/2.0f;
			
			drawBeat(nowp->extra==0.0f? amp_left[nowp->fft] : amp_right[nowp->fft],
				cx, cy,
				nowp->fft_r, nowp->fft_g, nowp->fft_b);
			
			b->setrota(0, b->width/2, b->height/2);
		} else if(nowp->bt==b_enemy_1) {
			b->setrota(nowp->extra, b->width/2, b->height/2);
		} else if(nowp->bt==b_kattack) {
			b->setrota(nowp->extra, b->width/2, b->height/2);

			int fft = amp_left[nowp->fft]+10;
			drawBeat(fft, cx, cy,
				nowp->fft_r, nowp->fft_g, nowp->fft_b, 0.3f);
		}/* else if(nowp->bt==b_star) {
			float v_ratio = sqrt(nowp->vx*nowp->vx + nowp->vy*nowp->vy)/5.0f;
			nowp->color_a = 255*v_ratio;
		}//*/

		b->setpos(nowp->nowx,nowp->nowy);
		b->drawAddColor(nowp->color_a, nowp->color_r, nowp->color_g, nowp->color_b, true);

		nowp = nowp->next;
	}
}


int boss_status;
mytime timer_e[5];
//==================================
//  ����:createEmeny()
//  Ŀ��:���������������ɵ���
//
void createEmeny() {
	float progress = float(song_pos) / nowplaying_song_length;

	static bool create_e1 = false;
	static int create_e1_num = 0;
	static bool create_e1_left = true;

	static bool create_e4 = false;
	static int create_e4_num = 0;

	if(boss_status!=1&&boss_status!=3) {
		if(song_pos > 5000) {
			if(!create_e1 && amp_left[10]>75) {
				create_e1 = true;
				create_e1_left = rand()%2;
			}

			if(amp_left[10]>55 && create_e1) {
				if(timer_e[1].sleep(500)) {
					if(create_e1_left) {
						addEnemy(e_1, 150.0f, 280.0f - create_e1_num*10.0f, -100.0f - create_e1_num*20.0f, -3.0f, 3.0f, 0.04f, 0.005f);
					} else {
						addEnemy(e_1, 150.0f, 280.0f + create_e1_num*10.0f, -100.0f - create_e1_num*20.0f, 3.0f, 3.0f, -0.04f, 0.005f);
					}

					if(create_e1_num++>10) {
						create_e1 = false;
						create_e1_num = 0;
					}
				}
			} else {
				create_e1 = false;
				create_e1_num = 0;
			}

			static bool can_create_e2 = true;
			if(amp_left[12]>80 && amp_left[45]>60 && can_create_e2) {
				addEnemy(e_2, 60.0f, 13.0f, -250.0f, 0.0f, 3.0f, -0.01f, 0.05f);
				addEnemy(e_2, 60.0f, 93.0f, -200.0f, 0.0f, 3.0f, -0.01f, 0.05f);
				addEnemy(e_2, 60.0f, 173.0f, -150.0f, 0.0f, 3.0f, -0.01f, 0.05f);
				addEnemy(e_2, 60.0f, 253.0f, -100.0f, 0.0f, 3.0f, 0.0f, 0.05f);
				addEnemy(e_2, 60.0f, 333.0f, -100.0f, 0.0f, 3.0f, 0.0f, 0.05f);
				addEnemy(e_2, 60.0f, 413.0f, -150.0f, 0.0f, 3.0f, 0.01f, 0.05f);
				addEnemy(e_2, 60.0f, 493.0f, -200.0f, 0.0f, 3.0f, 0.01f, 0.05f);
				addEnemy(e_2, 60.0f, 573.0f, -250.0f, 0.0f, 3.0f, 0.01f, 0.05f);

				can_create_e2 = false;
			} else if(!can_create_e2) {
				if(timer_e[2].sleep(12000))
					can_create_e2 = true;
			}


			if(!create_e4 && amp_left[20]>70) {
				create_e4 = true;
			}

			if(amp_left[20]>55 && create_e1) {
				if(timer_e[3].sleep(550)) {
					int r = rand()%8;
					addEnemy(e_4, 60.0f, r*80.0f + 31.0f, -100.0f, (3.0f-r)*0.8f, 3.0f, 0.0f, 0.0f, rand()%NUM_FFT);

					if(create_e1_num++>8) {
						create_e4 = false;
						create_e4_num = 0;
					}
				}
			} else {
				create_e4 = false;
				create_e4_num = 0;
			}
		}

		if(amp_left[10]>50) {
			if(timer_e[4].sleep(1000) && !(rand()%2))
				addEnemy(e_stone, 60, rand()%640 - 100, -200);
		}
	}

	if(progress > 0.8f) {
		if(boss_status==2 && amp_left[10] > 70) {
			MyMusic::playAudio(6);
			addEnemy(e_boss, 3000.0f, 236.0f, -130.0f, 0.0f, 0.5f, 0.0f, 0.0f);
			boss_status++;
		}

	} else if(progress > 0.4f) {
		if(boss_status==0 && amp_left[10] > 70) {
			MyMusic::playAudio(6);
			addEnemy(e_boss, 2000.0f, 236.0f, -150.0f, 0.0f, 0.5f, 0.0f, 0.0f);
			boss_status++;
		}
	}
}


//==================================
//  ����:addEnemy()
//  Ŀ��:���ӵл�
//
void addEnemy(enemyType etype, float hp, float nowx, float nowy, float vx, float vy, float ax, float ay, int amp_track) {
	Enemy *temp = new Enemy;

	temp->etype = etype;
	float v_ratio = 0.2f;		//�ٶ�ϵ��
	v_ratio += (amp_left[amp_track]/80.0f);
	temp->vx = vx * v_ratio;
	temp->vy = vy * v_ratio;
	temp->ax = ax;
	temp->ay = ay;
	temp->rota = 0.0f;

	switch(etype) {
	case e_1:
		temp->img = &img_enemy[0];
		temp->width = img_enemy[0].width;
		temp->height = img_enemy[0].height;
		break;

	case e_2:
		temp->img = &img_enemy[1];
		temp->width = img_enemy[1].width;
		temp->height = img_enemy[1].height;
		break;

	case e_3:
		temp->img = &img_enemy[2];
		temp->width = img_enemy[2].width;
		temp->height = img_enemy[2].height;
		break;

	case e_4:
		temp->img = &img_enemy[3];
		temp->width = img_enemy[3].width;
		temp->height = img_enemy[3].height;
		break;

	case e_stone: {
		switch(rand()%3) {
		case 0:
			temp->img = &img_stone[0];
			temp->width = img_stone[0].width;
			temp->height = img_stone[0].height;
			break;
		case 1:
			temp->img = &img_stone[1];
			temp->width = img_stone[1].width;
			temp->height = img_stone[1].height;
			break;
		case 2:
			temp->img = &img_stone[2];
			temp->width = img_stone[2].width;
			temp->height = img_stone[2].height;
			break;
			
		}

		float cx_p = img_player.nowx+img_player.width/2.0f, cy_p = img_player.nowy+img_player.height/2.0f;
		float cx_e = nowx + temp->width/2.0f, cy_e = nowy + temp->height/2.0f;
		double tmp=4.0/sqrt(pow(cx_e-cx_p,2)+pow(cy_e-cy_p,2));

		temp->vx = (cx_p-cx_e)*tmp*v_ratio;
		temp->vy = (cy_p-cy_e)*tmp*v_ratio;
		temp->ax = 0.0f;
		temp->ay = 0.0f;

		temp->rota = (rand()%10)/10.0f;
		break;
		}
	case e_boss:
		temp->img = &img_boss;
		temp->width = img_boss.width;
		temp->height = img_boss.height;
		break;
		break;
	default:
		break;
	}

	temp->hp = hp;
	temp->fly_in = true;
	temp->inattack = false;
	temp->nowx = nowx;
	temp->nowy = nowy;
	
	temp->time = 0.0f;
	temp->extra = 0.0f;
	temp->next= 0;

	if(emyhead) {
		temp->front = emynow;
		emynow->next = temp;
		emynow = temp;
	} else {
		temp->front = 0;
		emyhead = temp;
	}
	emynow = temp;
}


//==================================
//  ����:enemyControl()
//  Ŀ��:�л���������
//
void enemyControl(Enemy *p) {
	// �л���ʱ���־����
	p->time++;

	switch (p->etype) {
	case e_stone:
		p->rota+=0.01f;
		break;
	case e_1:
		if(int(p->time)%60==1) {
			if(!(rand()%6))
				addTargetBullet(b_enemy_1, p->nowx+p->width/2.0f, p->nowy+p->height/2.0f, 4.0f);
		}
		break;
	case e_3:
		if(int(p->time)%60==1)
			addTargetBullet(b_enemy_1, p->nowx+p->width/2.0f, p->nowy+p->height/2.0f, 4.0f);
		break;
	case e_4:
		p->rota+=0.2f;
		break;
	case e_boss:
		if(int(p->time)%10 == 0) {
			addTargetBullet(b_enemy_1, p->nowx+p->width/2.0f, p->nowy+p->height/2.0f,3);
			addTargetBullet(b_enemy_1, p->nowx+p->width/2.0f - 20.0f, p->nowy+p->height/2.0f + 20.0f,3);
			addTargetBullet(b_enemy_1, p->nowx+p->width/2.0f + 20.0f, p->nowy+p->height/2.0f + 20.0f,3);
		}
		break;
	}
}


//==================================
//  ����:enemyRender()
//  Ŀ��:�л���Ⱦ
//
void enemyRender() {
	Enemy *nowp = emyhead;
	
	while(nowp) {
		dximg *emytemp = nowp->img;

		enemyControl(nowp);
		nowp->vx += nowp->ax;
		nowp->vy += nowp->ay;
		nowp->nowx += nowp->vx;
		nowp->nowy += nowp->vy;

		double center_emeny_x = nowp->nowx + nowp->width/2;
		double center_emeny_y = nowp->nowy + nowp->height/2;
		double center_me_x = img_player.nowx + img_player.width/2;
		double center_me_y = img_player.nowy + img_player.height/2;
		//������ת��
		if(nowp->etype == e_1) {
			double angle = 0.0;
			//*�л���ͷ������player
			if(center_me_y-center_emeny_y >= 0)
				angle = D3DX_PI*2 - atan((center_me_x-center_emeny_x)/(center_me_y-center_emeny_y));
			else
				angle = D3DX_PI - atan((center_me_x-center_emeny_x)/(center_me_y-center_emeny_y));//*/

			angle = fmod(angle, D3DX_PI*2);

			if(nowp->rota > angle) {
				if(((angle+D3DX_PI*2) - nowp->rota) < (nowp->rota - angle)) {
					float rota = nowp->rota + 0.05f;
					if(rota > angle)
						rota = angle;
					nowp->rota = fmod(rota, D3DX_PI*2);
				} else {
					float rota = nowp->rota + D3DX_PI*2 - 0.05f;
					if(rota < angle)
						rota = angle;
					nowp->rota = fmod(rota, D3DX_PI*2);
				}
			} else if(nowp->rota < angle) {
				if(((nowp->rota+D3DX_PI*2) - angle) < (angle - nowp->rota)) {
					float rota = nowp->rota + D3DX_PI*2 - 0.05f;
					if(rota < angle)
						rota = angle;
					nowp->rota = fmod(rota, D3DX_PI*2);
				} else {
					float rota = nowp->rota + 0.05f;
					if(rota > angle)
						rota = angle;
					nowp->rota = fmod(rota, D3DX_PI*2);
				}
			}

			/*
			if(nowp->vy >= 0)
				angle = D3DX_PI*2 - atan(nowp->vx/nowp->vy);
			else
				angle = D3DX_PI - atan(nowp->vx/nowp->vy);//*/

			//nowp->rota = angle;
		} else if(nowp->etype > e_1 && nowp->etype < e_4) {
			if(nowp->vy!=0.0f) {
				if(nowp->vy >= 0)
					nowp->rota = D3DX_PI*2 - atan(nowp->vx/nowp->vy);
				else
					nowp->rota = D3DX_PI - atan(nowp->vx/nowp->vy);
			}
		}

		//�Ƿ񳬳���Ļ
		bool isover = (nowp->nowx+emytemp->width<0)||(nowp->nowx>GameWidth)||(nowp->nowy+emytemp->height<0)||(nowp->nowy>GameHeight);

		if(nowp->fly_in) {
			if(!isover)
				nowp->fly_in = false;
			else
				isover = false;
		} else
			isover = (nowp->nowx+emytemp->width<0)||(nowp->nowx>GameWidth)||(nowp->nowy+emytemp->height<0)||(nowp->nowy>GameHeight);

		//�Ƿ�hpΪ0
		bool hpover = (nowp->hp <= 0.0f);
		//�Ƿ���ײ
		bool ishit = false;

		//�ж��Ƿ��������ײ
		if(!me_is_hide && (center_me_x > nowp->nowx) && (center_me_x < nowp->nowx + nowp->width) &&
			(center_me_y > nowp->nowy) && (center_me_y < nowp->nowy + nowp->height)) {
			int offset_x = center_me_x - nowp->nowx;
			int offset_y = center_me_y - nowp->nowy;
			if(emytemp->map[offset_x][offset_y] == true) {
				if(nowp->etype != e_boss) {
					point_hp -= 25;
					me_inattack = true;
					ishit = true;
				} else {
					point_hp -= 100;
					me_inattack = true;
				}
			}
		}

		if(isover||hpover||ishit) {
			if(nowp->etype == e_boss) {
				if(boss_status==1)
					boss_status++;
				else if(boss_status==3)
					boss_status++;
			}

			if(hpover||ishit) {
				addStar((nowp->nowx+nowp->width/2),(nowp->nowy+nowp->height/2));
				MyMusic::playAudio(3);
			}

			//�б�ͷ��
			if(emyhead==nowp) {
				if(nowp->next) {
					nowp->next->front = 0;
					emyhead = nowp->next;
				} else {
					emyhead = 0;
					delete nowp;
					break;
				}
			}
			//�б��в�
			else if(nowp->front&&nowp->next) {
				nowp->front->next = nowp->next;
				nowp->next->front = nowp->front;
			}
			//�б�β��
			else {
				emynow = nowp->front;
				nowp->front->next = 0;
				delete nowp;
				break;
			}

			Enemy *tempp = nowp;
			nowp = nowp->next;
			if(tempp)
				delete tempp;

			continue;
		}

		emytemp->setpos(nowp->nowx, nowp->nowy);
		emytemp->setrota(nowp->rota, emytemp->width/2.0f, emytemp->height/2.0f);
		if(!nowp->inattack)
			emytemp->draw(255);
		else {
			emytemp->drawAddColor(255, 100, 0, 0);
			if(time_inattack.sleep(50))
				nowp->inattack = false;
		}

		nowp = nowp->next;
	}
}


void initGame() {
	me_is_hide = false;
	point_hide = 200.0f;
	point_kattack = 1.0f;
	point_hp = 100.0;
	point_score = 0.0f;
	me_inattack = false;
	kattacking = false;

	gameover_alpha = 0.0f;
	gameover_progress = 0;
	gamefin_alpha = 0.0f;
	gamefin_progress = 0;
	tmp_score = 0;
	wsprintf(str_tmp_scroe, L"%d", tmp_score);

	boss_status = 0;

	img_player.setpos(320, 300);

	clearGameList();
}

//==================================
//  ����:setupViewAndProjMat()
//  Ŀ��:��ʼ��View��ProjMat����
//
void setupViewAndProjMat() {
	//���ù۲����
	D3DXMATRIXA16 matView;
	D3DXMatrixIdentity( &matView );
	g_pd3dD->SetTransform( D3DTS_VIEW, &matView );

	//����ͶӰ����
	D3DXMATRIXA16 matProj;
	D3DXMatrixOrthoOffCenterLH( &matProj, 0.0f, GameWidth, GameHeight, 0.0f, 0.0f, 1.0f );
	g_pd3dD->SetTransform( D3DTS_PROJECTION, &matProj );
}


//==================================
//  ����:loadimg()
//  Ŀ��:��ȡ��ϷͼƬ
//
void loadimg() {
	img_logo.loadimg(L"res/logo.jpg");
	img_gameover.loadimg(L"res/gamevoer.jpg");
	img_gamefin.loadimg(L"res/fin.png");

	img_player.loadimg(L"res/player.png");
	img_player.getmap();
	img_enemy[0].loadimg(L"res/enemy.png");
	img_enemy[0].getmap();
	img_enemy[1].loadimg(L"res/enemy2.png");
	img_enemy[1].getmap();
	img_enemy[2].loadimg(L"res/enemy3.png");
	img_enemy[2].getmap();
	img_enemy[3].loadimg(L"res/enemy4.png");
	img_enemy[3].getmap();

	img_boss.loadimg(L"res/boss.png");
	img_boss.getmap();

	img_stone[0].loadimg(L"res/stone_1.png");
	img_stone[0].getmap();
	img_stone[1].loadimg(L"res/stone_2.png");
	img_stone[1].getmap();
	img_stone[2].loadimg(L"res/stone_3.png");
	img_stone[2].getmap();

	img_bullet.loadimg(L"res/bullet.png");
	img_star.loadimg(L"res/star.png");
	img_beat.loadimg(L"res/beat.png");
	img_button.loadimg(L"res/box.png");
	img_button.setWH(BTN_WIDTH, BTN_HEIGHT);
	img_progress.loadimg(L"res/box.png");
	img_progress.setWH(1, PROGRESS_HEIGHT_NORAML);
}


void initBtn() {
	//������ť������ʼ����ťλ��
	for(int i=0; i<vector_music.size(); i++) {

		AccelerateHelper *btn = new AccelerateHelper();
		btn->setNowPos(GameWidth - img_button.width, i*img_button.height + BTN_SCREEN_CENTER_Y);

		vector_btn.push_back(btn);
	}
	btn_selected = 0;
}


//==================================
//  ����:InitD3D( HWND )
//  Ŀ��:��ʼ��Direct3D�豸����
//
bool InitD3D() {
	srand(time(0));
	//����Direct3D����    [���ڻ�ȡӲ����Ϣ]
	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if(g_pD3D==NULL) return false;

	//����Ƿ�֧��Ӳ�����㴦��
	D3DCAPS9 caps;
	g_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,&caps);
	int vp=0;
	if(caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp=D3DCREATE_HARDWARE_VERTEXPROCESSING;  //Ӳ������
	else
		vp=D3DCREATE_SOFTWARE_VERTEXPROCESSING;  //�������

	//����D3DPRESENT_PARAMETERS�ṹ
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp,sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	//����Direct3D�豸����     [������Ⱦͼ��]
	if( FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,hwnd,vp,&d3dpp,&g_pd3dD)) )
		return false;

	//���ù۲��ͶӰ����
	setupViewAndProjMat();

	//�������Թ���
	g_pd3dD->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	g_pd3dD->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	g_pd3dD->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	g_pd3dD->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

	//��������Ѱַģʽ
	g_pd3dD->SetSamplerState(0, D3DSAMP_BORDERCOLOR, 0x00000000);
	g_pd3dD->SetSamplerState(0, D3DSAMP_ADDRESSU,    D3DTADDRESS_BORDER);
	g_pd3dD->SetSamplerState(0, D3DSAMP_ADDRESSV,    D3DTADDRESS_BORDER);
	g_pd3dD->SetSamplerState(1, D3DSAMP_BORDERCOLOR, 0x00000000);
	g_pd3dD->SetSamplerState(1, D3DSAMP_ADDRESSU,    D3DTADDRESS_BORDER);
	g_pd3dD->SetSamplerState(1, D3DSAMP_ADDRESSV,    D3DTADDRESS_BORDER);
	g_pd3dD->SetSamplerState(2, D3DSAMP_BORDERCOLOR, 0x00000000);
	g_pd3dD->SetSamplerState(2, D3DSAMP_ADDRESSU,    D3DTADDRESS_BORDER);
	g_pd3dD->SetSamplerState(2, D3DSAMP_ADDRESSV,    D3DTADDRESS_BORDER);

	g_pd3dD->SetRenderState(D3DRS_ALPHABLENDENABLE, true);

	//�����涼����
	g_pd3dD->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	//�رչ���
	g_pd3dD->SetRenderState(D3DRS_LIGHTING, FALSE);

	InitVertex();      //��ʼ�����㻺��,����װ����ͼ
	loadimg();
	initBtn();

	initGame();

	return true;
}


//==================================
//  ����:clearGameList()
//  Ŀ��:�����������͵л�����
//
void clearGameList() {
	BulletPath *bp_tmp = bphead;
	while(bp_tmp != NULL) {
		BulletPath *bp_need_del = bp_tmp;
		bp_tmp = bp_tmp->next;
		delete bp_need_del;
	}
	bphead = NULL;

	Enemy *emy_tmp = emyhead;
	while(emy_tmp != NULL) {
		Enemy *emy_need_del = emy_tmp;
		emy_tmp = emy_tmp->next;
		delete emy_need_del;
	}
	emyhead = NULL;
}


//==================================
//  ����:D3DRelease()
//  Ŀ��:�ͷ�Direct3D��Direct3D�豸����
//
void D3DRelease() {
	clearGameList();

	//�ͷŶ��㻺��
	ReleaseVertex();

	if(g_pd3dD != NULL)
		g_pd3dD->Release();

	if(g_pD3D != NULL)
		g_pD3D->Release();
}