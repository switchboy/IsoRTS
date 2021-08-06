#pragma once
#include "globalfunctions.h"
#include <vector>

//TODO: this struct never seems to be used. Safe to delete and use a cords instead?
struct screenPositionProjectile {
	float x;
	float y;
};

class projectile
{
public:
	projectile(int projectileStartX, int projectileStartY, int projectileTargetX, int projectileTargetY, int projectileType, int damageOnImpact, int splashDamageOnImpact, int firedBy);
	void doDamage() const;
	void doSplashDamage();
	void drawProjectile() const;
	void updatePosition();

	float getTimeLastUpdate() const;

private:
	cords projectilePosition;
	cords projectileTarget;
	float timeFired;
	int   projectileType;
	int   damageOnImpact;
	int   splashDamageOnImpact;
	int   firedBy;
	float X;
	float Y;
	float Z;
	float deltaX;
	float deltaY;
	float deltaZ;
	float projectileRotation;
	bool  reachedTarget;
};

extern std::vector<projectile> listOfProjectiles;
//extern double distEuclidean(double x1, double y1, double x2, double y2);
//extern const float PI; 