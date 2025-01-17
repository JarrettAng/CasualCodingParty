/*!
@file	  Win.c
@author	  Justine Ilao (justine.c@digipen.edu)
@date     26/11/2022
@brief    This source file defines the GameOver states and all the function definitions to render all the
		  text in the Game Over screen

@license  Copyright � 2022 DigiPen, All rights reserved.
________________________________________________________________________________________________________*/
#include "cprocessing.h"
#include "ColorTable.h"
#include "GameOver.h"
#include "Grid.h"
#include "MainMenu.h"
#include "UIManager.h"
#include "Hearts.h"
#include "Win.h"
#include "WaveSystem.h"
#include <stdio.h>
#include "SoundManager.h"

CP_Font main_font;
Button backtomainmenuBtn;
CP_Image selectPointer;

TextOnlyHearts win_text;
RectHearts win_bg;

TextOnly Win_Title;
TextOnly Enemies_Killed;
TextOnly Enemies_Killed_Number;
TextOnly Turns_Made;
TextOnly Turns_Made_Number;

/*----------------------------------------------------------------------------
@brief Initializes the buttons for the Win Screen
------------------------------------------------------------------------------*/
void InitializeButtonsWinScreen(void) {
	float xPos = (float)CP_System_GetWindowWidth() / 6;
	float yPos = (float)CP_System_GetWindowHeight() / 2;

	/*==============Button Graphics & Text Settings===================*/
	// Currently all buttons in menu are using this graphic & text setting

	GraphicData GameOverGraphicData = {
	.color = TRANSPERANT,
	.strokeColor = TRANSPERANT,
	.strokeWeight = 0,
	// Draw from top left corner so menu button aligns.
	.imagePosMode = CP_POSITION_CORNER
	};

	TextData GameOverTextData = {
	.color = MENU_WHITE,
	.font = CP_Font_Load("Assets/PressStart2P-Regular.ttf"),
	.textSize = 50 * GetHeightScale(),
	// Text will be drawn on the X Y pos of btn rect (which is top left corner),
	// So we shift text to top left.
	.hAlign = CP_TEXT_ALIGN_H_LEFT,
	.vAlign = CP_TEXT_ALIGN_V_TOP,
	};

	/*========================Main Menu button============================*/
	Rect BackMainMenuBtnRect = {
	.x = xPos - CP_System_GetWindowWidth() / 10,
	.y = yPos + CP_System_GetWindowHeight() / 3,
	.heigth = GameOverTextData.textSize,
	.width = 400 * GetWidthScale(),
	};

	GameOverTextData.text = "MAIN MENU";
	// Intialize button in UI manager.
	// Onclick: Start game.
	InitializeButton(&backtomainmenuBtn, BackMainMenuBtnRect, GameOverGraphicData, GameOverTextData, BackToMainMenu);
}

//===========================================
// Text and Button function definitions
//===========================================

/*----------------------------------------------------------------------------
@brief Initializes the text for the Win Screen
------------------------------------------------------------------------------*/
void InitializeAllTextWin(void) {
	Win_Title.color = MENU_WHITE;
	Win_Title.font_size = CP_System_GetWindowWidth() / 15.f;
	Win_Title.words = "YOU WIN! :D";
	Win_Title.xpos = CP_System_GetWindowWidth() / 2.f;
	Win_Title.ypos = CP_System_GetWindowHeight() / 4.f;

	Enemies_Killed.words = "ZOMBOIYOS KILLED :";
	Enemies_Killed.font_size = CP_System_GetWindowWidth() / 30.f;
	Enemies_Killed.color = MENU_WHITE;
	Enemies_Killed.xpos = CP_System_GetWindowWidth() / 2.f;
	Enemies_Killed.ypos = CP_System_GetWindowHeight() * 1.5f / 3.f;


	Turns_Made.words = "TURNS MADE :";
	Turns_Made.font_size = CP_System_GetWindowWidth() / 30.f;
	Turns_Made.color = MENU_WHITE;
	Turns_Made.xpos = CP_System_GetWindowWidth() / 2.f;
	Turns_Made.ypos = CP_System_GetWindowHeight() * 1.3f / 2.f;
}

/*----------------------------------------------------------------------------
@brief Renders all the text for the Win Screen as well as the tracked number for
the Waves and the amount of Enemies Killed
------------------------------------------------------------------------------*/
void RenderAllTextWin(void) {
	RenderGOText(Win_Title);

	char wave[25] = { 0 };
	sprintf_s(wave, 25, "WAVES SURVIVED : %02d/%02d", GetCurrentWave(), WAVES_TO_WIN);
	Turns_Made.words = wave;
	RenderGOText(Turns_Made);

	char killed[25] = { 0 };
	sprintf_s(killed, 25, "ZOMBOIYOS KILLED : %02d", GetEnemiesKilled());
	Enemies_Killed.words = killed;
	RenderGOText(Enemies_Killed);
}

//=========================================================
// 
// WIN STATE DEFINITIONS
// 
//=========================================================


/*----------------------------------------------------------------------------
@brief Initializes the text and background for the title card 
for the transition when winning the game
------------------------------------------------------------------------------*/
void WinTextInit(void) {
	// Make box for You Lose title card after losing all life
	win_bg.xpos = CP_System_GetWindowWidth() / 2.f;
	win_bg.ypos = CP_System_GetWindowHeight() / 2.f;
	win_bg.width = (float)CP_System_GetWindowWidth();
	win_bg.height = CP_System_GetWindowHeight() / 3.f;

	// Text for You Lose title card after losing all life
	win_text.color = TETRIS_SLOT_COLOR;
	win_text.font_size = CP_System_GetWindowHeight() / 10.f;
	win_text.xpos = CP_System_GetWindowWidth() / 2.f;
	win_text.ypos = CP_System_GetWindowHeight() / 2.f;
	win_text.words = "YOU SURVIVED!";
}

/*----------------------------------------------------------------------------
@brief Checks if the player has won the game
------------------------------------------------------------------------------*/
void CheckWinCondition(void) {
	if (GetCurrentWave() >= WAVES_TO_WIN && IsAllEnemiesDead() && CheckLoseCondition() != 1) WinAnimation();
}

/*----------------------------------------------------------------------------
@brief Renders the Win Animation
------------------------------------------------------------------------------*/
static float time_elapsed = 0.f;
void WinAnimation(void) {
	//Makes it such that the win sound only plays once
	if(time_elapsed <=0) PlaySound(GAMEWIN,CP_SOUND_GROUP_SFX);
	float bg_transistion_time_max = 2.f;
	float stay_time_max = 5.8f;

	time_elapsed += CP_System_GetDt();

	// BG Drawing
	float bg_transistion_time = time_elapsed / bg_transistion_time_max;
	float stay_time = time_elapsed / stay_time_max;
	int alpha_bg = CP_Math_ClampInt((int)(255 * bg_transistion_time), 0, 255);
	// Draw BG
	CP_Settings_RectMode(CP_POSITION_CENTER);
	CP_Settings_Stroke(GRID_COLOR);
	CP_Settings_StrokeWeight(CP_System_GetWindowHeight() / 250.f);
	CP_Settings_Fill(CP_Color_Create(35, 35, 35, alpha_bg));
	CP_Graphics_DrawRect(win_bg.xpos, win_bg.ypos, win_bg.width, win_bg.height);
	// Draw Text
	CP_Settings_Fill(CP_Color_Create(227, 23, 23, alpha_bg)); // Color of text
	CP_Settings_TextSize(win_text.font_size); // Size of text
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE); // Origin of text is it's absolute center
	CP_Font_DrawText(win_text.words, win_text.xpos, win_text.ypos);

	if (stay_time >= 0.9f) {
		time_elapsed = 0.f;
		CP_Engine_SetNextGameState(WinInit, WinUpdate, WinExit);
	}
}

/*----------------------------------------------------------------------------
@brief WinInit state, loads the background, font and all the texts and buttons
------------------------------------------------------------------------------*/
void WinInit(void) {
	CP_Graphics_ClearBackground(GAMEOVER_DARKGRAY);
	main_font = CP_Font_Load("Assets/PressStart2P-Regular.ttf");

	// Populate buttons with positional, size and text values
	InitializeButtonsWinScreen();
	// Create the texts
	InitializeAllTextWin();
}

/*----------------------------------------------------------------------------
@brief Updates the Win Screen, renders the background, font, all the texts and
buttons
------------------------------------------------------------------------------*/
void WinUpdate(void) {
	CP_Graphics_ClearBackground(GAMEOVER_DARKGRAY);
	CP_Font_Set(main_font); // Uses main_font for all fonts

	// Render text
	RenderAllTextWin();

	// Draw buttons
	RenderButtons();
	HandleButtonClick();
}

/*----------------------------------------------------------------------------
@brief WinExit state, reset the wave and the enemies killed counter and also frees
the UI elements like the buttons
------------------------------------------------------------------------------*/
void WinExit(void) {
	ResetGame();
	ClearInteractCache();
	FreeUI();
}
