#ifndef CHART_H
#define CHART_H

#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QPushButton>
#include <QGridLayout>

#include "wifi_lib.h"
#include "wifiScanner.h"

class Chart : public QWidget
{
    Q_OBJECT
public:
    explicit Chart(QWidget *parent = nullptr);
    QtCharts::QBarSet* set;
    QtCharts::QBarSeries *series;
    QtCharts::QChart *chart;
    QStringList categoriesY;
    QStringList nameY;
    QtCharts::QChartView *chartView;
    QtCharts::QValueAxis *axisX;
    QtCharts::QBarCategoryAxis *axisY;
    QPushButton* button;
    QGridLayout* mainLayout;
private:
    std::vector<Signals>* sig;
    Netlink* netlink;
    Wifi* wifi;
    WifiScanner* wifiscanner;

    int checkMaxSignals(std::vector<Signals>* sig);
    void createChart();
    int doScanning();
    void resetChart();
    bool chartCreated;
signals:

};

#endif // CHART_H
