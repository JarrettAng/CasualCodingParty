/*!
@file	  ColorTable.h
@author	  Amadeus Chia (amadeusjinhan.chia@digipen.edu)
@author	  Ang Jiawei Jarrett (a.jiaweijarrett@digipen.edu)
@author	  Justine Carlo Villa Ilao (justine.c@digipen.edu)
@author	  Muhammad Farhan Bin Ahmad (b.muhammadfarhan@digipen.edu)
@author	  Tan Jun Rong (t.junrong@digipen.edu)
@date     27/11/2022
@brief    This header file contains codes and CP_Colors for various colors

@license  Copyright � 2022 DigiPen, All rights reserved.
________________________________________________________________________________________________________*/

#pragma once // Only include this header file once

#include <cprocessing.h>

#define WHITE CP_Color_Create(255, 255, 255, 255)
#define BLACK CP_Color_Create(0, 0, 0, 255)
#define GREEN CP_Color_Create(0, 255, 0, 255)
#define TRANSPERANT CP_Color_Create(0,0,0,0)

#define TRANSPERANT_CODE 0,0,0,0

/*==============*/
/*	MENU COLORS	*/
/*==============*/
#define MENU_BLACK CP_Color_Create(35, 35, 35, 255)
#define MENU_GRAY CP_Color_Create(179, 179, 179, 255)
#define MENU_RED CP_Color_Create(227, 23, 23, 255)
#define MENU_WHITE CP_Color_Create(239, 239, 239, 255)
#define LOGO_RED CP_Color_Create(233, 0, 78,255)

#define MENU_RED_CODE 227, 23, 23, 255
#define MENU_LOGO_RED_CODE 233, 0, 78,255

/*======================*/
/*	GAME OVER COLORS	*/
/*======================*/
#define GAMEOVER_RED CP_Color_Create(175, 100, 100, 255)
#define GAMEOVER_DARKGRAY CP_Color_Create(25, 25 ,25 ,255)

/*=====================*/
/*	GAME LEVEL PIECES  */
/*=====================*/
#define GRID_COLOR CP_Color_Create(72, 72, 72, 255)

#define TETRIS_COLOR CP_Color_Create(197, 197, 197, 255)
#define TETRIS_SLOT_COLOR CP_Color_Create(66, 66, 66, 255)
#define TETRIS_ICON_WALL_COLOR CP_Color_Create(23, 23, 227, 255)
#define TETRIS_ICON_WALL_COLOR2 CP_Color_Create(139, 228, 235, 255)
#define TETRIS_ICON_ATTACK_COLOR CP_Color_Create(227, 23, 23, 255)
#define TETRIS_HOVER_COLOR CP_Color_Create(197, 197, 197, 130)
#define TETRIS_HOVER_RED_COLOR CP_Color_Create(227, 23, 23, 130)
#define TETRIS_HOVER_GREY_COLOR CP_Color_Create(157, 157, 157, 130)

#define HEALTH_GREEN CP_Color_Create(66, 219, 127, 150)
#define HEALTH_YELLOW CP_Color_Create(222, 206, 62, 150)
#define HEALTH_RED CP_Color_Create(227, 23, 23, 150)
#define ENEMY_MOVEMENT CP_Color_Create(227, 23, 23, 120)

#define TUTORIAL_COLOR CP_Color_Create(222, 206, 62, 255)
