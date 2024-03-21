#include "SoundManager.h"
#include "Assets.h"

using namespace NCL;
using namespace CSC8503;

SoundManager::SoundManager() {
	InitAudioDevice();
}

void SoundManager::SM_AddSoundToLoad(std::string fileName) {
	soundFileNames.push_back(fileName);
}

void SoundManager::SM_AddSoundsToLoad(std::vector<std::string> fileNames) {
	for (std::string fn : fileNames) {
		SM_AddSoundToLoad(fn);
	}
}

void SoundManager::SM_PlaySound(std::string soundName) {
	if (!SM_IsSoundReady(soundName)) return;
	PlaySound(*sounds[soundName]);
}

void SoundManager::SM_StopSound(std::string soundName) {
	if (!SM_IsSoundReady(soundName)) return;
	StopSound(*sounds[soundName]);
}

void SoundManager::SM_PauseSound(std::string soundName)
{
	if (!SM_IsSoundReady(soundName)) return;
	PauseSound(*sounds[soundName]);
}

void SoundManager::SM_ResumeSound(std::string soundName) {
	if (!SM_IsSoundReady(soundName)) return;
	ResumeSound(*sounds[soundName]);
}

bool SoundManager::SM_IsSoundPlaying(std::string soundName) {
	if (!SM_IsSoundReady(soundName)) return false;
	return IsSoundPlaying(*sounds[soundName]);
}

void SoundManager::SM_LoopIfEnd(std::string soundName) {
	if (!SM_IsSoundReady(soundName)) return;
	if (!SM_IsSoundPlaying(soundName)) SM_PlaySound(soundName);
}

void SoundManager::SM_SetSoundVolume(std::string soundName, float volume) {
	if (!SM_IsSoundReady(soundName)) return;
	SetSoundVolume(*sounds[soundName], volume);
}

void SoundManager::SM_SetSoundPitch(std::string soundName, float pitch) {
	if (!SM_IsSoundReady(soundName)) return;
	SetSoundPitch(*sounds[soundName], pitch);
}

void SoundManager::SM_AddSongToLoad(std::string fileName) {
	songFileNames.push_back(fileName);
}

void SoundManager::SM_AddSongsToLoad(std::vector<std::string> fileNames) {
	for (std::string fn : fileNames) {
		SM_AddSongToLoad(fn);
	}
}

bool SoundManager::SM_IsSoundReady(std::string soundName) {
	return sounds.contains(soundName);
}

std::string SoundManager::SM_SelectRandomSong() {
	int totalSongs = songFileNames.size();
	if (totalSongs == 0) return "";
	srand(time(NULL));
	int randomSong = (rand() % totalSongs);
	currentSong = songFileNames[randomSong];
	return currentSong;
}

bool SoundManager::SM_LoadSoundList() {

	for (std::string fn : soundFileNames) {
		std::string path = Assets::SOUNDSDIR + fn;
		Sound* sound = new Sound();
		*sound = LoadSound(path.c_str());
		if (!IsSoundReady(*sound)) return false;
		sounds.insert({ fn, sound });
	}
	return true;
}

void SoundManager::SM_UnloadSoundList() {

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