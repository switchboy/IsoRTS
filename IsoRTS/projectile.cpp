#include <math.h>
#include <iostream>
#include "actors.h"
#include "buildings.h"
#include "gamestate.h"
#include "projectile.h"

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
	this->X = static_cast<float>(worldSpace({ projectileStartX, projectileStartY }).x);
	this->Y = static_cast<float>(worldSpace({ projectileStartX, projectileStartY }).y);
	this->Z = 0;
	this->projectileType = projectileType;
	this->damageOnImpact = damageOnImpact;
	this->projectileTarget = { projectileTargetX, projectileTargetY };
	this->projectilePosition = {projectileStartX, projectileStartY};
	this->splashDamageOnImpact = splashDamageOnImpact;
	float travelTimeInSeconds = static_cast<float>(distEuclidean(projectileStartX * 32.f, projectileStartY * 32.f, projectileTargetX * 32.f, projectileTargetY * 32.f)) / 128.f; //32 pixel/s
	this->deltaX = (this->X - static_cast<float>(worldSpace({ this->projectileTarget.x, this->projectileTarget.y }).x)) / travelTimeInSeconds;
	this->deltaY = (this->Y - static_cast<float>(worldSpace({ this->projectileTarget.x, this->projectileTarget.y }).y)) / travelTimeInSeconds;
	this->deltaZ = travelTimeInSeconds*3;
	this->timeFired = currentGame.getTime();
	this->reachedTarget = false;
	this->projectileRotation = 0.0f;
	this->firedBy = firedBy;
	this->X += 32;
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
			this->X -= this->deltaX / 10.f;
			this->Y -= this->deltaY / 10.f;
			this->Z -= this->deltaZ;
			this->projectileRotation = giveAngleOfSpriteInDGR(this->deltaX, this->deltaY + (this->deltaZ * 60));
			this->deltaZ -= 0.096f;
			if (this->Z >= 0.0f) {
				doDamage();
				doSplashDamage();
				reachedTarget = true;
			}
		}
	}
}

void projectile::interprolatePositionForDrawCall() {
	if (this->lastSimulationUpdate > this->lastInterprolationUpdate) {
		this->interProlateX = this->X;
		this->interProlateY = this->Y;
		this->interProlateZ = this->Z;
		this->lastInterprolation = this->timeFired;
		this->interProlateDeltaZ = this->deltaZ;
	}
	if (this->interProlateZ < 0.0f) {
		if (this->lastInterprolation + 16 < currentGame.getTime()) { //Window updates 60 frames per second
			this->timeFired = currentGame.getTime();
			this->interProlateX -= this->deltaX / 60.f;
			this->interProlateY -= this->deltaY / 60.f;
			this->interProlateZ -= this->deltaZ;
			this->projectileRotation = giveAngleOfSpriteInDGR(this->deltaX, this->deltaY + (this->deltaZ * 60));
			this->interProlateDeltaZ -= 0.096f;
		}
	}
}

void projectile::drawProjectile()
{
	interprolatePositionForDrawCall();
	currentGame.spriteArrow.setRotation(this->projectileRotation);
	currentGame.spriteArrow.setPosition(this->interProlateX, this->interProlateY + this->interProlateZ);
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
