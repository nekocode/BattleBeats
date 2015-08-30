#ifndef H_MUSIC
#define H_MUSIC

#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include "libzplay/libzplay.h"

#define NUM_AUDIO_TRACK 10		//“ÙπÏ ˝¡ø
#define NUM_FFT 64

struct MusicInfo {
	wchar_t path[MAX_PATH];
	wchar_t title[100];
	wchar_t artist[100];
};

namespace MyMusic {
	void initMusic();
	void releaseMusic();
	void playMusic(int music_position);
	void stopMusic();
	void setVolume(unsigned int vol);
	unsigned int getPosition();
	wchar_t* getPositionStr();
	unsigned int getDuration();
	wchar_t* getDurationStr();
	void playAudio(int track);
	void stopAudio(int track);
	void updataFFT();
}
#endif