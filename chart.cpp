#include "chart.h"

QT_CHARTS_USE_NAMESPACE

Chart::Chart(QWidget *parent) : QWidget(parent)
{
    chart = new QChart();
    set = new QtCharts::QBarSet("Signal Strength");
    series = new QtCharts::QBarSeries();
    axisX = new QValueAxis();
    axisY = new QBarCategoryAxis();
    chartView = new QChartView(chart);
    mainLayout = new QGridLayout;

    wifi = new Wifi;
    netlink = new Netlink;

    wifiscanner = new WifiScanner;
    sig = new std::vector<Signals>;

    button = new QPushButton("Scan", this);

    button->setGeometry(QRect(QPoint(300, 20), QSize(200, 50)));
    connect(button,&QPushButton::clicked,this, &Chart::updateChart);

    chartCreated = false;
}

void Chart::updateChart()
{
    if(chartCreated)
    {
        resetChart();
    }

    sig->clear();

    doScanning();

    createChart();

    chartCreated = true;
}

void Chart::resetChart()
{
    delete set;

    set = new QtCharts::QBarSet("Signal Strength");
    chart->removeAllSeries();
    chart->removeAxis(axisX);
    chart->removeAxis(axisY);
    series = new QtCharts::QBarSeries();

    axisY->clear();

    categoriesY.clear();
}

int Chart::doScanning()
{
    netlink->id = wifiscanner->initNl80211(netlink, wifi);

    wifiscanner->getWifiStatus(netlink, wifi);

    if (netlink->id < 0)
    {
        std::cout<<"Error initializing netlink 802.11\n"<<std::endl;
        wifiscanner->deleteNetlink(netlink);
        return -1;
    }

    int err = wifiscanner->doScanTrigger(netlink, wifi, sig);
    if (err != 0) {
        std::cout<<"do_scan_trigger() failed with"<< err <<std::endl;
        wifiscanner->deleteNetlink(netlink);
        return err;
    }

    wifiscanner->deleteNetlink(netlink);
    return 0;
}

void Chart::createChart()
{
    std::sort (sig->begin(), sig->end(), ([](Signals sig1, Signals sig2) { return sig1.signalStrength > sig2.signalStrength; }));

    for(int i=0; i<checkMaxSignals(sig);++i)
    {
        *set << (*sig)[i].signalStrength;
    }

    series->append(set);

    chart->addSeries(series);
    chart->setTitle("Wifi Access Points");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    for(int i = 0; i<checkMaxSignals(sig);++i)
    {
        QString qstr = QString::fromStdString((*sig)[i].name);

        categoriesY << qstr;
    }

    axisX->setRange(0,80.0);
    axisY->append(categoriesY);
    axisY->setLabelsAngle(90);

    chart->addAxis(axisY, Qt::AlignBottom);
    chart->addAxis(axisX, Qt::AlignLeft);
    series->attachAxis(axisY);
    series->attachAxis(axisX);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->resize(700,600);

    mainLayout->addWidget(chartView, 1, 1);

    button->setText("Refresh");

    mainLayout->addWidget(button, 2, 1);
    setLayout(mainLayout);

}

int Chart::checkMaxSignals(std::vector<Signals> *sig)
{
    int maxCount = (int)(*sig).size();

    if(maxCount>15)
    {
        maxCount = 15;
    }

    return maxCount;
}
