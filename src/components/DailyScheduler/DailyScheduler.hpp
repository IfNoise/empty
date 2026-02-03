#pragma once

#include "core/Component.hpp"
#include "core/Output.hpp"
#include "core/Application.hpp"
#include "mgos_config.h"
#include "mgos_sys_config.h"
#include "mgos.h"

struct SchedulerItem // On Off interval seconds after midnight
{
    unsigned int start;
    unsigned int stop;
};

const static int MANUAL_OFF = 0;
const static int MANUAL_ON = 1;
const static int AUTO = 2;
const static int AUTO_MAP = 3;

//+++Base class on off scheduler
class Scheduler : public PollingComponent
{

public:
    typedef std::function<void()> Callback;
    Scheduler(std::string name, std::string output);
    Status Init() override;
    void check();
    inline void setOnCallback(Callback f) { _onCallback = f; }
    inline void setOffCallback(Callback f) { _offCallback = f; }
    void printItems();

protected:
    Status saveItems();
    Status loadItems();
    std::vector<SchedulerItem> _items{}; // Setpoints vector
    Callback _onCallback{nullptr};
    Callback _offCallback = {nullptr};
    void on();
    void off();
    bool map_is_loaded{false};
    std::string _output;
    BinaryOutput *_out{nullptr};
};

Scheduler::Scheduler(std::string name, std::string output) : PollingComponent(name, 10000), _output(output)
{
}

Status Scheduler::Init()
{
    _out = App.getBinOutputByName(_output);
    _timer = new Timer(_interval, MGOS_TIMER_REPEAT, std::bind(&PollingComponent::callback, this));
    _timer->Reset(_interval, MGOS_TIMER_REPEAT);
    if (_timer->IsValid())
        return Status::OK();
    else
        return Status::CANCELLED();
}

void Scheduler::check()
{
    time_t now = 3600 * 7 + time(0);
    tm *tm_info = gmtime(&now);
    LOG(LL_INFO, (" %s checking,now %d:%d:%d", _name.c_str(), tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec));
    unsigned int cursec = tm_info->tm_sec + tm_info->tm_min * 60 + tm_info->tm_hour * 3600;
    LOG(LL_INFO, (" %s checking,cursec =%d c.", _name.c_str(), cursec));
    if (_out)
    {
        bool curState = _out->getState();
        bool state{false};
        for (SchedulerItem item : _items)
        {
            state = item.start > item.stop ? ((cursec >= item.start) || (cursec < item.stop)) : ((cursec >= item.start) && (cursec < item.stop));
            if (state)
            {
                LOG(LL_INFO, (" Item start=%d stop=%d", item.start, item.stop));
                break;
            }
        }
        if (state != curState)
        {
            if (state)
            {
                on();
                LOG(LL_INFO, (" Item is on"));
            }
            else
            {
                off();
                LOG(LL_INFO, (" Item is off"));
            }
        }
    }
    else
        LOG(LL_ERROR, (" Outpun is not setting Up"));
}

void Scheduler::on()
{
    if (_out != nullptr)
    {
        if(!_out->getState()){
        _out->setState(true);
        LOG(LL_INFO, (" setState called ON"));
        }
    }
    if (_onCallback != nullptr)
    {
        _onCallback();
    }
}

void Scheduler::off()
{
    if (_out != nullptr)
    {
        if(_out->getState()){
        _out->setState(false);
        LOG(LL_INFO, (" setState called OFF"));
        }
    }
    if (_offCallback != nullptr)
    {
        _offCallback();
    }
}
Status Scheduler::loadItems()
{
    char fnameBuf[32];
    json_token t;
    sprintf(fnameBuf, "%s.json", _name.c_str());
    LOG(LL_INFO, ("Loading items from file: %s", fnameBuf));
    
    // Используем fopen вместо json_fread для совместимости с RPC методами
    FILE *file = fopen(fnameBuf, "r");
    if (file == nullptr)
    {
        LOG(LL_WARN, ("File '%s' not found or cannot be opened", fnameBuf));
        return Status(STATUS_DATA_LOSS, "File not found or empty");
    }
    
    // Читаем содержимое файла
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    if (file_size <= 0)
    {
        fclose(file);
        LOG(LL_WARN, ("File '%s' is empty", fnameBuf));
        return Status(STATUS_DATA_LOSS, "File is empty");
    }
    
    char *json = (char*)malloc(file_size + 1);
    if (json == nullptr)
    {
        fclose(file);
        LOG(LL_ERROR, ("Failed to allocate memory for file reading"));
        return Status(STATUS_DATA_LOSS, "Memory allocation failed");
    }
    
    size_t bytes_read = fread(json, 1, file_size, file);
    json[bytes_read] = '\0';
    fclose(file);
    
    LOG(LL_DEBUG, ("File content length: %d bytes", bytes_read));
    
    int i, len = strlen(json);
    _items.clear(); // Очистка перед загрузкой
    
    for (i = 0; json_scanf_array_elem(json, len, ".items", i, &t) > 0; i++)
    {
        unsigned int start, stop;
        if (json_scanf(t.ptr, t.len, "{start: %d,stop: %d}", &start, &stop) == 2)
        {
            SchedulerItem item{start, stop};
            _items.push_back(item);
            LOG(LL_INFO, ("Item { start: %d, stop: %d } added to %s", start, stop, _name.c_str()));
        }
        else
        {
            LOG(LL_ERROR, ("Item format error. Skipped"));
        }
    }
    free(json); // Освобождение памяти
    
    if (i > 0)
    {
        map_is_loaded = true;
        LOG(LL_INFO, ("Successfully loaded %d items for %s", i, _name.c_str()));
        return Status::OK();
    }
    else
    {
        LOG(LL_ERROR, ("No items found in file %s", fnameBuf));
        map_is_loaded = false;
        return Status(STATUS_DATA_LOSS, "Items not loaded");
    }
}
void Scheduler::printItems()
{
    std::string items;
    int i = 0;
    for (SchedulerItem item : _items)
    {
        if (i == 0)
        {
            mgos::JSONAppendStringf(&items, "[{start : %d,stop : %d}", item.start, item.stop);
        }
        else if (i > 0)
        {
            mgos::JSONAppendStringf(&items, ",{start : %d,stop : %d}", item.start, item.stop);
        }
        ++i;
    }
    items += "]";
    LOG(LL_INFO, ("Items of %s : %s", _name.c_str(), items.c_str()));
}
Status Scheduler::saveItems()
{
    char fnameBuf[32];
    sprintf(fnameBuf, "%s.json", _name.c_str());
    std::string items;
    int i = 0;
    for (SchedulerItem item : _items)
    {
        if (i == 0)
        {
            mgos::JSONAppendStringf(&items, "[{start : %d,stop : %d}", item.start, item.stop);
        }
        else if (i > 0)
        {
            mgos::JSONAppendStringf(&items, ",{start : %d,stop : %d}", item.start, item.stop);
        }
        ++i;
    }
    items += "]";

    if (json_fprintf(fnameBuf, "{ items : %Q }", items.c_str()) > 0)
    {
        LOG(LL_INFO, ("Items of %s saved to file", _name.c_str()));
        return Status::OK();
    }
    else
    {
        LOG(LL_ERROR, ("Something went wrong,Items not saved "));
        return Status(STATUS_DATA_LOSS, "Items not saved");
    }
}