#pragma once
#include <vector>
#include <mutex>
#include "order.h"
#ifndef WAREHOUSE_H
#define WAREHOUSE_H

class Warehouse
{
private:
    int steak = 10;
    int cheese = 10;
    int sausage = 10;
    int dough = 20;
    int pasta = 20;
	int sauce = 40;
    int soup = 30;

	std::mutex warehouseMutex;
public:
    void restock(Menu item)
    {
		std::unique_lock<std::mutex> lock(warehouseMutex);
        switch (item)
        {
            case Menu::PIZZA:
                dough += 10;
                cheese += 10;
                sausage += 10;
				break;
           case Menu::PASTA:
                pasta += 10;
				sauce += 10;
                break;
            case Menu::STEAK:
                steak += 10;
                break;
            case Menu::SOUP:
                soup += 10;
                break;
            default:
				break;
        }
		std::this_thread::sleep_for(std::chrono::seconds(2));
    }

	bool useIngredients(Menu item)
    {
        switch (item)
        {          
            case Menu::PIZZA:
            {
                if (dough >= 1 && cheese >= 1 && sausage >= 1)
                {
                    dough -= 1;
                    cheese -= 1;
                    sausage -= 1;
                    return true;
                }
                return false;
            }
            break;
            case Menu::PASTA:
            {
                if(pasta >= 1 && sauce >= 1)
                {
                    pasta -= 1;
                    sauce -= 2;
                    return true;
                }
                return false;
            }
            break;
            case Menu::STEAK:
            {
                if(steak >= 1)
                {
                    steak -= 1;
                    return true;
                }
                return false;
            }
            break;
            case Menu::SOUP:
            {
                if(soup >= 1)
                {
                    soup -= 2;
                    return true;
                }
                return false;
            }
            break;
            default:
			break;
		}

	}
};
#endif // !WAREHOUSE_H

