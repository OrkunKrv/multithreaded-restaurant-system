#pragma once
#ifndef CASHIER_H
#define CASHIER_H
#include "order.h"
#include "logger.h"

class Cashier
{
public:
	void paymentProccess(PaymentQueue& payQueue, std::atomic<bool>& isRunning, std::atomic<int>& totalRevenue)
	{
		Order finishedOrder;

		while (isRunning)
		{
			switch (finishedOrder.type)
			{
				case Menu::PIZZA:
					std::this_thread::sleep_for(std::chrono::seconds(5));
					break;
				case Menu::PASTA:
					std::this_thread::sleep_for(std::chrono::seconds(3));
					break;
				case Menu::STEAK:
					std::this_thread::sleep_for(std::chrono::seconds(6));
					break;
				case Menu::SOUP:
					std::this_thread::sleep_for(std::chrono::seconds(2));
					break;
			}

			payQueue.paymentDone(finishedOrder);
			std::this_thread::sleep_for(std::chrono::seconds(2));

			int currentRev = totalRevenue.fetch_add(finishedOrder.customerPayment) + finishedOrder.customerPayment;

			std::stringstream ssPaymentDone;
			ssPaymentDone << finishedOrder.customerPayment << "$ payment from table " << finishedOrder.tableId << " - Current Revenue: " << currentRev << "$" << std::endl;
			Logger::log(ssPaymentDone.str());
		}
	}
};

#endif // !CASHIER_H

