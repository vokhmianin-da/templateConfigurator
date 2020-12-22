#ifndef CONFIGJSON_H
#define CONFIGJSON_H
#include <QString>

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/istreamwrapper.h"
#include <iostream>
#include <fstream>
#include <String>
#include <sstream>


struct Devices
{
    QString boardName;
    QString linkAddress;
    QString ASDUAddress;
};

class configJson
{    
public:
    configJson();

    const QString templName = "template_";
    const QString master = "master_";
    QString configName; //имя загружаемого файла
    QString templateName; //имя шаблона
    QString sJson = ".json";
    std::vector <Devices> devices;

    int count = 0;
    int greatestAddr =  0;
    int offset = 0;
};

#endif // CONFIGJSON_H
