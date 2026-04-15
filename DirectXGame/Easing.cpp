#include "Easing.h"
#include <algorithm>

float EaseOut(float start, float end, float t) {
	t = std::clamp(t, 0.0f, 1.0f);

	float easedT = t * (2.0f - t);

	return start + (end - start) * easedT;
}

float EaseIn(float start, float end, float t) {
	t = std::clamp(t, 0.0f, 1.0f);

	float easedT = t * t;

	return start + (end - start) * easedT;
}