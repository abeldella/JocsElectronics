#include "sound.h"

SoundManager * SoundManager::instance = NULL;

SoundManager::SoundManager()
{
	BASS_Init(AUDIO_DEVICE, 44100, 0, 0, NULL);
	total_channels = 0;
	channels.resize(20 * sizeof(HCHANNEL));
}

void SoundManager::startStream(const std::string& s, bool restart)
{
	auto it = SoundManager::s_streams.find(s);
	if (it == s_streams.end()) {
		std::cout << "Could not find stream " << std::endl;
	}
	else {	
		BOOL result = BASS_ChannelPlay(*(it->second), restart);
		if (result == FALSE)
			std::cerr << "Error [" << BASS_ErrorGetCode() << "] while playing sample" << std::endl;
	}
}

void SoundManager::stopStream(const std::string& s)
{
	auto it = SoundManager::s_streams.find(s);
	if (it == s_streams.end()) {
		std::cout << "Could not find stream " << std::endl;
	}
	else BASS_ChannelStop(*(it->second));
}

void SoundManager::startSound(const std::string& s, bool restart, const int &ch) 
{
	auto it = SoundManager::s_samples.find(s);
	if (it == s_samples.end()) {
		std::cout << "Could not find sample " << std::endl;
	}
	else BASS_ChannelPlay(channels[ch], restart);
}

bool SoundManager::loadSample(const std::string& s) 
{
	auto it = SoundManager::s_samples.find(s);
	if (it == s_samples.end())
	{
		std::string path = "data/sounds/" + s;
		HSAMPLE* sample = new HSAMPLE();

		*sample = BASS_SampleLoad(false, path.c_str(), 0, 0, 3, 0); //use BASS_SAMPLE_LOOP in the last param to have a looped sound

		if (sample == 0) {
			int err = BASS_ErrorGetCode();
			std::cerr << "Error [" << err << "] while loading sample " << path.c_str() << std::endl;
			return false;
		}
		s_samples.insert(std::pair<std::string, HSAMPLE*>(s, sample));

		channels[total_channels] = BASS_SampleGetChannel(*sample, false);
		if (channels[total_channels] == 0) {
			int err = BASS_ErrorGetCode();
			if (err != BASS_ERROR_NOCHAN)
				std::cerr << "Error [" << err << "] no channel id" << std::endl;
			return false;
		}
		total_channels++;
		if (total_channels > 10) std::cout << "You just have 10 channels it will fail!" << std::endl;

		return true;
	}
	return true;
}

bool SoundManager::loadStream(const std::string& s) 
{
	auto it = SoundManager::s_streams.find(s);
	if (it == s_streams.end())
	{
		std::string path = "data/sounds/" + s;
		HSTREAM* stream = new HSTREAM();
		if (BASS_StreamCreateFile(false, path.c_str(), 0, 0, 0) != 0)
		{
			*stream = BASS_StreamCreateFile(false, path.c_str(), 0, 0, 0);
			s_streams.insert(std::pair<std::string, HSTREAM*>(s, stream));
			return true;
		}
		std::cout << "Could not load stream " << s.c_str() << std::endl;
		return false;
	}
	else return true;
}

void SoundManager::streamVol(const std::string& s, const float &vol) 
{
	auto it = SoundManager::s_streams.find(s);
	if (it != s_streams.end()) {
		BASS_ChannelSetAttribute(*(it->second), BASS_ATTRIB_VOL, vol);
		return;
	}
	else std::cout << "Could not find stream " << s.c_str() << std::endl;
}

void SoundManager::soundVol(const std::string& s, const float &vol) 
{
	auto it = SoundManager::s_samples.find(s);
	if (it != s_samples.end()) {
		BASS_ChannelSetAttribute(*(it->second), BASS_ATTRIB_VOL, vol);
		return;
	}
	else std::cout << "Could not find stream " << s.c_str() << std::endl;
}
