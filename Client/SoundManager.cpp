#include "SoundManager.h"
#include "Assets.h"

using namespace NCL;
using namespace CSC8503;

SoundManager::SoundManager() {
	InitAudioDevice();
}

void SoundManager::AddSoundToLoad(std::string fn) {
	fileNames.push_back(fn);
}

void SoundManager::PlaySound(std::string soundName) {
	PlaySound();
}

bool SoundManager::LoadSoundList() {

	for (std::string fn : fileNames) {
		std::string path = Assets::SOUNDSDIR + fn;
		sounds.insert({ fn, LoadSound(path.c_str()) });
		if (!IsSoundReady(sounds[fn])) return false;
	}
	return true;
}
SoundManager::~SoundManager() {
	CloseAudioDevice();
}