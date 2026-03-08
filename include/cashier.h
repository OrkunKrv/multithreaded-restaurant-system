#pragma once
#ifndef CASHIER_H
#define CASHIER_H
#include "order.h"
#include "logger.h"

class Cashier
{
public:
	void paymentProccess(PaymentQueue& payQueue, std::atomic<bool>& isRunning, std::atomic<int>& totalRevenue, std::atomic<long long>& totalDur, std::atomic<int>& completedOrders, std::atomic<int>* tableStates)
	{
		Order finishedOrder;
		while (isRunning)
		{
			if (!payQueue.paymentDone(finishedOrder, isRunning))
			{
				break;
			}

			std::this_thread::sleep_for(std::chrono::seconds(2));
			auto finishTime = std::chrono::steady_clock::now();

			auto duration = std::chrono::duration_cast<std::chrono::seconds>(finishTime - finishedOrder.orderTime);
			totalDur.fetch_add(duration.count());
			completedOrders.fetch_add(1);

			int currentRev = totalRevenue.fetch_add(finishedOrder.customerPayment) + finishedOrder.customerPayment;

			std::stringstream ssPaymentDone;
			ssPaymentDone << finishedOrder.customerPayment << "$ payment from table " << finishedOrder.tableId << " - Current Revenue: " << currentRev << "$";
			Logger::log(ssPaymentDone.str());
			tableStates[finishedOrder.tableId].store(0);
		}
	}
};

#endif // !CASHIER_H

