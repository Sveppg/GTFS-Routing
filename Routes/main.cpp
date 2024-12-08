//lib header
#include <vector>
#include <iostream>
#include <fstream>
#include <unordered_map>
//custom Header
#include "network.h"
#include "CSVReader.h"
#include "types.h"

#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
