

class MotorController
{
private:
    const int forward_pin;
    const int backward_pin;
    bool enabled = false;
    bool reversed = false;
    int speed = 255;
private:
    void set_pin_speed(const int pin, const int speed)
    {
        analogWrite(pin, speed);
    }

    void update_pins()
    {
        if(this->enabled)
        {
            if(this->reversed)
            {
                this->set_pin_speed(this->backward_pin, this->speed);
                this->set_pin_speed(this->forward_pin, 0);
            }
            else {
                this->set_pin_speed(this->forward_pin, this->speed);
                this->set_pin_speed(this->backward_pin, 0);
            }

        }
        else
        {
            this->set_pin_speed(this->forward_pin, 0);
            this->set_pin_speed(this->backward_pin, 0);
        }
    }
public:
    MotorController(const int forward_pin, const int backward_pin, const int speed = 255)
        :forward_pin{forward_pin}, backward_pin{backward_pin}, speed{speed}
        {
            pinMode(forward_pin, OUTPUT);
            pinMode(backward_pin, OUTPUT);
        }
    
    void set_speed(const int new_speed)
    {
        this->speed = new_speed;
    }

    void set_enabled(const int enabled)
    {
        this->enabled = enabled;
        this->update_pins();
    }

    void set_reversed(const bool reversed)
    {
        this->reversed = reversed;
        this->update_pins();
    }

};