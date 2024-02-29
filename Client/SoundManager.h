#pragma once
#include "raudio.h"

namespace NCL {
    namespace CSC8503 {

        class SoundManager {

        public:

            SoundManager();
            ~SoundManager();

            void AddSoundsToLoad(std::vector<std::string> sounds);
            void AddSoundToLoad(std::string fileName);

            bool LoadSoundList();
            void UnloadSoundList();

            void SM_PlaySound(std::string soundName);
            void SM_StopSound(std::string soundName);
            void SM_ResumeSound(std::string soundName);
            bool SM_IsSoundPlaying(std::string soundName);


        protected:

            std::vector<std::string> soundFileNames;
            std::unordered_map<std::string, Sound*> sounds;

        };


    }
}