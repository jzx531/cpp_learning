#include <iostream>

//无参特化
void xprintf()
{
	std::cout << std::endl;
}

template<typename T, typename... Args>
void xprintf(const T& v, Args&&... args)
{
	std::cout<<v;
	if(sizeof...(args)>0)
	{
		xprintf(std::forward<Args>(args)...);
	}else{
		xprintf();
	}
}


int main()
{
    xprintf("Hello, ", "world!", 123, 45.67, '\n');
    return 0;
}

