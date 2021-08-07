#include "gametext.h"
#include "gamestate.h"

gametext gameText;

void gametext::addNewMessage(std::string message, int color)
{
    this->listOfMessages.push_back({message, color, currentGame.getTime()});
    if(this->listOfMessages.size() > 20)
    {
        this->listOfMessages.pop_front();
    }
}

void gametext::drawMessages() const
{
    int charSize = 16;
    int startPositionY = (mainWindowHeigth/2)-(20*(charSize+2)) ;
    currentGame.text.setCharacterSize(charSize);
    currentGame.text.setOutlineColor(sf::Color::Black);
    currentGame.text.setOutlineThickness(1.f);
    for (auto &textItem : this->listOfMessages)
    {
        currentGame.text.setString(textItem.message);
        switch(textItem.color)
        {
        case 0:
            //white
            currentGame.text.setFillColor(sf::Color::White);
            break;
        case 1:
            currentGame.text.setFillColor(sf::Color::Red);
            break;
        }
        currentGame.text.setPosition(5, static_cast<float>(startPositionY));
        window.draw(currentGame.text);
        startPositionY += +28;
    }
}

void gametext::throwOutOldMessages()
{
    bool thisIsANewMessage = false;
    while(!thisIsANewMessage)
    {
        if(!this->listOfMessages.empty())
        {
            if((this->listOfMessages.front().timeAdded+60) < currentGame.getTime())
            {
                this->listOfMessages.pop_front();
            }
            else
            {
                thisIsANewMessage = true;
            }
        }
        else
        {
            thisIsANewMessage = true;
        }
    }
}
