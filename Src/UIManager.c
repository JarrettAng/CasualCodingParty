#include <cprocessing.h>
#include "UIManager.h"
#include "ColorTable.h"

Button* btns[MAX_UI_BUTTONS];
int btnsCount = 0;


/// <summary>
/// Intialize button with the given data. Also store btn address in array so it manager can draw them all at once.
/// </summary>
void InitializeButton(Button* btn, Rect transform, GraphicData graphicsData, TextData textData, Callback callBack) {
	btn->transform = transform;
	// Initialized position.
	btn->transform.cachedPos = CP_Vector_Set(btn->transform.x, btn->transform.y);
	btn->graphicData = graphicsData;
	btn->textData = textData;
	btn->callBack = callBack;

	btns[btnsCount++] = btn;
}

/// <summary>
/// Require button to be intialized before drawing.
/// </summary>
void DrawButton(Button* btn){
	// Draw rect
	SetGraphicSetting(btn->graphicData);
	CP_Graphics_DrawRect(btn->transform.x, btn->transform.y, btn->transform.width, btn->transform.heigth);

	// Draw text
	SetTextSetting(btn->textData);
	CP_Font_DrawText(btn->textData.text, btn->transform.x, btn->transform.y);
}

/// <summary>
/// Draw all buttons cached in btns array.
/// </summary>
void DrawButtons() {
	for (int i = 0; i < btnsCount; i++) {
		// Draw rect
		SetGraphicSetting(btns[i]->graphicData);
		CP_Graphics_DrawRect(btns[i]->transform.x, btns[i]->transform.y, btns[i]->transform.width, btns[i]->transform.heigth);

		// Draw text
		SetTextSetting(btns[i]->textData);
		CP_Font_DrawText(btns[i]->textData.text, btns[i]->transform.x, btns[i]->transform.y);
	}
}

/// <summary>
/// For updating engine graphic settings before drawing an UI element.
/// </summary>
void SetGraphicSetting(GraphicData data) {
	CP_Settings_RectMode(data.imagePosMode);
	CP_Settings_ImageFilterMode(data.imageFilterMode);
	CP_Settings_Fill(data.color);
	CP_Settings_Stroke(data.strokeColor);
	CP_Settings_StrokeWeight(data.strokeWeight);
}


/// <summary>
/// For updating engine text settings before drawing an UI element.
/// </summary>
void SetTextSetting(TextData data) {
	CP_Font_Set(data.font);
	CP_Settings_TextSize(data.textSize);
	CP_Settings_Fill(data.color);
	CP_Settings_TextAlignment(data.hAlign, data.vAlign);
}

/// <summary>
/// Empty button array when exiting a scene, so that next scene can reuse the array.
/// </summary>
void FreeButton(){
	// Empty array so next scene can use.
	memset(btns, 0, sizeof(btns));
	btnsCount = 0;
}


#pragma region UI_INTERACTION

void HandleButtonClick() {
	if (CP_Input_MouseTriggered(MOUSE_BUTTON_LEFT))
	{
		Callback callBack = NULL;

		float xPos = CP_Input_GetMouseX();
		float yPos = CP_Input_GetMouseY();

		// Loop through all buttons initialized in this scene.
		for (int i = 0; i < btnsCount; i++) {
			// Check if player is clicking a button.
			if (MouseWithinArea(btns[i]->transform.x, btns[i]->transform.y, btns[i]->transform.width, btns[i]->transform.heigth, xPos, yPos, btns[i]->graphicData.imagePosMode)) {
				// Cache callback to be triggered after the conditional statement.
				// Will cause error if callback is triggered here.
				callBack = btns[i]->callBack;
				// Stop checking for buttons once a button is clicked.
				// Make sure the button has a onClick event before breaking loop.
				// In an event where 2 buttons are overlapping, and one button doesnt have a callback, the other button callback will still be triggered.
				if (callBack != NULL) break;
			}
		}

		// Trigger onclick event if needed.
		if (callBack != NULL) {
			callBack();
		}
	}
}

Button* GetButtonHover(){
	float xPos = CP_Input_GetMouseX();
	float yPos = CP_Input_GetMouseY();

	// Loop through all buttons initialized in this scene.
	for (int i = 0; i < btnsCount; i++) {
		// Check if player is hovering a button.
		if (MouseWithinArea(btns[i]->transform.x, btns[i]->transform.y, btns[i]->transform.width, btns[i]->transform.heigth, xPos, yPos, btns[i]->graphicData.imagePosMode)) {
			// Break loop and return the button the player is hovering.
			return btns[i];
		}
	}
	// Player not hovering on a button.
	return NULL;
}

_Bool MouseWithinArea(float areaX, float areaY, float areaWidth, float areaHeigth, float mouseX, float mouseY, CP_POSITION_MODE areaMode)
{
	// If image is drawn from the corner, click detection will be different from image drawn from center.
	if (areaMode == CP_POSITION_CORNER) {
		if ((mouseX < areaX + areaWidth) && (mouseX > areaX) && (mouseY < areaY + areaHeigth) && (mouseY > areaY)) {
			return TRUE;
		}
		return FALSE;
	}

	float halfWidth = areaWidth / 2;
	float halfHeigth = areaHeigth / 2;

	// Image drawn from center.
	if (mouseX > areaX - halfWidth && mouseX < areaX + halfWidth && mouseY < areaY + halfHeigth && mouseY > areaY - halfHeigth) {
		return TRUE;
	}
	return FALSE;
}

#pragma endregion
