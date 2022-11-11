#ifndef regulator_h
#define regulator_h
/*
Simple  regulator for physical process control



*/
#include "core/Component.hpp"
#include "core/Sensor.hpp"
#include "core/Output.hpp"
#include "core/Application.hpp"
#include "mgos_config.h"

enum regType
{
    UP,
    DOWN
}; // Positive or negative histeresis

class Regulator : public PollingComponent
{
protected:
    Sensor *_input{nullptr};
    BinaryOutput *_output{nullptr};
    const struct mgos_config_reg *_cfg{nullptr};
public:
    //Regulator(std::string name,int interval):PollingComponent(name,interval){}
    Regulator(const struct mgos_config_reg *cfg) : PollingComponent(std::string(cfg->name), cfg->inter),
                                             _cfg(cfg)
    {
    }
    Status Init() override;
    void Update() override;
    Status deInit() override;
    //std::string getInfo()override;
};


Status Regulator::Init()
{
    _output = App.getBinOutputByName(std::string(_cfg->output));
    _input = App.getSensorByName(std::string(_cfg->input));
    _timer.Reset(_interval, MGOS_TIMER_REPEAT);
    if (_timer.IsValid())
        return Status::OK();
    else
        return Status::CANCELLED();
}
Status Regulator::deInit(){return Status::OK();}




void Regulator::Update()
{
    if (_cfg->enable)
    {
        float curInput = this->_input->getState();
        float setpoint = this->_cfg->setpoint;
        float hist=this->_cfg->hist;
        if (_cfg->type == 0)
        {
            if (curInput < setpoint)
                _output->setState(true);
            else if (curInput >= setpoint + hist)
                _output->setState(false);
        }
        if (_cfg->type == 1)
        {
            if (curInput >= setpoint)
                _output->setState(true);
            else if (curInput < setpoint - hist)
                _output->setState(false);
        }
    }
    else
    {
        return;
    }
}

// std::string Regulator::getInfo()
// {
//     return mgos::JSONPrintStringf("{name:%s,type: \"enable: %b,}",_name.c_str(),_cfg->enable);
// }

typedef Regulator Termostat;
#endif