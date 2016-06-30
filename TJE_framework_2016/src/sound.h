#ifndef SOUND_H
#define SOUND_H
#include "includes.h"
#include "framework.h"
#include "bass.h"
#include <map>

#define AUDIO_DEVICE 1

class SoundManager
{
private:
	//Bullet bullets[MAX_BULLETS];
	SoundManager();

public:

	static SoundManager* instance;
	static SoundManager* getInstance() {
		if (instance == NULL)
			instance = new SoundManager();
		return instance;
	}

	std::map< std::string, HSAMPLE* > s_samples;
	std::map< std::string, HSTREAM* > s_streams;
	std::vector< HCHANNEL > channels;
	int total_channels;

	void startStream(const std::string& s, bool restart);
	void stopStream(const std::string& s);
	void startSound(const std::string& s, bool restart, const int &ch);
	bool loadSample(const std::string& s);
	bool loadStream(const std::string& s);
	void streamVol(const std::string& s, const float &vol);
	void soundVol(const std::string& s, const float &vol);
	
};

#endif 