#ifndef BUTTON_H
#define BUTTON_H

class Button
{
private:
  int pin;
  bool isPressed = false;
  bool isRoad = false;
  unsigned long lastDebounceTime = 0;
  unsigned long debounceDelay = 100;

public:
  Button(int pin)
  {
    this->pin = pin;
  }

  bool isClick()
  {
    if (!this->isRoad && this->isPressed)
    {
      this->isRoad = true;
      return true;
    }
    return false;
  }

  void setup()
  {
    pinMode(this->pin, INPUT_PULLUP);
  }

  void loop()
  {
    int btnValue = digitalRead(this->pin);

    if((millis() - lastDebounceTime) <= debounceDelay) return;

    if (btnValue == LOW)
    {
      if(this->isPressed) return;

      this->isPressed = true;
      this->isRoad = false;
      this->lastDebounceTime = millis();
    }
    else if(this->isPressed){
      this->isPressed = false;
      this->lastDebounceTime = millis();
    }
    
  }
};

#endif // BUTTON_H