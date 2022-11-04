#include "DailyScheduler.h"

Scheduler::Scheduler(String name, uint8_t size, uint8_t v_port, uint8_t hw_port) : _name(name), _size(size), _v_port(v_port), _hw_port(hw_port)
{
    _items = new SchedulerItem[size];
}

Scheduler::~Scheduler()
{
    delete[] _items;
    _items = NULL;
    _counter = 0;
    _size = 0;
}
bool Scheduler::addItem(int start, int stop)
{
    if (_counter < _size)
    {
        _items[_counter].startMin = start;
        _items[_counter].stopMin = stop;
        ++_counter;
        return true;
    }
    else
        return false;
}
void Scheduler::clear()
{
    delete[] _items;
    _items = NULL;
    _counter = 0;
    _items = new SchedulerItem[_size];
}
void Scheduler::on()
{
    _state = true;
    digitalWrite(_hw_port, ON);
    if (_onCallback != NULL)
    {
        (*(callback)_onCallback)();
    }
    Serial.print(_name);
    Serial.print(" Timer");
    Serial.println(" ON");
}
SchedulerItem Scheduler::getItem(int i)
{
    if (i < _counter)
        return _items[i];
    else
        return _items[_counter - 1];
}
void Scheduler::off()
{
    _state = false;
    digitalWrite(_hw_port, OFF);
    if (_offCallback != NULL)
    {
        (*(callback)_offCallback)();
    }
    Serial.print(_name);
    Serial.print(" Timer");
    Serial.println(" Off");
}
void Scheduler::manOn()
{
    if (_manual)
    {
        if (!_state)
            on();
    }
}
void Scheduler::manOff()
{
    if (_manual)
    {
        if (_state)
            off();
    }
}

void Scheduler::save()
{
    String fileName ="/"+ _name + "conf.txt";
    // Delete existing file, otherwise the configuration is appended to the file
    if(LITTLEFS.exists(fileName))LITTLEFS.remove(fileName);

    // Open file for writing
    File file = LITTLEFS.open(fileName, "w");
    if (!file)
    {
        Serial.println("Failed to create file");
       
    }
    Serial.println("file OK");
    StaticJsonDocument<256> doc;
    doc["counter"] = _counter;
    // Set the values in the document
    for (int i = 0; i < (_counter); i++)
    {
        String key;
        key = String("set");
        key += i;
        doc[key]["startMin"] = _items[i].startMin;
        doc[key]["stopMin"] = _items[i].stopMin;
    }
    // Serialize JSON to file
    if (serializeJsonPretty(doc, file) == 0)
    {
        Serial.println(F("Failed to write to file"));
    }

    // Close the file
    file.close();
}
void Scheduler::load()
{
    String fileName ="/" +_name + "conf.txt";
    File file = LITTLEFS.open(fileName, "r");

    StaticJsonDocument<512> doc;

    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, file);
    if (error)
    {
        Serial.println(F("Failed to read file, using default configuration"));
        clear();
        addItem(0,60);
        save();
        _manual=true;
        manOff();

    }else
    {
        clear();
    int counter = doc["counter"] | 0;
    for (int i = 0; i < (counter); i++)
    {
        String key;
        int startMin,stopMin;
        key = String("set");
        key += i;
        startMin = doc[key]["startMin"] | 0;
        stopMin = doc[key]["stopMin"] | 0;
        addItem(startMin,stopMin);
    }
    }
    file.close();
}

void Scheduler::check(int curmin)
{
    bool prevState = _state;
    if (!_manual)
    {
        for (int i = 0; i < _counter; i++)

        {
            _state = _items[i].startMin > _items[i].stopMin ? ((curmin >= _items[i].startMin) || (curmin < _items[i].stopMin)) : ((curmin >= _items[i].startMin) && (curmin < _items[i].stopMin));

            if (_state)
            {

                break;
            }
        }
    }
    if (prevState != _state)
    {
        if (_state)
        {
            on();
        }
        else
        {
            off();
        }
    }
}

IrrigationScheduler::IrrigationScheduler(String name, uint8_t size, uint8_t v_port, uint8_t hw_pin) : Scheduler(name, size, v_port, hw_pin), _startmin(0), _stopmin(1200), _irrNumber(1), _irrWindow(2)
{
}

IrrigationScheduler::~IrrigationScheduler()
{
}
void IrrigationScheduler::setStartStop(int start, int stop)
{
    _startmin = start;
    _stopmin = stop;
}
void IrrigationScheduler::save()
{
    String fileName ="/"+ _name + "conf.txt";
    // Delete existing file, otherwise the configuration is appended to the file
    if(LITTLEFS.exists(fileName))LITTLEFS.remove(fileName);

    // Open file for writing
    File file = LITTLEFS.open(fileName, "w");
    if (!file)
    {
        Serial.println("Failed to create file");
       return;
    }
    StaticJsonDocument<512> doc;
    doc["startmin"]=_startmin;
    doc["stopmin"]=_stopmin;
    doc["irrwindow"]=_irrWindow;
    doc["irrnumber"]=_irrNumber;
    // Set the values in the document
    // Serialize JSON to file
    if (serializeJsonPretty(doc, file) == 0)
    {
        Serial.println(F("Failed to write to file"));
    }

    // Close the file
    file.close();
}
void IrrigationScheduler::load()
{
    String fileName ="/" + _name + "conf.txt";
    File file = LITTLEFS.open(fileName, "r");

    StaticJsonDocument<512> doc;

    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, file);
    if (error)
    {
        Serial.println(F("Failed to read file, using default configuration"));
        clear();
        _startmin=0;
        _stopmin=0;
        _irrWindow=0;
       _irrNumber=0;
        update();
        _manual=true;
        manOff();

    }else
    { 
    _startmin=doc["startmin"]|0;
    _stopmin=doc["stopmin"]|0;
    _irrWindow=doc["irrwindow"]|0;
    _irrNumber=doc["irrnumber"]|0;
    update();
    }
    file.close();
}

void IrrigationScheduler::update()
{
    int begin, end, step;
    begin = _startmin - _irrWindow;
    end = _stopmin + _irrWindow;
    if (begin > end)
    {
        step = _irrNumber < 2 ? (1440 - begin + end) / 2 : (1440 - begin + end) / (_irrNumber - 1);
    }
    else
    {

        step = _irrNumber < 2 ? (end - begin) / 2 : (end - begin) / (_irrNumber - 1);
    }

    clear();
    for (int i = 0; i < _irrNumber; ++i)
    {
        int start = begin + step * i;
        if (start > 1440)
            start -= 1440;
        int stop = start + _irrWindow;
        if (stop > 1440)
            stop -= 1440;
        addItem(start, stop);
    }
}