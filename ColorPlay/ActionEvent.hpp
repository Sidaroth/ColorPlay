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

	Action action;
	ActionEvent();
	ActionEvent(unsigned short value, Action action=Action::None);

	void setAction(Action action);
	void setValue(unsigned short value);
	unsigned short getValue();

private:
	unsigned short value;
};