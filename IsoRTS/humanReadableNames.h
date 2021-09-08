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

enum class objectTypes : uint32_t
{
    cactus,
    cypress,
    maple,
    pine,
    stone,
    gold,
    berry
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
    spriteMiningCamp,
    spriteWall,
    spriteGate,
    spriteOpenGate,
    spriteCivilBuilding,
    spriteMilitairyBuilding,
    spriteGoBack
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
    actionBuildMiningCamp,
    actionBuildWall,
    actionMakeGate,
    actionOpenGate,
    actionCloseGate,
    actionShowCivilBuildings,
    actionShowMilitairyBuildings,
    actionShowBack
};

enum class stackOrderTypes
{
    stackActionMove,
    stackActionGather,
    stackActionBuild,
    stackActionAttack
};

#endif HUMANREADABLENAMES_H