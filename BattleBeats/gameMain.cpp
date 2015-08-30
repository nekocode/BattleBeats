#include "gameMain.h"
#include <Windows.h>
#include <fstream>
using namespace std;
#define scenetime 1000

float hz = 0.06f;		//屏幕刷新率/1000,默认为0.06
int scene = 0;			//控制当前场景标志
bool game_inited = false;
extern Enemy *emyhead;
extern std::vector<MusicInfo*> vector_music;

extern libZPlay::ZPlay *player[NUM_AUDIO_TRACK];
extern std::vector<AccelerateHelper*> vector_btn;
extern int btn_selected;

extern float point_hp;
extern bool is_gameover;
extern bool is_gamefin;

mytime timer_updata_music_fft;
mytime timer_scene_interval;

unsigned int song_pos;
extern unsigned int nowplaying_song_length;

namespace MyGame {
	// 游戏主过程
	void gameMain() {
		switch(scene) {
		case SCENE_INIT://初始化
			if(game_inited) {
				MyMusic::setVolume(100);
				MyMusic::playAudio(5);
				scene = SCENE_LOGO;
			}
			break;

		case SCENE_LOGO:
			//显示logo
			break;

		case SCENE_SELC_SONG:
			if(is_gameover||is_gamefin)
				break;

			//按下回车键
			if(KeyIsDown(DIK_RETURN)) {
				vector_btn[0]->setTargetPos(GameWidth+10, vector_btn[0]->now_y);
				MyMusic::stopMusic();
				scene = SCENE_SELC_SONG_OUT;
			}
			if(KeyIsDown(DIK_LEFT)) {
				vector_btn[0]->setTargetPos(395, vector_btn[0]->now_y);
			}
			if(KeyIsDown(DIK_RIGHT)) {
				vector_btn[0]->setTargetPos(GameWidth+10, vector_btn[0]->now_y);
			}
			break;

		case SCENE_SELC_SONG_OUT:
			//按钮超出屏幕
			if(vector_btn[0]->now_x > GameWidth) {
				if(timer_scene_interval.sleep(400)) {
					initGame();
					MyMusic::playMusic(btn_selected);
					timer_scene_interval.clear();
					scene = SCENE_GAME;
				}
			}
			break;

		case SCENE_GAME:
			if(KeyIsDown(DIK_ESCAPE)) {
				vector_btn[0]->setTargetPos(395, vector_btn[0]->now_y);
				MyMusic::playMusic(btn_selected);
				scene = SCENE_SELC_SONG;
			}

			if(point_hp<=0.0f) {
				is_gameover = true;
				MyMusic::playAudio(2);
			}

			song_pos = MyMusic::getPosition();
			if(song_pos >= (nowplaying_song_length-3000)) {
				is_gamefin = true;
				MyMusic::playAudio(7);
			}
			break;

		case SCENE_GAME_OVER:
			break;

		case SCENE_GAME_FIN:
			break;
		}

		if(timer_updata_music_fft.sleep(5)) {
			MyMusic::updataFFT();
			timer_updata_music_fft.clear();
		}

		Render();
	}

	void gameInit() {
		MyMusic::initMusic();

		if(!InitD3D())
			MessageBox(hwnd,TEXT("Direct3D init error!"),TEXT("Error"),MB_OK);
		if(!InitDxInput())
			MessageBox(hwnd,TEXT("DirectInput init error!"),TEXT("Error"),MB_OK);
		//初始化D3Dfont
		if(!initText())
			MessageBox(hwnd,TEXT("myText init error!"),TEXT("Error"),MB_OK);

		//获得屏幕刷新率
		DEVMODEW dev;
		EnumDisplaySettings(NULL,0,&dev);
		hz = float(dev.dmDisplayFrequency)*0.001f;

		game_inited = true;
	}

	void release() {
		ReleaseText();
		D3DRelease();
		ReleaseDxInput();
		MyMusic::releaseMusic();
	}
}