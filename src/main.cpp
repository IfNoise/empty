/*
 * Copyright (c) 2014-2018 Cesanta Software Limited
 * All rights reserved
 *
 * Licensed under the Apache License, Version 2.0 (the ""License"");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an ""AS IS"" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "main.hpp"
#include "mgos.hpp"

#include "core/Application.hpp"
#include "components/DTSensor/DTSensor.hpp"
#include "components/BMESensor/BMESensor.hpp"
#include "components/InternalOut/InternalOut.hpp"
#include "components/Regulator/Regulator.hpp"
#include "components/DailyScheduler/IrrigationScheduler.hpp"
#include "components/DailyScheduler/LightTimer.hpp"
//#include <cstring>
#include <string>

Application App;
//=================================Components================================================
static DTComponent *dtComp{nullptr};
static BMEComponent *bmeComp{nullptr};
static Termostat *HeaterTermostat{nullptr};
static Termostat *FanTermostat{nullptr};
static Termostat *FanSpeedTermostat{nullptr};
static IrrigationScheduler *Irrigator{nullptr};
static LightTimer *LightTmr{nullptr};
//=================================Outputs====================================================
static InternalOut *Heater{nullptr};
static InternalOut *Fan{nullptr};
static InternalOut *FanSpeed{nullptr};
static InternalOut *Light{nullptr};
static InternalOut *Pump{nullptr};
//=======================================Timers===============================================
Timer *SensorPrintT{nullptr};


void InitApp(void)
{
  //=====================================Creating Components Objects==========================

  dtComp = new DTComponent((mgos_config_dtcomp*)mgos_sys_config_get_dtcomp());
  bmeComp = new BMEComponent(0x76);
  HeaterTermostat = new Termostat((mgos_config_reg*)mgos_sys_config_get_reg1());
  FanTermostat = new Termostat((mgos_config_reg*)mgos_sys_config_get_reg2());
  FanSpeedTermostat = new Termostat((mgos_config_reg*)mgos_sys_config_get_reg3());
  Irrigator = new IrrigationScheduler((mgos_config_irr*)mgos_sys_config_get_irr1());
  LightTmr = new LightTimer((mgos_config_light*)mgos_sys_config_get_light1());

  //====================================Creating Outputs Objects =============================

  Heater = new InternalOut((mgos_config_intout*)mgos_sys_config_get_intout1());
  Fan = new InternalOut((mgos_config_intout*)mgos_sys_config_get_intout2());
  FanSpeed = new InternalOut((mgos_config_intout*)mgos_sys_config_get_intout3());
  Light = new InternalOut((mgos_config_intout*)mgos_sys_config_get_intout4());
  Pump = new InternalOut((mgos_config_intout*)mgos_sys_config_get_intout5());

  //======================================Registration Components Objects=====================

  App.registerComponent(dtComp);
  App.registerComponent(bmeComp);
  //======================================Registration  Outputs Objects ======================

  App.registerBinOutput(Heater);
  App.registerBinOutput(Fan);
  App.registerBinOutput(FanSpeed);
  App.registerBinOutput(Light);
  App.registerBinOutput(Pump);


  
  App.registerComponent(HeaterTermostat);
  App.registerComponent(FanTermostat);
  App.registerComponent(FanSpeedTermostat);
  App.registerComponent(LightTmr);
  App.registerComponent(Irrigator);


  App.InitAll();

  //========================================Creating Timers===================================
  SensorPrintT = new Timer(5000, MGOS_TIMER_REPEAT,std::bind(&Application::publishAll,&App));
}
//============================================================================================
extern "C" enum mgos_app_init_result mgos_app_init(void)
{
  InitApp();
  return MGOS_APP_INIT_SUCCESS;
}