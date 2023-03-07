#pragma once
#include "core/Component.hpp"
#include "core/Output.hpp"
#include "mgos.hpp"
#include "mgos_config.h"
#include "mgos_modbus.h"
#include <vector>
#include "core/Application.hpp"


class adm4108rComp : public PollingComponent
{
private:
  uint16_t _state[8]={0,0,0,0,0,0,0,0};
  mgos_config_admcomp *_cfg;
public:
  static void respReadCallback(uint8_t status, struct mb_request_info info, struct mbuf response, void* param);
  static void respWriteCallback(uint8_t status, struct mb_request_info info, struct mbuf response, void* param);
  adm4108rComp(mgos_config_admcomp *cfg);
  Status Init() override;
  void Update() override;
  Status deInit() override;
  bool Read(int pin);
  void Write(int pin, bool state);
};

void adm4108rComp::respReadCallback(uint8_t status, mb_request_info info, mbuf response, void *param)
{
  auto *st = static_cast<adm4108rComp *>(param);
  char str[1024];
  int length = 0;
  for (int i = 0; i < response.len && i < sizeof(str) / 3; i++)
  {
    length += sprintf(str + length, "%.2x ", response.buf[i]);
  }
  if (status == RESP_SUCCESS)
  {
    LOG(LL_INFO, ("%f - VALID RESPONSE, Status: %d, id :%d,addres : %d,qty :%d, Data : %s", mgos_uptime(), status,info.slave_id,info.read_address,info.read_qty,str));
    st->_state[info.read_address]=response.buf[6]<<8 | (response.buf[7]&0xFF);
  }
  else
  {
    LOG(LL_INFO, ("%f - Invalid response, Status: %d, id :%d,addres : %d,qty :%d", mgos_uptime(), status,info.slave_id,info.read_address,info.read_qty));
  }
}

void adm4108rComp::respWriteCallback(uint8_t status, mb_request_info info, mbuf response, void *param)
{
  auto *st = static_cast<adm4108rComp *>(param);
  char str[1024];
  int length = 0;
  for (int i = 0; i < response.len && i < sizeof(str) / 3; i++)
  {
    length += sprintf(str + length, "%.2x ", response.buf[i]);
  }
  if (status == RESP_SUCCESS)
  {
    LOG(LL_INFO, ("%f - VALID RESPONSE, Status: %d, id :%d,addres : %d,qty :%d ,data: %s", mgos_uptime(), status,info.slave_id,info.write_address,info.write_qty,str));
  }
  else
  {
    LOG(LL_INFO, ("%f - Invalid response, Status: %d, id :%d,addres : %d,qty :%d", mgos_uptime(), status,info.slave_id,info.write_address,info.write_qty));
  }
}

adm4108rComp::adm4108rComp(mgos_config_admcomp *cfg) : PollingComponent(cfg->name, cfg->inter), _cfg(cfg)
{

}

Status adm4108rComp::Init()
{
  if(mgos_modbus_connect()){
    _timer = new Timer(_interval, MGOS_TIMER_REPEAT, std::bind(&PollingComponent::callback, this));
    _timer->Reset(_interval, MGOS_TIMER_REPEAT);
    if (_timer->IsValid())
        return Status::OK();
    else
        return Status::CANCELLED();
  }else return Status::CANCELLED();

}

void adm4108rComp::Update()
{
  uint8_t id =_cfg->slave_id;
  uint16_t adr=1;
  uint16_t qty=1;
  LOG(LL_INFO, ("ADM Comp Updating"));
  mb_read_coils(id,adr,qty, &adm4108rComp::respReadCallback,this);
}

Status adm4108rComp::deInit()
{
  return Status::OK();
}

bool adm4108rComp::Read(int pin)
{
    if( pin<1&&pin>8){
    LOG(LL_ERROR, ("Pin number not valid"));
    return false;
  }
  return _state[pin-1];//TODO
}

void adm4108rComp::Write(int pin, bool state)
{ 
  uint16_t data=0;
  uint8_t id=_cfg->slave_id;
  if(state){
    data=1;
  }
  if( pin<1&&pin>8){
    LOG(LL_ERROR, ("Pin number not valid"));
    return;
  }
  mb_write_single_coil(id,(uint8_t)pin ,data, &adm4108rComp::respWriteCallback, this);
  
}
