#pragma once
#include "raudio.h"


namespace NCL {
    namespace CSC8503 {

        class SoundManager {

        public:

            SoundManager();
            ~SoundManager();

            void SM_AddSoundsToLoad(std::vector<std::string> sounds);
            void SM_AddSoundToLoad(std::string fileName);

            bool SM_LoadSoundList();
            void SM_UnloadSoundList();

            // SM_ is here cuz Raudio has already claimed the names I wanted and now I am malding.
            void SM_PlaySound(std::string soundName);
            void SM_StopSound(std::string soundName);
            void SM_ResumeSound(std::string soundName);
            bool SM_IsSoundPlaying(std::string soundName);
            void SM_LoopIfEnd(std::string soundName);

            bool SM_IsSoundReady(std::string soundName);

            void SM_AddSongsToLoad(std::vector<std::string> songs);
            void SM_AddSongToLoad(std::string fileName);

            std::string SM_SelectRandomSong();
            std::string GetCurrentSong() { return currentSong; }
            int GetLoadedSoundCount() {
                return sounds.size();
            }

        protected:

            std::vector<std::string> soundFileNames;
            std::unordered_map<std::string, Sound*> sounds;
            std::vector<std::string> songFileNames;
            std::string currentSong;
        };


    }
}