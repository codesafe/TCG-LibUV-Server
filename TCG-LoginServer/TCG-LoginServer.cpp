#include "GameService.h"

int main()
{
	GameService *gameservice = new GameService();
	gameservice->init();
	gameservice->runLoop();
	gameservice->uninit();


    return 0;
}

