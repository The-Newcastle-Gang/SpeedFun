#include "CinematicCamera.h"
#include "CinematicCamera.h"

using namespace NCL;
using namespace CSC8503;

void CinematicCamera::WriteCameraInfo(Camera* camera, std::string filename)
{
    std::ofstream file;
    file.open(Assets::LEVELDIR + filename, std::ios_base::app);
    file << camera->GetPosition().x << "," << camera->GetPosition().y << "," << camera->GetPosition().z
        << "," << camera->GetPitch() << "," << camera->GetYaw() << "\n";
    file.close();
}

void CinematicCamera::AddInitialCamera(Vector3 position)
{
    cameraPositions.emplace_back(position);
    pitches.emplace_back(0.68f);
    yaws.emplace_back(269.43f);

    maxCameras = cameraPositions.size();
    if (isContinuous) { maxCameras--; } // avoid overflow
}

Vector3 CinematicCamera::LerpVector3(Vector3& start, Vector3 end, float p)
{
    Vector3 newPos = Vector3(
        start.x + (end.x - start.x) * p,
        start.y + (end.y - start.y) * p,
        start.z + (end.z - start.z) * p
    );
    return newPos;
}

float CinematicCamera::LerpYaw(float start, float end, float p)
{
    float difference = end - start;
    if (difference > 180.0f) { difference -= 360.0f; }
    if (difference <= -180.0f) { difference += 360.0f; }
    return start + difference * p;
}


void CinematicCamera::ReadPositionsFromFile(std::string filename)
{
    cameraPositions.clear();
    std::ifstream file(Assets::CAMERADIR + filename);
    if (!file.is_open()) { std::cerr << "Cannot open file"; }

    std::string line;
    char _; // this is used to discard the commas. Just ignore it.
    while (std::getline(file, line))
    {
        Vector3 tempVec;
        float tempPitch;
        float tempYaw;

        std::istringstream ss(line);
        ss >> tempVec.x >> _ >> tempVec.y >> _ >> tempVec.z >> _ >> tempPitch >> _ >> tempYaw;

        cameraPositions.emplace_back(tempVec);
        pitches.emplace_back(tempPitch);
        yaws.emplace_back(tempYaw);

    }
    file.close();
}

void CinematicCamera::UpdateCinematicCamera(Camera* camera)
{
    currentCamera = (((int)std::floor(timer)) * (isContinuous ? 1 : 2)) % (maxCameras - 1);
    float timerRemainder = std::fmodf(timer, 1.0f);

    float percentage = timerRemainder / MAX_TIMER;

    std::cout << "Camera: " << currentCamera << std::endl;

    
    camera->SetPosition(LerpVector3(cameraPositions[currentCamera], cameraPositions[currentCamera + 1], percentage));
    camera->SetPitch(std::lerp(pitches[currentCamera], pitches[currentCamera + 1], percentage));
    camera->SetYaw(LerpYaw(yaws[currentCamera], yaws[currentCamera + 1], percentage));

    

}