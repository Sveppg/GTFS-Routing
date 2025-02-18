#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "types.h"
#include "network.h"
#include "renderarea.h"
//#include <iomanip>
#include <QDebug>
#include <QGraphicsProxyWidget>
#include <ostream>
#include <iostream>
//erstellung dres fenste
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , network(nullptr), // Initialize network to nullptr

    //für Graphen
    renderArea(new RenderArea)
{
    ui->setupUi(this);
    gtfsDirectory = "/home/tw3/Code/OOP/Assignments/SomSem24/Qt/Graphing/build/Desktop-Debug/GTFSShort";

    network = new bht::Network(gtfsDirectory);
    ui->comboBox->addItem(QString());
    for (const auto& route : network->getRoutes())
    {
        ui->comboBox->addItem(
            QString::fromStdString(network->getRouteDisplayName(route)), // Convert std::string to QString
            QVariant(QString::fromStdString(route.id))                   // Convert std::string to QString for QVariant
            );
    }
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(ui->comboBox);
    layout->addWidget(ui->comboBox_fahrt);
    layout->addWidget(ui->searchLineEdit);
    layout->addWidget(ui->tableWidget);
    layout->addWidget(ui->widget);

    ui->tableWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    ui->tableWidget->setFixedHeight(250);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);
    // Setze die Header für die Tabelle
    QStringList headers = {"Nr. ", "Name Haltestelle:  ", "Ankunftszeit: ", "Abfahrtszeit: "};
    ui->tableWidget->setColumnCount(headers.size());       // Anzahl der Spalten festlegen
    ui->tableWidget->setHorizontalHeaderLabels(headers);   // Header-Texte setzen
    ui->widget->setNetworkData(network->routeShapes, network->routeColors);

}



MainWindow::~MainWindow()
{
    delete network; // Clean up dynamically allocated memory
    delete ui;
}

void MainWindow::on_comboBox_currentTextChanged(const QString &arg1)
{
    (void)arg1;
    ui->comboBox_fahrt->clear();
    //auto rout =  ui->comboBox->currentText();
    auto route =  ui->comboBox->currentText();       //get text from combobox
    ui->comboBox_fahrt->addItem(QString(route));     //display in combobox_fahrt

    // Get the ID associated with the selected item
    QVariant id = ui->comboBox->currentData();      //get id from combobox

    // Check if the data is valid
    if (id.isValid()) {
        std::string routeId = id.toString().toStdString();
        for (auto trip: network->getTripsForRoute(routeId)){
            ui->comboBox_fahrt->addItem(QString::fromStdString(network->getTripDisplayName(trip)),
                                        QVariant(QString::fromStdString(trip.id))
                                        );
        };
        //    qDebug() << "Selected ID:" << routeId.to_string();

    } else {
        qDebug() << "No valid ID associated with this item.";
    }
}


void MainWindow::on_comboBox_fahrt_currentTextChanged(const QString &arg1)
{
    (void)arg1;
    ui->tableWidget->clearContents();   //refresh table
    ui->tableWidget->setRowCount(0); // Remove all rows
    QString        trip = ui->comboBox_fahrt->currentText();       //get text from combobox_fahrt
    QVariant    trip_id = ui->comboBox_fahrt->currentData();       //get id from combobox_fahrt
    QString        rout = ui->comboBox->currentText();             //get text from combobox
    QVariant    rout_id = ui->comboBox->currentData();             //get id from combobox
    // Check if the data is valid
    if (trip_id.isValid()) {

        std::string tripId = trip_id.toString().toStdString();
        std::string routId = rout_id.toString().toStdString();

        std::cout << "Neue Route: " << routId << std::endl;
        ui->widget->setHighlight(routId); // `ui->widget` ist die `RenderArea`


        for (auto stoptime : network->getStopTimesForTrip(tripId)){
            std::string arrivalTime     = network->castTime(stoptime.arrivalTime);
            std::string departureTime   = network->castTime(stoptime.departureTime);
            std::string stopSequence    = std::to_string(stoptime.stopSequence);
            auto stop            = network->getStopById(stoptime.stopId);
            int row = ui->tableWidget->rowCount();  // Get the current number of rows
            ui->tableWidget->insertRow(row);       // Insert a new row
            // Add items to the cells
            ui->tableWidget->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(stopSequence)));
            ui->tableWidget->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(stop.name)));
            ui->tableWidget->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(arrivalTime)));
            ui->tableWidget->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(departureTime)));
        };


    }
    else {
        qDebug() << "No valid ID found in the comboxes.";
    }

}


void MainWindow::on_searchLineEdit_textChanged(const QString &arg1)
{
    (void)arg1; // for unused paramter arg1
    ui->tableWidget->clearContents();   //refresh table
    ui->tableWidget->setRowCount(0); // Remove all rows

    QString        trip = ui->comboBox_fahrt->currentText();       //get text from combobox_fahrt
    QVariant    trip_id = ui->comboBox_fahrt->currentData();       //get id from combobox_fahrt
    QString        rout = ui->comboBox->currentText();             //get text from combobox
    QVariant    rout_id = ui->comboBox->currentData();             //get id from combobox
    QString serchText = ui->searchLineEdit->text();

    if (trip_id.isValid()) {
        std::string tripId = trip_id.toString().toStdString();
        std::string routId = rout_id.toString().toStdString();

        for (auto stoptime : network->searchStopTimesForTrip(serchText.toStdString(),tripId)){
            std::string arrivalTime     = network->castTime(stoptime.arrivalTime);
            std::string departureTime   = network->castTime(stoptime.departureTime);
            std::string stopSequence    = std::to_string(stoptime.stopSequence);
            auto stop            = network->getStopById(stoptime.stopId);
            int row = ui->tableWidget->rowCount();  // Get the current number of rows
            ui->tableWidget->insertRow(row);       // Insert a new row
            // Add items to the cells
            ui->tableWidget->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(stopSequence)));
            ui->tableWidget->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(stop.name)));
            ui->tableWidget->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(arrivalTime)));
            ui->tableWidget->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(departureTime)));


        }
    }else{
        qDebug() << "No valid ID found in the lineEdited.";
    }
}



