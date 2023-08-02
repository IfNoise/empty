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
static PCFComp *pcfComp1{nullptr};
static PCFComp *pcfComp2{nullptr};
static IrrigationScheduler *Irrigator1{nullptr};
static IrrigationScheduler *Irrigator2{nullptr};
static IrrigationScheduler *Irrigator3{nullptr};
static IrrigationScheduler *Irrigator4{nullptr};
static IrrigationScheduler *Irrigator5{nullptr};
static IrrigationScheduler *Irrigator6{nullptr};
static IrrigationScheduler *Irrigator7{nullptr};
static IrrigationScheduler *Irrigator8{nullptr};
static IrrigationScheduler *Irrigator9{nullptr};
static IrrigationScheduler *Irrigator10{nullptr};
static IrrigationScheduler *Irrigator11{nullptr};
static IrrigationScheduler *Irrigator12{nullptr};
static IrrigationScheduler *Irrigator13{nullptr};
static IrrigationScheduler *Irrigator14{nullptr};
static IrrigationScheduler *Irrigator15{nullptr};
static IrrigationScheduler *Irrigator16{nullptr};
//=================================Outputs====================================================
static PCFOut *Valve1{nullptr};
static PCFOut *Valve2{nullptr};
static PCFOut *Valve3{nullptr};
static PCFOut *Valve4{nullptr};
static PCFOut *Valve5{nullptr};
static PCFOut *Valve6{nullptr};
static PCFOut *Valve7{nullptr};
static PCFOut *Valve8{nullptr};
static PCFOut *Valve9{nullptr};
static PCFOut *Valve10{nullptr};
static PCFOut *Valve11{nullptr};
static PCFOut *Valve12{nullptr};
static PCFOut *Valve13{nullptr};
static PCFOut *Valve14{nullptr};
static PCFOut *Valve15{nullptr};
static PCFOut *Valve16{nullptr};
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
  pcfComp1 = new PCFComp((mgos_config_pcfcomp*)mgos_sys_config_get_pcfcomp1());
  pcfComp2 = new PCFComp((mgos_config_pcfcomp*)mgos_sys_config_get_pcfcomp2());
  // dtComp->Init();
  pcfComp1->Init();
  pcfComp2->Init();
  //======================================Registration Components Objects=====================
  // App.registerDtComp(dtComp);
  App.registerPCFComp(pcfComp1);
  App.registerPCFComp(pcfComp2);
  


  //====================================Creating Outputs Objects =============================
  Valve1 = new PCFOut((mgos_config_pcfout*)mgos_sys_config_get_pcfout1());
  Valve2 = new PCFOut((mgos_config_pcfout*)mgos_sys_config_get_pcfout2());
  Valve3 = new PCFOut((mgos_config_pcfout*)mgos_sys_config_get_pcfout3());
  Valve4 = new PCFOut((mgos_config_pcfout*)mgos_sys_config_get_pcfout4());
  Valve5 = new PCFOut((mgos_config_pcfout*)mgos_sys_config_get_pcfout5());
  Valve6 = new PCFOut((mgos_config_pcfout*)mgos_sys_config_get_pcfout6());
  Valve7 = new PCFOut((mgos_config_pcfout*)mgos_sys_config_get_pcfout7());
  Valve8 = new PCFOut((mgos_config_pcfout*)mgos_sys_config_get_pcfout8());
  Valve9 = new PCFOut((mgos_config_pcfout*)mgos_sys_config_get_pcfout9());
  Valve10 = new PCFOut((mgos_config_pcfout*)mgos_sys_config_get_pcfout10());
  Valve11 = new PCFOut((mgos_config_pcfout*)mgos_sys_config_get_pcfout11());
  Valve12 = new PCFOut((mgos_config_pcfout*)mgos_sys_config_get_pcfout12());
  Valve13 = new PCFOut((mgos_config_pcfout*)mgos_sys_config_get_pcfout13());
  Valve14 = new PCFOut((mgos_config_pcfout*)mgos_sys_config_get_pcfout14());
  Valve15 = new PCFOut((mgos_config_pcfout*)mgos_sys_config_get_pcfout15());
  Valve16 = new PCFOut((mgos_config_pcfout*)mgos_sys_config_get_pcfout16());



  //======================================Registration  Outputs Objects ======================

  App.registerBinOutput(Valve1);
  App.registerBinOutput(Valve2);
  App.registerBinOutput(Valve3);
  App.registerBinOutput(Valve4);
  App.registerBinOutput(Valve5);
  App.registerBinOutput(Valve6);
  App.registerBinOutput(Valve7);
  App.registerBinOutput(Valve8);
  App.registerBinOutput(Valve9);
  App.registerBinOutput(Valve10);
  App.registerBinOutput(Valve11);
  App.registerBinOutput(Valve12);
  App.registerBinOutput(Valve13);
  App.registerBinOutput(Valve14);
  App.registerBinOutput(Valve15);
  App.registerBinOutput(Valve16);

  Irrigator1 = new IrrigationScheduler((mgos_config_irr*)mgos_sys_config_get_irr1());
  Irrigator2 = new IrrigationScheduler((mgos_config_irr*)mgos_sys_config_get_irr2());
  Irrigator3 = new IrrigationScheduler((mgos_config_irr*)mgos_sys_config_get_irr3());
  Irrigator4 = new IrrigationScheduler((mgos_config_irr*)mgos_sys_config_get_irr4());
  Irrigator5 = new IrrigationScheduler((mgos_config_irr*)mgos_sys_config_get_irr5());
  Irrigator6 = new IrrigationScheduler((mgos_config_irr*)mgos_sys_config_get_irr6());
  Irrigator7 = new IrrigationScheduler((mgos_config_irr*)mgos_sys_config_get_irr7());
  Irrigator8 = new IrrigationScheduler((mgos_config_irr*)mgos_sys_config_get_irr8());
  Irrigator9 = new IrrigationScheduler((mgos_config_irr*)mgos_sys_config_get_irr9());
  Irrigator10 = new IrrigationScheduler((mgos_config_irr*)mgos_sys_config_get_irr10());
  Irrigator11 = new IrrigationScheduler((mgos_config_irr*)mgos_sys_config_get_irr11());
  Irrigator12 = new IrrigationScheduler((mgos_config_irr*)mgos_sys_config_get_irr12());
  Irrigator13 = new IrrigationScheduler((mgos_config_irr*)mgos_sys_config_get_irr13());
  Irrigator14 = new IrrigationScheduler((mgos_config_irr*)mgos_sys_config_get_irr14());
  Irrigator15 = new IrrigationScheduler((mgos_config_irr*)mgos_sys_config_get_irr15());
  Irrigator16 = new IrrigationScheduler((mgos_config_irr*)mgos_sys_config_get_irr16());

  App.registerComponent(Irrigator1);
  App.registerComponent(Irrigator2);
  App.registerComponent(Irrigator3);
  App.registerComponent(Irrigator4);
  App.registerComponent(Irrigator5);
  App.registerComponent(Irrigator6);
  App.registerComponent(Irrigator7);
  App.registerComponent(Irrigator8);
  App.registerComponent(Irrigator9);
  App.registerComponent(Irrigator10);
  App.registerComponent(Irrigator11);
  App.registerComponent(Irrigator12);
  App.registerComponent(Irrigator13);
  App.registerComponent(Irrigator14);
  App.registerComponent(Irrigator15);
  App.registerComponent(Irrigator16);


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