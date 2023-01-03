#pragma once
#include "Component.hpp"
#include "Sensor.hpp"
#include "Input.hpp"
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
  std::vector<BinaryInput *> _binInputs{};
  std::vector<FloatInput *> _floatInputs{};

public:

  //Registring Objects in application
  Status registerComponent(Component *component);
  Status registerSensor(Sensor *sensor);
  Status registerBinOutput(BinaryOutput *output);
  Status registerFloatOutput(FloatOutput *output);
  Status registerBinInput(BinaryInput *output);
  Status registerFloatInput(FloatInput *output);

  // Getting pointer to Object 
  Sensor *getSensorByName(const std::string name);
  BinaryOutput *getBinOutputByName(const std::string name);
  FloatOutput *getFloatOutputByName(const std::string name);
  BinaryInput *getBinInputByName(const std::string name);
  FloatInput *getFloatInputByName(const std::string name);
  Component *getComponentByName(const std::string);

  //========================================
  Status InitAll();
  void UpdateAll();
  std::string printSensors();
  std::string printOutputs();
  std::string printState();
  void saveSensorsToFile();
  void publishSensors();
  void publishState();
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

Status Application::registerBinInput(BinaryInput *input)
{
  _binInputs.push_back(input);
  return Status::OK();
}

Status Application::registerFloatInput(FloatInput *input)
{
  _floatInputs.push_back(input);
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

BinaryInput *Application::getBinInputByName(const std::string name)
{
  for (auto *ptr : this->_binInputs)
  {
    if (ptr->getName() == name)
      return ptr;
  }
  return nullptr;
}

FloatInput *Application::getFloatInputByName(const std::string name)
{
  for (auto *ptr : this->_floatInputs)
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
  std::string res = "";
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
std::string Application::printOutputs()
{
  std::string res = "";
  int i = 0;
  for (auto *output : this->_binOutputs)
  {
    if (i == 0)
    {
      mgos::JSONAppendStringf(&res, "{outputs: [%s", output->getInfo().c_str());
    }
    else
    {
      mgos::JSONAppendStringf(&res, ",%s", output->getInfo().c_str());
    }
    ++i;
  }
  res += "]}";

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
      mgos_msleep(100);
    }
    LOG(LL_INFO, ("Sensors is published"));
  }
  else
  {
    LOG(LL_INFO, ("Sensors is not published,MQTT brocker connection is lost"));
  }
}
void Application::publishState()
{
  if (mgos_mqtt_global_is_connected())
  {
    char buf[64];
    sprintf(buf, "%s/state/", mgos_sys_config_get_device_id());
    mgos_mqtt_pubf(buf, 0, false, "%s", this->printState().c_str());
    mgos_msleep(100);
    LOG(LL_INFO, ("State is published"));
  }
  else
  {
    LOG(LL_INFO, ("State is not published,MQTT brocker connection is lost"));
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
      mgos_mqtt_pubf(buf, 0, false, "%d", int(output->getState()));
      mgos_msleep(100);
    }
    LOG(LL_INFO, ("Binary Outputs states is published"));
  }
  else
  {
    LOG(LL_INFO, ("Binary Output is not published,MQTT brocker connection is lost"));
  }
}
std::string Application::printState()
{
  std::string res;
  int i = 0;
  time_t now = 3600 * 3 + time(0);
    tm *tm_info = gmtime(&now);
  for (auto *sensor : this->_sensors)
  {
    if (i == 0)
    {
      mgos::JSONAppendStringf(&res, "{sensors: [{name:%Q,state:%.2f}", sensor->getName().c_str(), sensor->getState());
    }
    else
    {
      mgos::JSONAppendStringf(&res, ",{name:%Q,state:%.2f}", sensor->getName().c_str(), sensor->getState());
    }
    ++i;
  }
  res += "]";
  i = 0;
  for (auto *output : this->_binOutputs)
  {
    if (i == 0)
    {
      mgos::JSONAppendStringf(&res, ",outputs: [{name:%Q,state:%B}", output->getName().c_str(), output->getState());
    }
    else
    {
      mgos::JSONAppendStringf(&res, ",{name:%Q,state:%B}", output->getName().c_str(), output->getState());
    }
    ++i;
  }
  res += "],";
  mgos::JSONAppendStringf(&res,"timestamp: %d",(unsigned int)now);
  res+="}";
  return res;
}

void Application::publishAll()
{
  publishSensors();
  publishBinOuts();
  // publishState();
  // saveSensorsToFile();
  // printSensors();
}
void Application::saveSensorsToFile()
{
  char templ[1024] = "{\"sensors\":[{\"name\":\" \",\"state\":0}]}";
  char *temp = templ;
  // FILE *fp = fopen("sensors.json", "w");
  char buf[1024];
  char *p = buf;
  int i = 0;
  struct json_out output = JSON_OUT_BUF(p, 1024);
  for (auto *sensor : this->_sensors)
  {
    char path[32];
    sprintf(path, ".sensors[%d]", i);
    json_setf(temp, strlen(temp), &output, path, "%s", sensor->getInfo().c_str());
    strncpy(temp, buf, strlen(buf));
    // json_prettify(buf,1024,&output);

    // LOG(LL_INFO, ("%s \n \n",temp.c_str()));
    ++i;
  }
  LOG(LL_INFO, ("%s", p));
  // fclose(fp);
}

extern Application App;