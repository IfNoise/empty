#pragma once
#include "mgos.hpp"
#include <string>

using namespace mgos;

class Sensor
{

 public:
  Sensor(const std::string name):_name(name){}
  Sensor(const Sensor &other) = delete;
  ~Sensor();
  float getState(){return this->_state;}
  std::string getName(){return this->_name;}
  Status setState(float state);
  virtual std::string getInfo() const;
 protected:
  std::string _name{" "};
  float _state{0.0};
};

Status Sensor::setState(float state)
{
  _state=state;
  return Status::OK();
}

std::string Sensor::getInfo() const
{
  std::string res=mgos::JSONPrintStringf("{name:%Q,state: %.2f}",_name.c_str(),_state);
  return res;
}