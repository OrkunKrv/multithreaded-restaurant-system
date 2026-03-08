#pragma once
#ifndef WAITER_H
#define WAITER_H

#include "logger.h"
#include "order.h"
#include "menu.h"
#include <random>

class Waiter
{
public:
	void takeOrder(OrderQueue &orderQueue, std::atomic<bool>& isRunning, std::atomic<int>& globalOrderId, Menu& foodType, Order& order, std::atomic<int>* tableStates)
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> distTable(1, 20); 
		std::uniform_int_distribution<> distFood(1, 4);

		while (isRunning)
		{
			int waitTime = (rand() % 4) + 1;
			std::this_thread::sleep_for(std::chrono::seconds(waitTime));

			int currentTableId = distTable(gen);
			int randomFoodIndex = distFood(gen);
			Menu randomFood = static_cast<Menu>(randomFoodIndex);
			int customerPayment = 0;
			int recentOrderId = globalOrderId.fetch_add(1); // get the current value and increment for the next order

			if (!isRunning) break;

			std::stringstream ssWaiting;
			ssWaiting << "Waiter is taking an order...";
			Logger::log(ssWaiting.str());
	
			std::stringstream ssTake;
			ssTake << "Waiter took an order for: " << Logger::menuToString(randomFood) << ", from table " << currentTableId << ", ID: " << recentOrderId;

			Logger::log(ssTake.str());
			FileLogger::logToFile(ssTake.str());	

			switch (randomFood)
			{
			case Menu::PIZZA:
				customerPayment = 12;
				break;

			case Menu::PASTA:
				customerPayment = 8;
				break;

			case Menu::STEAK:
				customerPayment = 15;
				break;

			case Menu::SOUP:
				customerPayment = 5;
				break;
			}

			Order newOrder;

			newOrder.id = recentOrderId;
			newOrder.type = randomFood;
			newOrder.tableId = currentTableId;
			newOrder.customerPayment = customerPayment;
			tableStates[currentTableId].store(1);

			newOrder.orderTime = std::chrono::steady_clock::now();
			orderQueue.pushOrder(newOrder);

			if (globalOrderId.load() > 10)
			{
				static std::mutex pauseMutex;
				std::lock_guard<std::mutex> lock(pauseMutex);

				if (globalOrderId <= 1) { return; } 

				std::stringstream ssFull;
				ssFull << "Kitchen is full. Please wait..."; 
				Logger::log(ssFull.str());
				std::this_thread::sleep_for(std::chrono::seconds(5));
				globalOrderId.store(1);
			}

		}
	}
};

#endif // !WAITER.H

