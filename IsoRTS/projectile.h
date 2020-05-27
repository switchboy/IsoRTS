#pragma once
#include "globalfunctions.h"
#include <vector>

struct screenPositionProjectile {
	float x;
	float y;
};

class projectile
{
public:
	projectile(int projectileStartX, int projectileStartY, int projectileTargetX, int projectileTargetY, int projectileType, int damageOnImpact, int splashDamageOnImpact);
	void updatePosition();
	void drawProjectile();
	void doDamage();
	void doSplashDamage();

private:
	mouseWorldCord projectilePosition;
	mouseWorldCord projectileTarget;
	float timeFired;
	int projectileType;
	int damageOnImpact;
	int splashDamageOnImpact;
	float X;
	float Y;
	float Z;
	float deltaX;
	float deltaY;
	float deltaZ;
	bool reachedTarget;
};

extern std::vector<projectile> listOfProjectiles;
extern double distEuclidean(double x1, double y1, double x2, double y2);