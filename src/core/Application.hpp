#pragma once
#include "Component.hpp"
#include "Sensor.hpp"
#include "Output.hpp"
#include "mgos.hpp"
#include "mgos_mqtt.h"
#include <vector>
#include <memory>

class Application
{
private:
  std::vector<Component *> _components{};
  std::vector<Sensor *> _sensors{};
  std::vector<BinaryOutput *> _binOutputs{};
  std::vector<FloatOutput *> _floatOutputs{};

public:
  Status registerComponent(Component *component);
  Status registerSensor(Sensor *sensor);
  Status registerBinOutput(BinaryOutput *output);
  Status registerFloatOutput(FloatOutput *output);
  Sensor *getSensorByName(const std::string name);
  BinaryOutput *getBinOutputByName(const std::string name);
  FloatOutput *getFloatOutputByName(const std::string name);
  Component *getComponentByName(const std::string);
  Status InitAll();
  void UpdateAll();
  std::string printSensors();
  void saveSensorsToFile();
  void publishSensors();
  void publishBinOuts();
  void publishAll();
};

Status Application::registerComponent(Component *component)
{
  _components.push_back(component);
  return Status::OK();
}

Status Application::registerSensor(Sensor *sensor)
{
  _sensors.push_back(sensor);
  return Status::OK();
}

Status Application::registerBinOutput(BinaryOutput *output)
{
  _binOutputs.push_back(output);
  return Status::OK();
}
Status Application::registerFloatOutput(FloatOutput *output)
{
  _floatOutputs.push_back(output);
  return Status::OK();
}

Sensor *Application::getSensorByName(const std::string name)
{
  for (auto *ptr : this->_sensors)
  {
    if (ptr->getName() == name)
      return ptr;
  }
  return nullptr;
}

BinaryOutput *Application::getBinOutputByName(const std::string name)
{
  for (auto *ptr : this->_binOutputs)
  {
    if (ptr->getName() == name)
      return ptr;
  }
  return nullptr;
}

FloatOutput *Application::getFloatOutputByName(const std::string name)
{
  for (auto *ptr : this->_floatOutputs)
  {
    if (ptr->getName() == name)
      return ptr;
  }
  return nullptr;
}
Component *Application::getComponentByName(const std::string name)
{
  for (auto *ptr : this->_components)
  {
    if (ptr->getName() == name)
      return ptr;
  }
  return nullptr;
}

Status Application::InitAll()
{

  for (auto *comp : this->_components)
  {
    comp->Init();
  }
  // TODO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  return Status::OK();
}

void Application::UpdateAll()
{
  for (auto *comp : this->_components)
  {
    comp->Update();
  }
}

std::string Application::printSensors()
{
  std::string res;
  int i = 0;
  for (auto *sensor : this->_sensors)
  {
    if (i == 0)
    {
      mgos::JSONAppendStringf(&res, "{sensors: [%s", sensor->getInfo().c_str());
    }
    else
    {
      mgos::JSONAppendStringf(&res, ",%s", sensor->getInfo().c_str());
    }
    ++i;
  }
  res += "]}";
  LOG(LL_INFO, ("Sensors %s", res.c_str()));
  return res;
}

void Application::publishSensors()
{
  if (mgos_mqtt_global_is_connected())
  {
    for (auto *sensor : this->_sensors)
    {

      char buf[64];
      sprintf(buf, "%s/state/sensors/%s", mgos_sys_config_get_device_id(), sensor->getName().c_str());
      mgos_mqtt_pubf(buf, 0, false, "%0.2f", sensor->getState());
    }
    LOG(LL_INFO, ("Sensors is published"));
  }
  else
  {
    LOG(LL_INFO, ("Sensors is not published,MQTT brocker connection is lost"));
  }
}
void Application::publishBinOuts()
{
  if (mgos_mqtt_global_is_connected())
  {
    for (auto *output : this->_binOutputs)
    {

      char buf[64];
      sprintf(buf, "%s/state/outputs/%s", mgos_sys_config_get_device_id(), output->getName().c_str());
      mgos_mqtt_pubf(buf, 0, false, "%s", OnOff(output->getState()));
    }
    LOG(LL_INFO, ("Binary Outputs states is published"));
  }
  else
  {
    LOG(LL_INFO, ("Binary Output is not published,MQTT brocker connection is lost"));
  }
}

void Application::publishAll()
{
  publishSensors();
  publishBinOuts();
  saveSensorsToFile();
  //printSensors();
}
void Application::saveSensorsToFile()
{
 char templ[1024]="{\"sensors\":[{\"name\":\" \",\"state\":0}]}";
 char *temp=templ;
  //FILE *fp = fopen("sensors.json", "w");
  char buf[1024];
  char *p=buf;
  int i=0;
  struct json_out output = JSON_OUT_BUF(p,1024);
  for (auto *sensor : this->_sensors){
    char path[32];
    sprintf(path,".sensors[%d]",i);
    json_setf(temp,strlen(temp),&output,path,"%s",sensor->getInfo().c_str());
    strcpy(temp,buf);
    //json_prettify(buf,1024,&output);
    
    //LOG(LL_INFO, ("%s \n \n",temp.c_str())); 
    ++i;
  }
  LOG(LL_INFO, ("%s",p));
  //fclose(fp);
}

extern Application App;