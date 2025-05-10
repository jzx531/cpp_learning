#include <algorithm>
#include <vector>
#include <numeric>
#include <iostream>
#include "mingw.thread.h"
#include "mingw.mutex.h"

std::mutex mtx;
void print_num(int num)
{
	std::lock_guard<std::mutex> lock(mtx);
	std::cout << num << " ";
}

class myclass
{
	public:
	 int x;
	 myclass(int i) : x(i) {}
	 void print() const{
		std::cout << x << " ";
	 }
	 void change(int i) {
		this->x = i;
		}
};


int main()
{
	std::vector<int> nums{ 1, 2, 3, 4, 5 };
	std::rotate(nums.begin(), nums.begin() + 2, nums.end());
	std::for_each(nums.begin(), nums.end(), print_num);
	std::for_each(nums.begin(), nums.end(), [](int &i) { i *= 2; });
	// std::transform(nums.begin(), nums.end(), nums.begin(), [](int i) { return i * 2; });
	std::for_each(nums.begin(), nums.end(), print_num);
	// std::transform(nums.begin(), nums.end(), nums.begin(), [](int i) { return i + 1; });
	std::cout << std::endl;
	const myclass m1(10);
	m1.print();
	return 0;
}