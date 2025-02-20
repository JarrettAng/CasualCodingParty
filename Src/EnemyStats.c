/*!
@file	  Enemy.c
@author	  Muhammad Farhan Bin Ahmad (b.muhammadfarhan@digipen.edu)
@date     21/11/2022
@brief    1.Creation and declaration of enemy types
		  2.Keep track of enemy movement and status
		  3.Render enemy in the grid

@license  Copyright � 2022 DigiPen, All rights reserved.
________________________________________________________________________________________________________*/

#include <cprocessing.h>
#include <stdio.h>
#include "EnemyDisplay.h"
#include "EnemyStats.h"
#include "WaveSystem.h"
#include "Grid.h"
#include "ColorTable.h"
#include "Particles.h"
#include "Hearts.h"
#include "SoundManager.h"



//-----ENEMYTYPE-----//
#define ZOMBIE 1,1,1,2,"Z",ZOMBIE
#define LEAPER 5,3,1,1,"L",LEAPER
#define TANK 10,1,3,2,"T",TANK
#define WALLBREAKER 15,1,2,3,"B",BREAKER
#define GRAVE 18,0,1,0,"G",GRAVE
//-----WALL-----//
#define WALL 0,0,3,0,"|x|",WALL//Not enemy, special type for player to use
//Gonna use their char sprite value to indicate type of enemy
 
EnemyInfo Enemy[ENEMYPOOL];
int enemyPoolIndex = 0;

/*______________________________________________________________
@brief Initialises the enemy pool. Edit this to add more enemy types
______________________________________________________________*/
void InitEnemyPool(void){
	enemyPoolIndex = 0;
	//Create zombie type to spawn
	CreateEnemy(WALL,MENU_GRAY);
	CreateEnemy(ZOMBIE,MENU_RED);
	CreateEnemy(LEAPER,MENU_RED);
	CreateEnemy(TANK,MENU_RED);
	CreateEnemy(WALLBREAKER,MENU_RED);
	CreateEnemy(GRAVE,GREEN);
}

/*______________________________________________________________
@brief Function that creates enemies and adds them to the enemy pool
______________________________________________________________*/
void CreateEnemy(int cost, int speed, int health,int damage, const char* sprite,ZombieType type,CP_Color color){
	EnemyInfo newEnemy = {
		.Cost = cost,
		.Health = health,
		.damage = damage,
		.MaxHealth = health,
		.CharSprite = sprite,
		.MovementSpeed = speed,
		.Color = color,
		.type = type,
		.moveCooldown = FALSE
	};
	Enemy[enemyPoolIndex] = newEnemy;
	enemyPoolIndex++;
}

/*______________________________________________________________
@brief Returns the total count of enemies in the enemy array
______________________________________________________________*/
int GetEnemyCount(void){
	return enemyPoolIndex;
}

/*______________________________________________________________
@brief Returns the the EnemyInfo in the stored index
______________________________________________________________*/
EnemyInfo* GetEnemyPrefab(int index){
	return &Enemy[index];
}

/*______________________________________________________________
@brief Returns the the EnemyInfo in the stored index
______________________________________________________________*/
EnemyInfo* GetRandomEnemyPrefab(void){
	//Need to use GetEnemyCount()-2 so you don't get graves that spawn graves
	return &Enemy[CP_Random_RangeInt(1,GetEnemyCount()-2)];
}

/*______________________________________________________________
@brief Enemy movement in the play area
______________________________________________________________*/
void MoveEnemy(EnemyInfo* enemy){
	if(enemy->MovementSpeed<=0)return;
	//If the enemy has reach last x element, it'll die and damage player
	for(short i =1; i<= enemy->MovementSpeed; ++i){
		//Check enemy in front of them
		if(GetAliveEnemyFromGrid(enemy->x-i,enemy->y)==NULL){//if no enemy or tombstone, will continue movement
			continue;
		}
		if(GetAliveEnemyFromGrid(enemy->x-i,enemy->y)->isAlive){	//if its a live enemy

			if(GetAliveEnemyFromGrid(enemy->x-i,enemy->y)->y == enemy->y){	//if they are in the same row

				if(GetAliveEnemyFromGrid(enemy->x-i,enemy->y)->x == (enemy->x-i)){	//get first live enemy infront

					if(GetAliveEnemyFromGrid(enemy->x-i,enemy->y)->Cost == 0){	//if it's a wall
						ZombieDealDamage(enemy->x-i,enemy->y,enemy->damage);	//Zombie deals damage to zombie
						if(!GetAliveEnemyFromGrid(enemy->x-i,enemy->y)){		//Check if the wall is still there
							enemy->x = enemy->x-i;								//Moves into the wall's space if not there
							PlaySound(WALLBREAK,CP_SOUND_GROUP_SFX);
							return;
							//Play whatever damage anim here for walls
						}
						enemy->x = enemy->x-i+1;								//stop right behind it
					}
					else{ //If not behind a wall
						enemy->x = enemy->x-i+1;								//stop right behind it
					}
					return;
				}
			}
		} 
	}

	enemy->x -= enemy->MovementSpeed;//enemy position in next turn

	if (enemy->x < 0 && enemy->isAlive)
	{
		//Special despawn animation over here
		ZombieToPlayerParticle(GridXToPosX(enemy->x),GridYToPosY(enemy->y));
		enemy->isAlive = FALSE;//enemy is ded(LOLXD)
		LoseLife(1); // LOSE ONE LIFE FOR EACH ENEMY ENTERING THE WALL
	}

	PlaySound(ZOMBIEMOVE, CP_SOUND_GROUP_SFX);
}

/*______________________________________________________________
@brief Render enemy
______________________________________________________________*/
void DrawEnemy(EnemyInfo* enemy) {

	CP_Settings_Fill(enemy->Color);
	CP_Settings_TextSize(GetCellSize() / 3.0f);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);

	char buffer[25] = { 0 };
	sprintf_s(buffer, 25, "%d", enemy->Health);
	CP_Font_DrawText(enemy->CharSprite, GridXToPosX(enemy->x), GridYToPosY(enemy->y));

	RenderEnemyDisplay(GridXToPosX(enemy->x), GridYToPosY(enemy->y), enemy->Health,enemy->MaxHealth, enemy->damage);
	RenderEnemyMovement(GridXToPosX(enemy->x), GridYToPosY(enemy->y), enemy->MovementSpeed);

}
