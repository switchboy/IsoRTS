#ifndef GAMETEXT_H
#define GAMETEXT_H
#include <list>
#include <string>

struct gameMessage
{
    std::string message;
    int color;
    int timeAdded;
};

class gametext
{
public:
    void addDebugMessage(std::string message, int color);
    void addNewMessage(std::string message, int color);
    void drawDebugMessages() const;
    void drawMessages() const;
    void throwOutOldMessages();

private:
    std::list<gameMessage> listOfMessages;
    std::list<gameMessage> debugMessages;
};

extern gametext gameText;

#endif // GAMETEXT_H
