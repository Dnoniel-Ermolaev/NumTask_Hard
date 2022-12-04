#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "MainWork2.h"
#include <QMainWindow>
#include <QChart>
#include <QChartView>
#include <QLineSeries>
#include <QtCore/QCoreApplication>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButtonMain2Run_clicked();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
