#include "projectile.h"
#include "gamestate.h"
#include "actors.h"
#include "buildings.h"
#include <math.h>
#include <iostream>

const float PI = static_cast<float>(acos(-1));
std::vector<projectile> listOfProjectiles;


float radiansToDegree(float radians) {
	return static_cast<float>(radians * (180.0 / PI));
}

float giveAnngleOfSpriteInDGR(float screenVelocityX, float screenVelocityY)
{
	if (screenVelocityX == 0.0f && screenVelocityY > 0.0f) { return 0.0f; }
	if (screenVelocityX == 0.0f && screenVelocityY < 0.0f) { return 180.0f; }
	if (screenVelocityX == 0.0f && screenVelocityY == 0.0f) { return 0.0f; }
	if (screenVelocityY == 0.0f && screenVelocityX > 0.0f) { return 90.0f; }
	if (screenVelocityY == 0.0f && screenVelocityX < 0.0f) { return 270.0f; }
	if (screenVelocityX < 0.0f && screenVelocityY > 0.0f) { return 90 - radiansToDegree( atan(fabs(screenVelocityY) / fabs(screenVelocityX)) ); }
	if (screenVelocityX > 0.0f && screenVelocityY > 0.0f) { return 360 - radiansToDegree( (90 - atan(fabs(screenVelocityY) / fabs(screenVelocityX))) ); }
	if (screenVelocityX < 0.0f && screenVelocityY < 0.0f) { return 90 + radiansToDegree( atan(fabs(screenVelocityY) / fabs(screenVelocityX)) ); }
	if (screenVelocityX > 0.0f && screenVelocityY < 0.0f) { return 360 - radiansToDegree( (90 + atan(fabs(screenVelocityY) / fabs(screenVelocityX))) ); }
}

projectile::projectile(int projectileStartX, int projectileStartY, int projectileTargetX, int projectileTargetY, int projectileType, int damageOnImpact, int splashDamageOnImpact, int firedBy)
{
	this->X = static_cast<float>(worldSpace(projectileStartX, projectileStartY, true));
	this->Y = static_cast<float>(worldSpace(projectileStartX, projectileStartY, false));
	this->Z = 0;
	this->projectileType = projectileType;
	this->damageOnImpact = damageOnImpact;
	this->projectileTarget = { projectileTargetX, projectileTargetY };
	this->projectilePosition = {projectileStartX, projectileStartY};
	this->splashDamageOnImpact = splashDamageOnImpact;
	float travelTimeInSeconds = static_cast<float>(distEuclidean(projectileStartX * 32.f, projectileStartY * 32.f, projectileTargetX * 32.f, projectileTargetY * 32.f)) / 128.f; //32 pixel/s
	this->deltaX = (this->X - static_cast<float>(worldSpace(this->projectileTarget.x, this->projectileTarget.y, true))) / travelTimeInSeconds;
	this->deltaY = (this->Y - static_cast<float>(worldSpace(this->projectileTarget.x, this->projectileTarget.y, false))) / travelTimeInSeconds;
	this->deltaZ = travelTimeInSeconds*3;
	this->timeFired = currentGame.getTime();
	this->reachedTarget = false;
	this->projectileRotation = 0.0f;
	this->firedBy = firedBy;
	this->X += 32;
}

float projectile::getTimeLastUpdate() {
	return this->timeFired;
}

void projectile::updatePosition()
{
	if (!reachedTarget) {
		//speed is in pixels per second
		if (this->timeFired + 0.016 < currentGame.getTime()) {
			this->timeFired = currentGame.getTime();
			this->X -= this->deltaX/60.f;
			this->Y -= this->deltaY/60.f;
			this->Z -= this->deltaZ;
			this->projectileRotation = giveAnngleOfSpriteInDGR(this->deltaX, this->deltaY + (this->deltaZ*60));
			this->deltaZ -= 0.096f;
			if (this->Z >= 0.0f) {
				doDamage();
				reachedTarget = true;
			}
		}
	}
}

void projectile::drawProjectile()
{
	currentGame.spriteArrow.setRotation(this->projectileRotation);
	currentGame.spriteArrow.setPosition(this->X, this->Y + this->Z);
	window.draw(currentGame.spriteArrow);
}

void projectile::doDamage()
{
	if (currentGame.occupiedByActorList[this->projectileTarget.x][this->projectileTarget.y] != -1) {
		listOfActors[currentGame.occupiedByActorList[this->projectileTarget.x][this->projectileTarget.y]].takeDamage(this->damageOnImpact, this->firedBy);
	}
	else if (currentGame.occupiedByBuildingList[this->projectileTarget.x][this->projectileTarget.y] != -1)
	{
		listOfBuildings[currentGame.occupiedByBuildingList[this->projectileTarget.x][this->projectileTarget.y]].takeDamage(this->damageOnImpact);
	}
}

void projectile::doSplashDamage()
{

}
