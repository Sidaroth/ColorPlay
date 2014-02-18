/*
	Purpose: Serve as an event identifier for communication between threads (/classes)

	Last edit: 14. Feb. 2014

	Authors: Christian Holt, Johannes Hovland, Henrik Lee Jotun
			 Gjøvik University College

*/

#pragma once

class ActionEvent
{
public:
	enum class Action{
		Up,
		Down,
		Finish,
		None
	};

	ActionEvent();
	ActionEvent(unsigned short value, unsigned short bulbID, Action action=Action::None);

	Action action;
	
	void setValue(unsigned short value);
	void setAction(Action action);
	void setBulbID(unsigned short bulbID);
	
	unsigned short getValue() const;
	unsigned short getBulbID() const;

private:
	unsigned short value;
	unsigned short bulbID;
};