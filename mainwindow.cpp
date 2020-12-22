#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QFileDialog>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    conf = new configJson;
    filter = trUtf8("Файл json(*.json)");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pbSelectFile_clicked()
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
           conf->configName.remove(".json");    //удаляем лишнее
           ui->lbStatus->setText("Файл выбран");
           ui->lbFileName->setText(conf->configName);
        }
        else
        {
            ui->lbStatus->setText("Неподходящий формат файла");
        }
    }
    else
    {
        ui->lbStatus->setText("Файл не выбран");
        ui->lbFileName->clear();
    }
}

void MainWindow::on_cbIsTemplate_clicked()
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

void MainWindow::on_pbSetLinkASDU_clicked()
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

void MainWindow::on_cbDevices_currentIndexChanged(int index)
{
    ui->leDeviceName->setText(conf->devices[index].boardName);
    ui->leLinkAddr->setText(conf->devices[index].linkAddress);
    ui->leASDUAddr->setText(conf->devices[index].ASDUAddress);
}

void MainWindow::on_pbResetLinkASDU_clicked()
{
    conf->devices.erase(conf->devices.begin() + ui->cbDevices->currentIndex()); //удаляем из vector
    conf->count--; //увеличиваем количество devices
    ui->lePlatsQuantity->setText(QString::number(conf->count)); //отображаем кол-во
    ui->cbDevices->removeItem(ui->cbDevices->currentIndex());   //удаляем из списка
    /*Отображаем новый текущий элемент*/
    ui->leDeviceName->setText(conf->devices[ui->cbDevices->currentIndex()].boardName);
    ui->leLinkAddr->setText(conf->devices[ui->cbDevices->currentIndex()].linkAddress);
    ui->leASDUAddr->setText(conf->devices[ui->cbDevices->currentIndex()].ASDUAddress);
}
