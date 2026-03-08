#pragma once
#include <vector>
#include <thread>
#include <iostream>
#include <windows.h>
#include "order.h"
#include "chef.h"
#include "waiter.h"
#include "cashier.h"
#include "menu.h"
#include "warehouse.h"

#ifndef RESTAURANTMANAGER_H
#define RESTAURANTMANAGER_H

class RestaurantManager
{
protected:
	std::vector<std::thread> systemThreads;
	OrderQueue orderQueue;
	PaymentQueue payQueue;
	Cashier cashier;
	Order order;
	ChefManager chefManager;
	Chef chef;
	Waiter waiter;
	Warehouse warehouse;
	Menu menu;
	std::atomic<bool> isRunning{ true };
	std::atomic<int> globalOrderId{ 1 };
	std::atomic<int> totalRevenue{ 0 };
	std::atomic<int> completedOrders{ 0 };
	std::atomic<long long> totalDuration{ 0 };
	std::atomic<int> tableStates[21];

public:
	void startSystem()
	{
		for (int i = 0; i < 5; ++i)
		{
			std::thread chefThread(&Chef::waitOrder, &chef, std::ref(orderQueue), std::ref(chefManager), std::ref(payQueue), std::ref(warehouse), std::ref(isRunning), tableStates);
			systemThreads.push_back(std::move(chefThread)); //move the thread into the vector to avoid copying
		}
		for (int i = 0; i < 5; ++i)
		{
			std::thread waiterThread(&Waiter::takeOrder, &waiter, std::ref(orderQueue), std::ref(isRunning), std::ref(globalOrderId), std::ref(menu), std::ref(order), tableStates);
			systemThreads.push_back(std::move(waiterThread));
		}
		std::thread cashierThread(&Cashier::paymentProccess, &cashier, std::ref(payQueue), std::ref(isRunning), std::ref(totalRevenue), std::ref(totalDuration), std::ref(completedOrders), tableStates);
		systemThreads.push_back(std::move(cashierThread));
		systemThreads.push_back(std::thread(&RestaurantManager::uiRenderer, this));
	}

	void stopSystem()
	{
		isRunning = false;
		orderQueue.wakeUpChefs();
		payQueue.wakeUpCashier();

		std::cout << "Stopping the restaurant management system..." << std::endl;
		for (auto& thread : systemThreads)
		{
			if (thread.joinable())
			{
				thread.join(); 
			}
		}
		if (completedOrders > 0) 
		{
			std::stringstream ssCustomerSatisfaction;
			long long avgWait = totalDuration.load() / completedOrders.load();
			ssCustomerSatisfaction << "Average wait duration: " << avgWait << " seconds." << std::endl;
			Logger::log(ssCustomerSatisfaction.str());
			FileLogger::logToFile(ssCustomerSatisfaction.str());
		}
		std::cout << "Today's revenue: " << totalRevenue.load() << std::endl;

		std::stringstream ssRevenue;
		ssRevenue << "Today's revenue: " << totalRevenue.load() << "$" << std::endl;
		FileLogger::logToFile(ssRevenue.str());

		std::this_thread::sleep_for(std::chrono::seconds(5));
		systemThreads.clear(); 
	}

	void uiRenderer()
	{
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // Get the console handle
		COORD cursorPosition; // Structure to hold cursor position

		while (isRunning)
		{		
			cursorPosition.X = 0; // Set X coordinate to the beginning of the line
			cursorPosition.Y = 0; // Set Y coordinate to the top of the console
			SetConsoleCursorPosition(hConsole, cursorPosition); // Move the cursor to the top-left corner of the console

			std::cout << "==================================" << std::endl;

			for (int i = 1; i <= 20; ++i)
			{
				int state = tableStates[i].load(); 

				if (state == 0) { std::cout << "[ ]\t"; }
				else if (state == 1) { std::cout << "[W]\t"; }
				else if (state == 2) { std::cout << "[E]\t"; }

				if (i % 5 == 0) { std::cout << std::endl; } // New line after every 5 tables
			}

			std::cout << "==================================" << std::endl;
			std::vector<std::string> logs = Logger().getLogHistory();

			for(const std::string& logMsg : logs)
			{
				std::cout << logMsg << "                                  " << std::endl;
			}	
			std::cout << "                                           " << std::endl;

			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}
};

#endif // !RESTAURANTMANAGER_H

