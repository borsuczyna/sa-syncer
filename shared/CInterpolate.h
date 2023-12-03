#pragma once

class CInterpolate
{
public:
    static int Value(int from, int to, float time)
    {
        return (int)(from + (to - from) * time);
    }

    static float Value(float from, float to, float time)
    {
        return from + (to - from) * time;
    }

    static CVector Value(CVector from, CVector to, float time)
    {
        return from + (to - from) * time;
    }

    static CVector3 Value(CVector3 from, CVector3 to, float time)
    {
		return from + (to - from) * time;
	}

    static float RadToDeg(float rad)
    {
        return rad * (180.0f / static_cast<float>(M_PI));
    }

    static float DegToRad(float deg)
    {
        return deg * (static_cast<float>(M_PI) / 180.0f);
    }

    static float AngleDifference(float from, float to) {
		from = RadToDeg(from);
		to = RadToDeg(to);

		// Ensure angles are in the range [0, 360)
		from = fmodf((fmodf(from, 360.0f) + 360.0f), 360.0f);
		to = fmodf((fmodf(to, 360.0f) + 360.0f), 360.0f);

		// Calculate the shortest angular distance between from and to
		float distance = fmodf((to - from + 360.0f), 360.0f);

		// Adjust the distance for the shortest path, considering negative angles
        if (std::abs(distance) > 180.0f) {
			distance = fmodf((distance + 360.0f), 360.0f) - 360.0f;
		}

		return DegToRad(distance);
	}

    static float Angle(float from, float to, float value) {
        from = RadToDeg(from);
        to = RadToDeg(to);

        // Ensure angles are in the range [0, 360)
        from = fmodf((fmodf(from, 360.0f) + 360.0f), 360.0f);
        to = fmodf((fmodf(to, 360.0f) + 360.0f), 360.0f);

        // Calculate the shortest angular distance between from and to
        float distance = fmodf((to - from + 360.0f), 360.0f);

        // Adjust the distance for the shortest path, considering negative angles
        if (std::abs(distance) > 180.0f) {
            distance = fmodf((distance + 360.0f), 360.0f) - 360.0f;
        }

        // Calculate the interpolated angle
        float interpolatedAngle = from + value * distance;

        // Wrap the result to the range [0, 360)
        interpolatedAngle = fmodf((fmodf(interpolatedAngle, 360.0f) + 360.0f), 360.0f);

        return DegToRad(interpolatedAngle);
    }
};