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

    ~MainWindow();

private slots:
    void on_pbSelectFile_clicked();

    void on_cbIsTemplate_clicked();

    void on_pbSetLinkASDU_clicked();

    void on_cbDevices_currentIndexChanged(int index);

    void on_pbResetLinkASDU_clicked();

    void on_pbSelectTemplate_clicked();

    void on_pbCreateTemplate_clicked();

    void on_pbCreateJson_clicked();

private:
    Ui::MainWindow *ui;
    configJson *conf;
    QString filter;  //фильтр форматов файлов
    using funcVector = std::vector <void (*)(MainWindow *)>;
    funcVector createTemplateFunctions;
    funcVector createMasterJSONFunctions;

    /*Функции для создания template*/
    static void createTemplateIEC101(MainWindow *mainWindow);
    static void createTemplateIEC104(MainWindow *mainWindow);

    /*Функции для создания template*/
    static void createMasterJSONIEC101(MainWindow *mainWindow);
    static void createMasterJSONIEC104(MainWindow *mainWindow);

signals:
    void notOpenSlaveJson();
};

#endif // MAINWINDOW_H
