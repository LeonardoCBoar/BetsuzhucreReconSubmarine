

class MotorController
{
private:
    const int forward_pin;
    const int backward_pin;
    const int speed_pin;
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
                digitalWrite(this->forward_pin, LOW);
                digitalWrite(this->backward_pin, HIGH);
            }
            else {
                digitalWrite(this->forward_pin, HIGH);
                digitalWrite(this->backward_pin, LOW);
            }
            this->set_speed(this->speed);
        }
        else
        {
            digitalWrite(this->forward_pin, LOW);
            digitalWrite(this->backward_pin, LOW);
        }
    }
public:
    MotorController(const int forward_pin, const int backward_pin, 
                    const int speed_pin,   const int speed = 255)
        :forward_pin{forward_pin}, backward_pin{backward_pin}, speed{speed}
        {
            pinMode(forward_pin, OUTPUT);
            pinMode(backward_pin, OUTPUT);
            pinMode(speed_pin, OUTPUT);

            analogWrite(speed_pin, speed);
        }
    
    void set_speed(const int new_speed)
    {
        this->speed = new_speed;
        analogWrite(speed_pin, speed);
    }

    void set_enabled(const bool enabled)
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
