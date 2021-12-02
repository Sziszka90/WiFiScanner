#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "chart.h"

#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    ,ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle("Wifi Scanner");

    Chart* chart = new Chart(this);

    setCentralWidget(chart);

    resize(800, 90);

}

MainWindow::~MainWindow()
{
    delete ui;
}


