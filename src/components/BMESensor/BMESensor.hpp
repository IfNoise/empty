#include "core/Component.hpp"
#include "core/Application.hpp"
#include "core/Sensor.hpp"
#include "BME280.h"



class BMEComponent : public PollingComponent
{
private:
Sensor* _temperature{nullptr};
Sensor* _humidity{nullptr};
Sensor* _pressure{nullptr};
uint8_t _address{0};
BME280* _controller{nullptr};

public:
  BMEComponent (uint8_t addres);
  Status Init() override;
  void Update() override;
  
};

BMEComponent ::BMEComponent (uint8_t addres):PollingComponent("BME280Component",1000)
{
  _controller=new BME280(addres);
  _temperature=new Sensor("BME280_temperature");
  _humidity=new Sensor("BME280_humidity");
  _pressure= new Sensor("BME280_pressure");
  LOG(LL_INFO, ("BME280 Component is ceated"));
}

Status BMEComponent ::Init()
{
  App.registerSensor(_temperature);
  App.registerSensor(_humidity);
  App.registerSensor(_pressure);
  LOG(LL_INFO, ("BME280 sensors is registred"));
  return Status::OK();
}
void BMEComponent ::Update()
{
  _temperature->setState(float(_controller->readTemperature()));
  _humidity->setState(float(_controller->readHumidity()));
  _pressure->setState(float(_controller->readPressure()));
}
