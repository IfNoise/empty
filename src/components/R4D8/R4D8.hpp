#pragma once

#include "core/Component.hpp"
#include "core/Output.hpp"
#include "mgos.hpp"
#include "mgos_config.h"
#include <vector>
#include "core/Application.hpp"

class R4D8Comp: public Component
{
private:
  /* data */
public:
  R4D8Comp(mgos_config_r4d8comp *cfg);
  ~R4D8Comp();
};


class R4D8Out: public BinaryOutput
{
private:
  mgos_config_r4d8out *_cfg;
public:
  R4D8Out(mgos_config_r4d8out *cfg);
  ~R4D8Out();
  Status setState(bool state) override; 
  bool getState() override;
};

R4D8Out::R4D8Out(mgos_config_r4d8out *cfg): 
    BinaryOutput(cfg->name,(bool)cfg->inverted),
    _cfg(cfg)
{
}

R4D8Out::~R4D8Out()
{
}

R4D8Comp::R4D8Comp(mgos_config_r4d8comp *cfg)
{
}

R4D8Comp::~R4D8Comp()
{
}

R4D8Out::R4D8Out(mgos_config_r4d8out *cfg): 
    BinaryOutput(cfg->name,(bool)cfg->inverted),
    _cfg(cfg)
{
}

R4D8Out::~R4D8Out()
{
  
}
Status R4D8Out::setState(bool state) 
{

} 

bool R4D8Out::getState() 
{

}