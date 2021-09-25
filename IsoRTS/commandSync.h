#ifndef COMMANDSYNC_H
#define COMMANDSYNC_H
#include "globalfunctions.h"
#include "humanReadableNames.h"

class commandSync
{

public:
	commandSync();
	void addCommand(command givenCommand);
	std::vector <command> getNextCommandsToExcecute(float gameTime);
	std::vector <command> getAllCommandsGiven();

private:
	std::vector<command> listOfCommands;
	std::vector<command> listOfAllCommandsExcecuted;

};

extern commandSync gameDirector;

#endif