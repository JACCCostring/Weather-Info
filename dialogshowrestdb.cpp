#include "dialogshowrestdb.h"
#include "ui_dialogshowrestdb.h"

DialogShowRESTDB::DialogShowRESTDB(QWidget *parent, QNetworkReply* newReply) :
    QDialog(parent),
    ui(new Ui::DialogShowRESTDB)
{
    ui->setupUi(this);
    //code here
    //init... all models and variables
    mainWeatherAPI = new sourceWeatherAPI(this);//to get updates from MainWindows
    managerRESTConsult = new QNetworkAccessManager(this); //its need for asking json data
    model = new QStandardItemModel(0, 1, this);//its need for asking json data to list reports
    modelPlacesView = new QStandardItemModel(0, 1, this); //its need for asking json data to list places/countries
    reply = newReply;
    isOpen = true;

    QByteArray dataFromJson = reply->readAll();
    QJsonDocument document = QJsonDocument::fromJson(dataFromJson); //getting data from json
    QJsonObject mainObj = document.object(); //getting obj of QJsonDocument obj

    ui->listView->setModel(model);
    ui->placesListView->setModel(modelPlacesView);
    //UI things
    setWindowTitle("Reports");
    ui->listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->placesListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //logic bussiness
    QStringList reportList = mainObj.keys();
    listAllKeys(reportList, *model); //listing all keys in json file requeried QStringList and QStandardItemModel
    //connecting
    connect(managerRESTConsult, SIGNAL(finished(QNetworkReply*)), this, SLOT(showRESTItems(QNetworkReply*)));
}

DialogShowRESTDB::~DialogShowRESTDB()
{
    qDebug()<<"Destructing DialogShowRESTDB";
    delete ui;
}

void DialogShowRESTDB::listAllKeys(const QStringList &list,  QStandardItemModel& newModel)
{
    newModel.clear();//clearing hole model before populate with more data

    int row = 0; //for rows to iterate with model

    foreach(const QString &keys, list){
            newModel.setItem(row, new QStandardItem(keys));
            row++;
    }
}

bool DialogShowRESTDB::getStatus()
{
    return isOpen;
}

void DialogShowRESTDB::setStatus(bool status)
{
    isOpen = status;
}

void DialogShowRESTDB::showRESTItems(QNetworkReply *replyData)
{
    QByteArray dataInJson = replyData->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(dataInJson);
    QJsonObject obj = doc.object();

    QStringList list = obj.keys();
    listAllKeys(list, *modelPlacesView);
    qDebug()<<"number of rows: "<<list.size();
}

void DialogShowRESTDB::showRESTItemsPlace(QNetworkReply *replyPlace)
{
    QByteArray dataInJson = replyPlace->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(dataInJson);
    QJsonObject obj = doc.object();
    QVariantMap variant;

    variant["temp"] = obj["temp"].toDouble();
    variant["feels_like"] = obj["feels"].toDouble();
    variant["lat"] = obj["latitud"].toDouble();
    variant["lo"] = obj["longitud"].toDouble();
    variant["pressure"] = obj["pressure"].toString();
    variant["humidity"] = obj["humidity"].toInt();
    variant["icon"] = obj["icon"].toString();
    variant["place"] = obj["place"].toString();
    variant["country"] = obj["country"].toString();
    //instancs and init of new dialog windows
    DialogFormPlaces = new Dialog(this, variant);
    DialogFormPlaces->show();
}

void DialogShowRESTDB::on_listView_doubleClicked(const QModelIndex &index)
{
    record = index.data().toString();
    QString endPoint = tr("https://petsdb-6df06-default-rtdb.europe-west1.firebasedatabase.app/%1.json").arg(record);
    endPoint.replace(" ", "%20"); //replacing spaces with %20 to parse raw url to valid json url
    //asking for more json data
    managerRESTConsult->get(QNetworkRequest(QUrl(endPoint)));
}


void DialogShowRESTDB::on_placesListView_doubleClicked(const QModelIndex &index)
{
    QNetworkAccessManager *managerRESTConsultPlace = new QNetworkAccessManager(this);

    QString place = index.data().toString();
    QString endPoint = tr("https://petsdb-6df06-default-rtdb.europe-west1.firebasedatabase.app/%1/%2.json").arg(record, place);
    endPoint.replace(" ", "%20"); //replacing spaces with %20 to parse raw url to valid json url
    //asking for more json data
    managerRESTConsultPlace->get(QNetworkRequest(QUrl(endPoint)));
    //connecting
    connect(managerRESTConsultPlace, SIGNAL(finished(QNetworkReply*)), this, SLOT(showRESTItemsPlace(QNetworkReply*)));
}

