#pragma once
#include "common/util/status.h"

#include <string>

using namespace mgos;




class BinaryInput
{
public:
  BinaryInput(std::string name ,bool invert=false) : _name(name),_inverted(invert){}
  virtual bool getState() = 0;
  virtual std::string getInfo()=0;
  std::string getName(){return this->_name;}
  void setInvert(bool invert){_inverted=invert;}
protected:
  std::string _name{" "};
  bool _inverted{false};
};

class FloatInput : public BinaryInput
{
 public:
 FloatInput(const std::string name):BinaryInput(name){}
 bool getState() override;
 virtual float getLevel()=0;

 protected:
 float _min{0.0f};
 float _max{1.0f};

};

bool FloatInput::getState()
{
  if ((this->getState() == this->_min))
    return !this->_inverted;
  else
    return !this->_inverted;
}