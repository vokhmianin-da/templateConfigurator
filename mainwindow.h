#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "configjson.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    configJson *conf;
    QString filter;  //фильтр форматов файлов
    ~MainWindow();

private slots:
    void on_pbSelectFile_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
