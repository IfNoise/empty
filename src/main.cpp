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

#include "mgos.hpp"
#include "mgos_rpc.h"
#include "core/Application.hpp"
#include "components/PCF857x/PCFOutput.hpp"
// #include "components/Regulator/Regulator.hpp"
#include "components/DailyScheduler/IrrigationScheduler.hpp"
#include "components/DailyScheduler/LightTimer.hpp"
#include <string>

Application App;
//=================================Components================================================
static PCFComp *pcfComp{nullptr};
static IrrigationScheduler *Irrigator1{nullptr};
static IrrigationScheduler *Irrigator2{nullptr};
static IrrigationScheduler *Irrigator3{nullptr};
static IrrigationScheduler *Irrigator4{nullptr};
static IrrigationScheduler *Irrigator5{nullptr};
static IrrigationScheduler *Irrigator6{nullptr};
static LightTimer *LightTmr{nullptr};
static LightTimer *CO2Tmr{nullptr};
//=================================Outputs====================================================
static PCFOut *Light{nullptr};
static PCFOut *Pump1{nullptr};
static PCFOut *CO2{nullptr};
static PCFOut *Pump3{nullptr};
static PCFOut *Pump4{nullptr};
static PCFOut *Pump5{nullptr};
static PCFOut *Pump6{nullptr};
static PCFOut *Pump7{nullptr};
//=======================================Timers===============================================
Timer *SensorPrintT{nullptr};

//================================RPC service methods=========================================
static void getState(struct mg_rpc_request_info *ri, void *cb_arg,struct mg_rpc_frame_info *fi, struct mg_str args)
{
 mg_rpc_send_responsef(ri, "%s", App.printState().c_str());
  (void) cb_arg;
  (void) fi;
}

// static void getSensors(struct mg_rpc_request_info *ri, void *cb_arg,struct mg_rpc_frame_info *fi, struct mg_str args)
// {
 
//     mg_rpc_send_responsef(ri, "%s", App.printSensors().c_str());

//   (void) cb_arg;
//   (void) fi;
// }

static void getOutputs(struct mg_rpc_request_info *ri, void *cb_arg,struct mg_rpc_frame_info *fi, struct mg_str args)
{
 mg_rpc_send_responsef(ri, "%s", App.printOutputs().c_str());
  (void) cb_arg;
  (void) fi;
}
//============================================================================================
void InitApp(void)
{
  //=====================================Creating Components Objects==========================

  // dtComp = new DTComponent((mgos_config_dtcomp*)mgos_sys_config_get_dtcomp());
  pcfComp = new PCFComp((mgos_config_pcfcomp*)mgos_sys_config_get_pcfcomp1());
  // dtComp->Init();
  pcfComp->Init();
  //======================================Registration Components Objects=====================
  // App.registerDtComp(dtComp);
  App.registerPCFComp(pcfComp);
  


  //====================================Creating Outputs Objects =============================
  Light = new PCFOut((mgos_config_pcfout*)mgos_sys_config_get_pcfout1());
  Pump1 = new PCFOut((mgos_config_pcfout*)mgos_sys_config_get_pcfout2());
  CO2 = new PCFOut((mgos_config_pcfout*)mgos_sys_config_get_pcfout3());
  Pump3 = new PCFOut((mgos_config_pcfout*)mgos_sys_config_get_pcfout4());
  Pump4 = new PCFOut((mgos_config_pcfout*)mgos_sys_config_get_pcfout5());
  Pump5 = new PCFOut((mgos_config_pcfout*)mgos_sys_config_get_pcfout6());
  Pump6 = new PCFOut((mgos_config_pcfout*)mgos_sys_config_get_pcfout7());
  Pump7 = new PCFOut((mgos_config_pcfout*)mgos_sys_config_get_pcfout8());




  //======================================Registration  Outputs Objects ======================

  App.registerBinOutput(Light);
  App.registerBinOutput(Pump6);
  App.registerBinOutput(Pump7);
  App.registerBinOutput(Pump1);
  App.registerBinOutput(CO2);
  App.registerBinOutput(Pump3);
  App.registerBinOutput(Pump4);
  App.registerBinOutput(Pump5);

  Irrigator1 = new IrrigationScheduler((mgos_config_irr*)mgos_sys_config_get_irr1());
  Irrigator2 = new IrrigationScheduler((mgos_config_irr*)mgos_sys_config_get_irr2());
  Irrigator3 = new IrrigationScheduler((mgos_config_irr*)mgos_sys_config_get_irr3());
  Irrigator4 = new IrrigationScheduler((mgos_config_irr*)mgos_sys_config_get_irr4());
  Irrigator5 = new IrrigationScheduler((mgos_config_irr*)mgos_sys_config_get_irr5());
  Irrigator6 = new IrrigationScheduler((mgos_config_irr*)mgos_sys_config_get_irr6());
  CO2Tmr = new LightTimer((mgos_config_light*)mgos_sys_config_get_light2());
  LightTmr = new LightTimer((mgos_config_light*)mgos_sys_config_get_light1()); 

  App.registerComponent(LightTmr);
  App.registerComponent(Irrigator1);
  App.registerComponent(Irrigator2);
  App.registerComponent(Irrigator3);
  App.registerComponent(Irrigator4);
  App.registerComponent(Irrigator5);
  App.registerComponent(Irrigator6);
  App.registerComponent(CO2Tmr);

  App.InitAll();

//=================================Register RPC methods======================================
mg_rpc_add_handler(mgos_rpc_get_global(), "Get.State", NULL, getState, NULL);
// mg_rpc_add_handler(mgos_rpc_get_global(), "Get.Sensors", NULL, getSensors, NULL);
mg_rpc_add_handler(mgos_rpc_get_global(), "Get.Outputs", NULL, getOutputs, NULL); 


  //========================================Creating Timers===================================
  SensorPrintT = new Timer(5000, MGOS_TIMER_REPEAT,std::bind(&Application::publishAll,&App));
}
//============================================================================================
extern "C" enum mgos_app_init_result mgos_app_init(void)
{
  InitApp();
  return MGOS_APP_INIT_SUCCESS;
}