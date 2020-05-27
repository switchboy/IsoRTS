#include "projectile.h"
#include "gamestate.h"
#include "actors.h"
#include "buildings.h"

std::vector<projectile> listOfProjectiles;

projectile::projectile(int projectileStartX, int projectileStartY, int projectileTargetX, int projectileTargetY, int projectileType, int damageOnImpact, int splashDamageOnImpact)
{
	this->X = worldSpace(projectileStartX, projectileStartY, true);
	this->Y = worldSpace(projectileStartX, projectileStartY, false);
	this->Z = 0;
	this->projectileType = projectileType;
	this->damageOnImpact = damageOnImpact;
	this->projectileTarget = { projectileTargetX, projectileTargetY };
	this->projectilePosition = {projectileStartX, projectileStartY};
	this->splashDamageOnImpact = splashDamageOnImpact;
	float travelTimeInSeconds = distEuclidean(projectileStartX * 32.f, projectileStartY * 32.f, projectileTargetX * 32.f, projectileTargetY * 32.f) / 128.f; //32 pixel/s
	this->deltaX = (this->X - float(worldSpace(this->projectileTarget.x, this->projectileTarget.y, true))) / travelTimeInSeconds;
	this->deltaY = (this->Y - float(worldSpace(this->projectileTarget.x, this->projectileTarget.y, false))) / travelTimeInSeconds;
	this->deltaZ = travelTimeInSeconds*3;
	this->timeFired = currentGame.getTime();
	this->reachedTarget = false;
	this->X += 32;
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
	int xScreenCord = this->X;
	int yScreenCord = this->Y + this->Z;
	currentGame.spriteArrow.setPosition(xScreenCord, yScreenCord);
	window.draw(currentGame.spriteArrow);
}

void projectile::doDamage()
{
	if (currentGame.occupiedByActorList[this->projectileTarget.x][this->projectileTarget.y] != -1) {
		listOfActors[currentGame.occupiedByActorList[this->projectileTarget.x][this->projectileTarget.y]].takeDamage(this->damageOnImpact);
	}
	else if (currentGame.occupiedByBuildingList[this->projectileTarget.x][this->projectileTarget.y] != -1)
	{
		listOfBuildings[currentGame.occupiedByBuildingList[this->projectileTarget.x][this->projectileTarget.y]].takeDamage(this->damageOnImpact);
	}
}

void projectile::doSplashDamage()
{

}


