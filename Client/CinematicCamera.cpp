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

Vector3 CinematicCamera::LerpVector3(Vector3& start, Vector3 end, float p)
{
    Vector3 newPos = Vector3(
        start.x + (end.x - start.x) * p,
        start.y + (end.y - start.y) * p,
        start.z + (end.z - start.z) * p
    );
    return newPos;
}

float NCL::CSC8503::CinematicCamera::LerpYaw(float start, float end, float p)
{
    float difference = end - start;
    if (difference > 180.0f) { difference -= 360.0f; }
    if (difference <= -180.0f) { difference += 360.0f; }
    return start + difference * p;
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
        pitches.emplace_back(tempPitch);
        yaws.emplace_back(tempYaw);

    }
    file.close();

    maxCameras = cameraPositions.size();
    if (isContinuous) { maxCameras--; } // avoid overflow
}

void NCL::CSC8503::CinematicCamera::UpdateCinematicCamera(Camera* camera, float dt)
{
    timer += dt;
    float percentage = timer / MAX_TIMER;

    camera->SetPosition(LerpVector3(cameraPositions[currentCamera], cameraPositions[currentCamera + 1], percentage));
    camera->SetPitch(std::lerp(pitches[currentCamera], pitches[currentCamera + 1], percentage));
    camera->SetYaw(LerpYaw(yaws[currentCamera], yaws[currentCamera + 1], percentage));

    if (timer >= MAX_TIMER)
    {
        timer = 0.0f;
        if (isContinuous)
        {
            currentCamera++;
        }
        else
        {
            currentCamera += 2;
        }

        if (currentCamera >= maxCameras) { currentCamera = 0; }
    }
}