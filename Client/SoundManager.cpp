#include "SoundManager.h"
#include "Assets.h"

using namespace NCL;
using namespace CSC8503;

SoundManager::SoundManager() {
	InitAudioDevice();
}

void SoundManager::AddSoundToLoad(std::string fileName) {
	soundFileNames.push_back(fileName);
}

void SoundManager::AddSoundsToLoad(std::vector<std::string> fileNames) {
	for (std::string fn : fileNames) {
		AddSoundToLoad(fn);
	}
}

void SoundManager::SM_PlaySound(std::string soundName) {
	PlaySound(*sounds[soundName]);
}

void SoundManager::SM_StopSound(std::string soundName) {
	StopSound(*sounds[soundName]);
}

void SoundManager::SM_ResumeSound(std::string soundName) {
	ResumeSound(*sounds[soundName]);
}

bool SoundManager::SM_IsSoundPlaying(std::string soundName) {
	return IsSoundPlaying(*sounds[soundName]);
}

void SoundManager::SM_LoopIfEnd(std::string soundName) {
	if (!SM_IsSoundPlaying(soundName)) SM_PlaySound(soundName);
}

bool SoundManager::LoadSoundList() {

	for (std::string fn : soundFileNames) {
		std::string path = Assets::SOUNDSDIR + fn;
		Sound* sound = new Sound();
		*sound = LoadSound(path.c_str());
		sounds.insert({ fn, sound });
		if (!IsSoundReady(*sounds[fn])) return false;
	}
	return true;
}

void SoundManager::UnloadSoundList() {

	for (std::string fn : soundFileNames) {
		UnloadSound(*sounds[fn]);
		delete sounds[fn];
	}
	sounds.clear();
	soundFileNames.clear();
}

SoundManager::~SoundManager() {
	CloseAudioDevice();
}