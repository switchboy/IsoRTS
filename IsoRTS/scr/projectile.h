#pragma once
#include "globalfunctions.h"
#include <vector>

class projectile
{
public:
	projectile(int projectileStartX, int projectileStartY, int projectileTargetX, int projectileTargetY, int projectileType, int damageOnImpact, int splashDamageOnImpact, int firedBy);
	void doDamage() const;//not detemenistic yet!
	void doSplashDamage();
	void drawProjectile();
	void updatePosition();//not detemenistic yet!
	void interprolatePositionForDrawCall();

	int getTimeLastUpdate() const;

private:
	cords projectilePosition;
	cords projectileTarget;

	int timeFired;
	int lastInterprolation = 0;

	int   projectileType;
	int   damageOnImpact;
	int   splashDamageOnImpact;
	int   firedBy;

	//not detemenistic yet!
	int X;
	int Y;
	int Z;

	int interProlateX = 0;
	int interProlateY = 0;
	int interProlateZ = 0;
	int interProlateDeltaZ = 0;

	int deltaX;
	int deltaY;
	int deltaZ;

	float projectileRotation;
	bool  reachedTarget;
};

extern std::vector<projectile> listOfProjectiles;