#pragma once
#include <iostream>
#include <mutex>


static std::mutex coutMutex;

template <class T> void pcout(T value)
{
	std::lock_guard<std::mutex> lg(coutMutex);
	std::cout << value;
}
