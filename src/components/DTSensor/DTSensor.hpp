#include "core/Component.hpp"
#include "core/Sensor.hpp"
#include "mgos.hpp"
#include "mgos_config.h"
#include "DallasRmt.h"
#include <vector>
#include "core/Application.hpp"
//#include "frozen.h"

class DTSensor;

std::string format_hex(const uint8_t *data)
{
 char buf[17];
    char *p = buf;
    for (int i = 0; i < 8; ++i)
    {
      p += snprintf(p, sizeof(buf) - 2 * i, "%02x", data[i]);
    }
  return std::string(buf);  
}

class DTComponent : public PollingComponent
{
public:
  DTComponent(uint8_t pin);
  Status Init() override;
  void Update() override;
  // std::string getInfo()override;
private:
  int _pin{0};
  uint8_t _numSensors{0};
  DallasRmt *_controller{nullptr};
  std::vector<DTSensor *> _sensors{};
};

class DTSensor : public Sensor
{
protected:
  friend DTComponent;
  uint8_t _address[8];

public:
  DTSensor(std::string name,  uint8_t *address);
  std::string getInfo()const override ;
 
};

DTSensor::DTSensor(std::string name,  uint8_t *address)
    : Sensor(name)
{
  for (int i = 0; i < 8; i++)
  {
    _address[i]= *address;
    ++address;
  }
}
std::string DTSensor::getInfo() const
{ 
  std::string res = mgos::JSONPrintStringf("{name: %Q,address: %Q,state: %.2f}", _name.c_str(), format_hex(_address).c_str(), _state);
  return res;
}
DTComponent::DTComponent(uint8_t pin)
    : PollingComponent("DTComponent", 10000),_pin(pin)
      
{
  
  LOG(LL_INFO, ("Dallas Temperature Component is ceated"));
}

Status DTComponent::Init()
{
  
  _controller = new DallasRmt(_pin,0,1);
  _controller->setResolution(9);
  _controller->begin();
  _numSensors = _controller->getDeviceCount();
  LOG(LL_INFO, ("Find %d DS18b20 sensors", _numSensors));
  if (_numSensors > 0)
  {
    for (int i = 0; i < _numSensors; ++i)
    {
      uint8_t address[8]{};
      if (_controller->getAddress(address, i))
      {
        char buf[13];
        sprintf(buf, "DTemp%d", i);
        DTSensor *add = new DTSensor(std::string(buf), address);
        _sensors.push_back(add);
        App.registerSensor(add);
        LOG(LL_INFO, ("DS18b20 sensor object is created"));
      }
    }
  }

  return mgos::Status::OK();
}

void DTComponent::Update()
{
  if (_numSensors > 0)
  {
    _controller->requestTemperatures();
    for (auto *sensor : this->_sensors)
    {
      sensor->_state = _controller->getTempC(sensor->_address);
    }
  }
}
