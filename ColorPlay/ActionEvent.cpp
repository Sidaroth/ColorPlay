#include "ActionEvent.hpp"

ActionEvent::ActionEvent()
{

}

ActionEvent::ActionEvent(unsigned short value, Action action)
{
	this -> action = action;
	this -> value = value;
}

void ActionEvent::setValue(unsigned short value)
{
	this -> value = value;
}

void ActionEvent::setAction(Action action)
{
	this -> action = action;
}

unsigned short ActionEvent::getValue()
{
	return this -> value;
}