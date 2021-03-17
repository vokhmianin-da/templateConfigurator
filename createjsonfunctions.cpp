#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QFileDialog>
#include <QTextStream>

using namespace rapidjson;
using namespace std;

void MainWindow::createTemplateIEC104(MainWindow *mainWindow)
{
    std::ofstream file;
    StringBuffer s;
    Writer<StringBuffer> writer(s);

    std::ifstream ifs((mainWindow->conf->configName + mainWindow->conf->sJson).toStdString());
    IStreamWrapper isw(ifs);
    Document documentFromFile;

    documentFromFile.ParseStream(isw);

    if (documentFromFile.IsNull()) {
        mainWindow->ui->lbStatus->setText("Не удалось прочитать slave.json");
        emit mainWindow->notOpenSlaveJson();
        return;
    }
    /*Удаляем лишнее*/
    QString tempString;
    tempString = mainWindow->conf->cropName(mainWindow->conf->configName);

    mainWindow->conf->templateName = mainWindow->conf->templName + tempString + mainWindow->conf->sJson;
    mainWindow->conf->templateName = QFileDialog::getSaveFileName(mainWindow, tr("Сохранить template как"), mainWindow->conf->templateName, mainWindow->filter);

    file.open(mainWindow->conf->templateName.toStdString());

    if (!file.is_open())
    {
        mainWindow->ui->lbStatus->setText("HARD FAULT! =)");
        return;
    }

    writer.StartObject();
    writer.Key("templates");
    writer.StartArray();


    for (auto iter = documentFromFile["tags"].Begin(); iter != documentFromFile["tags"].End(); ++iter)
    {
        auto tag = iter->GetObject();
        auto iec = tag["mappings"].GetObject().FindMember("IEC104S");

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
                    mainWindow->conf->greatestAddr = iecObj["IOA"].GetUint();
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


    mainWindow->ui->lbStatus->setText("template OK!");
}

void MainWindow::createMasterJSONIEC101(MainWindow *mainWindow)
{
    ofstream file;
    StringBuffer s;
    Writer<StringBuffer> writer(s);
    std::ifstream ifs(mainWindow->conf->templateName.toStdString());
    IStreamWrapper isw(ifs);
    Document documentFromFile;
    documentFromFile.ParseStream(isw);

    if (mainWindow->conf->templateName.isEmpty())
    {
        mainWindow->ui->lbStatus->setText("Не выбран файл template");
        return;
    }
    if (!mainWindow->conf->count)
    {
        mainWindow->ui->lbStatus->setText("Не заданы devices");
        return;
    }

    s.Clear();
    writer.Reset(s);

    /*Удаляем лишнее*/
    QString tempString;
    tempString = mainWindow->conf->cropName(mainWindow->conf->configName);

    tempString = mainWindow->conf->master + tempString + mainWindow->conf->sJson;
    tempString = QFileDialog::getSaveFileName(mainWindow, tr("Сохранить master.json как"), tempString, mainWindow->filter);
    string tmp = tempString.toStdString();

    file.open(tmp.c_str());

    if (!file.is_open())
    {
        mainWindow->ui->lbStatus->setText("HARD FAULT! =)");
        return;
    }

    writer.StartObject();
    writer.Key("devices");
    writer.StartArray();
    for (int i = 0; i < mainWindow->conf->count; i++)
    {
        writer.StartObject();

        writer.Key("class");
        writer.String("IEC101GenericSlave");

        writer.Key("parent");
        writer.String("IEC101M");

        writer.Key("name");
        writer.String(mainWindow->conf->devices[i].boardName.toStdString().c_str());

        writer.Key("template");
        tempString = mainWindow->conf->cropName(mainWindow->conf->templateName);
        writer.String(tempString.toStdString().c_str());

        writer.Key("parameters");

        writer.StartObject();

        writer.Key("linkAddress");

        writer.String(mainWindow->conf->devices[i].linkAddress.toStdString().c_str());

        writer.Key("ASDUAddress");

        writer.String(mainWindow->conf->devices[i].ASDUbrokenTime.toStdString().c_str());

        writer.EndObject();

        writer.EndObject();

    }
    writer.EndArray();


    writer.Key("tags");
    writer.StartArray();

    for (int i = 0; i < mainWindow->conf->count; i++)
    {
        for (auto iter = documentFromFile["templates"].Begin(); iter != documentFromFile["templates"].End(); ++iter)
        {
            auto tag = iter->GetObject();

            writer.StartObject();

            writer.Key("type");
            writer.String("Instance");

            writer.Key("name");

            tmp = mainWindow->conf->devices[i].boardName.toStdString() + "." + tag["name"].GetString();

            writer.String(tmp.c_str());

            writer.Key("base");
            writer.String(tag["name"].GetString());

            writer.Key("mappings");
            writer.StartObject();

//            auto connection = tag["parameters"].GetObject();

//            if (connection.HasMember("spontType") || connection.HasMember("interType") || connection.HasMember("cmdType") || connection.HasMember("IOA"))
//            {
//                writer.Key("IEC101S");
//                writer.StartObject();

//                if (connection.HasMember("spontType"))
//                {
//                    writer.Key("spontType");
//                    writer.Uint(connection["spontType"].GetUint());
//                }

//                if (connection.HasMember("interType"))
//                {
//                    writer.Key("interType");
//                    writer.Uint(connection["interType"].GetUint());
//                }

//                if (connection.HasMember("cmdType"))
//                {
//                    writer.Key("cmdType");
//                    writer.Uint(connection["cmdType"].GetUint());
//                }


//                if (connection.HasMember("IOA"))
//                {
//                    writer.Key("IOA");
//                    writer.Uint(connection["IOA"].GetUint() + (mainWindow->conf->offset * i));

//                }
//            }

//            writer.EndObject();
            writer.EndObject();
            writer.EndObject();
        }
    }
    writer.EndArray();
    writer.EndObject();

    file << s.GetString() << endl;
    file.close();

    mainWindow->ui->lbStatus->setText("finished!");
}

void MainWindow::createMasterJSONIEC104(MainWindow *mainWindow)
{
    ofstream file;
    StringBuffer s;
    Writer<StringBuffer> writer(s);
    std::ifstream ifs(mainWindow->conf->templateName.toStdString());
    IStreamWrapper isw(ifs);
    Document documentFromFile;
    documentFromFile.ParseStream(isw);

    if (mainWindow->conf->templateName.isEmpty())
    {
        mainWindow->ui->lbStatus->setText("Не выбран файл template");
        return;
    }
    if (!mainWindow->conf->count)
    {
        mainWindow->ui->lbStatus->setText("Не заданы devices");
        return;
    }

    s.Clear();
    writer.Reset(s);

    /*Удаляем лишнее*/
    QString tempString;
    tempString = mainWindow->conf->cropName(mainWindow->conf->configName);

    tempString = mainWindow->conf->master + tempString + mainWindow->conf->sJson;
    tempString = QFileDialog::getSaveFileName(mainWindow, tr("Сохранить master.json как"), tempString, mainWindow->filter);
    string tmp = tempString.toStdString();

    file.open(tmp.c_str());

    if (!file.is_open())
    {
        mainWindow->ui->lbStatus->setText("HARD FAULT! =)");
        return;
    }

    writer.StartObject();
    writer.Key("devices");
    writer.StartArray();
    for (int i = 0; i < mainWindow->conf->count; i++)
    {
        writer.StartObject();

        writer.Key("class");
        writer.String("IEC104GenericSlave");

        writer.Key("parent");
        writer.String("IEC104M");

        writer.Key("name");
        writer.String(mainWindow->conf->devices[i].boardName.toStdString().c_str());

        writer.Key("template");
        tempString = mainWindow->conf->cropName(mainWindow->conf->templateName);
        writer.String(tempString.toStdString().c_str());

        writer.Key("parameters");

        writer.StartObject();

        writer.Key("linkAddress");

        writer.String(mainWindow->conf->devices[i].linkAddress.toStdString().c_str());

        writer.Key("ASDUAddress");

        writer.String(mainWindow->conf->devices[i].ASDUbrokenTime.toStdString().c_str());

        writer.EndObject();

        writer.EndObject();

    }
    writer.EndArray();


    writer.Key("tags");
    writer.StartArray();

    for (int i = 0; i < mainWindow->conf->count; i++)
    {
        for (auto iter = documentFromFile["templates"].Begin(); iter != documentFromFile["templates"].End(); ++iter)
        {
            auto tag = iter->GetObject();

            writer.StartObject();

            writer.Key("type");
            writer.String("Instance");

            writer.Key("name");

            tmp = mainWindow->conf->devices[i].boardName.toStdString() + "." + tag["name"].GetString();

            writer.String(tmp.c_str());

            writer.Key("base");
            writer.String(tag["name"].GetString());

            writer.Key("mappings");
            writer.StartObject();

//            auto connection = tag["parameters"].GetObject();

//            if (connection.HasMember("spontType") || connection.HasMember("interType") || connection.HasMember("cmdType") || connection.HasMember("IOA"))
//            {
//                writer.Key("IEC101S");
//                writer.StartObject();

//                if (connection.HasMember("spontType"))
//                {
//                    writer.Key("spontType");
//                    writer.Uint(connection["spontType"].GetUint());
//                }

//                if (connection.HasMember("interType"))
//                {
//                    writer.Key("interType");
//                    writer.Uint(connection["interType"].GetUint());
//                }

//                if (connection.HasMember("cmdType"))
//                {
//                    writer.Key("cmdType");
//                    writer.Uint(connection["cmdType"].GetUint());
//                }


//                if (connection.HasMember("IOA"))
//                {
//                    writer.Key("IOA");
//                    writer.Uint(connection["IOA"].GetUint() + (mainWindow->conf->offset * i));

//                }
//            }

//            writer.EndObject();
            writer.EndObject();
            writer.EndObject();
        }
    }
    writer.EndArray();
    writer.EndObject();

    file << s.GetString() << endl;
    file.close();

    mainWindow->ui->lbStatus->setText("finished!");
}

void MainWindow::createTemplateIEC101(MainWindow *mainWindow)
{
    std::ofstream file;
    StringBuffer s;
    Writer<StringBuffer> writer(s);

    std::ifstream ifs((mainWindow->conf->configName + mainWindow->conf->sJson).toStdString());
    IStreamWrapper isw(ifs);
    Document documentFromFile;

    documentFromFile.ParseStream(isw);

    if (documentFromFile.IsNull()) {
        mainWindow->ui->lbStatus->setText("Не удалось прочитать slave.json");
        emit mainWindow->notOpenSlaveJson();
        return;
    }
    /*Удаляем лишнее*/
    QString tempString;
    tempString = mainWindow->conf->cropName(mainWindow->conf->configName);

    mainWindow->conf->templateName = mainWindow->conf->templName + tempString + mainWindow->conf->sJson;
    mainWindow->conf->templateName = QFileDialog::getSaveFileName(mainWindow, tr("Сохранить template как"), mainWindow->conf->templateName, mainWindow->filter);
    file.open(mainWindow->conf->templateName.toStdString());

    if (!file.is_open())
    {
        mainWindow->ui->lbStatus->setText("HARD FAULT! =)");
        return;
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
                    mainWindow->conf->greatestAddr = iecObj["IOA"].GetUint();
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


    mainWindow->ui->lbStatus->setText("template OK!");
}

void MainWindow::createTemplateModbus(MainWindow *mainWindow)
{
    std::ofstream file;
    StringBuffer s;
    Writer<StringBuffer> writer(s);

    std::ifstream ifs((mainWindow->conf->configName + mainWindow->conf->sJson).toStdString());
    IStreamWrapper isw(ifs);
    Document documentFromFile;

    documentFromFile.ParseStream(isw);

    if (documentFromFile.IsNull()) {
        mainWindow->ui->lbStatus->setText("Не удалось прочитать slave.json");
        emit mainWindow->notOpenSlaveJson();
        return;
    }
    /*Удаляем лишнее*/
    QString tempString;
    tempString = mainWindow->conf->cropName(mainWindow->conf->configName);

    mainWindow->conf->templateName = mainWindow->conf->templName + tempString + mainWindow->conf->sJson;
    mainWindow->conf->templateName = QFileDialog::getSaveFileName(mainWindow, tr("Сохранить template как"), mainWindow->conf->templateName, mainWindow->filter);
    file.open(mainWindow->conf->templateName.toStdString());

    if (!file.is_open())
    {
        mainWindow->ui->lbStatus->setText("HARD FAULT! =)");
        return;
    }

    writer.StartObject();
    writer.Key("templates");
    writer.StartArray();


    for (auto iter = documentFromFile["tags"].Begin(); iter != documentFromFile["tags"].End(); ++iter)
    {
        auto tag = iter->GetObject();
        auto iec = tag["mappings"].GetObject().FindMember("MBSlave");

        if (iec->value.IsObject())
        {
            auto iecObj = iec->value.GetObject();

            if (!(iecObj.HasMember("regType") || iecObj.HasMember("regAddr") || iecObj.HasMember("reqTime") || iecObj.HasMember("grouping")))
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

            if (iecObj.HasMember("regType"))
            {
                writer.Key("regType");
                writer.String(iecObj["regType"].GetString());
            }

            if (iecObj.HasMember("grouping"))
            {
                writer.Key("grouping");
                writer.String(iecObj["grouping"].GetString());
            }

            if (iecObj.HasMember("reqTime"))
            {
                writer.Key("reqTime");
                writer.Uint(iecObj["reqTime"].GetUint());
            }

            if (iecObj.HasMember("regAddr"))
            {
                writer.Key("regAddr");
                writer.Uint(iecObj["regAddr"].GetUint());

                if (iter == documentFromFile["tags"].End())
                {
                    mainWindow->conf->greatestAddr = iecObj["regAddr"].GetUint();
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


    mainWindow->ui->lbStatus->setText("template OK!");
}

void MainWindow::createMasterJSONModbus(MainWindow *mainWindow)
{
    ofstream file;
    StringBuffer s;
    Writer<StringBuffer> writer(s);
    std::ifstream ifs(mainWindow->conf->templateName.toStdString());
    IStreamWrapper isw(ifs);
    Document documentFromFile;
    documentFromFile.ParseStream(isw);

    if (mainWindow->conf->templateName.isEmpty())
    {
        mainWindow->ui->lbStatus->setText("Не выбран файл template");
        return;
    }
    if (!mainWindow->conf->count)
    {
        mainWindow->ui->lbStatus->setText("Не заданы devices");
        return;
    }

    s.Clear();
    writer.Reset(s);

    /*Удаляем лишнее*/
    QString tempString;
    tempString = mainWindow->conf->cropName(mainWindow->conf->configName);

    tempString = mainWindow->conf->master + tempString + mainWindow->conf->sJson;
    tempString = QFileDialog::getSaveFileName(mainWindow, tr("Сохранить master.json как"), tempString, mainWindow->filter);
    string tmp = tempString.toStdString();

    file.open(tmp.c_str());

    if (!file.is_open())
    {
        mainWindow->ui->lbStatus->setText("HARD FAULT! =)");
        return;
    }

    writer.StartObject();
    writer.Key("devices");
    writer.StartArray();
    for (int i = 0; i < mainWindow->conf->count; i++)
    {
        writer.StartObject();

        writer.Key("class");
        writer.String("MBGenericSlave");

        writer.Key("parent");
        writer.String("MBMaster");

        writer.Key("name");
        writer.String(mainWindow->conf->devices[i].boardName.toStdString().c_str());

        writer.Key("template");
        tempString = mainWindow->conf->cropName(mainWindow->conf->templateName);
        writer.String(tempString.toStdString().c_str());

        writer.Key("parameters");

        writer.StartObject();

        writer.Key("linkAddress");

        writer.String(mainWindow->conf->devices[i].linkAddress.toStdString().c_str());

        writer.Key("brokenTime");

        writer.String(mainWindow->conf->devices[i].ASDUbrokenTime.toStdString().c_str());

        writer.EndObject();

        writer.EndObject();

    }
    writer.EndArray();


    writer.Key("tags");
    writer.StartArray();

    for (int i = 0; i < mainWindow->conf->count; i++)
    {
        for (auto iter = documentFromFile["templates"].Begin(); iter != documentFromFile["templates"].End(); ++iter)
        {
            auto tag = iter->GetObject();

            writer.StartObject();

            writer.Key("type");
            writer.String("Instance");

            writer.Key("name");

            tmp = mainWindow->conf->devices[i].boardName.toStdString() + "." + tag["name"].GetString();

            writer.String(tmp.c_str());

            writer.Key("base");
            writer.String(tag["name"].GetString());

            writer.Key("mappings");
            writer.StartObject();

//            auto connection = tag["parameters"].GetObject();

//            if (connection.HasMember("spontType") || connection.HasMember("interType") || connection.HasMember("cmdType") || connection.HasMember("IOA"))
//            {
//                writer.Key("IEC101S");
//                writer.StartObject();

//                if (connection.HasMember("spontType"))
//                {
//                    writer.Key("spontType");
//                    writer.Uint(connection["spontType"].GetUint());
//                }

//                if (connection.HasMember("interType"))
//                {
//                    writer.Key("interType");
//                    writer.Uint(connection["interType"].GetUint());
//                }

//                if (connection.HasMember("cmdType"))
//                {
//                    writer.Key("cmdType");
//                    writer.Uint(connection["cmdType"].GetUint());
//                }


//                if (connection.HasMember("IOA"))
//                {
//                    writer.Key("IOA");
//                    writer.Uint(connection["IOA"].GetUint() + (mainWindow->conf->offset * i));

//                }
//            }

//            writer.EndObject();
            writer.EndObject();
            writer.EndObject();
        }
    }
    writer.EndArray();
    writer.EndObject();

    file << s.GetString() << endl;
    file.close();

    mainWindow->ui->lbStatus->setText("finished!");
}

QString configJson::cropName(QString originalString)
{
    /*Удаляем лишнее*/
    int deleter = originalString.lastIndexOf("/");
    QString newString;
    newString = originalString.remove(0, deleter+1);
    newString = newString.remove(".json");
    return newString;
}
