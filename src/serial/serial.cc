#include "serial.hh"

#include <iostream>
#include <pigpio.h>

namespace serial
{
    SerialCommunicator::SerialCommunicator()
    {
        int ret = 0;
        ret = gpioInitialise();
        if (ret < 0)
        {
            std::cerr << "Failed to initialize GPIO\n";
            return;
        }

        ret = serOpen((char *)"/dev/ttyAMA0", 115200, 0);
        if (ret < 0)
        {
            gpioTerminate();
            std::cerr << "Failed to open ttyAMA0\n";
            return;
        }

        handle_ = ret;
    }

    SerialCommunicator::~SerialCommunicator()
    {
        serClose(handle_);
    }

    int SerialCommunicator::send(const char data) const
    {
        return serWriteByte(handle_, data);
    }
} // namespace serial
