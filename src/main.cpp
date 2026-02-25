#include <iostream>
#include "restaurantManager.h"
//next to do --> inventory management, subfoods for each menu item  
int main()
{
	srand(time(0)); 

	RestaurantManager restaurantManager;
	restaurantManager.startSystem();

	std::this_thread::sleep_for(std::chrono::seconds(5));

	restaurantManager.stopSystem();

	return 0;
}