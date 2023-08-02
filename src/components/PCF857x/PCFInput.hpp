#pragma once
#include "core/Component.hpp"
#include "PCFComp.hpp"
#include "core/Input.hpp"
#include "mgos.hpp"
#include "mgos_config.h"
#include "mgos_pcf857x.h"
#include <vector>
#include "core/Application.hpp"

class PCFInput:public BinaryInput
{
private:
  mgos_config_pcfin *_cfg;
public:
  PCFInput(mgos_config_pcfin *cfg); 
  bool getState() override;
  std::string getInfo() override;
};

PCFInput::PCFInput(mgos_config_pcfin *cfg):BinaryInput(std::string(cfg->name)),_cfg(cfg)
{
}


bool PCFInput::getState()
{
  return false;
}

std::string PCFInput::getInfo()
{
  return std::string();
}
