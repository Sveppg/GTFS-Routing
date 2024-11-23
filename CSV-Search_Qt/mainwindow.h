#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPlainTextEdit>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include "network.h" // Ihre Netzwerkklasse

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void performSearch();
    void resetSearch();

private:
    QWidget *centralWidget;
    QPlainTextEdit *searchInput;
    QListWidget *resultsList;
    QPushButton *searchButton;
    QPushButton *resetButton;
    bht::Network network; // Netzwerkobjekt
};

#endif // MAINWINDOW_H
