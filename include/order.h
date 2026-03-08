#pragma once
#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include "menu.h"

#ifndef ORDER_H
#define ORDER_H

struct Order
{
	int id = 0;
	Menu type = Menu::BLANK;
	int tableId = 0;
	int customerPayment = 0;
	std::chrono::steady_clock::time_point orderTime;
};

class OrderQueue
{
private:
	std::queue<Order> orders;
	std::mutex m_mutex;
	std::condition_variable cond_variable;

public:
	void pushOrder(const Order& order)
	{
		const std::lock_guard<std::mutex> lock(m_mutex); // Lock the mutex for thread safety
		orders.push(order);
		cond_variable.notify_one(); // Notify one waiting thread
	}

	bool popOrder(Order& order, std::atomic<bool>& isRunning)
	{
		std::unique_lock<std::mutex> lock(m_mutex); // Lock the mutex for thread safety
		cond_variable.wait(lock, [this, &isRunning]() { return !orders.empty() || !isRunning; }); // Wait until there is an order

		if(!isRunning && orders.empty())
		{
			return false; 
		}
		order = orders.front(); // Retrieve the order
		orders.pop(); // Remove the order from the queue

		return true;
	}

	bool orderQueueEmpty()
	{
		std::lock_guard<std::mutex> lock(m_mutex); 

		return orders.empty();
	}

	void wakeUpChefs()
	{
		cond_variable.notify_all(); 
	}
};

class PaymentQueue
{
private:
	std::queue<Order> paymentQueue;
	std::mutex paymentMutex;
	std::condition_variable payment_variable;

public:
	void paymentProccess(const Order& paymentAmount)
	{
		const std::lock_guard<std::mutex> lock(paymentMutex);
		paymentQueue.push(paymentAmount);
		payment_variable.notify_one();
	}

	bool paymentDone(Order& payment, std::atomic<bool>& isRunning)
	{
		std::unique_lock<std::mutex> lock(paymentMutex);
		payment_variable.wait(lock, [this, &isRunning]() {return !paymentQueue.empty() || !isRunning; });

		if (!isRunning && paymentQueue.empty())
		{
			return false;
		}

		payment = paymentQueue.front();
		paymentQueue.pop();

		return true;
	}

	void wakeUpCashier()
	{
		payment_variable.notify_all();
	}
};


#endif // !ORDER_H

