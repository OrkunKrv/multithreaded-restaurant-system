#pragma once
#include <vector>
#include <thread>
#include <iostream>
#include "order.h"
#include "chef.h"
#include "waiter.h"
#include "cashier.h"
#include "menu.h"

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
	Menu menu;
	std::atomic<bool> isRunning{ true };
	std::atomic<int> globalOrderId{ 1 };
	std::atomic<int> totalRevenue{ 0 };

public:
	void startSystem()
	{
		isRunning = true;
		std::cout << "Starting the restaurant management system..." << std::endl;
		for (int i = 0; i < 5; ++i)
		{
			std::thread chefThread(&Chef::waitOrder, &chef, std::ref(orderQueue), std::ref(chefManager), std::ref(payQueue));
			systemThreads.push_back(std::move(chefThread)); //move the thread into the vector to avoid copying
		}
		for (int i = 0; i < 5; ++i)
		{
			std::thread waiterThread(&Waiter::takeOrder, &waiter, std::ref(orderQueue), std::ref(isRunning), std::ref(globalOrderId), std::ref(menu));
			systemThreads.push_back(std::move(waiterThread));
		}
		std::thread cashierThread(&Cashier::paymentProccess, &cashier, std::ref(payQueue), std::ref(isRunning), std::ref(totalRevenue));
		systemThreads.push_back(std::move(cashierThread));
	}

	void stopSystem()
	{

		std::cout << "Stopping the restaurant management system..." << std::endl;
		for (auto& thread : systemThreads)
		{
			if (thread.joinable())
			{
				thread.join(); 
			}
		}
		std::stringstream ssRevenue;
		isRunning = false;

		ssRevenue << "Today's revenue: " << totalRevenue.load();
		Logger::log(ssRevenue.str());
		FileLogger::logToFile(ssRevenue.str());
		std::this_thread::sleep_for(std::chrono::seconds(5));
		systemThreads.clear(); 
	}
};

#endif // !RESTAURANTMANAGER_H

