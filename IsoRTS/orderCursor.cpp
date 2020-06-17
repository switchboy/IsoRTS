#include "orderCursor.h"
#include "gamestate.h"

orderCursor::orderCursor(sf::Vector2f& clickCords)
{
	this->locationOfClick = clickCords;
	this->finished = false;
	this->timeLastFrameUpdate = currentGame.getTime();
	this->xOffSet = 0;
}

void orderCursor::drawCursor()
{
	if (this->timeLastFrameUpdate + 0.1 < currentGame.getTime())
	{
		this->timeLastFrameUpdate = currentGame.getTime();
		this->xOffSet += 1;
	}
	if (this->xOffSet == 15) {
		this->finished = true;
	}
	currentGame.spriteCommandCursor.setTextureRect(sf::IntRect(currentGame.spriteCommandCursor.getTextureRect().width * this->xOffSet, 0, currentGame.spriteCommandCursor.getTextureRect().width, currentGame.spriteCommandCursor.getTextureRect().height));
	currentGame.spriteCommandCursor.setPosition(locationOfClick.x, this->locationOfClick.y);
	window.draw(currentGame.spriteCommandCursor);
}

bool orderCursor::isFinished()
{
	return this->finished;
}

std::vector<orderCursor> listOfOrderCursors;