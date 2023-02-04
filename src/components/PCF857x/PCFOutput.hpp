#pragma once
#include "PCFComp.hpp"
#include "core/Output.hpp"
#include "mgos.hpp"
#include "mgos_config.h"
#include <vector>
#include "core/Application.hpp"

class PCFOut : public BinaryOutput
{
private:
  mgos_config_pcfout *_cfg;
  PCFComp *_parent{nullptr};

public:
  PCFOut(mgos_config_pcfout *cfg);
  Status setState(bool state) override;
  bool getState() override;
  std::string getInfo() override;
};

PCFOut::PCFOut(mgos_config_pcfout *cfg) : BinaryOutput(std::string(cfg->name)), _cfg(cfg)
{
  _parent = App.getPCFCompByName(std::string(_cfg->prnt)); ///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  if (_parent != nullptr)
    LOG(LL_INFO, ("Parent set %s", _cfg->prnt));
  else
    LOG(LL_ERROR, ("Parent object not found or wrong"));
}

Status PCFOut::setState(bool state)
{
  if (_parent)
  {
    _parent->Write(_cfg->pin, (state ^ (_cfg->inverted)));
    return Status::OK();
  }
    LOG(LL_ERROR, ("Parent object not found or wrong"));
    return Status::CANCELLED();

}

bool PCFOut::getState()
{
  if (_parent)
  {
    return _parent->Read(_cfg->pin) ^ _cfg->inverted;
  }
  else
  {
    LOG(LL_ERROR, ("Parent object not found or wrong,state not actual"));
    return false;
  }
}

std::string PCFOut::getInfo()
{
  return mgos::JSONPrintStringf("{name:%Q,type:%Q,pin: %d}", _name.c_str(), "PCF", _cfg->pin);
}
