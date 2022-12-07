#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTimer>
#include <QThread>
#include <QMainWindow>
#include <QMessageBox>
#include <QStandardItemModel>

#include "sourceweatherapi.h"
#include "dialogshowrestdb.h"
#include "networkcapability.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    //enum
    enum columns{temp=0, feels, latitud, longitud, country, place, icon, pressure, humidity};

private slots:
    void gettingData(QNetworkReply*);
    void verifyEntityData(const QByteArray&);
    void showingDBRest(QNetworkReply*);
    void changeStateComponent(int);
    void afterTimeOut();
    void on_actionStop_Report_triggered();
    void on_actionStart_Reports_triggered();
    void on_actionSend_Report_triggered();
    void on_actionShow_Reports_triggered();
    void on_onlineBtn_clicked();

private:
    Ui::MainWindow *ui;
    //declaring instances
    DialogShowRESTDB *dialogDBREST;
    networkCapability *networkLayer;

    sourceWeatherAPI *weatherSource;
    QStandardItemModel *model;

    QTimer *timer;
    QStringList city;

    static int row;

    bool reporting = false;
    bool isReported = false;
    bool isServerUp = false;
    bool btnOnlineOne = false;
};
#endif // MAINWINDOW_H
