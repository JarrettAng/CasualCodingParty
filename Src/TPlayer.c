/*!
@file	  TPlayer.c
@author	  Ang Jiawei Jarrett (a.jiaweijarrett@digipen.edu)
@date     18/11/2022
@brief    This source file handles the rendering and storing of piece information of the Tetris pieces in the player's
		  hand and the peek queue. There are 7 functions in this file,

		  TPlayerInit - Needs to be called at the start of the game level so that the player has pieces to play with.

          TPlayerProcessInput - Needs to be called in update, checks for player clicks, updates TPlayerHeld to render the held piece accordingly.
          RenderHand - Needs to be called in update, renders everything in the player's hand, all the slots, pieces, text, etc.
          RemovePieceHeldFromHand - When a Tetris Piece is dropped onto the grid, remove it from the player's hand.

		  RemovePieceHeldFromHand - When a Tetris Piece is dropped onto the grid, remove the piece and update the player's hand.
		  ArrayShiftFowardFrom - Used by RemovePieceHeldFromHand to shift the pieces to the right of the piece used leftwards.
		  FreeIconImages - Needs to be called on the exit of game level, frees the icons for the attack and defend pieces.

@license  Copyright � 2022 DigiPen, All rights reserved.
________________________________________________________________________________________________________*/

#include "ColorTable.h"		// For Tetris colors

#include "Utils.h"			// For click detection
#include "SoundManager.h"	// For sound on Tetris interaction

#include "TManager.h"		// For drawing new Tetris pieces
#include "TPlayer.h"	
#include "TPlayerHeld.h"	// For handling the piece held
#include "GameLoop.h"		// For player's turn event

PlayerHandSlot hand[HAND_SIZE];			// Array containing information of all pieces on hand
PlayerHandSlot peek_hand[PEEK_SIZE];	// Array containing information of next few pieces in queue

CP_Vector text_peek_pos;
CP_Image attack_icon, shield_icon;


#pragma region
void RecalculateHandRenderPositions(void);
void ArrayShiftFowardFrom(PlayerHandSlot* array, int start, int end);
#pragma endregion Forward Declarations

//______________________________________________________________
// All "public" functions (Basically those in the TPlayer.h)

/*______________________________________________________________
@brief Needs to be called at the start of the game level (and after the Tetris Pieces
	   has been initialized) so that the player has pieces to play with.
*/
void TPlayerInit(void){
	//______________________________________________________________
	// Fill the player's hand and peek queue
	// First, fill the player's hand
	PlayerHandSlot* current;
	for (int index = 0; index < HAND_SIZE; ++index) {
		current = &hand[index];
		current->piece = DrawFromBag();
	}

	// Second, fill the peek's queue (Basically the upcoming pieces)
	for (int index = 0; index < PEEK_SIZE; ++index) {
		current = &peek_hand[index];
		current->piece = DrawFromBag();
	}

	// Update the positions to draw all the slots
	RecalculateHandRenderPositions();

	// Subscribe player input to player turn update
	SubscribeEvent(PLAYER_UPDATE, TPlayerProcessInput, 10);

	// Initialize the settings for piece held
	TPlayerHeldInit();

	// Load the icons
	attack_icon = CP_Image_Load("Assets/AttackIcon.png");
	shield_icon = CP_Image_Load("Assets/ShieldIcon.png");
	LoadIconImages(attack_icon, shield_icon); // For player held piece
}

/*______________________________________________________________
@brief Needs to be called in update, checks for player clicks, if the player clicks on a piece,
	   it updates TPlayerHeld to render the held piece accordingly.
*/
void TPlayerProcessInput(void){
	// When the player first clicks
	if (CP_Input_MouseTriggered(MOUSE_BUTTON_1)) {
		CP_Vector mouse_pos = CP_Vector_Set(CP_Input_GetMouseX(), CP_Input_GetMouseY());
		PlayerHandSlot* current;

		// Check which slot the player which on, if any
		for (int index = 0; index < HAND_SIZE; ++index) {
			current = &hand[index];
			// If we found the slot clicked on,
			if (pointWithinArea(current->pos.x, current->pos.y, hand_slot_length, hand_slot_length, mouse_pos.x, mouse_pos.y, CP_POSITION_CORNER)) {
				// Update the piece held render settings
				NewPieceHeld(&current->piece, index);

				// Play "piece selected" sound
				PlaySound(MOUSECLICK, CP_SOUND_GROUP_SFX);
				break;
			}
		}
	}

	TPlayerHeldProcessInput();
}

/*______________________________________________________________
@brief Renders the panel at the bottom of the panel, consisting of the player's hand, peek queue, and
	   supporting text like "< NEXT <"
*/
void RenderHand(void){
	PlayerHandSlot* current;

	CP_Settings_RectMode(CP_POSITION_CORNER);

	//______________________________________________________________
	// Render each piece in the player's hand
	for (int index = 0; index < HAND_SIZE; ++index) {
		current = &hand[index];

		CP_Settings_StrokeWeight(hand_tile_stroke);

		// Render the background square surrounding each piece
		CP_Settings_Fill(MENU_BLACK);
		CP_Settings_Stroke(current->piece.color);
		CP_Graphics_DrawRect(current->pos.x, current->pos.y, hand_slot_length, hand_slot_length);

		// Render the icon in the top-left corner
		if (index == 0) { // If wall piece
			CP_Image_Draw(shield_icon, current->icon_pos.x, current->icon_pos.y, text_icon_size.x, text_icon_size.y, 255);
		}
		else { // If attack piece
			CP_Image_Draw(attack_icon, current->icon_pos.x, current->icon_pos.y, text_icon_size.x, text_icon_size.y, 255);
		}

		if (IsThisPieceHeld(&current->piece)) continue; // Don't render the piece if it's held

		// Settings for tile rendering
		CP_Settings_StrokeWeight(hand_tile_stroke);
		CP_Settings_Fill(current->piece.color);
		CP_Settings_Stroke(current->piece.color_stroke);
		// Render each tile in the Tetris Piece
		float* x_screen_length = &current->piece.x_screen_length;
		float* y_screen_length = &current->piece.y_screen_length;
		for (int index_x = 0; index_x < SHAPE_BOUNDS; ++index_x) {
			for (int index_y = 0; index_y < SHAPE_BOUNDS; ++index_y) {
				if (current->piece.shape[index_x][index_y]) {
					CP_Graphics_DrawRect(current->piece.draw_pos.x + index_x * *x_screen_length, current->piece.draw_pos.y + index_y * *y_screen_length, *x_screen_length, *y_screen_length);
				}
			}
		}
	}

	//______________________________________________________________
	// Render each piece in the peek queue & the "NEXT" text
	CP_Settings_Fill(TETRIS_COLOR);
	CP_Settings_TextSize(text_peek_size);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_TOP);
	CP_Font_DrawText("<NEXT<", text_peek_pos.x, text_peek_pos.y);

	for (int index = 0; index < PEEK_SIZE; ++index) {
		current = &peek_hand[index];

		CP_Settings_StrokeWeight(peek_tile_stroke);

		// Render the background square surrounding each piece
		CP_Settings_Fill(MENU_BLACK);
		CP_Settings_Stroke(MENU_GRAY);
		CP_Graphics_DrawRect(current->pos.x, current->pos.y, peek_slot_length, peek_slot_length);

		// Settings for tile rendering
		CP_Settings_Fill(MENU_GRAY);
		CP_Settings_Stroke(current->piece.color_stroke);
		// Render each tile in the Tetris Piece
		float* x_screen_length = &current->piece.x_screen_length;
		float* y_screen_length = &current->piece.y_screen_length;
		for (int index_x = 0; index_x < SHAPE_BOUNDS; ++index_x) {
			for (int index_y = 0; index_y < SHAPE_BOUNDS; ++index_y) {
				if (current->piece.shape[index_x][index_y]) {
					CP_Graphics_DrawRect(current->piece.draw_pos.x + index_x * *x_screen_length, current->piece.draw_pos.y + index_y * *y_screen_length, *x_screen_length, *y_screen_length);
				}
			}
		}
	}

	RenderPieceHeld(); // Render the piece held last, so it is over the pieces in the player's hand
}

//______________________________________________________________
// More initialization functions

/*______________________________________________________________
@brief Recalculate the positions for rendering based on screen size and stores the new values
*/
void RecalculateHandRenderPositions(void){
	//______________________________________________________________
	// Initialize player hand factors
	hand_total_height = (float)CP_System_GetWindowHeight() * .175f; // The hand queue takes up 17.5% of the height
	hand_bottom_buffer = hand_total_height * .175f;					// The bottom buffer takes up 17.5% of the hand height

	float total_width = (float)CP_System_GetWindowWidth();
	hand_total_length = total_width * .55f;							// The hand queue takes up 55% of the width
	hand_left_buffer = hand_total_length * .1f;						// The hand edge buffers takes up 10% of the hand queue
	hand_slot_length = min(hand_total_height, (hand_total_length - hand_left_buffer) * .9f / HAND_SIZE);	 // The length of each slot depends on whether height or 90% of the hand queue width is shorter
	hand_slot_spacing = (hand_total_length - hand_left_buffer - (hand_slot_length * HAND_SIZE)) / HAND_SIZE; // The spacing of each slot is the remaining space

	// If the spacing between slots are too wide (more than 20% the length of a slot), add more padding to the left
	if ((hand_slot_spacing - hand_slot_length * 0.2f) > 0) {
		hand_left_extra_buffer = (hand_slot_spacing - hand_slot_length * 0.2f) * HAND_SIZE;
		hand_slot_spacing = hand_slot_length * 0.2f;
	}

	hand_tile_length = hand_slot_length / SHAPE_BOUNDS;				// How big each tile piece of a Tetris Piece is
	hand_tile_stroke = hand_tile_length * 0.15f;					// The stroke of each tile is 15% of the width of the tile

	//______________________________________________________________
	// Initialize peek hand factors
	peek_total_length = total_width - hand_total_length;			// The peek queue takes up the remaining width
	peek_right_buffer = peek_total_length * .25f;					// The peek edge buffer takes up 25% of the peek queue
	peek_slot_length = min(hand_total_height * .6f, (peek_total_length - peek_right_buffer) * .9f / PEEK_SIZE); // The length of each slot depends on whether height or 90% of the peek queue width is shorter;
	peek_slot_spacing = (peek_total_length - peek_right_buffer - (peek_slot_length * PEEK_SIZE)) / PEEK_SIZE;	// The spacing of each slot is the remaining space

	// If the spacing between slots are too wide (more than 33% the length of a slot), reduce the spacing
	if ((peek_slot_spacing - peek_slot_length / 3.0f) > 0) {
		peek_slot_spacing = peek_slot_length / 3.0f;
	}

	peek_tile_length = peek_slot_length / SHAPE_BOUNDS;				// How big each tile piece of a Tetris Piece is
	peek_tile_stroke = peek_tile_length * 0.15f;					// The stroke of each tile is 15% of the width of the tile

	text_peek_pos = CP_Vector_Set(hand_total_length, (float)CP_System_GetWindowHeight() - hand_total_height - hand_bottom_buffer);
	text_peek_size = hand_total_height * 0.25f;

	// Text icon size is 30% of slot length
	text_icon_size.x = hand_slot_length * 0.3f;
	text_icon_size.y = hand_slot_length * 0.3f;

	//______________________________________________________________
	// Update the positions
	PlayerHandSlot* current;
	for (int index = 0; index < HAND_SIZE; ++index) {
		current = &hand[index];

		current->pos.x = hand_left_buffer + hand_left_extra_buffer + (hand_slot_length + hand_slot_spacing) * index;
		current->pos.y = (float)CP_System_GetWindowHeight() - hand_slot_length - hand_bottom_buffer;
		current->piece.draw_pos.x = current->pos.x + (SHAPE_BOUNDS - current->piece.x_length) / 2.0f * hand_tile_length;
		current->piece.draw_pos.y = current->pos.y + (SHAPE_BOUNDS - current->piece.y_length) / 2.0f * hand_tile_length;
		current->piece.x_screen_length = hand_tile_length;
		current->piece.y_screen_length = hand_tile_length;

		// Position for icon type
		current->icon_pos.x = current->pos.x + text_icon_size.x / 2.0f - hand_tile_length / 2.0f;
		current->icon_pos.y = current->pos.y + text_icon_size.y / 2.0f - hand_tile_length / 2.0f;
	}

	for (int index = 0; index < PEEK_SIZE; ++index) {
		current = &peek_hand[index];
		current->pos.x = hand_total_length + (peek_slot_length + peek_slot_spacing) * index;
		current->pos.y = (float)CP_System_GetWindowHeight() - peek_slot_length - hand_bottom_buffer;
		current->piece.draw_pos.x = current->pos.x + (SHAPE_BOUNDS - current->piece.x_length) / 2.0f * peek_tile_length;
		current->piece.draw_pos.y = current->pos.y + (SHAPE_BOUNDS - current->piece.y_length) / 2.0f * peek_tile_length;
		current->piece.x_screen_length = peek_tile_length;
		current->piece.y_screen_length = peek_tile_length;
	}
}

//______________________________________________________________
// Player interaction functions

/*______________________________________________________________
@brief When a Tetris Piece is dropped onto the grid, remove the piece and update the player's hand
*/
void RemovePieceHeldFromHand(void){
	PlayerHandSlot* current;
	int played_index = 0;
	for (; played_index < HAND_SIZE; ++played_index) {
		current = &hand[played_index];
		if (IsThisPieceHeld(&current->piece)) {
			break;
		}
	}

	// Remove the piece from the player's hand, and shift the pieces behind fowards
	ArrayShiftFowardFrom(hand, played_index, HAND_SIZE - 1);

	// Move the next piece from the peek queue into the hand
	hand[HAND_SIZE - 1].piece = peek_hand[0].piece;

	// Shift the peek queue piece foward
	ArrayShiftFowardFrom(peek_hand, 0, PEEK_SIZE - 1);

	// Draw another piece from the queue
	peek_hand[PEEK_SIZE - 1].piece = DrawFromBag();

	//______________________________________________________________
	// Update the positions
	for (int index = 0; index < HAND_SIZE; ++index) {
		current = &hand[index];
		current->piece.draw_pos.x = current->pos.x + (SHAPE_BOUNDS - current->piece.x_length) / 2.0f * hand_tile_length;
		current->piece.draw_pos.y = current->pos.y + (SHAPE_BOUNDS - current->piece.y_length) / 2.0f * hand_tile_length;
		current->piece.x_screen_length = hand_tile_length;
		current->piece.y_screen_length = hand_tile_length;
	}

	for (int index = 0; index < PEEK_SIZE; ++index) {
		current = &peek_hand[index];
		current->piece.draw_pos.x = current->pos.x + (SHAPE_BOUNDS - current->piece.x_length) / 2.0f * peek_tile_length;
		current->piece.draw_pos.y = current->pos.y + (SHAPE_BOUNDS - current->piece.y_length) / 2.0f * peek_tile_length;
		current->piece.x_screen_length = peek_tile_length;
		current->piece.y_screen_length = peek_tile_length;
	}
}

/*______________________________________________________________
@brief Used by RemovePieceHeldFromHand to shift the pieces to the right of the piece used leftwards
*/
void ArrayShiftFowardFrom(PlayerHandSlot* array, int start, int end){
	for (int index = start; index < end; ++index) {
		array[index].piece = array[index + 1].piece;
	}
}

/*______________________________________________________________
@brief Frees the images used for icon rendering, should be called on game level exit
*/
void FreeIconImages(void){
	CP_Image_Free(&attack_icon);
	CP_Image_Free(&shield_icon);
}
