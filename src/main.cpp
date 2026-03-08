#include <iostream>
#include "restaurantManager.h"

int main()
{
	srand(time(0)); 

	RestaurantManager restaurantManager;
	restaurantManager.startSystem();

	std::this_thread::sleep_for(std::chrono::seconds(5));
	std::cout << "Press Enter to stop the system..." << std::endl;
	char quit = std::cin.get();
	if (quit == '\n')
	{
		restaurantManager.stopSystem();
	}

	return 0;
}