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
        createTemplateIEC104,
        createTemplateModbus
        },
    createMasterJSONFunctions{
        createMasterJSONIEC101,
        createMasterJSONIEC104,
        createMasterJSONModbus
        }
{
    ui->setupUi(this);
    conf = new configJson;
    filter = trUtf8("Файл json(*.json)");

    /*Можно вводить только целочисленные данные */
    ui->leLinkAddr->setValidator( new QIntValidator);
    ui->leASDUAddr->setValidator( new QIntValidator);
    connect(this, SIGNAL(notOpenSlaveJson()), this, SLOT(on_pbSelectFile_clicked()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pbSelectFile_clicked()  //выбор json-slave
{
    conf->configName = QFileDialog::getOpenFileName(this, tr("Выбрать slave.json"), QDir::current().path(), filter);
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
        temp.ASDUbrokenTime = ui->leASDUAddr->text();

        conf->devices.push_back(temp);  //добавляем новый device
        ui->cbDevices->insertItem(conf->count, conf->devices[conf->count].boardName);   //отображаем в списке
        conf->count++; //увеличиваем количество devices
        ui->lePlatsQuantity->setText(QString::number(conf->count)); //отображаем кол-во
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
            ui->leASDUAddr->setText(conf->devices[ui->cbDevices->currentIndex()].ASDUbrokenTime);
        }
    }
}

void MainWindow::on_pbSelectTemplate_clicked()  //выбор template
{
    conf->templateName = QFileDialog::getOpenFileName(this, tr("Выбрать template.json"), QDir::current().path(), filter);
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
            //conf->templateName.remove(".json");    //удаляем лишнее
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


void MainWindow::on_pbCreateJson_clicked()  //создание json-master
{
    createMasterJSONFunctions[ui->cbInterface->currentIndex()](this);
}

void MainWindow::on_cbDevices_activated(int index)
{
    if(!conf->devices.empty())
    {        
        ui->leDeviceName->setText(conf->devices[index].boardName);
        ui->leLinkAddr->setText(conf->devices[index].linkAddress);
        ui->leASDUAddr->setText(conf->devices[index].ASDUbrokenTime);
    }
    else
    {
        ui->leDeviceName->clear();
        ui->leLinkAddr->clear();
        ui->leASDUAddr->clear();
    }
}

void MainWindow::on_cbInterface_currentIndexChanged(int index)  //изменение имени столбца ASDU/brokenTime в зависимости от выбранного интерфейса
{
    if(index == 2)
    {
        ui->lbASDUbrokenTime->setText("brokenTime:");
    }
    else
    {
        ui->lbASDUbrokenTime->setText("ASDUAdress:"); //отображение ASDUAdress для IEC или brokenTime для Modbus
    }
}
