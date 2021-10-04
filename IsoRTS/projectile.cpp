#include <math.h>
#include <iostream>
#include "actors.h"
#include "buildings.h"
#include "gamestate.h"
#include "projectile.h"
#include <iomanip>

std::vector<projectile> listOfProjectiles;

namespace
{
	const float PI = static_cast<float>(acos(-1));

	float radiansToDegree(float radians) {
		return static_cast<float>(radians * (180.0 / PI));
	}

	float giveAngleOfSpriteInDGR(float screenVelocityX, float screenVelocityY)
	{
		if (screenVelocityX == 0.0f && screenVelocityY > 0.0f) { return 0.0f; }
		else if (screenVelocityX == 0.0f && screenVelocityY < 0.0f) { return 180.0f; }
		else if (screenVelocityX == 0.0f && screenVelocityY == 0.0f) { return 0.0f; }
		else if (screenVelocityY == 0.0f && screenVelocityX > 0.0f) { return 90.0f; }
		else if (screenVelocityY == 0.0f && screenVelocityX < 0.0f) { return 270.0f; }
		else if (screenVelocityX < 0.0f && screenVelocityY > 0.0f) { return 90 - radiansToDegree(atan(fabs(screenVelocityY) / fabs(screenVelocityX))); }
		else if (screenVelocityX > 0.0f && screenVelocityY > 0.0f) { return 360 - radiansToDegree((90 - atan(fabs(screenVelocityY) / fabs(screenVelocityX)))); }
		else if (screenVelocityX < 0.0f && screenVelocityY < 0.0f) { return 90 + radiansToDegree(atan(fabs(screenVelocityY) / fabs(screenVelocityX))); }
		else if (screenVelocityX > 0.0f && screenVelocityY < 0.0f) { return 360 - radiansToDegree((90 + atan(fabs(screenVelocityY) / fabs(screenVelocityX)))); }
		else { return 0.0f; }
	}

}

projectile::projectile(int projectileStartX, int projectileStartY, int projectileTargetX, int projectileTargetY, int projectileType, int damageOnImpact, int splashDamageOnImpact, int firedBy)
{
	this->X = worldSpace({ projectileStartX, projectileStartY }).x*1000;
	this->Y = worldSpace({ projectileStartX, projectileStartY }).y*1000;
	this->Z = 0;
	this->projectileType = projectileType;
	this->damageOnImpact = damageOnImpact;
	this->projectileTarget = { projectileTargetX, projectileTargetY };
	this->projectilePosition = {projectileStartX, projectileStartY};
	this->splashDamageOnImpact = splashDamageOnImpact;
	int travelTimeInSeconds = distEuclidean(projectileStartX * 32000, projectileStartY * 32000, projectileTargetX * 32000, projectileTargetY * 32000) / 128; //32 pixel/s
	this->deltaX = ((this->X - (worldSpace(this->projectileTarget).x*1000))*1000) / travelTimeInSeconds;
	this->deltaY = ((this->Y - (worldSpace(this->projectileTarget).y*1000))*1000) / travelTimeInSeconds;
	this->deltaZ = travelTimeInSeconds*3;
	this->timeFired = currentGame.getTime();
	this->reachedTarget = false;
	this->projectileRotation = 0.0f;
	this->firedBy = firedBy;
	this->X += 32000;
}

float projectile::getTimeLastUpdate() const {
	return this->timeFired;
}

void projectile::updatePosition()
{
	if (!reachedTarget) {
		//speed is in pixels per second
		if (this->timeFired + 100 < currentGame.getTime()) { //simulation runs at 10 frames per second
			this->timeFired = currentGame.getTime();
			this->X -= this->deltaX / 10;
			this->Y -= this->deltaY / 10;
			this->Z -= this->deltaZ;
			this->projectileRotation = giveAngleOfSpriteInDGR(this->deltaX, this->deltaY + (this->deltaZ * 60));
			this->deltaZ -= 576;
			if (this->Z >= 0) {
				doDamage();
				doSplashDamage();
				reachedTarget = true;
			}
		}
	}
}

void projectile::interprolatePositionForDrawCall() {
	if (this->timeFired > this->lastInterprolation) {
		this->interProlateX = this->X;
		this->interProlateY = this->Y;
		this->interProlateZ = this->Z;
		this->lastInterprolation = this->timeFired;
		this->interProlateDeltaZ = this->deltaZ;
	}
	
	if (this->lastInterprolation + 16 < currentGame.getTime()) { //Window updates 60 frames per second
		this->lastInterprolation = currentGame.getTime();
		if (this->Z <= 0) {
			this->interProlateX -= this->deltaX / 60;
			this->interProlateY -= this->deltaY / 60;
			this->interProlateZ -= this->deltaZ;
			this->interProlateDeltaZ -= 96;
		}
		this->projectileRotation = giveAngleOfSpriteInDGR(this->deltaX, this->deltaY + (this->interProlateDeltaZ  * 60) );
	}
}

void projectile::drawProjectile()
{
	interprolatePositionForDrawCall();
	currentGame.spriteArrow.setRotation(this->projectileRotation);
	currentGame.spriteArrow.setPosition(this->interProlateX/1000, (this->interProlateY/1000) + (this->interProlateZ/1000));
	window.draw(currentGame.spriteArrow);
}

void projectile::doDamage() const
{
	if (!currentGame.occupiedByActorList[this->projectileTarget.x][this->projectileTarget.y].empty()) {
		for (auto& id : currentGame.occupiedByActorList[this->projectileTarget.x][this->projectileTarget.y]) {
			listOfActors[id].takeDamage(this->damageOnImpact, this->firedBy);
		}
	}
	else if (currentGame.occupiedByBuildingList[this->projectileTarget.x][this->projectileTarget.y] != -1)
	{
		listOfBuildings[currentGame.occupiedByBuildingList[this->projectileTarget.x][this->projectileTarget.y]].takeDamage(this->damageOnImpact);
	}
}

void projectile::doSplashDamage()
{

}
