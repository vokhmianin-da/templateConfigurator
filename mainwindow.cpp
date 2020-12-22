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
    }
}
