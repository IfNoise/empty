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

// Определение статических членов IrrigationScheduler
BinaryOutput* IrrigationScheduler::_sharedPump = nullptr;
BinaryOutput* IrrigationScheduler::_sharedReverseValve = nullptr;
bool IrrigationScheduler::_pumpInitialized = false;
int IrrigationScheduler::_activeIrrigators = 0;

Application App;
//=================================Components================================================
static PCFComp *pcfComp{nullptr};
static IrrigationScheduler *Irrigator1{nullptr};
static IrrigationScheduler *Irrigator2{nullptr};
static IrrigationScheduler *Irrigator3{nullptr};
static IrrigationScheduler *Irrigator4{nullptr};
static IrrigationScheduler *Irrigator5{nullptr};
static LightTimer *LightTmr{nullptr};
//=================================Outputs====================================================
static PCFOut *Light{nullptr};
static PCFOut *Valve1{nullptr};
static PCFOut *Valve2{nullptr};
static PCFOut *Valve3{nullptr};
static PCFOut *Valve4{nullptr};
static PCFOut *Valve5{nullptr};
static PCFOut *Valve6{nullptr};
static PCFOut *Pump{nullptr};
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
static void setIrrigationTable(struct  mg_rpc_request_info *ri, void *cb_arg,struct mg_rpc_frame_info *fi, struct mg_str args)
{
json_token irrigator_name = JSON_INVALID_TOKEN;
json_scanf(args.p, args.len, "{irrigator: %T}", &irrigator_name);


if (irrigator_name.len > 0) {
  std::string irrigator(irrigator_name.ptr, irrigator_name.len);
  json_token reg_map = JSON_INVALID_TOKEN;
  json_scanf(args.p, args.len, "{reg_map: %T}", &reg_map);

  if (reg_map.len > 0) {
    std::string reg_map_str(reg_map.ptr, reg_map.len);
    std::string file_path = irrigator + ".json";
    FILE *file = fopen(file_path.c_str(), "w");
    if (file != nullptr) {
      // Обернуть массив в объект { "items": ... }
      std::string wrapped = "{\"items\": " + reg_map_str + "}";
      fwrite(wrapped.c_str(), 1, wrapped.size(), file);
      fclose(file);
    } else {
      mg_rpc_send_errorf(ri, 500, "Failed to open file for writing");
      return;
    }
  } else {
    mg_rpc_send_errorf(ri, 400, "Invalid or missing 'reg_map' parameter");
    return;
  }
  mg_rpc_send_responsef(ri, "{irrigator: %Q, status: %Q}", irrigator.c_str(), "ok");
} else {
  mg_rpc_send_errorf(ri, 400, "Invalid or missing 'irrigator' parameter");
}

(void) cb_arg;
(void) fi;
}
//============================================================================================
static void getIrrigationTable(struct  mg_rpc_request_info *ri, void *cb_arg,struct mg_rpc_frame_info *fi, struct mg_str args)
{
json_token irrigator_name = JSON_INVALID_TOKEN;
json_scanf(args.p, args.len, "{irrigator: %T}", &irrigator_name);
if (irrigator_name.len > 0) {
  std::string irrigator(irrigator_name.ptr, irrigator_name.len);
  std::string file_path = irrigator + ".json";
  FILE *file = fopen(file_path.c_str(), "r");
  if (file != nullptr) {
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    std::string file_content(file_size, '\0');
    fread(&file_content[0], 1, file_size, file);
    fclose(file);
    mg_rpc_send_responsef(ri, "%s", file_content.c_str());
  } else {
    mg_rpc_send_errorf(ri, 500, "Failed to open file for reading");
  }
} else {
  mg_rpc_send_errorf(ri, 400, "Invalid or missing 'irrigator' parameter");
}

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
  Valve1 = new PCFOut((mgos_config_pcfout*)mgos_sys_config_get_pcfout2());
  Valve2 = new PCFOut((mgos_config_pcfout*)mgos_sys_config_get_pcfout3());
  Valve3 = new PCFOut((mgos_config_pcfout*)mgos_sys_config_get_pcfout4());
  Valve4 = new PCFOut((mgos_config_pcfout*)mgos_sys_config_get_pcfout5());
  Valve5 = new PCFOut((mgos_config_pcfout*)mgos_sys_config_get_pcfout6());
  Valve6 = new PCFOut((mgos_config_pcfout*)mgos_sys_config_get_pcfout7());
  Pump = new PCFOut((mgos_config_pcfout*)mgos_sys_config_get_pcfout8());




  //======================================Registration  Outputs Objects ======================

  App.registerBinOutput(Light);
  App.registerBinOutput(Valve1);
  App.registerBinOutput(Valve2);
  App.registerBinOutput(Valve3);
  App.registerBinOutput(Valve4);
  App.registerBinOutput(Valve5);
  App.registerBinOutput(Valve6);
  App.registerBinOutput(Pump);

  Irrigator1 = new IrrigationScheduler((mgos_config_irr*)mgos_sys_config_get_irr1());
  Irrigator2 = new IrrigationScheduler((mgos_config_irr*)mgos_sys_config_get_irr2());
  Irrigator3 = new IrrigationScheduler((mgos_config_irr*)mgos_sys_config_get_irr3());
  Irrigator4 = new IrrigationScheduler((mgos_config_irr*)mgos_sys_config_get_irr4());
  Irrigator5 = new IrrigationScheduler((mgos_config_irr*)mgos_sys_config_get_irr5());
  LightTmr = new LightTimer((mgos_config_light*)mgos_sys_config_get_light1()); 

  App.registerComponent(LightTmr);
  App.registerComponent(Irrigator1);
  App.registerComponent(Irrigator2);
  App.registerComponent(Irrigator3);
  App.registerComponent(Irrigator4);
  App.registerComponent(Irrigator5);

  App.InitAll();

//=================================Register RPC methods======================================
mg_rpc_add_handler(mgos_rpc_get_global(), "Get.State", NULL, getState, NULL);
// mg_rpc_add_handler(mgos_rpc_get_global(), "Get.Sensors", NULL, getSensors, NULL);
mg_rpc_add_handler(mgos_rpc_get_global(), "Get.Outputs", NULL, getOutputs, NULL); 
/**
 * Set.IrrigationTable
 * {
 * "irrigator_name": "Irrigator1",
 * "reg_map": "[{\"start\": 0,\"stop\": 100},{\"start\": 200,\"stop\": 300}]"
 * }
 */
mg_rpc_add_handler(mgos_rpc_get_global(), "Set.IrrigationTable", NULL, setIrrigationTable, NULL);
/**
 * Get.IrrigationTable
 * {
 * "irrigator_name": "Irrigator1"
 * }
 */

mg_rpc_add_handler(mgos_rpc_get_global(), "Get.IrrigationTable", NULL, getIrrigationTable, NULL);


  //========================================Creating Timers===================================
  SensorPrintT = new Timer(5000, MGOS_TIMER_REPEAT,std::bind(&Application::publishAll,&App));
}
//============================================================================================
extern "C" enum mgos_app_init_result mgos_app_init(void)
{
  InitApp();
  return MGOS_APP_INIT_SUCCESS;
}