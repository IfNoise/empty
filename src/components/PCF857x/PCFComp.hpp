#include "core/Component.hpp"
#include "core/Output.hpp"
#include "mgos.hpp"
#include "mgos_config.h"
#include "mgos_pcf857x.h"
#include <vector>
#include "core/Application.hpp"

class PCFComp:public Component
{
private:
  mgos_pcf857x *_PCF{nullptr};
  mgos_config_pcfcomp *_cfg;
public:
  PCFComp( mgos_config_pcfcomp *cfg);
  Status Init()override;
  void Update()override;
  Status deInit()override;
};

PCFComp::PCFComp(mgos_config_pcfcomp *cfg):Component(std::string(cfg->name)),_cfg(cfg)
{
}

// PCFRelayComp::~PCFRelayComp( )
// {
// }

Status PCFComp::Init(/* args */)
{
  if (!(_PCF = mgos_pcf8574_create(mgos_i2c_get_global(),_cfg->address,
                                mgos_sys_config_get_pcf857x_int_gpio()))) {
    LOG(LL_ERROR, ("Could not create PCF857X"));
    return Status::CANCELLED();
  }
  for(int i=0;i<8;i++){

  }

}

void PCFComp::Update(/* args */)
{
}
Status PCFComp::deInit(/* args */)
{
  if(mgos_pcf857x_destroy(&_PCF))return Status::OK();
  LOG(LL_ERROR, ("Could not destroy PCF857X"));
  return Status::CANCELLED(); 
}


