#include "objectTemplates.h"

objectTemplates::objectTemplates(objectTypes objectId, resourceTypes typeOfResource, int startAmountOfResources, std::string realName, cords spriteOrigin, cords textureRect, std::string texture)
{
    this->objectId = objectId;
    this->typeOfResource = typeOfResource;
    this->startAmountOfResources = startAmountOfResources;
    this->realName = realName;
    this->spriteOrigin = spriteOrigin;
    this->textureRect = textureRect;
    if (!this->texture.loadFromFile(texture))
    {
        std::cout << "Error loading texture: " << texture << std::endl;
    }
}

objectTypes objectTemplates::getObjectId() const
{
    return this->objectId;
}

resourceTypes objectTemplates::getTypeOfResource() const
{
    return this->typeOfResource;
}

int objectTemplates::getStartAmountOfResources() const
{
    return this->startAmountOfResources;
}

std::string objectTemplates::getRealName() const
{
    return this->realName;
}

cords objectTemplates::getSpriteOrigin() const
{
    return this->spriteOrigin;
}

cords objectTemplates::getTextureRect() const
{
    return this->textureRect;
}

sf::Sprite& objectTemplates::getSprite()
{
    return this->sprite;
}

sf::Texture objectTemplates::getTexture()
{
    return this->texture;
}

void objectTemplates::setTexture()
{
    this->sprite.setTexture(this->texture);
    this->sprite.setTextureRect(sf::IntRect(0, 0, this->textureRect.x, this->textureRect.y));
    this->sprite.setOrigin(static_cast<float>(this->spriteOrigin.x), static_cast<float>(this->spriteOrigin.y));
}



void loadObjects()
{
    //cactus
    listOfObjectTemplates.push_back({
        objectTypes::objectCactus,          //objectTypes             objectId,
        resourceTypes::resourceWood,        //resourceTypes           typeOfResource,
        200,                                //int                     startAmountOfResources,
        "Cactus",                           //std::string             realName,
        {0 , 32},                           //cords                   spriteOrigin,
        {64, 64},                           //cords                   textureRect,
        "textures/cactus.png"               //std::string             texture
        });

    //cactus
    listOfObjectTemplates.push_back({
        objectTypes::objectCypress,         //objectTypes             objectId,
        resourceTypes::resourceWood,        //resourceTypes           typeOfResource,
        200,                                //int                     startAmountOfResources,
        "Cypress",                          //std::string             realName,
        {0 , 32},                           //cords                   spriteOrigin,
        {64, 64},                           //cords                   textureRect,
        "textures/cypressTree.png"          //std::string             texture
        });

    //Maple
    listOfObjectTemplates.push_back({
        objectTypes::objectMaple,           //objectTypes             objectId,
        resourceTypes::resourceWood,        //resourceTypes           typeOfResource,
        200,                                //int                     startAmountOfResources,
        "Maple",                            //std::string             realName,
        {0 , 32},                           //cords                   spriteOrigin,
        {64, 64},                           //cords                   textureRect,
        "textures/normalTree.png"           //std::string             texture
        });

    //Pine
    listOfObjectTemplates.push_back({
        objectTypes::objectPine,            //objectTypes             objectId,
        resourceTypes::resourceWood,        //resourceTypes           typeOfResource,
        200,                                //int                     startAmountOfResources,
        "Pine",                             //std::string             realName,
        {0 , 32},                           //cords                   spriteOrigin,
        {64, 64},                           //cords                   textureRect,
        "textures/pineTree.png"             //std::string             texture
        });

    //stone
    listOfObjectTemplates.push_back({
        objectTypes::objectStone,           //objectTypes             objectId,
        resourceTypes::resourceStone,       //resourceTypes           typeOfResource,
        2000,                               //int                     startAmountOfResources,
        "Stone",                            //std::string             realName,
        {0 , 2},                            //cords                   spriteOrigin,
        {64, 32},                           //cords                   textureRect,
        "textures/stone.png"                //std::string             texture
        });

    //gold
    listOfObjectTemplates.push_back({
        objectTypes::objectGold,            //objectTypes             objectId,
        resourceTypes::resourceGold,        //resourceTypes           typeOfResource,
        1500,                               //int                     startAmountOfResources,
        "Gold",                             //std::string             realName,
        {0 , 2},                            //cords                   spriteOrigin,
        {64, 32},                           //cords                   textureRect,
        "textures/gold.png"                 //std::string             texture
        });

    //Berrybush
    listOfObjectTemplates.push_back({
        objectTypes::objectBerry,          //objectTypes             objectId,
        resourceTypes::resourceFood,       //resourceTypes           typeOfResource,
        5000,                              //int                     startAmountOfResources,
        "Berrybush",                       //std::string             realName,
        {0 , 2},                           //cords                   spriteOrigin,
        {64, 32},                          //cords                   textureRect,
        "textures/berrybush.png"           //std::string             texture
        });

    for (int i = 0; i < listOfObjectTemplates.size(); i++) {
        listOfObjectTemplates[i].setTexture();
    }
}

std::vector<objectTemplates> listOfObjectTemplates;


