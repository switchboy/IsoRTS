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
    void addNewMessage(std::string message, int color);
    void drawMessages() const;
    void throwOutOldMessages();

private:
    std::list<gameMessage> listOfMessages;
};

extern gametext gameText;

#endif // GAMETEXT_H
