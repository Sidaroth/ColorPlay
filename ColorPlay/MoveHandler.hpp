#include "psmove.h"

class MoveHandler
{
public:
	MoveHandler();
	bool connect();
private:
	PSMove *move;
	enum PSMove_connection_Type ctype;
};