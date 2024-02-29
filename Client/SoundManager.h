#pragma once
#include "raudio.h"

namespace NCL {
    namespace CSC8503 {

        class SoundManager {

        public:

            SoundManager();
            ~SoundManager();

            void AddSoundToLoad(std::string);

            bool LoadSoundList();
            void UnloadSoundList();

            void PlaySound(std::string);
            void StopSound(std::string);
            void ResumeSound(std::string);



        protected:

            std::vector<std::string> fileNames;
            std::unordered_map<std::string, Sound> sounds;
        };


    }
}