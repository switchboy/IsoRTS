#ifndef BUTTON_H
#define BUTTON_H
#include "humanReadableNames.h"

class button
{
public:
    button(int positionX, int positionY, spriteTypes spriteId, actionTypes actionType, int actorOrBuildingId, int buttonId, int taskId);
    virtual ~button(); 
    
    bool isClicked(sf::Vector2i mousePosition);
    bool isHovered(sf::Vector2i mousePosition) const;
    void drawButton() const;
    void performAction();
    void showToolTip() const; 

private:
    int actorOrBuildingId;
    int buttonId;
    int positionX;
    int positionY;
    int realPositionX;
    int realPositionY;
    int taskId;
    actionTypes actionType;
    spriteTypes spriteId;
};

namespace buttons {
    extern bool requirementForButtonIsMet(actionTypes actionType, int unitOrBuildingId, int playerId);
    }
}

extern std::list<button> listOfButtons;

#endif // BUTTON_H
