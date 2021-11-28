#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <iostream>

QT_CHARTS_USE_NAMESPACE


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    ,ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    button = new QPushButton("Scan", this);
    chart = new QChart();
    set = new QtCharts::QBarSet("Signal Strength");
    series = new QtCharts::QBarSeries();
    axis = new QBarCategoryAxis();
    chartView = new QChartView(chart);

    button->setGeometry(QRect(QPoint(300, 20), QSize(200, 50)));

    connect(button,SIGNAL(clicked()),this, SLOT(updateChart()));

    this->setWindowTitle("Wifi Scanner");
    resize(800, 90);

}

void MainWindow::setSignals(std::vector<Signals>* signal)
{
    sig = signal;
}

void MainWindow::updateChart()
{

    delete button;
    ui->setupUi(this);

    this->setWindowTitle("Wifi Scanner");

    for(int i=0; i<checkMaxSignals(sig);++i)
    {
        *set << (*sig)[i].signalStrength;
    }

    series->append(set);

    chart->addSeries(series);
    chart->setTitle("Wifi Access Points");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    for(int i=0; i<checkMaxSignals(sig);++i)
    {
        std::string str((char*)(*sig)[i].name);
        QString qstr = QString::fromStdString(str);

        categories << qstr;
    }

    axis->append(categories);
    axis->setLabelsAngle(90);

    chart->createDefaultAxes();
    chart->addAxis(axis, Qt::AlignBottom);

    series->attachAxis(axis);

    chartView->setParent(ui->horizontalFrame);
    chartView->resize(800,600);

    update();

}

int MainWindow::checkMaxSignals(std::vector<Signals> *sig)
{
    int maxCount = (int)(*sig).size();

    if(maxCount>10)
    {
        maxCount = 10;
    }

    return maxCount;
}

MainWindow::~MainWindow()
{
    delete ui;
}
