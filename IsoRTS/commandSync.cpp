#include "commandSync.h"
#include "player.h"

commandSync::commandSync()
{

}

void commandSync::addCommand(command givenCommand)
{
	listOfCommands.push_back(givenCommand);
	if (givenCommand.playerId == currentPlayer.getTeam()) {
		//send command to network stack TO DO!

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
