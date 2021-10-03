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

	float getTimeLastUpdate() const;

private:
	cords projectilePosition;
	cords projectileTarget;

	int timeFired;
	int lastSimulationUpdate;
	int lastInterprolationUpdate;
	int lastInterprolation;

	int   projectileType;
	int   damageOnImpact;
	int   splashDamageOnImpact;
	int   firedBy;

	//not detemenistic yet!
	float X;
	float Y;
	float Z;

	float interProlateX;
	float interProlateY;
	float interProlateZ;
	float interProlateDeltaZ;

	float deltaX;
	float deltaY;
	float deltaZ;

	float projectileRotation;
	bool  reachedTarget;
};

extern std::vector<projectile> listOfProjectiles;