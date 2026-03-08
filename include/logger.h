#pragma once
#ifndef LOGGER_H
#define LOGGER_H
#include <iostream>
#include <mutex>
#include <string>
#include <sstream>
#include <fstream>
#include "menu.h"

class Logger
{
private:
	static inline std::deque<std::string> logHistory;
	static inline std::mutex logMutex;
	static inline const size_t MAX_LOG_HISTORY = 15; // Limit the log history to the most recent 15 entries

public:
	// Function to log a message with thread safety and maintain a history of logs
	static void log(const std::string& message) 
	{
		std::lock_guard<std::mutex> lock(logMutex);

		logHistory.push_back(message);

		if (logHistory.size() > MAX_LOG_HISTORY)
		{
			logHistory.pop_front(); // Remove the oldest log entry to maintain the size limit
		}
	}

	// Function to retrieve the current log history
	std::vector<std::string> getLogHistory() 
	{
		std::lock_guard<std::mutex> lock(logMutex); 
		return std::vector<std::string>(logHistory.begin(), logHistory.end());
	}

	static std::string menuToString(Menu m) 
	{
		switch (m) 
		{
			case Menu::PIZZA: 
				return "Pizza";

			case Menu::PASTA: 
				return "Pasta";

			case Menu::STEAK: 
				return "Steak";

			case Menu::SOUP:  
				return "Soup";

			default: return "Invalid";
		}
	}
};

class FileLogger
{
public:
	static inline std::mutex m_fileMutex;

	static void logToFile(const std::string& message, const std::string& filename = "restaurant_log.txt")
	{
		std::lock_guard<std::mutex> fileLock(m_fileMutex);
		std::ofstream logFile(filename, std::ios::app);
		if (logFile.is_open())
		{
			logFile << message << std::endl;
			logFile.close();
		}
		else
		{
			std::cerr << "Unable to open log file: " << filename << std::endl;
		}
	}

	void deleteLogFile(const std::string& filename = "restaurant_log.txt")
	{
		std::lock_guard <std::mutex> fileLock(m_fileMutex);
		if (std::remove(filename.c_str()) != 0)
		{
			std::cerr << "Error deleting log file: " << std::endl;
		}
		else
		{
			std::cout << "Log file deleted successfully: " << std::endl;
		}
	}
};

#endif // !LOGGER_H

