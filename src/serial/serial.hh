#pragma once

namespace serial
{
    class SerialCommunicator
    {
    private:
        int handle_;

    public:
        SerialCommunicator();
        ~SerialCommunicator();
        int send(const char data) const;
    };
}
