#pragma once

#include "components/DailyScheduler/DailyScheduler.hpp"


class IrrigationScheduler : public Scheduler
{
private:
  mgos_config_irr *_cfg{nullptr};
  bool _isActive{false}; // Текущее состояние клапана данного ирригатора
  static BinaryOutput *_sharedPump;
  static BinaryOutput *_sharedReverseValve;
  static bool _pumpInitialized;
  static int _activeIrrigators; // Счётчик активных ирригаторов

  void initSharedPump()
  {
    if (!_pumpInitialized)
    {
      const char* pump_name = mgos_sys_config_get_pump_output();
      if (pump_name != nullptr && strlen(pump_name) > 0)
      {
        _sharedPump = App.getBinOutputByName(pump_name);
        if (_sharedPump != nullptr)
        {
          LOG(LL_INFO, ("IrrigationScheduler: global pump '%s' initialized", pump_name));
        }
        else
        {
          LOG(LL_WARN, ("IrrigationScheduler: global pump '%s' not found", pump_name));
        }
      }
      
      const char* reverse_name = mgos_sys_config_get_reverse_output();
      if (reverse_name != nullptr && strlen(reverse_name) > 0)
      {
        _sharedReverseValve = App.getBinOutputByName(reverse_name);
        if (_sharedReverseValve != nullptr)
        {
          LOG(LL_INFO, ("IrrigationScheduler: global reverse valve '%s' initialized", reverse_name));
        }
        else
        {
          LOG(LL_WARN, ("IrrigationScheduler: global reverse valve '%s' not found", reverse_name));
        }
      }
      
      _pumpInitialized = true;
    }
  }



public:
  IrrigationScheduler(mgos_config_irr *_cfg);
  void Update() override;
  Status Init() override;
  void calc();
};

IrrigationScheduler::IrrigationScheduler(mgos_config_irr *cfg) : Scheduler(cfg->name, cfg->output),
                                                                 _cfg(cfg)
{
  // Инициализация имени управляющего входа и сохраненного режима
  _controlInputName = cfg->ctrl_input ? std::string(cfg->ctrl_input) : "";
  _savedMode = cfg->mode;
  
  // Инициализация общего насоса (выполняется один раз для первого экземпляра)
  initSharedPump();
  
  // Установка колбеков для управления насосом
  if (_sharedPump != nullptr)
  {
    setOnCallback([this]() {
      if (_out != nullptr)
      {
        bool currentState = _out->getState();
        // Увеличиваем счётчик только при реальном переходе OFF->ON
        if (!_isActive && currentState)
        {
          _isActive = true;
          _activeIrrigators++;
          
          // Включаем насос
          if (_sharedPump != nullptr && !_sharedPump->getState())
          {
            _sharedPump->setState(true);
            LOG(LL_INFO, ("Global pump turned ON (active irrigators: %d)", _activeIrrigators));
          }
          
          // Включаем клапан обратки
          if (_sharedReverseValve != nullptr && !_sharedReverseValve->getState())
          {
            _sharedReverseValve->setState(true);
            LOG(LL_INFO, ("Global reverse valve turned ON (active irrigators: %d)", _activeIrrigators));
          }
        }
      }
    });
    
    setOffCallback([this]() {
      if (_out != nullptr)
      {
        bool currentState = _out->getState();
        // Уменьшаем счётчик только при реальном переходе ON->OFF
        if (_isActive && !currentState)
        {
          _isActive = false;
          _activeIrrigators--;
          if (_activeIrrigators <= 0)
          {
            _activeIrrigators = 0;
            
            // Выключаем насос
            if (_sharedPump != nullptr && _sharedPump->getState())
            {
              _sharedPump->setState(false);
              LOG(LL_INFO, ("Global pump turned OFF (no active irrigators)"));
            }
            
            // Выключаем клапан обратки
            if (_sharedReverseValve != nullptr && _sharedReverseValve->getState())
            {
              _sharedReverseValve->setState(false);
              LOG(LL_INFO, ("Global reverse valve turned OFF (no active irrigators)"));
            }
          }
          else
          {
            LOG(LL_INFO, ("Global pump stays ON (active irrigators: %d)", _activeIrrigators));
          }
        }
      }
    });
  }
  
  calc();
  if (_cfg->mode != AUTO_MAP)
  {
    printItems();
  }
}

Status IrrigationScheduler::Init()
{
  if (_cfg->mode == AUTO_MAP)
  {
    LOG(LL_INFO, ("IrrigationScheduler %s: trying to load map items from file", _name.c_str()));
    Status s = loadItems();
    if(s.ok())
    {
      map_is_loaded = true;
      LOG(LL_INFO, ("IrrigationScheduler %s: loaded %d map items from file", _name.c_str(), _items.size()));
    }
    else
    {
      LOG(LL_WARN, ("IrrigationScheduler %s: failed to load map items from file: %s", _name.c_str(), s.error_message().c_str()));
      LOG(LL_WARN, ("IrrigationScheduler %s: will try to load in Update() or use manual mode", _name.c_str()));
      map_is_loaded = false;
    }
  }
  _out = App.getBinOutputByName(_output);
  _timer = new Timer(_interval, MGOS_TIMER_REPEAT, std::bind(&PollingComponent::callback, this));
  _timer->Reset(_interval, MGOS_TIMER_REPEAT);
  if (_timer->IsValid())
    return Status::OK();
  else
    return Status::CANCELLED();
}

void IrrigationScheduler::Update()
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
    case AUTO_MAP:
      if (map_is_loaded){
        check();
      }else{
        Status s = loadItems();
        if (s.ok())
        {
          map_is_loaded = true;
          LOG(LL_INFO, ("IrrigationScheduler %s loaded map items from file", _name.c_str()));
          check();
        }else
        {
          LOG(LL_ERROR, ("IrrigationScheduler %s failed to load map items from file", _name.c_str()));
          off();
        }
      } 
      break;
    default:
      off();
      break;
    }
  }
  else
    off();
}
void IrrigationScheduler::calc()
{
  if (_cfg->mode == AUTO_MAP)
  {
    return;
  }
  map_is_loaded = false;
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