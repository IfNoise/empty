#pragma once
#include "adm4108rComp.hpp"
#include "core/Output.hpp"
#include "mgos.hpp"
#include "mgos_config.h"
#include <vector>
#include "core/Application.hpp"

class admOut : public BinaryOutput
{
private:

 adm4108rComp *_parent{nullptr};
 mgos_config_admout *_cfg;
public:
  admOut(mgos_config_admout *cfg);
  Status setState(bool state) override;
  bool getState() override;
  std::string getInfo() override;
};

admOut::admOut(mgos_config_admout *cfg): BinaryOutput(std::string(cfg->name)), _cfg(cfg)
{
  _parent = App.getADMCompByName(std::string(_cfg->prnt)); ///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  if (_parent != nullptr)
    LOG(LL_INFO, ("Parent set %s", _cfg->prnt));
  else
    LOG(LL_ERROR, ("Parent object not found or wrong"));
}

Status admOut::setState(bool state)
{
  if (_parent)
  {
    _parent->Write(_cfg->pin, state);
    return Status::OK();
  }
    LOG(LL_ERROR, ("Parent object not found or wrong"));
    return Status::CANCELLED();
}

bool admOut::getState()
{
if (_parent)
  {
    return _parent->Read(_cfg->pin) ;//^ _cfg->inverted;
  }
  else
  {
    LOG(LL_ERROR, ("Parent object not found or wrong,state not actual"));
    return false;
  }
}

std::string admOut::getInfo()
{
  return mgos::JSONPrintStringf("{name:%Q,type:%Q,pin: %d}", _name.c_str(), "ADM4108r", _cfg->pin);
}
