#pragma once

#include <deque>
#include <stack>
#include <string>

// Allows to throw strings easily: `throw "error message"s`
using namespace std::string_literals;

// Pop and return the popped item (unlike std::stack::pop)
template <typename T>
T pop(std::stack<T> &stack) {
	T ret = stack.top();
	stack.pop();
	return ret;
}

template <typename T>
T pop_back(std::deque<T> &deque) {
	T ret = deque.back();
	deque.pop_back();
	return ret;
}
