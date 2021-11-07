#include "commandSync.h"
#include "player.h"
#include "gamestate.h"
#include "connection.h"

commandSync::commandSync()
{

}

void commandSync::addCommand(command givenCommand)
{
	listOfCommands.push_back(givenCommand);
	if (givenCommand.playerId == currentPlayer.getTeam()) {
		listOfUnsentCommands.push_back(givenCommand);
	}
}

void commandSync::sendNetWorkCommands()
{
	sf::Packet commandPacket;
	sf::Uint8 header = dataType::commandPacket;
	sf::Uint32 amounOfCommands = listOfUnsentCommands.size();
	commandPacket << header << currentGame.getTime() << multiplayerPlayerId << amounOfCommands;
	for (command& unsentCommand : listOfUnsentCommands) {
		int timeCommandGiven = unsentCommand.timeCommandGiven;
		int playerId = unsentCommand.playerId;
		int subjectId = unsentCommand.subjectId;
		bool placingBuilding = unsentCommand.placingBuilding;
		bool isStackedCommand = unsentCommand.isStackedCommand;
		int commandCordsX = unsentCommand.commandCords.x;
		int commandCordsY = unsentCommand.commandCords.y;
		int subjectType = static_cast<int>(unsentCommand.subjectType);
		int orderType = static_cast<int>(unsentCommand.orderType);
		int actionToPerform = static_cast<int>(unsentCommand.actionToPerform);
		commandPacket << timeCommandGiven << playerId << subjectId<< placingBuilding << isStackedCommand << commandCordsX << commandCordsY << subjectType << orderType << actionToPerform;
	}
	currentConnection.getTcpSocket()->send(commandPacket);
	listOfUnsentCommands.clear();
}

void commandSync::recieveNetworkCommands()
{
	sf::Packet recievePacket;
	sf::Socket::Status statusOfSocket = currentConnection.getTcpSocket()->receive(recievePacket);
	if (statusOfSocket == sf::Socket::Disconnected) {
		//complain
	}
	else if (statusOfSocket == sf::Socket::Done) {
		sf::Uint8 recievedHeader;
		recievePacket >> recievedHeader;
		switch (recievedHeader) {
		case dataType::commandPacket:
		{
			int time;
			recievePacket >> time;
			int fromPlayer;
			recievePacket >> fromPlayer;
			sf::Int32 amountOfCommands;
			recievePacket >> amountOfCommands;
			for (int i = 0; i < amountOfCommands; i++) {
				int timeCommandGiven;
				int playerId;
				int subjectId;
				bool placingBuilding;
				bool isStackedCommand;
				int commandCordsX;
				int commandCordsY;
				int subjectType;
				int orderType;
				int actionToPerform;
				recievePacket >> timeCommandGiven >> playerId >> subjectId >> placingBuilding >> isStackedCommand >> commandCordsX >> commandCordsY >> subjectType >> orderType >> actionToPerform;
				listOfCommands.push_back({ timeCommandGiven, playerId, subjectId, placingBuilding, isStackedCommand, {commandCordsX, commandCordsY}, static_cast<worldObject>(subjectType), static_cast<stackOrderTypes>(orderType) , static_cast<actionTypes>(actionToPerform) });
				std::cout << "Command recieved! for "<< timeCommandGiven << " - current Time: "<< currentGame.getTime() << std::endl;
			}
			break;
		}
		}
	}
}

std::vector<command> commandSync::getNextCommandsToExcecute(int nextCommandWindow)
{
	std::vector<command> commandsToBeExcecuted;

	std::sort(this->listOfCommands.begin(), this->listOfCommands.end(),
		[](const command& a, const command& b) {
			return a.timeCommandGiven > b.timeCommandGiven;
		});

	for (int i = 0; i < this->listOfCommands.size(); i++) {
		if (this->listOfCommands.back().timeCommandGiven < nextCommandWindow) {
			commandsToBeExcecuted.push_back(this->listOfCommands.back());
			this->listOfAllCommandsExcecuted.push_back(this->listOfCommands.back());
			this->listOfCommands.pop_back();
		}
		else {
			break;
		}
	}

	return commandsToBeExcecuted;
}

std::vector<command> commandSync::getAllCommandsGiven()
{
	return this->listOfAllCommandsExcecuted;
}

commandSync gameDirector;
