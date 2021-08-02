#ifndef RESOURCEIDS_H
#define RESOURCEIDS_H

enum resourceTypes
{
    ResourceWood,
    ResourceFood,
    ResourceStone,
    ResourceGold
};

enum objectTypes
{
    ObjectCactus,
    ObjectCypress,
    ObjectMaple,
    ObjectPine,
    ObjectStone,
    ObjectGold,
    ObjectBerry
};

enum spriteTypes
{
    SpriteTownCenter,
    SpriteHouse,
    SpriteVillager,
    Sprite3,
    SpriteMill,
    SpriteLumberCamp,
    SpriteBarracks,
    SpriteSwordsman,
    SpriteMiningCamp
};

enum actionTypes
{
    ActionBuildTownCenter,
    ActionBuildHouse,
    ActionUnitSelect,
    ActionMakeVillager,
    ActionCancelBuilding,
    ActionCancelProduction,
    ActionBuildMill,
    ActionBuildLumberCamp,
    ActionBuildBarracks,
    ActionCreateSwordsman,
    ActionBuildMiningCamp
};

#endif RESOURCEIDS_H