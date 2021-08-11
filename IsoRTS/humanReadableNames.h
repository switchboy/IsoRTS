#ifndef HUMANREADABLENAMES_H
#define HUMANREADABLENAMES_H

enum class resourceTypes
{
    resourceWood,
    resourceFood,
    resourceStone,
    resourceGold,
    All,
    None
};

enum objectTypes
{
    objectCactus,
    objectCypress,
    objectMaple,
    objectPine,
    objectStone,
    objectGold,
    objectBerry
};

enum class spriteTypes
{
    spriteTownCenter,
    spriteHouse,
    spriteVillager,
    spriteCancel,
    spriteMill,
    spriteLumberCamp,
    spriteBarracks,
    spriteSwordsman,
    spriteMiningCamp
};

enum class actionTypes
{
    actionBuildTownCenter,
    actionBuildHouse,
    actionUnitSelect,
    actionMakeVillager,
    actionCancelBuilding,
    actionCancelProduction,
    actionBuildMill,
    actionBuildLumberCamp,
    actionBuildBarracks,
    actionMakeSwordsman,
    actionBuildMiningCamp
};

enum class stackOrderTypes
{
    stackActionMove,
    stackActionGather,
    stackActionBuild,
    stackActionAttack
};

#endif HUMANREADABLENAMES_H