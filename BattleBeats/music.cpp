#include "music.h"
using namespace std;

vector<MusicInfo *> vector_music;
libZPlay::ZPlay *player[NUM_AUDIO_TRACK];		//三个音轨

int nowplaying_position = -1;
unsigned int nowplaying_song_length = 0;
int amp_left[NUM_FFT+1];
int amp_right[NUM_FFT+1];

void getMusicList();

namespace MyMusic {
	void initMusic() {
		for(int i=0; i<NUM_AUDIO_TRACK; i++) {
			player[i] = libZPlay::CreateZPlay();
		}

		player[1]->OpenFileW(L"res/btn_mousein.wav", libZPlay::sfAutodetect);
		player[2]->OpenFileW(L"res/failsound.mp3", libZPlay::sfAutodetect);
		player[3]->OpenFileW(L"res/explosion.mp3", libZPlay::sfAutodetect);
		player[4]->OpenFileW(L"res/ship_fire.mp3", libZPlay::sfAutodetect);
		player[5]->OpenFileW(L"res/logo.mp3", libZPlay::sfAutodetect);
		player[6]->OpenFileW(L"res/warn.mp3", libZPlay::sfAutodetect);
		player[7]->OpenFileW(L"res/success.mp3", libZPlay::sfAutodetect);

		getMusicList();
	}

	void releaseMusic() {
		vector_music.clear();

		for(int i=0; i<NUM_AUDIO_TRACK; i++) {
			player[i]->Release();
		}
	}

	void playMusic(int music_position) {
		if(music_position < vector_music.size()) {
			player[0]->Close();
			player[0]->OpenFileW(vector_music[music_position]->path, libZPlay::sfAutodetect);
			player[0]->Play();
			nowplaying_position = music_position;
			nowplaying_song_length = getDuration();
		}
	}

	void stopMusic() {
		player[0]->Stop();
	}

	unsigned int getPosition() {
		libZPlay::TStreamTime pos;
		player[0]->GetPosition(&pos);
		return pos.ms;
	}

	wchar_t* getPositionStr() {
		libZPlay::TStreamTime pos;
		player[0]->GetPosition(&pos);

		wchar_t *rlt = new wchar_t[10];
		wsprintf(rlt, L"%02i:%02i", pos.hms.minute, pos.hms.second);
		return rlt;
	}

	unsigned int getDuration() {
		libZPlay::TStreamInfo pInfo;
		player[0]->GetStreamInfo(&pInfo);
		return pInfo.Length.ms;
	}

	wchar_t* getDurationStr() {
		libZPlay::TStreamInfo pInfo;
		player[0]->GetStreamInfo(&pInfo);

		wchar_t *rlt = new wchar_t[10];
		wsprintf(rlt, L"%02i:%02i", pInfo.Length.hms.minute, pInfo.Length.hms.second);
		return rlt;
	}

	void setVolume(unsigned int vol) {
		player[0]->SetPlayerVolume(vol,vol);
	}

	void playAudio(int track) {
		if(track < NUM_AUDIO_TRACK) {
			player[track]->Play();
		}
	}

	void stopAudio(int track) {
		if(track < NUM_AUDIO_TRACK) {
			player[track]->Stop();
		}
	}

	void updataFFT() {
		player[0]->GetFFTData(NUM_FFT * 2, libZPlay::fwTriangular, 0, 0,amp_left, amp_right, 0, 0);
	}
}

void getMusicList() {
	wchar_t *str_find = L"music/*.*";
	WIN32_FIND_DATA find_data;
	HANDLE handle_find = FindFirstFile(str_find, &find_data);

	if(handle_find != INVALID_HANDLE_VALUE) {
		while(true) {
			if(!(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				//如果不是文件夹的话
				MusicInfo *mi = new MusicInfo();
				wcscpy(mi->path, L"music/");
				wcscat(mi->path, find_data.cFileName);

				libZPlay::TID3InfoExW id3_info;
				if(player[0]->LoadFileID3ExW(mi->path, libZPlay::sfAutodetect, &id3_info, 0)) {
					wcscpy(mi->title, id3_info.Title);
					wcscpy(mi->artist, id3_info.Artist);
				} else {
					wcscpy(mi->title, find_data.cFileName);
				}

				vector_music.push_back(mi);
			}

			if(!FindNextFile(handle_find, &find_data))
				break;
		}
	}

	FindClose(handle_find);
}