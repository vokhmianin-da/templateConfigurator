#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/istreamwrapper.h"
#include <iostream>
#include <fstream>
#include <String>
#include <sstream>
//#include <experimental/filesystem>

using namespace rapidjson;

using namespace std;


template <typename T>
std::string toString(T val)
{
    std::ostringstream oss;
    oss << val;
    return oss.str();
}

int main()
{
    const string templName = "template_";
    const string master = "master_";
    string      configName;
    string      templateName;
    string      sJson = ".json";

    string* boardNames;
    int         count;
    int         greatestAddr =  0;
    int         offset = 0;

    string interfaceDriver; //тип интерфейсного драйвера, для которого содается json

    setlocale(LC_ALL, "Russian");


    std::cout << endl << endl << 
                 "            easy config 0.0.1b" << endl <<
                 "          developed at last night " << endl << endl;
    std::cout << "               !!! NOTE !!!" << endl << 
                 " write all file names without file extension" << endl << endl << endl;

    ofstream file;
    StringBuffer s;
    Writer<StringBuffer> writer(s);

    std::cout << "do you want to make new template?" << endl << "    y     --> yes" << endl << "    n     --> no" << endl << "    other --> exit" << endl;
    char mode;
    std::cin >> mode;

    if (!(mode == 'y' || mode == 'Y') && !(mode == 'n' || mode == 'N'))
    {
        return 0;
    }

    cout << "Введите тип драйвера интерфейса" << endl;
    std::cin >>  interfaceDriver;

    if (mode == 'y' || mode == 'Y')
    {

        std::cout << "enter configuration file name: "<<endl;
        std::cin >> configName;

        std::cout << "making template..." << endl;
        std::ifstream ifs(configName + sJson);
        IStreamWrapper isw(ifs);
        Document documentFromFile;

        documentFromFile.ParseStream(isw);

        if (documentFromFile.IsNull()) {
            std::cout << "Unable to read document" << std::endl;
            return 1;
        }

        templateName = templName + configName + sJson;
        file.open(templateName);

        if (!file.is_open())
        {
            std::cout << "HARD FAULT! =)" << endl;
            return 1;
        }

        writer.StartObject();
        writer.Key("templates");
        writer.StartArray();


        for (auto iter = documentFromFile["tags"].Begin(); iter != documentFromFile["tags"].End(); ++iter)
        {
            auto tag = iter->GetObject();
            auto iec = tag["mappings"].GetObject().FindMember("IEC101S");

            if (iec->value.IsObject())
            {
                auto iecObj = iec->value.GetObject();

                if (!(iecObj.HasMember("spontType") || iecObj.HasMember("interType") || iecObj.HasMember("cmdType") || iecObj.HasMember("IOA")))
                {
                    continue;
                }




                writer.StartObject();

                writer.Key("type");
                writer.String(tag["type"].GetString());

                writer.Key("name");
                writer.String(tag["name"].GetString());

                writer.Key("parameters");
                writer.StartObject();







                if (iecObj.HasMember("spontType"))
                {
                    writer.Key("spontType");
                    writer.Uint(iecObj["spontType"].GetUint());
                }

                if (iecObj.HasMember("interType"))
                {
                    writer.Key("interType");
                    writer.Uint(iecObj["interType"].GetUint());
                }

                if (iecObj.HasMember("cmdType"))
                {
                    writer.Key("cmdType");
                    writer.Uint(iecObj["cmdType"].GetUint());
                }


                if (iecObj.HasMember("IOA"))
                {
                    writer.Key("IOA");
                    writer.Uint(iecObj["IOA"].GetUint());

                    if (iter == documentFromFile["tags"].End())
                    {
                        greatestAddr = iecObj["IOA"].GetUint();
                    }
                }


                writer.EndObject();
                writer.EndObject();
            }
            
        }
        writer.EndArray();
        writer.EndObject();

        file << s.GetString() << endl;

        file.close();
        writer.Reset(s);
        templateName = templName + configName;// +sJson;

        std::cout << "template OK!" << endl << endl;
    }
    


    if (!(mode == 'y' || mode == 'Y')) 
    {
        std::cout << "enter path to template file: ";
        std::cin >> templateName;
        std::cout << "tamplateName OK!" << endl;


    }

    std::cout << "how much boards do you want?" << endl;

        std::cin >> count;
        std::cout << "count OK!" << endl;
 
    
    

    

    if (count == 0)
    {
        return 0;
    }
    else if (count > 1)
    {

        std::cout << "set addres offset between boards" << endl;
        while (1)
        {
            std::cin >> offset;
            
            if (greatestAddr + count * offset >= 65535)
            {
                std::cout << "offset is too big" << endl;
            }
            else
            {
                std::cout << "offset OK!" << endl;
                break;
            }
        }
    }
        boardNames = new string[count];

        for (int i = 0; i < count; i++)
        {
            std::cout << "enter name for board " << i+1 << endl;
            cin >> boardNames[i];
        }

        std::cout << "making master 101 config... " << endl;
        std::ifstream ifs(templateName + sJson);
        IStreamWrapper isw(ifs);
        Document documentFromFile;
        documentFromFile.ParseStream(isw);
        
        s.Clear();
        writer.Reset(s);

        string tmp = master + configName + sJson;

        file.open(tmp.c_str());

        if (!file.is_open())
        {
            std::cout << "HARD FAULT! =)" << endl;
            return 1;
        }

        writer.StartObject();
        writer.Key("devices");
        writer.StartArray();
        for (int i = 0; i < count; i++)
        {
            uint32_t a = 0;
            writer.StartObject();
            
            writer.Key("class");
            writer.String("IEC101GenericSlave");
            
            writer.Key("parent");
            writer.String("IEC101M");
            
            writer.Key("name");
            writer.String(boardNames[i].c_str());

            writer.Key("template");
            writer.String(templateName.c_str());

            writer.Key("parameters");

                writer.StartObject();

                writer.Key("linkAddress");
                std::cout << "enter linkAddress for board " << i + 1 << endl;
                cin >> a;

                writer.Uint(a);

                writer.Key("ASDUAddress");
                std::cout << "enter ASDUAddress for board " << i + 1 << endl;
                cin >> a;
                writer.Uint(a);

                writer.EndObject();

            writer.EndObject();

        }
        writer.EndArray();
        

            writer.Key("tags");
                writer.StartArray();

                for (int i = 0; i < count; i++)
                {            
                    for (auto iter = documentFromFile["templates"].Begin(); iter != documentFromFile["templates"].End(); ++iter)
                    {
                        auto tag = iter->GetObject();

                            writer.StartObject();

                            writer.Key("type");
                            writer.String("Instance");

                            writer.Key("name");

                            tmp = boardNames[i] + "." + tag["name"].GetString();

                            writer.String(tmp.c_str());

                            writer.Key("base");
                            writer.String(tag["name"].GetString());

                            writer.Key("mappings");
                                writer.StartObject();

                                auto connection = tag["parameters"].GetObject();

                                if (connection.HasMember("spontType") || connection.HasMember("interType") || connection.HasMember("cmdType") || connection.HasMember("IOA"))
                                {
                                    writer.Key("IEC101S");
                                    writer.StartObject();



                                    if (connection.HasMember("spontType"))
                                    {
                                        writer.Key("spontType");
                                        writer.Uint(connection["spontType"].GetUint());
                                    }

                                    if (connection.HasMember("interType"))
                                    {
                                        writer.Key("interType");
                                        writer.Uint(connection["interType"].GetUint());
                                    }

                                    if (connection.HasMember("cmdType"))
                                    {
                                        writer.Key("cmdType");
                                        writer.Uint(connection["cmdType"].GetUint());
                                    }


                                    if (connection.HasMember("IOA"))
                                    {
                                        writer.Key("IOA");
                                        writer.Uint(connection["IOA"].GetUint() + (offset * i));

                                    }
                                }


                                    writer.EndObject();
                                writer.EndObject();
                            writer.EndObject();
                    }
                }
                writer.EndArray();
            writer.EndObject();

        file << s.GetString() << endl;
        file.close();
    
        std::cout << "finished!" << endl;
        getchar();
  return 0;
}

