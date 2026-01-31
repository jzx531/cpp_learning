#include <iostream>
#include "processpool.hpp"

int main()
{
	mission_load();
	processpool_init();
	process_control();
    process_clean();
	return 0;
}

