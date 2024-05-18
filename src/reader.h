#ifndef READER_H
#define READER_H

class Reader
{
private:
  Led led;
  int pin;
  bool status = false;
  bool isRoadStatus = false;

  unsigned long lastRoadDebounceTime = 0;
  unsigned long RoadDebounceDelay = 30000;

  unsigned long lastToggleDebounceTime = 0;
  unsigned long toggleDebounceDelay = 500;

public:
  Reader(int pin, Led &ledRef) : led(ledRef)
  {
    this->led = led;
    this->pin = pin;
  }

  bool getIsRoadStatus()
  {
    return this->isRoadStatus;
  }

  bool getStatus()
  {
    this->isRoadStatus = true;
    return this->status;
  }

  void setup()
  {
    this->led.setup();
    pinMode(this->pin, INPUT_PULLUP);
  }

  void loop()
  {
    if((millis() - lastToggleDebounceTime) <= toggleDebounceDelay) return;

    int roadData = digitalRead(this->pin);
    bool newStatus;
    if (roadData == LOW)
    {
      newStatus = true;
    }
    else
    {
      newStatus = false;
    }

    if (newStatus != this->status)
    {
      newStatus ? led.on() : led.off() ;
      this->status = newStatus;
      this->isRoadStatus = false;
      lastRoadDebounceTime = millis();
      lastToggleDebounceTime = millis();
    }
    else if (millis() - lastRoadDebounceTime > RoadDebounceDelay)
    {
      this->isRoadStatus = false;
      lastRoadDebounceTime = millis();
    }
  }
};

#endif // READER_H