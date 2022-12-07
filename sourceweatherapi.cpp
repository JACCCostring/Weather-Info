#include "sourceweatherapi.h"

int sourceWeatherAPI::counter = 0; //init... static variable out of the class

sourceWeatherAPI::sourceWeatherAPI(QObject *parent) : QObject(parent)
{
    manageSourceWeather = new QNetworkAccessManager(this);
    //connecting
    connect(manageSourceWeather, SIGNAL(finished(QNetworkReply*)), this, SLOT(slotFinished(QNetworkReply*)));
}

sourceWeatherAPI::~sourceWeatherAPI(){
    qDebug()<<"Destructing SourceWeatherAPI";
}
void sourceWeatherAPI::setAPIKey(const QString &newKey)
{
    APIKey = newKey;
}

void sourceWeatherAPI::setQuery(const QString &newQuery)
{
    query = newQuery;
}

void sourceWeatherAPI::performQuery()
{
    //setting endpoint
    endPoint = tr("https://api.openweathermap.org/data/2.5/weather?q=%1&appid=%2").arg(query, APIKey);
    //getting data from endpoint
    manageSourceWeather->get(QNetworkRequest(QUrl(endPoint)));
}

void sourceWeatherAPI::makeReport(const QVariantMap &report, const QDateTime &date, const QString &country)
{
    QNetworkRequest request;

    QJsonDocument jsonReport = QJsonDocument::fromVariant(report);

    QString endPointReport =
            tr("https://petsdb-6df06-default-rtdb.europe-west1.firebasedatabase.app/%1/%2/.json").arg(date.toString(), country);

    request.setUrl(QUrl(endPointReport));
    request.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));

    manageSourceWeather->put(request, jsonReport.toJson());
}

void sourceWeatherAPI::performRESTdb()
{
    QNetworkAccessManager *manageRestDB = new QNetworkAccessManager(this);

    QString endPointForDB = "https://petsdb-6df06-default-rtdb.europe-west1.firebasedatabase.app/.json";

    manageRestDB->get(QNetworkRequest(QUrl(endPointForDB)));

    connect(manageRestDB, SIGNAL(finished(QNetworkReply*)), this, SLOT(manageDBRest(QNetworkReply*)));
}

void sourceWeatherAPI::setCounter(int newCount)
{
    counter = newCount;
}

const QString sourceWeatherAPI::readAPIKeyFomFile()
{
    QFile fileToRead(":/Source/Files/APIKey.txt");

    if(!fileToRead.open(QIODevice::ReadOnly | QIODevice::Text))
        qDebug()<<"System can not read File!";

    QString key = fileToRead.readLine();

    return key;
}

const QStringList sourceWeatherAPI::getCitiesFromFile()
{
    QFile fileToReadCities(":/Source/Files/countryList.txt");
    QStringList citiesList;

    if(!fileToReadCities.open(QIODevice::ReadOnly | QIODevice::Text))
        qDebug()<<"System can not open File!";

    while(!fileToReadCities.atEnd()){
            QString city = fileToReadCities.readLine().replace("\n", "");
            citiesList.append(city);
    }

    return citiesList;
}

void sourceWeatherAPI::slotFinished(QNetworkReply *finishedReply)
{
    emit finishedData(finishedReply);
}

void sourceWeatherAPI::manageDBRest(QNetworkReply *reply)
{
    emit sendingDBRest(reply);
}
