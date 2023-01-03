#include "core/Component.hpp"
#include "PCFComp.hpp"
#include "core/Output.hpp"
#include "mgos.hpp"
#include "mgos_config.h"
#include "mgos_pcf857x.h"
#include <vector>
#include "core/Application.hpp"

class PCFOut:public BinaryOutput
{
private:
  mgos_config_pcfout *_cfg;
public:
  PCFOut(mgos_config_pcfout *cfg);
  Status setState(bool state) override; 
  bool getState() override;
  std::string getInfo() override;
};

PCFOut::PCFOut(mgos_config_pcfout *cfg):BinaryOutput(std::string(cfg->name)),_cfg(cfg)
{
}

Status PCFOut::setState(bool state)
{
  return Status();
}

bool PCFOut::getState()
{
  return false;
}

std::string PCFOut::getInfo()
{
  return std::string();
}
