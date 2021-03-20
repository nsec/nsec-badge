#pragma once

namespace rpg
{

class TimerDevice
{
  public:
    bool has_expired(unsigned int timer)
    {
        return timer >= value;
    }

    void start(unsigned int timer, unsigned int limit, unsigned int jitter)
    {
        value = timer + limit + ((timer * 17) % jitter);
    }

  private:
    unsigned int value = 0;
};

} // namespace rpg
