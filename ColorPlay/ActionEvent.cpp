#include "ActionEvent.hpp"

ActionEvent::ActionEvent()
{

}

ActionEvent::ActionEvent(unsigned short value, unsigned short bulbID, Action action)
{
	this -> action = action;
	this -> value = value;
	this -> bulbID = bulbID;

}

void ActionEvent::setValue(unsigned short value)
{
	this -> value = value;
}

void ActionEvent::setAction(Action action)
{
	this -> action = action;
}

void ActionEvent::setBulbID(unsigned short bulbID)
{
	this -> bulbID = bulbID;
}

unsigned short ActionEvent::getValue() const
{
	return this -> value;
}

unsigned short ActionEvent::getBulbID() const
{
	return this -> bulbID;
}

bool ActionEvent::operator!=(const ActionEvent& other) const
{
	return this -> action == other.action;
}