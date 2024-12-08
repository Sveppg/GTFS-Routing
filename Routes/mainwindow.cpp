#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "types.h"
#include "network.h"
//erstellung des fensters
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , network(nullptr) // Initialize network to nullptr
{
    ui->setupUi(this);
    gtfsDirectory = "/Users/svenemann/Code/OOP/Assignments/SomSem24/Qt/Routes/build/Desktop_x86_darwin_generic_mach_o_64bit-Debug/GTFSShort";

    // Dynamically allocate the network object
    network = new bht::Network(gtfsDirectory);
    ui->comboBox->addItem(QString());
    for (const auto& route : network->getRoutes())
    {
        ui->comboBox->addItem(
            QString::fromStdString(network->getRouteDisplayName(route)), // Convert std::string to QString
            QVariant(QString::fromStdString(route.id))                   // Convert std::string to QString for QVariant
            );
    }
}


MainWindow::~MainWindow()
{
    delete network;
    delete ui;
}

//pushButton action
void MainWindow::on_pushButton_clicked()
{
    //get text from seachLineEdit
    QString searchTerm = ui->searchLineEdit->text();
    std::string temp = searchTerm.toStdString();

    // Ensure network is valid before using it
    if (!network) {
        qWarning() << "Network object is not initialized!";
        return;
    }

    auto stops = network->search(temp);
    //ui->listWidget->clear(); // Clear the list widget before adding new items

    for (const auto& stop : stops) {
        QString itemText = QString("Stop ID: %1, Name: %2")
        .arg(QString::fromStdString(stop.id)) // Convert stop.id to QString
            .arg(QString::fromStdString(stop.name)); // Convert stop.name to QString

        QListWidgetItem* item = new QListWidgetItem(itemText);
        ui->listWidget->addItem(item);
    }
}

void MainWindow::on_resetPushButton_clicked()
{
   ui->listWidget->clear();
}


void MainWindow::on_searchLineEdit_returnPressed()
{
    //get text from seachLineEdite
    QString searchTerm = ui->searchLineEdit->text();
    std::string temp = searchTerm.toStdString();

    // Ensure network is valid before using it
    if (!network) {
        qWarning() << "Network object is not initialized!";
        return;
    }




    auto stops = network->search(temp);
    //ui->listWidget->clear(); // Clear the list widget before adding new items

    for (const auto& stop : stops) {
        QString itemText = QString("Stop ID: %1, Name: %2")
        .arg(QString::fromStdString(stop.id)) // Convert stop.id to QString
            .arg(QString::fromStdString(stop.name)); // Convert stop.name to QString

        QListWidgetItem* item = new QListWidgetItem(itemText);
        ui->listWidget->addItem(item);
    }
}



void MainWindow::on_comboBox_currentTextChanged(const QString &arg1) {
    (void)&arg1;
    ui->comboBox_fahrt->clear();
    //auto rout =  ui->comboBox->currentText();
    auto rout =  ui->comboBox->currentText();       //get text from combobox
    ui->comboBox_fahrt->addItem(QString(rout));     //display in combobox_fahrt

    // Get the ID associated with the selected item
    QVariant id = ui->comboBox->currentData();      //get id from combobox

    // Check if the data is valid
    if (id.isValid()) {
        std::string routeId = id.toString().toStdString();
        for (auto trip: network->getTripsForRoute(routeId)){
            ui->comboBox_fahrt->addItem(QString::fromStdString(network->getTripDisplayName(trip)));
        };
        qDebug() << "Selected ID:" << routeId;

    } else {
        qDebug() << "No valid ID associated with this item.";
    }
}

