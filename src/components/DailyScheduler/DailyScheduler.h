#ifndef dailyscheduler_h
#define dailyscheduler_h
/*
/It's library using LittleFS filesystem for saving 
configuration.You must initial LittleFS file system on
setup() function
*/
#include <Arduino.h>
#include <LITTLEFS.h>
#include <ArduinoJson.h>
#define ON_HIGH 
#if defined(ON_HIGH)
  #define ON HIGH
  #define OFF LOW
#else
  #define ON LOW
  #define OFF HIGH
#endif
typedef void (*callback)(void);
//===On Off Pin Daily Scheduler
struct SchedulerItem //On Interval setpoint data structure
{
    int startMin;
    int stopMin;
};
//+++Base class on off scheduler
class Scheduler
{
protected:
    SchedulerItem *_items = NULL; //Setpoints array pointer
    String _name;
    uint8_t _size;
    bool _state = false;
    bool _manual = true;
    uint8_t _v_port;
    uint8_t _hw_port;
    uint8_t _counter = 0;
    void on();
    void off();
    void* _onCallback=NULL;
    void* _offCallback=NULL;

public:
    Scheduler(String name, uint8_t size,uint8_t v_port, uint8_t hw_port);
    ~Scheduler();
    bool addItem(int start, int stop);
    SchedulerItem getItem(int i);
    void clear();
    void check(int curmin);
    inline void setManual() { _manual = true; }
    inline void setAuto() { _manual = false; }
    inline bool getState() { return _state; }
    inline uint8_t getCount(){return _counter;}
    inline uint8_t v_port(){return _v_port;}
    inline void setOnCallback(callback f){_onCallback=(void *)f;}
    inline void setOffCallback(callback f){_offCallback=(void *)f;}
    void manOn();
    void manOff();
};

class IrrigationScheduler : public Scheduler
{
private:
    int _startmin;
    int _stopmin;
    uint8_t _irrNumber;
    uint8_t _irrWindow;

public:
    IrrigationScheduler(String name, uint8_t size,uint8_t v_port, uint8_t hw_pin);
    ~IrrigationScheduler();
    void load();
    void save();
    void setStartStop(int start, int stop);
    void setIrrNumber(int8_t num) { _irrNumber = num; }
    void setIrrWindow(int8_t win) { _irrWindow = win; }
    void update();
};


#endif