#pragma once
#ifndef CHEF_H
#define CHEF_H
#include "order.h"
#include "logger.h"
#include "warehouse.h"
#include <vector>

class ChefManager
{
private:
	std::vector<int> chefIds;
	std::mutex chefMutex;
	std::condition_variable m_chefCV;

public:
	ChefManager()
	{
		for (int i = 1; i <= 5; ++i)
		{
			chefIds.push_back(i);
		}
	}

	int getChefID()
	{
		std::unique_lock<std::mutex> lock(chefMutex);
		m_chefCV.wait(lock, [this]() {return !chefIds.empty(); });
		int chefId = chefIds.back();
		chefIds.pop_back();

		return chefId;
	}

	void releaseChefID(int chefId)
	{
		std::lock_guard<std::mutex> lock(chefMutex);
		chefIds.push_back(chefId);
		m_chefCV.notify_one();
	}
};

class Chef
{
public:
	void waitOrder(OrderQueue &orderQueue, ChefManager &chefIdManager, PaymentQueue& payQueue, Warehouse& item, std::atomic<bool>& isRunning, std::atomic<int>* tableStates)
	{
		Order order;
		
		while (isRunning)
		{
			int chefId = chefIdManager.getChefID();

			if (orderQueue.orderQueueEmpty())
			{
				std::stringstream ssWaiting;
				ssWaiting << "Chef " << chefId <<  " is waiting for orders...";
				Logger::log(ssWaiting.str());
			}

			if (!orderQueue.popOrder(order, isRunning)) 
			{
				break; 
			}
			std::stringstream ssOrder;
			ssOrder << "Chef " << chefId << " received " << Logger::menuToString(order.type) << " order for table " << order.tableId << ". Order ID: " << order.id;
			Logger::log(ssOrder.str());

			switch (order.type)
			{
				case Menu::PIZZA:
					prepareOrder(chefId, order.tableId, order.type, item);
					break;

				case Menu::PASTA:
					prepareOrder(chefId, order.tableId, order.type, item);
					break;

				case Menu::STEAK:
					prepareOrder(chefId, order.tableId, order.type, item);
					break;

				case Menu::SOUP:
					prepareOrder(chefId, order.tableId, order.type, item);
					break;
			}

			tableStates[order.tableId].store(2);
			std::stringstream ssDone;
			ssDone << "Chef " << chefId << " has finished preparing " << Logger::menuToString(order.type) << " for table " << order.tableId << ". Order ID: " << order.id;
			Logger::log(ssDone.str());
			payQueue.paymentProccess(order);
			chefIdManager.releaseChefID(chefId);
		}		
	}

	void prepareOrder(int chefId, int tableId, Menu foodType, Warehouse& warehouse)
	{
		std::string foodName = Logger::menuToString(foodType);
		int prepTime = 0;

		switch (foodType)
		{
			case Menu::PIZZA:
				prepTime = 4;
				if (warehouse.useIngredients(foodType) == false)
				{
					Logger::log("Not enough ingredients for pizza, restocking...");
					warehouse.restock(foodType);
					warehouse.useIngredients(foodType);
				}
				break;
			case Menu::PASTA:
				prepTime = 3;
				if (warehouse.useIngredients(foodType) == false)
				{
					Logger::log("Not enough ingredients for pasta, restocking...");
					warehouse.restock(foodType);
					warehouse.useIngredients(foodType);
				}
				break;
			case Menu::STEAK:
				prepTime = 5;
				if (warehouse.useIngredients(foodType) == false)
				{
					Logger::log("Not enough ingredients for steak, restocking...");
					warehouse.restock(foodType);
					warehouse.useIngredients(foodType);
				}
				break;
			case Menu::SOUP:
				prepTime = 2;
				if (warehouse.useIngredients(foodType) == false)
				{
					Logger::log("Not enough ingredients for soup, restocking...");
					warehouse.restock(foodType);
					warehouse.useIngredients(foodType);
				}
				break;
			default:
				return;
		}

		std::stringstream ss;
		ss << "Chef " << chefId << " is preparing a " << foodName << " for table " << tableId;
		Logger::log(ss.str());
		std::this_thread::sleep_for(std::chrono::seconds(prepTime));
	}
};
#endif // !CHEF_H

