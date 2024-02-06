#include "CinematicCamera.h"

using namespace NCL;
using namespace CSC8503;

const float LERP_MULTIPLIER = 0.15f;

void CinematicCamera::GetCameraInfo(Camera* camera)
{
    // print camera information to the console
    std::cout << "Position: " << camera->GetPosition() << std::endl;
    std::cout << "Pitch: " << camera->GetPitch() << std::endl;
    std::cout << "Yaw: " << camera->GetYaw() << std::endl;
}

Vector3 CinematicCamera::Lerp(Vector3& start, Vector3 end, float time)
{
    Vector3 newPos = Vector3(
        start.x + (end.x - start.x) * time * LERP_MULTIPLIER,
        start.y + (end.y - start.y) * time * LERP_MULTIPLIER,
        start.z + (end.z - start.z) * time * LERP_MULTIPLIER
    );
    return newPos;
}

void NCL::CSC8503::CinematicCamera::ReadPositionsFromFile(std::string filename)
{
    cameraPositions.clear();
    std::ifstream file(Assets::LEVELDIR + filename);
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

    }

    maxCameras = cameraPositions.size() / 2;
    file.close();
}

void NCL::CSC8503::CinematicCamera::UpdateCinematicCamera(Camera* camera)
{
    camera->SetPosition(Lerp(cameraPositions[currentCamera], cameraPositions[currentCamera + 1], timer));
    camera->SetPitch(pitches[currentCamera]);
    camera->SetYaw(yaws[currentCamera]);
}
