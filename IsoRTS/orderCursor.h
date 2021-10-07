#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class orderCursor
{
public:
	orderCursor(const sf::Vector2f& clickCords);
	void drawCursor();
	bool isFinished() const;

private:
	sf::Vector2f locationOfClick;
	int timeLastFrameUpdate;
	bool finished;
	int xOffSet;
};

extern std::vector<orderCursor> listOfOrderCursors;
