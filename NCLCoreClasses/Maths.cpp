/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#include "Maths.h"
#include "Vector2.h"
#include "Vector3.h"

namespace NCL {
	namespace Maths {
		void ScreenBoxOfTri(const Vector3& v0, const Vector3& v1, const Vector3& v2, Vector2& topLeft, Vector2& bottomRight) {
			topLeft.x = std::min(v0.x, std::min(v1.x, v2.x));
			topLeft.y = std::min(v0.y, std::min(v1.y, v2.y));

			bottomRight.x = std::max(v0.x, std::max(v1.x, v2.x));
			bottomRight.y = std::max(v0.y, std::max(v1.y, v2.y));
		}

		int ScreenAreaOfTri(const Vector3 &a, const Vector3 &b, const Vector3 & c) {
			int area =(int) (((a.x * b.y) + (b.x * c.y) + (c.x * a.y)) -
				((b.x * a.y) + (c.x * b.y) + (a.x * c.y)));
			return (area >> 1);
		}

		float FloatAreaOfTri(const Vector3 &a, const Vector3 &b, const Vector3 & c) {
			float area = ((a.x * b.y) + (b.x * c.y) + (c.x * a.y)) -
				((b.x * a.y) + (c.x * b.y) + (a.x * c.y));
			return (area * 0.5f);
		}

		float CrossAreaOfTri(const Vector3 &a, const Vector3 &b, const Vector3 & c) {
			Vector3 area = Vector3::Cross(a - b, a - c);
			return area.Length() * 0.5f;
		}
	

		/*Vector3 Clamp(const Vector3& a, const Vector3 &mins, const Vector3& maxs) {
			return Vector3(
				Clamp(a.x, mins.x, maxs.x),
				Clamp(a.y, mins.y, maxs.y),
				Clamp(a.z, mins.z, maxs.z)
			);
		}*/

		//ed function
		Vector3 Clamp(const Vector3& input, const Vector3& mins, const Vector3& maxs) {

			return Vector3(
				std::clamp(input.x, mins.x, maxs.x),
				std::clamp(input.y, mins.y, maxs.y),
				std::clamp(input.z, mins.z, maxs.z)
			);
		}

		float fClamp(float input, const float lower, const float upper) {
			return std::max(lower, std::min(input, upper));
		}

		

	   //Returns the squared distance between point c and line Segment AB
		float SqDistPointSegment(const Vector3& a, const Vector3& b, const Vector3& c) {
			Vector3 ab = b - a;
			Vector3 ac = c - a;
			Vector3 bc = c - b;

			float e = Vector3::Dot(ac, ab);

			if (e <= 0.0f) return Vector3::Dot(ac, ac);
			float f = Vector3::Dot(ab, ab);
			if (e >= f) return Vector3::Dot(bc, bc);

			return Vector3::Dot(ac, ac) - e * e / f;
		}

		//Returns the closest point to point on the line segment ab
		Vector3 ClosestPointLineSegment(const Vector3& a, const Vector3& b, const Vector3& c) {
			Vector3 LineSeg = b - a;

			float fraction = (Vector3::Dot(c - a, LineSeg) / Vector3::Dot(LineSeg, LineSeg));
			fraction = fClamp(fraction, 0, 1);
			Vector3 d = a + Vector3(fraction, fraction, fraction) * LineSeg;
			return d;
		}

		float ClosetstPtSSegment(const Vector3& p1, const Vector3& q1,
			const Vector3& p2, const Vector3& q2,
			float& s, float& t,
			Vector3& c1, Vector3& c2) {

			Vector3 d1 = q1 - p1;                            //direction vector of s1
			Vector3 d2 = q1 - p2;                            //direction vector of s2
			Vector3 r = p1 - p2;

			float a = Vector3::Dot(d1, d1);          //sq len of segment s1
			float e = Vector3::Dot(d2, d2);          //sq len of segment s2
			float f = Vector3::Dot(d2, r);
			if (a <= 0.0001 && e <= 0.00001) {
				// Both segments degenerate into points
				s = t = 0.0f;
				c1 = p1;
				c2 = p2;
				return Vector3::Dot(c1 - c2, c1 - c2);
			}
			if (a <= 0.00001) {
				// First segment degenerates into a point
				s = 0.0f;
				t = f / e; // s = 0 => t = (b*s + f) / e = f / e
				t = Maths::fClamp(t, 0.0f, 1.0f);
			}
			else {
				float c = Vector3::Dot(d1, r);
				if (e <= 0.00001) {
					// Second segment degenerates into a point
					t = 0.0f;
					s = Maths::fClamp(-c / a, 0.0f, 1.0f); // t = 0 => s = (b*t - c) / a = -c / a
				}
				else {
					// The general nondegenerate case starts here
					float b = Vector3::Dot(d1, d2);
					float denom = a * e - b * b; // Always nonnegative

					if (denom != 0.0f) {
						s = Maths::fClamp((b * f - c * e) / denom, 0.0f, 1.0f);
					}
					else s = 0.0f;

					t = (b * s + f) / e;

					if (t < 0.0f) {
						t = 0.0f;
						s = Maths::fClamp(-c / a, 0.0f, 1.0f);
					}
					else if (t > 1.0f) {
						t = 1.0f;
						s = Maths::fClamp((b - c) / a, 0.0f, 1.0f);
					}
				}
			}
			c1 = p1 + d1 * s;
			c2 = p2 + d2 * t;
			return Vector3::Dot(c1 - c2, c1 - c2);
		}


	}
	
}