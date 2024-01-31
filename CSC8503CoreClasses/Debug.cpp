#include "Debug.h"
using namespace NCL;

std::vector<Debug::DebugStringEntry>	Debug::stringEntries;
std::vector<Debug::DebugLineEntry>		Debug::lineEntries;

SimpleFont* Debug::debugFont = nullptr;

const Vector4 Debug::RED		= Vector4(1, 0, 0, 1);
const Vector4 Debug::GREEN		= Vector4(0, 1, 0, 1);
const Vector4 Debug::BLUE		= Vector4(0, 0, 1, 1);

const Vector4 Debug::BLACK		= Vector4(0, 0, 0, 1);
const Vector4 Debug::WHITE		= Vector4(1, 1, 1, 1);

const Vector4 Debug::YELLOW		= Vector4(1, 1, 0, 1);
const Vector4 Debug::MAGENTA	= Vector4(1, 0, 1, 1);
const Vector4 Debug::CYAN		= Vector4(0, 1, 1, 1);

void Debug::Print(const std::string& text, const Vector2& pos, const Vector4& colour) {
	DebugStringEntry newEntry;

	newEntry.data = text;
	newEntry.position = pos;
	newEntry.colour = colour;

	stringEntries.emplace_back(newEntry);
}

void Debug::DrawLine(const Vector3& startpoint, const Vector3& endpoint, const Vector4& colour, float time) {
	DebugLineEntry newEntry;

	newEntry.start = startpoint;
	newEntry.end = endpoint;
	newEntry.colourA = colour;
	newEntry.colourB = colour;
	newEntry.time = time;

	lineEntries.emplace_back(newEntry);
}

void Debug::DrawAABBLines(const Vector3& centre, const Vector3& halfSizes, const Vector4& colour, float time)
{
	float scalarX[4] = { 1,1,-1,-1 };
	float scalarZ[4] = { 1,-1,-1,1 };
	for (int i = 0; i < 4; i++)
	{
		Vector3 start = centre + halfSizes * Vector3(scalarX[i], 0, scalarZ[i]);
		Vector3 end = centre + halfSizes * Vector3(scalarX[(i + 1) % 4], 0, scalarZ[(i + 1) % 4]);
		DrawLine(start + Vector3(0, halfSizes.y, 0), end + Vector3(0, halfSizes.y, 0), colour, time);
		DrawLine(start - Vector3(0, halfSizes.y, 0), end - Vector3(0, halfSizes.y, 0), colour, time);
		DrawLine(start + Vector3(0, halfSizes.y, 0), start - Vector3(0, halfSizes.y, 0), colour, time);
	}
}

void Debug::DrawSphereLines(const Vector3& centre, const Quaternion& rotation, const float radius, const Vector4& colour, float time)
{
	const int circleAmount = 16;
	Vector3 spin[3] = { Vector3(1,0,0), Vector3(0,0,1), Vector3(0,1,0) };
	Vector3 trans[3] = { Vector3(0,radius,0),Vector3(0,radius,0),Vector3(radius, 0, 0) };
	for (int spn = 0; spn < 3; spn++)
	{
		for (int i = 0; i < circleAmount; i++)
		{
			Vector3 start = centre + (
				Matrix4(rotation) *
				Matrix4::Rotation(360 / (circleAmount - 2) * i, spin[spn]) *
				Matrix4::Translation(trans[spn])

				).GetPositionVector();
			Vector3 end = centre + (
				Matrix4(rotation) *
				Matrix4::Rotation(360 / (circleAmount - 2) * (i + 1), spin[spn]) *
				Matrix4::Translation(trans[spn])

				).GetPositionVector();
			DrawLine(start, end, colour, time);
		}
	}
}

void Debug::DrawCapsuleLines(const Vector3& centre, const Quaternion& direction, const float radius, const float halfSize, const Vector4& colour, float time)
{
	DrawSphereLines(centre + (Matrix3(direction) * Vector3(0, 1, 0)) * halfSize, direction, radius, colour, time);
	DrawSphereLines(centre - (Matrix3(direction) * Vector3(0, 1, 0)) * halfSize, direction, radius, colour, time);
}

void Debug::DrawOBBLines(const Vector3& centre, const Quaternion& rotaiton, const Vector3& halfSizes, const Vector4& colour, float time)
{
	float scalarX[4] = { 1,1,-1,-1 };
	float scalarZ[4] = { 1,-1,-1,1 };
	int upDown[2] = { 1,-1 };
	for (int ud = 0; ud < 2; ud++)
	{
		for (int i = 0; i < 4; i++)
		{

			Vector3 start = halfSizes * Vector3(scalarX[i], 0, scalarZ[i]);
			Vector3 end = halfSizes * Vector3(scalarX[(i + 1) % 4], 0, scalarZ[(i + 1) % 4]);

			Matrix4 transformStart =
				Matrix4::Translation(centre) *
				Matrix4(rotaiton) *
				Matrix4::Translation(start + Vector3(0, halfSizes.y * upDown[ud], 0));

			Matrix4 transformEnd =
				Matrix4::Translation(centre) *
				Matrix4(rotaiton) *
				Matrix4::Translation(end + Vector3(0, halfSizes.y * upDown[ud], 0));

			DrawLine(transformStart.GetPositionVector(), transformEnd.GetPositionVector(), colour, time);

		}
	}
	for (int i = 0; i < 4; i++)
	{
		Vector3 start = halfSizes * Vector3(scalarX[i], 0, scalarZ[i]);
		Matrix4 transformStart =
			Matrix4::Translation(centre) *
			Matrix4(rotaiton) *
			Matrix4::Translation(start + Vector3(0, halfSizes.y * upDown[0], 0));

		Matrix4 transformEnd =
			Matrix4::Translation(centre) *
			Matrix4(rotaiton) *
			Matrix4::Translation(start + Vector3(0, halfSizes.y * upDown[1], 0));

		DrawLine(transformStart.GetPositionVector(), transformEnd.GetPositionVector(), colour, time);

	}

}

void Debug::DrawAxisLines(const Matrix4& modelMatrix, float scaleBoost, float time) {
	Matrix4 local = modelMatrix;
	local.SetPositionVector({ 0, 0, 0 });

	Vector3 fwd = local * Vector4(0, 0, -1, 1.0f);
	Vector3 up = local * Vector4(0, 1, 0, 1.0f);
	Vector3 right = local * Vector4(1, 0, 0, 1.0f);

	Vector3 worldPos = modelMatrix.GetPositionVector();

	DrawLine(worldPos, worldPos + (right * scaleBoost), Debug::RED, time);
	DrawLine(worldPos, worldPos + (up * scaleBoost), Debug::GREEN, time);
	DrawLine(worldPos, worldPos + (fwd * scaleBoost), Debug::BLUE, time);
}

void Debug::UpdateRenderables(float dt) {
	int trim = 0;
	for (int i = 0; i < lineEntries.size(); ) {
		DebugLineEntry* e = &lineEntries[i];
		e->time -= dt;
		if (e->time < 0) {
			trim++;
			lineEntries[i] = lineEntries[lineEntries.size() - trim];
		}
		else {
			++i;
		}
		if (i + trim >= lineEntries.size()) {
			break;
		}
	}
	lineEntries.resize(lineEntries.size() - trim);
	stringEntries.clear();
}

SimpleFont* Debug::GetDebugFont() {
	if (!debugFont) {
		debugFont = new SimpleFont("PressStart2P.fnt", "PressStart2P.png");
	}
	return debugFont;
}

const std::vector<Debug::DebugStringEntry>& Debug::GetDebugStrings() {
	return stringEntries;
}

const std::vector<Debug::DebugLineEntry>& Debug::GetDebugLines() {
	return lineEntries;
}