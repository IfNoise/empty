#pragma once

#include "components/DailyScheduler/DailyScheduler.hpp"

class LightTimer : public Scheduler
{
private:
  mgos_config_light *_cfg{nullptr};

public:
  LightTimer(mgos_config_light *_cfg);
  void Update() override;
};

LightTimer::LightTimer(mgos_config_light *cfg) : Scheduler(cfg->name, cfg->output),
                                                 _cfg(cfg)
{
  SchedulerItem item;
  item.start = _cfg->start;
  item.stop = _cfg->stop;
  _items.push_back(item);
}

void LightTimer::Update()
{
  if (_cfg->enable)
  {
    uint32_t mode = _cfg->mode;
    switch (mode)
    {
    case MANUAL_OFF:
      off();
      break;
    case MANUAL_ON:
      on();
      break;
    case AUTO:
      check();
      break;
    default:
      off();
      break;
    }
  }
}