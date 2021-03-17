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
#include <string>
#include <sstream>
#include <vector>


struct Devices
{
    QString boardName;
    QString linkAddress;
    QString ASDUbrokenTime;    //ASDUAddress - параметр IEC101 и IEC104
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

    QString cropName(QString);
    int count = 0;
    int greatestAddr =  0;
    int offset = 0;
};

#endif // CONFIGJSON_H
