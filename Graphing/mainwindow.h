#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsProxyWidget>
#include <QGraphicsView>
//header hinzufügen
#include "types.h"
#include "network.h"
//#include "CSVReader.h"
#include "renderarea.h"

QT_BEGIN_NAMESPACE
//Namepsace für UI /*Vorher da*/
namespace Ui {
class MainWindow;
}
//namespace für unsere klasse
namespace bht{
class Network;
}
QT_END_NAMESPACE
//Haupt Fenster erstellen
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    // Only needed for Showing all Data of GTFS
    //void on_pushButton_clicked();

    //void on_resetPushButton_clicked();

    //void on_searchLineEdit_returnPressed();

    void on_comboBox_currentTextChanged(const QString &arg1);

    void on_comboBox_fahrt_currentTextChanged(const QString &arg1);

    void on_searchLineEdit_textChanged(const QString &arg1);

    //void setRenderAreaData();


private:
    Ui::MainWindow *ui;
    //verwendete Variablen
    bht::Network *network;
    bht::Stop *stop;
    std::string gtfsDirectory;
    std::vector<bht::Stop> search(std::string& query);
    //eventuell für Path implement
    //std::vector<bht::Stop> getTravelPath(const std::string &fromStopId, const std::string& toStopId);

    QGraphicsView *graphicsView;
    QGraphicsScene *scene;
    RenderArea *renderArea;

};
#endif // MAINWINDOW_H
