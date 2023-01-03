#pragma once
#include "common/util/status.h"
#include "mgos_timers.hpp"
#include <string>

using namespace mgos;

class Component
{

public:
  Component(std::string name):_name(name){};
  virtual Status Init()=0;
  virtual void Update()=0;
  virtual Status deInit()=0;
  //virtual std::string getInfo()=0;
  Component(const Component &other) = delete;
  std::string getName(){return _name;}
protected:
  const std::string _name{""};
};

class PollingComponent: public Component
{ 
  public:
  PollingComponent(std::string name,int interval);
  Status Init()override;
  Status deInit()override;
  void setInterval(uint32_t interval){_interval=interval;}
  void callback(){Update();}
  protected:
  uint32_t _interval{0};
  Timer _timer;
};

PollingComponent::PollingComponent(std::string name,int interval):
Component(name),
_interval(interval),
_timer(Timer(_interval,MGOS_TIMER_REPEAT,std::bind(&PollingComponent::callback,this))){}

Status PollingComponent::deInit()
{
   _timer.Clear(); 
   return Status::OK();
}
Status PollingComponent::Init()
{
_timer.Reset(_interval,MGOS_TIMER_REPEAT);
if(_timer.IsValid())return Status::OK();
else return Status::CANCELLED();
}
