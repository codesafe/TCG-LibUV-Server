#ifndef GAME_SERVICE
#define GAME_SERVICE



class GameService
{
public :
	GameService();
	~GameService();

public :
	bool	init();
	bool	uninit();

	bool	runLoop();

	bool	onNewConnection();
	bool	onCloseConnection();

};



#endif


