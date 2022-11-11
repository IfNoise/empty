#pragma once

#include "components/DailyScheduler/DailyScheduler.hpp"

class IrrigationScheduler : public Scheduler
{
private:
  mgos_config_irr *_cfg{nullptr};

public:
  IrrigationScheduler(mgos_config_irr *_cfg);
  void Update() override;
  void calc();
};
IrrigationScheduler::IrrigationScheduler(mgos_config_irr * cfg) : 
Scheduler(cfg->name, cfg->output),
_cfg(cfg)
{
}

void IrrigationScheduler::Update()
{ 
  if(_cfg->enable)
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
void IrrigationScheduler::calc()
{
  unsigned int begin, end, step, num;
  const uint32_t DAY = 86400;
  begin = _cfg->start - _cfg->win;
  end = _cfg->stop + _cfg->win;
  num = _cfg->num;
  if (begin > end)
  {
    step = num < 2 ? (DAY - begin + end) / 2 : (DAY - begin + end) / (num - 1);
  }
  else
  {

    step = num < 2 ? (end - begin) / 2 : (end - begin) / (num - 1);
  }

  _items.clear();
  for (int i = 0; i < num; ++i)
  {
    int start = begin + step * i;
    if (start > DAY)
      start -= DAY;
    int stop = start + _cfg->win;
    if (stop > DAY)
      stop -= DAY;
    SchedulerItem item;
    item.start = start;
    item.stop = stop;
    _items.push_back(item);
  }
}