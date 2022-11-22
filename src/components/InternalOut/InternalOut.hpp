#include "./core/Output.hpp"
#include "mgos_gpio.h"
#include "mgos.hpp"
#include "mgos_config.h"


class InternalOut : public BinaryOutput
{
private:
  mgos_config_intout *_cfg{nullptr};

public:
  InternalOut(struct mgos_config_intout *cfg);
  Status setState(bool state) override; 
  bool getState() override;
  std::string getInfo() override;
};

InternalOut::InternalOut(struct mgos_config_intout *cfg)
: BinaryOutput(std::string(cfg->name),bool(cfg->inverted)),_cfg(cfg)
{ 
  mgos_gpio_set_mode(_cfg->pin, MGOS_GPIO_MODE_OUTPUT);
  LOG(LL_INFO, ("OutputPin %s: pin %d", _name.c_str(),_cfg->pin));
}

std::string InternalOut::getInfo()
{
   return mgos::JSONPrintStringf("{name:%Q,type:%Q,pin: %d}",_name.c_str(),"internal",_cfg->pin);
}

Status InternalOut::setState( bool state)
{
  bool cur_state = getState();
if (state == cur_state) return Status::OK();
  mgos_gpio_write(_cfg->pin, (state^(_cfg->inverted)));
  LOG(LL_INFO,
      ("Output %s: %s -> %s ", _name.c_str(), OnOff(cur_state), OnOff(state)));
  return Status::OK();
}

bool InternalOut::getState()
{
  return mgos_gpio_read_out(_cfg->pin) ^ (_cfg->inverted);
}