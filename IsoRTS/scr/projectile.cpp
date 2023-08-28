#include <math.h>
#include <iostream>
#include "Actors/Actor.h"
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

projectile::projectile(int projectileStartX, int projectileStartY, int projectileTargetX, int projectileTargetY, int projectileType, int damageOnImpact, int splashDamageOnImpact, int firedBy, targetTypes firedByType)
{
	_X = worldSpace({ projectileStartX, projectileStartY }).x*1000;
	_Y = worldSpace({ projectileStartX, projectileStartY }).y*1000;
	_Z = 0;
	_projectileType = projectileType;
	_damageOnImpact = damageOnImpact;
	_projectileTarget = { projectileTargetX, projectileTargetY };
	_projectilePosition = {projectileStartX, projectileStartY};
	_splashDamageOnImpact = splashDamageOnImpact;
	int travelTimeInSeconds = static_cast<int>(distEuclidean(projectileStartX * 32000, projectileStartY * 32000, projectileTargetX * 32000, projectileTargetY * 32000) / (int)128); //32 pixel/s
	_deltaX = ((_X - (worldSpace(_projectileTarget).x*1000))*1000) / travelTimeInSeconds;
	_deltaY = ((_Y - (worldSpace(_projectileTarget).y*1000))*1000) / travelTimeInSeconds;
	_deltaZ = travelTimeInSeconds*3;
	_timeFired = currentGame.getTime();
	_reachedTarget = false;
	_projectileRotation = 0.0f;
	_firedBy = firedBy;
	_X += 32000;
	_firedByType = firedByType;
}

int projectile::getTimeLastUpdate() const {
	return _timeFired;
}

void projectile::updatePosition()
{
	if (!_reachedTarget) {
		//speed is in pixels per second
		if (_timeFired + 100 < currentGame.getTime()) { //simulation runs at 10 frames per second
			_timeFired = currentGame.getTime();
			_X -= _deltaX / 10;
			_Y -= _deltaY / 10;
			_Z -= _deltaZ;
			_projectileRotation = giveAngleOfSpriteInDGR(static_cast<float>(_deltaX), static_cast<float>(_deltaY + (_deltaZ * 60)));
			_deltaZ -= 576;
			if (_Z >= 0) {
				doDamage();
				doSplashDamage();
				_reachedTarget = true;
			}
		}
	}
}

void projectile::interprolatePositionForDrawCall() {
	if (_timeFired > _lastInterprolation) {
		_interProlateX = _X;
		_interProlateY = _Y;
		_interProlateZ = _Z;
		_lastInterprolation = _timeFired;
		_interProlateDeltaZ = _deltaZ;
	}
	
	if (_lastInterprolation + 16 < currentGame.getTime()) { //Window updates 60 frames per second
		_lastInterprolation = currentGame.getTime();
		if (_Z <= 0) {
			_interProlateX -= _deltaX / 60;
			_interProlateY -= _deltaY / 60;
			_interProlateZ -= _deltaZ;
			_interProlateDeltaZ -= 96;
		}
		_projectileRotation = giveAngleOfSpriteInDGR(static_cast<float>(_deltaX), static_cast<float>(_deltaY + (_interProlateDeltaZ  * 60)));
	}
}

void projectile::drawProjectile()
{
	interprolatePositionForDrawCall();
	currentGame.spriteArrow.setRotation(_projectileRotation);
	currentGame.spriteArrow.setPosition(static_cast<float>(_interProlateX)/1000.f, (static_cast<float>(_interProlateY)/1000.f) + (static_cast<float>(_interProlateZ)/1000.f));
	window.draw(currentGame.spriteArrow);
}

void projectile::doDamage() const
{
	if (!currentGame.occupiedByActorList[_projectileTarget.x][_projectileTarget.y].empty()) {
		for (auto& id : currentGame.occupiedByActorList[_projectileTarget.x][_projectileTarget.y]) {
			listOfActors[id].takeDamage(_damageOnImpact, _firedBy, _firedByType);
		}
	}
	else if (currentGame.occupiedByBuildingList[_projectileTarget.x][_projectileTarget.y] != -1)
	{
		listOfBuildings[currentGame.occupiedByBuildingList[_projectileTarget.x][_projectileTarget.y]].takeDamage(_damageOnImpact);
	}
}

void projectile::doSplashDamage()
{

}
