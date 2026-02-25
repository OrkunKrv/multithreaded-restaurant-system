#pragma once
#ifndef CHEF_H
#define CHEF_H
#include "order.h"
#include "logger.h"
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
	bool systemStatus = true;
	
	void waitOrder(OrderQueue &orderQueue, ChefManager &chefIdManager, PaymentQueue& payQueue)
	{
		Order order;
		
		while (systemStatus)
		{
			int chefId = chefIdManager.getChefID();

			if (orderQueue.orderQueueEmpty())
			{
				std::stringstream ssWaiting;
				ssWaiting << "Chef " << chefId <<  " is waiting for orders..." << std::endl;
				Logger::log(ssWaiting.str());
			}

			orderQueue.popOrder(order);
			std::stringstream ssOrder;
			ssOrder << "Chef " << chefId << " received " << Logger::menuToString(order.type) << " order for table " << order.tableId << ". Order ID: " << order.id << std::endl;
			Logger::log(ssOrder.str());

			switch (order.type)
			{
				case Menu::PIZZA:
					prepareOrder(chefId, order.tableId, order.type);
					break;

				case Menu::PASTA:
					prepareOrder(chefId, order.tableId, order.type);
					break;

				case Menu::STEAK:
					prepareOrder(chefId, order.tableId, order.type);
					break;

				case Menu::SOUP:
					prepareOrder(chefId, order.tableId, order.type);
					break;
			}

			std::stringstream ssDone;
			ssDone << "Chef " << chefId << " has finished preparing " << Logger::menuToString(order.type) << " for table " << order.tableId << ". Order ID: " << order.id << std::endl;
			Logger::log(ssDone.str());
			payQueue.paymentProccess(order);
			chefIdManager.releaseChefID(chefId);
		}		
	}

	void prepareOrder(int chefId, int tableId, Menu foodType)
	{
		std::string foodName = Logger::menuToString(foodType);
		int prepTime = 0;

		switch (foodType)
		{
			case Menu::PIZZA:
				prepTime = 4;
				break;
			case Menu::PASTA:
				prepTime = 3;
				break;
			case Menu::STEAK:
				prepTime = 5;
				break;
			case Menu::SOUP:
				prepTime = 2;
				break;
			default:
				return;
		}

		std::stringstream ss;
		ss << "Chef " << chefId << " is preparing a " << foodName << " for table " << tableId << std::endl;
		Logger::log(ss.str());
		std::this_thread::sleep_for(std::chrono::seconds(prepTime));
	}
};
#endif // !CHEF_H

