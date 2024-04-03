#include <inttypes.h>
#include <stdio.h>


struct Message
{

public:
    static constexpr uint8_t RESET_MASK = 0b11000000;
    uint8_t data[1] = {0};

    void set_id(const uint8_t id)
    {
        this->data[0] |= id << 6;
    }

    void set_command(const uint8_t command_mask)
    {
        this->data[0] |= command_mask;
    }

    bool get_command(const uint8_t command_mask)
    {
        return this->data[0] & command_mask;
    }

    void reset()
    {
        this->data[0] = 0;
    }

    const void print_binary()
    {
      
        for(int i = 7; i >= 0; i--)
        {
            Serial.print((this->data[0] & ( 1 << i)) != 0);
        }
        Serial.println();
    }
};

class ControlMessage : public Message
{
    static constexpr uint8_t COMMAND_MESSAGE_ID = 1;
    
public:
    enum Command : uint8_t {
        FORWARD  = 0b00100000,
        BACKWARD = 0b00010000,
        LEFT     = 0b00001000,
        RIGHT    = 0b00000100,
        DOWN     = 0b00000010,
        UP       = 0b00000001,
    };

public:
    ControlMessage(const uint8_t payload)
    {
        this->data[0] = payload;
    }
    ControlMessage()
    {
        this->set_id(COMMAND_MESSAGE_ID);
    }

};
