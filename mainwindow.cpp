#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QFileDialog>
#include <QTextStream>

using namespace rapidjson;
using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    createTemplateFunctions{
        createTemplateIEC101,
        createTemplateIEC104
        }
{
    ui->setupUi(this);
    conf = new configJson;
    filter = trUtf8("Файл json(*.json)");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pbSelectFile_clicked()  //выбор json-slave
{
    conf->configName = QFileDialog::getOpenFileName(this, tr("Открыть"), QDir::current().path(), filter);
    if (conf->configName.length() > 0)
    {
        int index = conf->configName.indexOf(".json");              // определяем, есть ли в
        // названии строка ".json"
        if (index != -1 && conf->configName.length() - 5 == index) // если это файл json,
            // выполняются 2 условия:
            // строка ".json" есть
            // в названии, и она
            // находится в конце
            // строки названия файла
        {
            QString temp("Slave-json: ");
            ui->lbFileName->setText(temp.append(conf->configName));
            conf->configName.remove(".json");    //удаляем лишнее
            ui->lbStatus->setText("Файл slave-json выбран");
        }
        else
        {
            ui->lbStatus->setText("Неподходящий формат файла");
        }
    }
    else
    {
        ui->lbStatus->setText("Файл slave-json не выбран");
        ui->lbFileName->clear();
    }
}

void MainWindow::on_cbIsTemplate_clicked()  //создать/выбрать template
{
    if(ui->cbIsTemplate->isChecked())
    {
        ui->pbSelectTemplate->setEnabled(false);
        ui->pbCreateTemplate->setEnabled(true);
    }
    else
    {
        ui->pbSelectTemplate->setEnabled(true);
        ui->pbCreateTemplate->setEnabled(false);
    }
}

void MainWindow::on_pbSetLinkASDU_clicked() //создание device
{
    if(!ui->leDeviceName->text().isEmpty() && !ui->leLinkAddr->text().isEmpty() && !ui->leASDUAddr->text().isEmpty())
    {
        Devices temp;
        temp.boardName = ui->leDeviceName->text();
        temp.linkAddress = ui->leLinkAddr->text();
        temp.ASDUAddress = ui->leASDUAddr->text();

        conf->devices.push_back(temp);  //добавляем новый device
        ui->cbDevices->insertItem(conf->count, conf->devices[conf->count].boardName);   //отображаем в списке
        conf->count++; //увеличиваем количество devices
        ui->lePlatsQuantity->setText(QString::number(conf->count)); //отображаем кол-во
    }
}

void MainWindow::on_cbDevices_currentIndexChanged(int index)    //отображение текущего device
{
    if(!conf->devices.empty())
    {
        ui->leDeviceName->setText(conf->devices[index].boardName);
        ui->leLinkAddr->setText(conf->devices[index].linkAddress);
        ui->leASDUAddr->setText(conf->devices[index].ASDUAddress);
    }
    else
    {
        ui->leDeviceName->clear();
        ui->leLinkAddr->clear();
        ui->leASDUAddr->clear();
    }
}

void MainWindow::on_pbResetLinkASDU_clicked()   //удаление device
{
    if(!conf->devices.empty())
    {
        conf->devices.erase(conf->devices.begin() + ui->cbDevices->currentIndex()); //удаляем из vector
        conf->count--; //уменьшаем количество devices
        ui->lePlatsQuantity->setText(QString::number(conf->count)); //отображаем кол-во
        ui->cbDevices->removeItem(ui->cbDevices->currentIndex());   //удаляем из списка
        if(conf->count)
        {
            /*Отображаем новый текущий элемент*/
            ui->leDeviceName->setText(conf->devices[ui->cbDevices->currentIndex()].boardName);
            ui->leLinkAddr->setText(conf->devices[ui->cbDevices->currentIndex()].linkAddress);
            ui->leASDUAddr->setText(conf->devices[ui->cbDevices->currentIndex()].ASDUAddress);
        }
    }
}

void MainWindow::on_pbSelectTemplate_clicked()  //выбор template
{
    conf->templateName = QFileDialog::getOpenFileName(this, tr("Открыть"), QDir::current().path(), filter);
    if (conf->templateName.length() > 0)
    {
        int index = conf->templateName.indexOf(".json");              // определяем, есть ли в
        // названии строка ".json"
        if (index != -1 && conf->templateName.length() - 5 == index) // если это файл json,
            // выполняются 2 условия:
            // строка ".json" есть
            // в названии, и она
            // находится в конце
            // строки названия файла
        {
            QString temp("Template-json: ");
            ui->lbStatus->setText("Файл template-json выбран");
            ui->lbTemplateName->setText(temp.append(conf->templateName));
            conf->templateName.remove(".json");    //удаляем лишнее
        }
        else
        {
            ui->lbStatus->setText("Неподходящий формат файла");
        }
    }
    else
    {
        ui->lbStatus->setText("Файл template-json не выбран");
        ui->lbTemplateName->clear();
    }
}

void MainWindow::on_pbCreateTemplate_clicked()  //создание template
{
    createTemplateFunctions[ui->cbInterface->currentIndex()](this);

}

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
        mainWindow->ui->lbStatus->setText("Unable to read document");
        return;
    }
    /*Удаляем лишнее*/
    int deleter = mainWindow->conf->configName.lastIndexOf("/");
    QString tempString;
    tempString = mainWindow->conf->configName.remove(0, deleter+1);

    mainWindow->conf->templateName = mainWindow->conf->templName + tempString + mainWindow->conf->sJson;
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
        mainWindow->ui->lbStatus->setText("Unable to read document");
        return;
    }
    /*Удаляем лишнее*/
    int deleter = mainWindow->conf->configName.lastIndexOf("/");
    QString tempString;
    tempString = mainWindow->conf->configName.remove(0, deleter+1);

    mainWindow->conf->templateName = mainWindow->conf->templName + tempString + mainWindow->conf->sJson;
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

void MainWindow::on_pbCreateJson_clicked()  //создание json-master
{
    ofstream file;
    StringBuffer s;
    Writer<StringBuffer> writer(s);
    std::ifstream ifs(conf->templateName.toStdString());
    IStreamWrapper isw(ifs);
    Document documentFromFile;
    documentFromFile.ParseStream(isw);

    s.Clear();
    writer.Reset(s);

    /*Удаляем лишнее*/
    int deleter = conf->configName.lastIndexOf("/");
    QString tempString;
    tempString = conf->configName.remove(0, deleter+1);

    string tmp = conf->master.toStdString() + tempString.toStdString() + conf->sJson.toStdString();

    file.open(tmp.c_str());

    if (!file.is_open())
    {
        ui->lbStatus->setText("HARD FAULT! =)");
        return;
    }

    writer.StartObject();
    writer.Key("devices");
    writer.StartArray();
    for (int i = 0; i < conf->count; i++)
    {
//        uint32_t a = 0;
        writer.StartObject();

        writer.Key("class");
        writer.String("IEC104GenericSlave");

        writer.Key("parent");
        writer.String("IEC104M");

        writer.Key("name");
        writer.String(conf->devices[i].boardName.toStdString().c_str());

        writer.Key("template");
        writer.String(conf->templateName.toStdString().c_str());

        writer.Key("parameters");

        writer.StartObject();

        writer.Key("linkAddress");

        writer.String(conf->devices[i].linkAddress.toStdString().c_str());

        writer.Key("ASDUAddress");

        writer.String(conf->devices[i].ASDUAddress.toStdString().c_str());
//        std::cout << "enter ASDUAddress for board " << i + 1 << endl;
//        cin >> a;
//        writer.Uint(a);

        writer.EndObject();

        writer.EndObject();

    }
    writer.EndArray();


    writer.Key("tags");
    writer.StartArray();

    for (int i = 0; i < conf->count; i++)
    {
        for (auto iter = documentFromFile["templates"].Begin(); iter != documentFromFile["templates"].End(); ++iter)
        {
            auto tag = iter->GetObject();

            writer.StartObject();

            writer.Key("type");
            writer.String("Instance");

            writer.Key("name");

            tmp = conf->devices[i].boardName.toStdString() + "." + tag["name"].GetString();

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
                    writer.Uint(connection["IOA"].GetUint() + (conf->offset * i));

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

    ui->lbStatus->setText("finished!");
}
