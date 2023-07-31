#pragma once
enum class BaseStateNames{
	NONE,
	Idle,
	WalikngAToB,
	Gathering,
	Building,
	Fighting,
	Dead
};

enum class GroundStateNames {
	NONE,
	Walking,
	AtTheResource,
	ReturningTheResource,
	AtTheBuilding,
	Attacking,
	Fleeing,
	Decomposing,
	FindAlternativeSource
};

enum class SubStateNames {
	NONE,
	WalkingToNextSquare,
	SearchingAPath,
	SearchingNextSimilairResource,
	SearchingDropOffPoint,
	SearchingAlternativeBuildingSpot,
	SearchingAlternativeBuildingToBuilt,
	MeleeAttacking,
	RangedAttacking,
	SearchNextTarget,
	SettingGoalToFleeTo,
	SettingMyDecompositionState,
	CountingDownToDestroySelf,
	WalkingToAction,
	WalkingBackFromAction,
	BuildingTheBuilding,
	GatheringTheResource,
	CanceldWhileWalking,
	DroppingOffResource
};

enum class ModesOfAttack {
	melee,
	ranged,
};