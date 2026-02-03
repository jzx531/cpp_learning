#ifndef LEVEL_HPP
#define LEVEL_HPP

/*
    1.定义枚举类
    2.定义转换接口
*/

namespace LOG
{
    class LogLevel
    {
        public:
            enum class value
            {
              UNKNOW = 0,
                DEBUG,
                INFO,
                WARN,
                ERROR,
                FATAL,
                OFF
            };

            static const char* toString(value level)
            {
                switch (level)
                {
                    case value::DEBUG: return "DEBUG";break;
                    case value::INFO:  return "INFO";break;
                    case value::WARN:  return "WARN";break;
                    case value::ERROR: return "ERROR";break;
                    case value::FATAL: return "FATAL";break;
                    case value::OFF:   return "OFF";break;
                    default:           return "UNKNOW";break;
                }
            }
    };
};

#endif // LEVEL_HPP

