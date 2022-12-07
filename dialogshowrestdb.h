#ifndef DIALOGSHOWRESTDB_H
#define DIALOGSHOWRESTDB_H

#include <QDialog>
#include <QStandardItemModel>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include "sourceweatherapi.h"
#include "dialog.h"

namespace Ui {
class DialogShowRESTDB;
}

class DialogShowRESTDB : public QDialog
{
    Q_OBJECT

public:
    explicit DialogShowRESTDB(QWidget *parent = nullptr, QNetworkReply *newReply = nullptr);
    ~DialogShowRESTDB();
    //methods
    void listAllKeys(const QStringList&, QStandardItemModel&);
    bool getStatus();
    void setStatus(bool);

private slots:
    void showRESTItems(QNetworkReply *);
    void showRESTItemsPlace(QNetworkReply*);
    void on_listView_doubleClicked(const QModelIndex &index);
    void on_placesListView_doubleClicked(const QModelIndex &index);

private:
    Ui::DialogShowRESTDB *ui;
    //declaring from here
    Dialog *DialogFormPlaces;
    sourceWeatherAPI *mainWeatherAPI;

    QStandardItemModel *model, *modelPlacesView;
    QNetworkAccessManager *managerRESTConsult;
    QNetworkReply *reply;

    QString record;
    bool isOpen = false;
};

#endif // DIALOGSHOWRESTDB_H
