#pragma once
#include "globalfunctions.h"
#include <SFML/Graphics.hpp>
#include <vector>


class orderCursor
{
public:
	orderCursor(sf::Vector2f& clickCords);
	void drawCursor();
	bool isFinished();

private:
	sf::Vector2f locationOfClick;
	float timeLastFrameUpdate;
	bool finished;
	int xOffSet;
};

extern std::vector<orderCursor> listOfOrderCursors;
