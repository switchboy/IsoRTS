#ifndef GAMETEXT_H
#define GAMETEXT_H
#include <list>
#include <string>

struct gameMessage
{
    std::string message;
    int color;
    float timeAdded;
};

class gametext
{
public:
    gametext();
    void addNewMessage(std::string message, int color);
    void drawMessages();
    void throwOutOldMessages();
    virtual ~gametext();

private:
    std::list<gameMessage> listOfMessages;
};

extern gametext gameText;

#endif // GAMETEXT_H
