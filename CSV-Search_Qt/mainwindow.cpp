#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), network("GTFSShort") {

// zentrales Layout
    centralWidget = new QWidget(this);
    auto *layout = new QVBoxLayout(centralWidget);

// Label-Definition für Sucheingabe
    auto *inputLabel = new QLabel("Welche Station suchst du?", centralWidget);
    layout->addWidget(inputLabel);

    searchInput = new QPlainTextEdit(centralWidget);
    layout->addWidget(searchInput);
    searchInput->setFixedSize(1080, 30);

// Resultate -> Liste
    resultsList = new QListWidget(centralWidget);
    layout->addWidget(resultsList);

// Search und Reset Knöpfe -> PushButton
    auto *buttonLayout = new QHBoxLayout();
    searchButton = new QPushButton("Search", centralWidget);
    resetButton = new QPushButton("Reset", centralWidget);
    buttonLayout->addWidget(searchButton);
    buttonLayout->addWidget(resetButton);
    layout->addLayout(buttonLayout);

// Signale verbinden von Knöpfen
    connect(searchButton, &QPushButton::clicked, this, &MainWindow::performSearch);
    connect(resetButton, &QPushButton::clicked, this, &MainWindow::resetSearch);

// Layout für Hauptfenster
    setCentralWidget(centralWidget);
    setWindowTitle("Station Search");
    resize(1100, 800);
}

void MainWindow::performSearch() {
    QString query = searchInput->toPlainText().trimmed();

    if (query.isEmpty()) {
        QMessageBox::warning(this, "ALARM!", "Bitte geben Sie einen Suchbegriff ein.");
        return;
    }

// Suchen -> Query to String
    auto stops = network.search(query.toStdString());

// Resultate Anzeigen -> Stop ID und Stop Name
    resultsList->clear();
    for (const auto &stop : stops) {
        QString itemText = QString("Stop ID: %1, Name: %2")
        .arg(QString::fromStdString(stop.id), QString::fromStdString(stop.name));
        resultsList->addItem(itemText);
    }

    if (stops.empty()) {
        QMessageBox::information(this, "Info", "Kein Resultate für die Suche.");
    }
}

void MainWindow::resetSearch() {
    searchInput->clear();
    resultsList->clear();
}
