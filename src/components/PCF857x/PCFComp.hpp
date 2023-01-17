#pragma once
#include "core/Component.hpp"
#include "core/Output.hpp"
#include "mgos.hpp"
#include "mgos_config.h"
#include "mgos_pcf857x.h"
#include <vector>
#include "core/Application.hpp"

class PCFComp : public Component
{
private:
  mgos_pcf857x *_PCF{nullptr};
  mgos_config_pcfcomp *_cfg;

public:
  PCFComp(mgos_config_pcfcomp *cfg);
  Status Init() override;
  void Update() override;
  Status deInit() override;
  bool Read(int pin);
  void Write(int pin, bool state);
};

PCFComp::PCFComp(mgos_config_pcfcomp *cfg) : Component(std::string(cfg->name)), _cfg(cfg)
{
 
}

// PCFRelayComp::~PCFRelayComp( )
// {
// }

Status PCFComp::Init(/* args */)
{
  if (!(_PCF = mgos_pcf8574_create(mgos_i2c_get_global(), _cfg->address,-1)))
  {
    LOG(LL_ERROR, ("Could not create PCF857X"));
    return Status::CANCELLED();
  }
  else
    LOG(LL_INFO, ("PCF857X Component %s created ",_name.c_str()));
  mgos_gpio_mode type = static_cast<mgos_gpio_mode>(_cfg->type);
  for (int i = 0; i < 8; ++i)
  {
    mgos_pcf857x_gpio_set_mode(_PCF, i, type);
    if (type == 1)
    {
      mgos_pcf857x_gpio_setup_input(_PCF, i,   MGOS_GPIO_PULL_NONE );
       mgos_pcf857x_gpio_write(_PCF, i, true);
      LOG(LL_INFO, ("Port # %d of %s configurated as input",i, _name.c_str()));
      
    }
    else if (type == 0)
    {
      mgos_pcf857x_gpio_setup_output(_PCF, i,false);
      mgos_pcf857x_gpio_write(_PCF, i, true);
      LOG(LL_INFO, ("Port # %d of %s configurated as output",i, _name.c_str()));
    
    }
  }
    return Status::OK();
}

void PCFComp::Update(/* args */)
{
}
Status PCFComp::deInit(/* args */)
{
  if (mgos_pcf857x_destroy(&_PCF))
    return Status::OK();
  LOG(LL_ERROR, ("Could not destroy PCF857X"));
  return Status::CANCELLED();
}

bool PCFComp::Read(int pin)
{
  return mgos_pcf857x_gpio_read(_PCF, pin);
}

void PCFComp::Write(int pin, bool state)
{
  mgos_pcf857x_gpio_write(_PCF, pin, state);
  LOG(LL_INFO, ("%s ports %d set %d", _name.c_str(), pin, state));
}
