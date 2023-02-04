#pragma once
#include "core/Component.hpp"
#include "core/Output.hpp"
#include "mgos.hpp"
#include "mgos_config.h"
#include "mgos_modbus.h"
#include <vector>
#include "core/Application.hpp"


class adm4108rComp : public Component
{
private:
  static void respCallback(uint8_t status, struct mb_request_info info, struct mbuf response, void* param);
  mgos_config_admcomp *_cfg;
public:
  adm4108rComp(mgos_config_admcomp *_cfg{nullptr});
  Status Init() override;
  void Update() override;
  Status deInit() override;
  bool Read(int pin);
  void Write(int pin, bool state);
};

void adm4108rComp::respCallback(uint8_t status, mb_request_info info, mbuf response, void *param)
{
  char str[1024];
  int length = 0;
  for (int i = 0; i < response.len && i < sizeof(str) / 3; i++)
  {
    length += sprintf(str + length, "%.2x ", response.buf[i]);
  }
  if (status == RESP_SUCCESS)
  {
    LOG(LL_INFO, ("%f - VALID RESPONSE, Status: %d, Buffer: %.*s", mgos_uptime(), status, length, str));
  }
  else
  {
    LOG(LL_INFO, ("%f - Invalid response, Status: %d, Buffer: %.*s", mgos_uptime(), status, length, str));
  }
}

adm4108rComp::adm4108rComp(mgos_config_admcomp *_cfg) : Component(std::string(cfg->name)), _cfg(cfg)
{

}

Status adm4108rComp::Init()
{
  if(mgos_modbus_connect()){
  return Status::OK();
  }else return Status::CANCELLED();

}

void adm4108rComp::Update()
{
}

Status adm4108rComp::deInit()
{
  return Status::OK();
}

bool adm4108rComp::Read(int pin)
{
    if( pin<1&&pin>8){
    LOG(LL_ERROR, ("Pin number not valid"));
    return;
  }
  return mb_read_coils(_cfg->slave_id, (uint16_t)pin , uint16_t read_qty, std::bind(&adm4108rComp::respCallback,this), nullptr);
}

void adm4108rComp::Write(int pin, bool state)
{ 
  uint16_t data=0;
  if(state){
    data=1;
  }
  if( pin<1&&pin>8){
    LOG(LL_ERROR, ("Pin number not valid"));
    return;
  }
  mb_write_single_coil(_cfg->slave_id,  (uint16_t)pin , data,std::bind(&adm4108rComp::respCallback,this), nullptr);
}
