#pragma once
#include "common/util/status.h"

#include <string>

using namespace mgos;

inline const char *OnOff(bool on) {
  return (on ? "on" : "off");
}


class BinaryOutput
{
public:
  BinaryOutput(std::string name ,bool invert=false) : _name(name),_inverted(invert){}
  virtual Status setState(bool state) = 0; 
  virtual bool getState() = 0;
  virtual std::string getInfo()=0;
  std::string getName(){return this->_name;}
  void setInvert(bool invert){_inverted=invert;}
protected:
  std::string _name{" "};
  bool _inverted{false};
};

class FloatOutput : public BinaryOutput
{
 public:
 FloatOutput(const std::string name):BinaryOutput(name){}
 Status setState(bool state) override;
 bool getState() override;
 virtual void setLevel(float state) = 0;
 virtual float getLevel()=0;

 protected:
 float _min{0.0f};
 float _max{1.0f};

};

Status FloatOutput::setState(bool state)
{
  if (state && (!this->_inverted))
  {
    return this->setState(this->_max);
  }
  else
  {
    return this->setState(this->_min);
  }
}
bool FloatOutput::getState()
{
  if ((this->getState() == this->_min))
    return !this->_inverted;
  else
    return !this->_inverted;
}