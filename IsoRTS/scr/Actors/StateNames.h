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
	GatheringTheResource,
	ReturningTheResource,
	BuildingTheBuilding,
	Attacking,
	Fleeing,
	Decomposing,
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
};