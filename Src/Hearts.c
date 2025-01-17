/*!
@file	  GameOver.c
@author	  Justine Ilao (justine.c@digipen.edu)
@date     26/11/2022
@brief    This source file defines the GameOver states and all the function definitions to render all the
text in the Game Over screen

@license  Copyright � 2022 DigiPen, All rights reserved.
________________________________________________________________________________________________________*/
#include "cprocessing.h"
#include "GameOver.h"
#include "Hearts.h"
#include "SoundManager.h"
#include "UIManager.h"
#include "ColorTable.h"
#include "Grid.h"

HeartContainer heart_stats[MAX_HEART_COUNT];
CP_Image heart_image;
TextOnlyHearts life_text;
TextOnlyHearts lose_text;
RectHearts lose_bg;

/*----------------------------------------------------------------------------
@brief Initializes the life image load, and fills in the HeartContainer struct
for the data of each heart to be initialized
------------------------------------------------------------------------------*/
void InitializeLife(void) {
	heart_image = CP_Image_Load("Assets/Heart.png");
	for (int i = 0; i < MAX_HEART_COUNT; ++i) {
		heart_stats[i].heartAlive = 1; // Boolean True
		heart_stats[i].xpos = CP_System_GetWindowWidth() / 4.f + (CP_System_GetWindowWidth() / 7.5f) * HEART_SPACING * i; // Spaces out the hearts by the size of the heart and adding the additional spacing
		heart_stats[i].ypos = (CP_System_GetWindowHeight() / 10.f - GetCellSize());
		heart_stats[i].size = CP_System_GetWindowWidth() / (CP_System_GetWindowWidth() * HEART_SIZE) * GetWidthScale();
		heart_stats[i].alpha = HEART_ALPHA;
		heart_stats[i].rotation = HEART_ROTATION;
	}

	// Initialize text for life
	life_text.color = TETRIS_COLOR;
	life_text.font_size = CP_System_GetWindowHeight() / 20.f;
	life_text.xpos = GetCellSize();
	life_text.ypos = CP_System_GetWindowHeight() / 17.5f;
	life_text.words = "HEARTS";

	// Make box for You Lose title card after losing all life
	lose_bg.xpos = CP_System_GetWindowWidth() / 2.f;
	lose_bg.ypos = CP_System_GetWindowHeight() / 2.f;
	lose_bg.width = (float)CP_System_GetWindowWidth();
	lose_bg.height = CP_System_GetWindowHeight() / 3.f;

	// Text for You Lose title card after losing all life
	lose_text.color = TETRIS_SLOT_COLOR;
	lose_text.font_size = CP_System_GetWindowHeight() / 10.f;
	lose_text.xpos = CP_System_GetWindowWidth() / 2.f;
	lose_text.ypos = CP_System_GetWindowHeight() / 2.f;
	lose_text.words = "YOU DIED!";
}

/*----------------------------------------------------------------------------
@brief Initializes the You Lose rendering and transistions to the Game Over screen
after a few seconds
------------------------------------------------------------------------------*/
static float time_elapsed_lose = 0.f;
void YouLoseTitleRender(void) {
	float bg_transistion_time_max = 2.f;
	float stay_time_max = 5.8f;

	time_elapsed_lose += CP_System_GetDt();

	// BG Drawing
	float bg_transistion_time = time_elapsed_lose / bg_transistion_time_max;
	float stay_time = time_elapsed_lose / stay_time_max;
	int alpha_bg = CP_Math_ClampInt((int)(255 * bg_transistion_time), 0, 255);
	// Draw BG
	CP_Settings_RectMode(CP_POSITION_CENTER);
	CP_Settings_Stroke(GRID_COLOR);
	CP_Settings_StrokeWeight(CP_System_GetWindowHeight() / 250.f);
	CP_Settings_Fill(CP_Color_Create(35, 35, 35, alpha_bg));
	CP_Graphics_DrawRect(lose_bg.xpos, lose_bg.ypos, lose_bg.width, lose_bg.height);
	// Draw Text
	CP_Settings_Fill(CP_Color_Create(227, 23, 23, alpha_bg)); // Color of text
	CP_Settings_TextSize(lose_text.font_size); // Size of text
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE); // Origin of text is it's absolute center
	CP_Font_DrawText(lose_text.words, lose_text.xpos, lose_text.ypos);

	if (stay_time >= 0.9f) {
		time_elapsed_lose = 0.f;
		CP_Engine_SetNextGameState(GameOverInit, GameOverUpdate, GameOverExit);
	}
}

//-----------------------
//  INPUTS
//-----------------------

/*----------------------------------------------------------------------------
@brief Function to allow for the gaining of life
------------------------------------------------------------------------------*/
void GainLife(int gain_life) {
	for (int i = 0; i < MAX_HEART_COUNT; ++i) {
		if (heart_stats[i].heartAlive == 0) {
			heart_stats[i].heartAlive = 1;
			heart_stats[i].alpha = 255;
			break;
		}
	}
	PlaySound(GAINHEART, CP_SOUND_GROUP_SFX);
}

/*----------------------------------------------------------------------------
@brief Function to allow for the losing of life
------------------------------------------------------------------------------*/
void LoseLife(int lose_life) {
	int i = 0;
	float total_flicker_time = 5.f;
	if (i < MAX_HEART_COUNT && heart_stats[0].heartAlive != 0) {
		while (heart_stats[i].heartAlive == 1) {
			++i;
		}
		if (i == 0) ++i;
		heart_stats[i - 1].heartAlive = 0;
		heart_stats[i - 1].alpha = 0;
	}
	PlaySound(LOSE, CP_SOUND_GROUP_SFX);
	// TO DO : Add Lose Life Animation
}

//-----------------------
// CHECKS
//-----------------------

/*----------------------------------------------------------------------------
@brief Checks if there are zero hearts, then if there are no hearts left for the player,
play the Game Over sound
------------------------------------------------------------------------------*/
int CheckLoseCondition(void) {
	// Sum the total of heartAlive in heart_stats to total_life
	int total_life = 0;
	for (int i = 0; i < MAX_HEART_COUNT; ++i) {
		total_life += heart_stats[i].heartAlive;
	}
	if (total_life == 0) {
		PlaySound(GAMEOVER, CP_SOUND_GROUP_SFX);
		return 1;
	}
	return 0;
}

//-----------------------
// RENDERS
//-----------------------

/*----------------------------------------------------------------------------
@brief Initializes the You Lose rendering and transistions to the Game Over screen
after a few seconds
------------------------------------------------------------------------------*/
void DrawLife(void) {
	for (int i = 0; i < MAX_HEART_COUNT; ++i) {
		if (heart_stats[i].heartAlive == 1) {
			CP_Image_DrawAdvanced(heart_image, heart_stats[i].xpos, heart_stats[i].ypos, (float)IMAGE_HEART_LENGTH * heart_stats[i].size, (float)IMAGE_HEART_LENGTH * heart_stats[i].size, heart_stats[i].alpha, heart_stats[i].rotation);
		}
	}
}
/*----------------------------------------------------------------------------
@brief Renders the heart text
------------------------------------------------------------------------------*/
void RenderHeartText(void) {
	CP_Settings_Fill(life_text.color); // Color of text
	CP_Settings_TextSize(life_text.font_size); // Size of text
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_TOP); // Origin of text is it's absolute center
	CP_Font_DrawText(life_text.words, life_text.xpos, life_text.ypos); // Draw text
}

/*----------------------------------------------------------------------------
@brief Function for Bezier Curve
@param Place the tick time here, to have a smoother animation
------------------------------------------------------------------------------*/
float BezierCurve(float t) {
	return t * t * (3.0f - 2.0f * t);
}

// AnimationLife variables
static float time_elapsed = 0;
static int up_tick_marker = 1;
static int down_tick_marker = 0;
static float duration = 0.75f; // 0.75 Seconds to loop

/*----------------------------------------------------------------------------
@brief Animates the hearts to go up and down
------------------------------------------------------------------------------*/
void AnimationLife(void) {
	float tick = time_elapsed / duration;
	// To check whether the heart needs to go down or up
	if (up_tick_marker == 1) time_elapsed += CP_System_GetDt();
	else if (down_tick_marker == 1) time_elapsed -= CP_System_GetDt();

	// Animate each heart
	for (int i = 0; i < MAX_HEART_COUNT; ++i) {
		heart_stats[i].ypos = CP_Math_LerpFloat(CP_System_GetWindowWidth() / 22.f, (CP_System_GetWindowWidth() / 22.f) - (CP_System_GetWindowWidth() / 22.f) * 0.1f, BezierCurve(tick));
	}
	// Check if it should be down ticking or up ticking
	if (time_elapsed >= duration) {
		down_tick_marker = 1;
		up_tick_marker = 0;
	}
	else if (time_elapsed <= 0) {
		up_tick_marker = 1;
		down_tick_marker = 0;
	}

}

/*----------------------------------------------------------------------------
@brief Updates the life to check for the lose condition, render the hearts and the
heart text and if the hearts are 0, start the You Lose render
------------------------------------------------------------------------------*/
void UpdateLife(void) {
	//Checks
	CheckLoseCondition();
	//Renders
	RenderHeartText();
	DrawLife();

	AnimationLife();
	if (CheckLoseCondition() == 1) YouLoseTitleRender();
}

/*----------------------------------------------------------------------------
@brief Clear the heart image that was loaded previously
------------------------------------------------------------------------------*/
void ClearHearts(void) {
	CP_Image_Free(&heart_image);
}
