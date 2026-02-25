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

	void popOrder(Order& order)
	{
		std::unique_lock<std::mutex> lock(m_mutex); // Lock the mutex for thread safety
		cond_variable.wait(lock, [this]() { return !orders.empty(); }); // Wait until there is an order
		order = orders.front(); // Retrieve the order
		orders.pop(); // Remove the order from the queue
	}

	bool orderQueueEmpty()
	{
		std::lock_guard<std::mutex> lock(m_mutex); 

		return orders.empty();
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

	void paymentDone(Order& payment)
	{
		std::unique_lock<std::mutex> lock(paymentMutex);
		payment_variable.wait(lock, [this]() {return !paymentQueue.empty(); });
		payment = paymentQueue.front();
		paymentQueue.pop();
	}
};


#endif // !ORDER_H

