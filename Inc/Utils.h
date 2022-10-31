#pragma once

// Base screen size.
// Aspect ratio: 16:9
#define BASE_WINDOW_WIDTH 1920
#define BASE_WINDOW_HEIGHT 1080

void UpdateUIScale();

float GetWindowWidth();
float GetWindowHeight();

float GetWidthScale();
float GetHeightScale();

typedef void(*Callback)();

// Struct to cache variables for a simple timer.
typedef struct Timer{
	float time;
	float elaspedTime;
}Timer;