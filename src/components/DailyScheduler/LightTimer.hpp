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
  // Инициализация имени управляющего входа и сохраненного режима
  _controlInputName = cfg->ctrl_input ? std::string(cfg->ctrl_input) : "";
  _savedMode = cfg->mode;
  
  SchedulerItem item;
  item.start = _cfg->start;
  item.stop = _cfg->stop;
  _items.push_back(item);
}

void LightTimer::Update()
{
  if (_cfg->enable)
  {
    int mode = _cfg->mode;
    
    // Проверяем управляющий вход
    if (checkControlInput(mode))
    {
      on();  // Принудительное включение
      return;
    }
    
    // Если вход был активен, а теперь нет - восстанавливаем режим
    if (mode == MANUAL_ON && _savedMode != MANUAL_ON)
    {
      mode = _savedMode;
    }
    
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
  }else{off();}
}