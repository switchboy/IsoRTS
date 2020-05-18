#ifndef BUTTON_H
#define BUTTON_H
#include <SFML/Graphics.hpp>


class button
{
public:
    button(int positionX, int positionY, int spriteId, int actionType, int actorOrBuildingId, int buttonId, int taskId);
    bool isClicked(sf::Vector2i mousePosition);
    void performAction();
    void drawButton();
    bool isHoverd(sf::Vector2i mousePosition);
    void showToolTip();
    virtual ~button();

protected:

private:
    int positionX;
    int positionY;
    int realPositionX;
    int realPositionY;
    int actionType;
    int actorOrBuildingId;
    int spriteId;
    int buttonId;
    int taskId;
};

#endif // BUTTON_H
