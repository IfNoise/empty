#include "mgos.hpp"
#include "main.hpp"
#include "mgos_timers.h"
#include "math.h"
#include "mgos_mqtt.h"
#include "frozen.h"
#include <string>

struct Sensor
{
  u_int8_t addres[8];
  std::string name;
  int16_t id;
};
static struct Sensor sensors[3]={
  {.addres={23,34,143,234,34,56,45,35},
   .name="Sensor1",
   .id=1
  },
  {.addres={23,34,143,234,34,56,45,35},
   .name="Sensor2",
   .id=2
  },
  {.addres={23,34,143,234,34,56,45,35},
   .name="Sensor3",
   .id=3
  }
};
//static uint8_t Ounputs[NUM_OUTPUTS] = {Off,On,Off,On,Off,Off,Off,On};
static float Temp[8]={28.3,22.0,23.0,24.0,26.0,21.0,20.0,30.0};
static char format_hex_char(uint8_t v) { return v >= 10 ? 'a' + (v - 10) : '0' + v; }
static std::string format_hex(const uint8_t *data, size_t length) {
  std::string ret;
  ret.resize(length * 2);
  for (size_t i = 0; i < length; i++) {
    ret[2 * i] = format_hex_char((data[i] & 0xF0) >> 4);
    ret[2 * i + 1] = format_hex_char(data[i] & 0x0F);
  }
  return ret;
}
// static void print_buffer(struct mbuf buffer, uint8_t status)
// {
//   char str[1024];
//   int length = 0;
//   for (int i = 0; i < buffer.len && i < sizeof(str) / 3; i++)
//   {
//     length += sprintf(str + length, "%.2x ", buffer.buf[i]);
//   }
//   if (status == RESP_SUCCESS)
//   {
//     LOG(LL_INFO, ("%f - VALID RESPONSE, Status: %d, Buffer: %.*s", mgos_uptime(), status, length, str));
//   }
//   else
//   {
//     LOG(LL_INFO, ("%f - Invalid response, Status: %d, Buffer: %.*s", mgos_uptime(), status, length, str));
//   }
// }
void BlynkCB(struct mg_connection *c, const char *cmd, int pin, int val, int id, void *user_data)
{
  LOG(LL_INFO, ("Blynk Event"));
  if (strcmp(cmd, "vw") == 0)
  {
    if (pin == 1)
    {
      r = val;
    }
    else if (pin == 2)
    {
      g = val;
    }
    else if (pin == 3)
    {
      b = val;
    }
  }
  mgos_ili9341_set_fgcolor(r, g, b);
  mgos_ili9341_drawCircle(160, 120, 80);
  mgos_ili9341_fillCircle(160, 120, 50);
  (void)user_data;
}
static uint16_t HexToDec(const char h, const char l)
{
  union
  {
    uint8_t bytes[2];
    uint16_t reg;
  } u;
  u.bytes[0] = l;
  u.bytes[1] = h;
  return u.reg;
}
static void PrintTemp()
{

  mgos_ili9341_set_fgcolor(0, 0, 0); // Red

  // mgos_ili9341_set_window(0, 0, 320, 240);      // 100x30 pixels
  mgos_ili9341_fillScreen();
  mgos_ili9341_set_fgcolor(0, 0xff, 0); // Green
  mgos_ili9341_set_font(&Picopixel);

  for (int i = 0; i < 8; i++)
  {
    float temp = Temp[i];
    int line = mgos_ili9341_line(i);

    if (temp < 150 && temp > -85)
    {
      mgos_ili9341_printf(10, line, "Sensor %d temperature %2.2f C", i, temp); /* code */
    }
    else if ((int)temp == -5553)
    {
      mgos_ili9341_printf(10, line, "Sensor %d disconnected", i);
    }
    else
    {
      mgos_ili9341_printf(10, line, "Sensor %d wrong temperature", i);
    }
  }
}
static void PubTempt(void *param)
{
  float *temp_t = (float *)param;
  if (mgos_mqtt_global_is_connected())
  {
    mgos_mqtt_pubf("sensors/temp", 0, false, "{temp:[{val:%2.2f},{val:%2.2f},{val:%2.2f},{val:%2.2f},{val:%2.2f},{val:%2.2f},{val:%2.2f},{val:%2.2f}]}",
                   temp_t[0], temp_t[1], temp_t[2], temp_t[3], temp_t[4], temp_t[5], temp_t[6], temp_t[7]);
    LOG(LL_INFO, ("Sensors is published"));
  }
}
static void printSensors(const struct Sensor* sens){
  std::string temp="{sensors:{num:3,items:[]}}";
  FILE *fp = fopen("sensors.json", "w");
  struct json_out output = JSON_OUT_FILE(fp);
  for (size_t i = 0; i < 3; i++)
  {
    char* item=NULL;
    struct json_out out = JSON_OUT_BUF(item, 64);
    json_printf(&out,"{address: %s,name: %s,id: %d}",format_hex(sensors[i].addres,8).c_str(),sensors[i].name.c_str(),sensors[i].id);
    json_setf(temp.c_str(),temp.length(),&output,"sensors.item[%s]",item);
  }
  fclose(fp);
}

void mb_read_response_handler(uint8_t status, struct mb_request_info mb_ri, struct mbuf response, void *param)
{
  uint16_t codes[8];
  if (response.len == 21)
  {
    if (int(response.buf[0]) == 0x01)
    {
      if (int(response.buf[1]) == 0x03)
      {
        if (int(response.buf[2]) == 0x10)
        {
          int j = 0;
          for (int i = 0; i < 8; i++)
          {
            codes[i] = HexToDec(response.buf[3 + j], response.buf[4 + j]);
            j += 2;
          }
          for (int i = 0; i < 8; i++)
          {
            if ((codes[i] < 10000) && (codes[i] < 65535))
            {
              Temp[i] = (static_cast<float>(codes[i])) * 0.1;
            }
            else if (codes[i] > 10000)
            {
              Temp[i] = (static_cast<float>(codes[i] - 10000)) * (-0.1);
            }
            else if (codes[i] == 65535)
            {
              Temp[i] = static_cast<float>(codes[i]);
            }
            LOG(LL_INFO, ("Sensor num %d temp %f", i, Temp[i]));
          }
          //PrintTemp();
        }
      }
    }
  }
  else
  {
    LOG(LL_INFO, ("Responseis wrong"));
  }
  print_buffer(response, status);
}
//=========================================================================================================================
void button_handler(int pin, void *arg)
{
  LOG(LL_INFO, ("Button Pressed, calling modbus"));
  uint16_t wd = ((uint16_t)CMD_TOGGLE << 8) | 0x00;
  mb_write_single_register(2, 1, wd, mb_read_response_handler, NULL);
}
//=========================================================================================================================
//=========================================================================================================================
void my_timer_cb(void *param)
{
  // LOG(LL_INFO, ("TIMER STARTED - %f", mgos_uptime()));
  mb_read_holding_registers(1, 0, 8, mb_read_response_handler, NULL);
}
//=========================================================================================================================
void InitApp(void)
{
  
  mgos_set_timer(15000, MGOS_TIMER_REPEAT, PubTempt, (void *)Temp);
  printSensors(sensors);
}
//=========================================================================================================================
extern "C" enum mgos_app_init_result mgos_app_init(void)
{
  InitApp();
  return MGOS_APP_INIT_SUCCESS;
}