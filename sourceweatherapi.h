#ifndef SOURCEWEATHERAPI_H
#define SOURCEWEATHERAPI_H

#include <QObject>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

class sourceWeatherAPI : public QObject
{
    Q_OBJECT
public:
    explicit sourceWeatherAPI(QObject *parent = nullptr);
    ~sourceWeatherAPI();
    //methods
    void setAPIKey(const QString& newKey);
    void setQuery(const QString &newQuery);
    void performQuery();
    void makeReport(const QVariantMap&, const QDateTime&, const QString &);
    void performRESTdb();
    void setCounter(int);

    const QString readAPIKeyFomFile();
    const QStringList getCitiesFromFile();

private slots:
    void slotFinished(QNetworkReply*);
    void manageDBRest(QNetworkReply*);

signals:
    void finishedData(QNetworkReply*);
    void sendingDBRest(QNetworkReply*);

private:
    QNetworkAccessManager *manageSourceWeather;
    QString APIKey;
    QString query;
    QString endPoint;
    static int counter;
};

#endif // SOURCEWEATHERAPI_H
