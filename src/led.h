#ifndef LED_H
#define LED_H

class Led
{
private:
  int pin;
  bool isOn = false;

public:
  Led(int pin)
  {
    this->pin = pin;
  }

  void setup()
  {
    pinMode(this->pin, OUTPUT);
    this->off();
  }

  bool getValue()
  {
    return this->isOn;
  }

  void toggle()
  {
    this->isOn = !this->isOn;
    digitalWrite(this->pin, this->isOn ? HIGH : LOW);
  }

  void on()
  {
    this->isOn = true;
    digitalWrite(this->pin, HIGH);
  }

  void off()
  {
    this->isOn = false;
    digitalWrite(this->pin, LOW);
  }
};

#endif // LED_H