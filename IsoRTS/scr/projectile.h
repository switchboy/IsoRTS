#pragma once
#include "globalfunctions.h"
#include <vector>

class projectile
{
public:
	projectile(int projectileStartX, int projectileStartY, int projectileTargetX, int projectileTargetY, int projectileType, int damageOnImpact, int splashDamageOnImpact, int firedBy, targetTypes firedByType);
	void doDamage() const;//not detemenistic yet!
	void doSplashDamage();
	void drawProjectile();
	void updatePosition();//not detemenistic yet!
	void interprolatePositionForDrawCall();

	int getTimeLastUpdate() const;

private:
	cords _projectilePosition;
	cords _projectileTarget;

	int _timeFired;
	int _lastInterprolation = 0;

	int   _projectileType;
	int   _damageOnImpact;
	int   _splashDamageOnImpact;
	int   _firedBy;
	targetTypes _firedByType;

	//not detemenistic yet!
	int _X;
	int _Y;
	int _Z;

	int _interProlateX = 0;
	int _interProlateY = 0;
	int _interProlateZ = 0;
	int _interProlateDeltaZ = 0;

	int _deltaX;
	int _deltaY;
	int _deltaZ;

	float _projectileRotation;
	bool  _reachedTarget;
};

extern std::vector<projectile> listOfProjectiles;