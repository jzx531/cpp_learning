#include "Connection.h"


void Connection::updateLastActiveTime()
{
    auto now = std::chrono::system_clock::now();
    last_active_time = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
}