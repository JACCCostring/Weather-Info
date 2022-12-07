#include "mainwindow.h"
#include "ui_mainwindow.h"

int MainWindow::row = 0; //init... static variable out of the class

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //code here
    weatherSource = new sourceWeatherAPI(this); //main obj to work with API
    networkLayer = new networkCapability(this);
    model = new QStandardItemModel(0, 7, this);
    timer = new QTimer(this);
    //getting key
    QString key = weatherSource->readAPIKeyFomFile();
    //setting key to obj
    weatherSource->setAPIKey(key);
    city = weatherSource->getCitiesFromFile(); //getting city from file early
    //adding list to the model
    model->setHorizontalHeaderLabels(QStringList()<<"Temp"<<"Feels Like"<<"Latitud"
                                                             <<"Longitud"<<"Country"<<"Place"<<"Icon"<<"Pressure"<<"Humidity");
    ui->tableView->setModel(model);
    //setting UI
    setWindowTitle("Weather Report");
    ui->lineAPIKey->setText(key); //setting key to UI
    ui->lineCity->setEnabled(false);
    ui->spinBox->setValue(1);//setting default value of spinBox
    ui->spinBox->setMinimum(1); //setting minimum
    ui->spinBox->setMaximum(3600); //maximum
    setContextMenuPolicy(Qt::ActionsContextMenu);
    addActions({ui->actionStop_Report, ui->actionStart_Reports, ui->actionSend_Report, ui->actionShow_Reports});
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setColumnWidth(columns::place, 200);
    //connecting
    connect(ui->checkboxLock, SIGNAL(stateChanged(int)), this, SLOT(changeStateComponent(int)));
    connect(weatherSource, SIGNAL(finishedData(QNetworkReply*)), this, SLOT(gettingData(QNetworkReply*)));
    connect(weatherSource, SIGNAL(sendingDBRest(QNetworkReply*)), this, SLOT(showingDBRest(QNetworkReply*)));
    connect(networkLayer, SIGNAL(entityData(QByteArray)), this, SLOT(verifyEntityData(QByteArray)));
    connect(timer, SIGNAL(timeout()), this, SLOT(afterTimeOut()));
    //checking to true now so it can make change on UI
    ui->checkboxLock->setChecked(true);
}

MainWindow::~MainWindow()
{
    delete ui;
    qDebug()<<"Destructing MainWindow";
    city.clear();
}

void MainWindow::gettingData(QNetworkReply *reply)
{
    QString icon;
    QByteArray data = reply->readAll();

    QJsonDocument document = QJsonDocument::fromJson(data);

    QJsonObject obj = document.object();

    QJsonObject coordObj = obj["coord"].toObject(); //coord in json file
    double latitud = coordObj["lat"].toDouble();
    double longitud = coordObj["lon"].toDouble();

    QJsonArray weatherObj = obj["weather"].toArray();

    foreach(const QJsonValue& value, weatherObj){ //iterating with json array
             icon = value.toObject().value("icon").toString();
    }

    QJsonObject mainObj = obj["main"].toObject(); //main in json file
    double temp = mainObj["temp"].toDouble() - 273.15;
    double feels = mainObj["feels_like"].toDouble() - 273.15;
    int pressure = mainObj["pressure"].toInt();
    int humidity = mainObj["humidity"].toInt();

    QJsonObject sysObj = obj["sys"].toObject(); //sys in json file
    QString country = sysObj["country"].toString();

    QString place = obj.value("name").toString(); //name in json file

    if(!reporting){ //if not reporting then populate model
    model->setItem(row, columns::temp, new QStandardItem(QString::number(temp)));
    model->setItem(row, columns::feels, new QStandardItem(QString::number(feels)));
    model->setItem(row, columns::latitud, new QStandardItem(QString::number(latitud)));
    model->setItem(row, columns::longitud, new QStandardItem(QString::number(longitud)));
    model->setItem(row, columns::country, new QStandardItem(country));
    model->setItem(row, columns::place, new QStandardItem(place));
    model->setItem(row, columns::icon, new QStandardItem(icon));
    model->setItem(row, columns::pressure, new QStandardItem(QString::number(pressure)+" hPa"));
    model->setItem(row, columns::humidity, new QStandardItem(QString::number(humidity)));
    row++;
    }
}

void MainWindow::verifyEntityData(const QByteArray &entityData)
{
   QList<QByteArray> cmd;//list for commands
   int interValue = 0;//for interval value
   QString operation;//to execute commands from clients
    isServerUp = true; //make it true if an entity connect to server
   if(entityData.contains("0015")){//checking who it is in network

        cmd = entityData.split(':');//splitting data

        if(cmd.size() > 2){//if start command is received
            interValue = cmd.at(1).toInt();//at position 1 is the interval
            operation = cmd.at(2);//at position 2 is the wanted operation
        }
        else{//if stop/send command is received
            operation = cmd.at(1);
        }

        if(operation.contains("start")){//checking operation to perform
            ui->spinBox->setValue(interValue);//setting interval to spinBox
            on_actionStart_Reports_triggered();//calling operation
        }

        else if(operation.contains("stop")){//checking operation to perform
            on_actionStop_Report_triggered();//calling operation
        }
        else if(operation.contains("send")){//checking operation to perform
            on_actionSend_Report_triggered();//calling operation
        }
    }
}

void MainWindow::changeStateComponent(int state)
{
    if(!state)
        ui->lineAPIKey->setEnabled(true);

    else
        ui->lineAPIKey->setEnabled(false);
}

void MainWindow::afterTimeOut()
{
    weatherSource->setQuery(city.at(row)); //setting city to search for
    ui->lineCity->setText(city.at(row)); //setting current city
    //writing back to client only if server is true
    if(isServerUp)//to avoid writing back to entity if still not connected to server
    networkLayer->writeToClient(QString::number(row+1).toUtf8() + "-");
    //calling to perform REST
    weatherSource->performQuery();

    if(row == 1030){
        on_actionStop_Report_triggered();
    }
}

void MainWindow::showingDBRest(QNetworkReply *dbRESTData)
{
    dialogDBREST = new DialogShowRESTDB(this, dbRESTData);//calling new Dialog windows
    dialogDBREST->show();
}

void MainWindow::on_actionStop_Report_triggered()
{
    row = 0; //restarting row to 0 again
    timer->stop(); //stoping timer
    ui->statusbar->showMessage("Stopped Reporting!");
    ui->spinBox->setEnabled(true);
}


void MainWindow::on_actionStart_Reports_triggered()
{
    if(!timer->isActive()){
    reporting = false;
    isReported = true;

    model->setRowCount(0);

    timer->setInterval(ui->spinBox->value()*1000); //setting interval to timer
    timer->start(); // calling timer every 5 seconds

    ui->lineCity->setEnabled(false); //disassembling lineEdit
    ui->spinBox->setEnabled(false);
    ui->statusbar->showMessage("Reporting...");
    }

    else{
        QMessageBox::information(nullptr, "Error", "System is already reporting...!");
        return;
    }
}

void MainWindow::on_actionSend_Report_triggered()
{
    //sending report to NoSQL DB
    QVariantMap report;
    int totalRow = model->rowCount();
    int howManyRows = 0;

    if(!timer->isActive() && isReported)//checking if timer it still running and reported is true
    {
         //notifying report
         ui->statusbar->showMessage("Sending Report...");

        for(int i=0; i<totalRow; i++){
        report["temp"] = model->index(i, columns::temp).data().toDouble();
        report["feels"] = model->index(i, columns::feels).data().toDouble();
        report["latitud"] = model->index(i, columns::latitud).data().toDouble();
        report["longitud"] = model->index(i, columns::longitud).data().toDouble();
        report["country"] = model->index(i, columns::country).data().toString();
        report["place"] = model->index(i, columns::place).data().toString();
        report["icon"] = model->index(i, columns::icon).data().toString();
        report["pressure"] = model->index(i, columns::pressure).data().toString();
        report["humidity"] = model->index(i, columns::humidity).data().toInt();
        //checking if its empty then dont append to report (QVariantMap)
        if(!model->index(i, columns::place).data().toString().isEmpty()){
        //put each REST
        weatherSource->makeReport(report, QDateTime::currentDateTime(), report["place"].toString());
        }
        else{
            howManyRows++;
        }
    }
    }

    else if(!isReported){
        QMessageBox::information(this, "Error", "No reports yet -> Start it first!");
        return;
    }
    else{//warning if timer is running and return control to current course
        QMessageBox::information(nullptr, "Error", "Still reporting... Operation no allowed");
        return;
    }
    //notifying UI that report is done
    ui->statusbar->showMessage("Report sent");
    reporting = true;
    isReported = false;

    report.clear(); //clearing variable after beeing used
    qDebug()<<"Total invalid rows: "<<howManyRows;
}


void MainWindow::on_actionShow_Reports_triggered()
{
    weatherSource->performRESTdb();
}

void MainWindow::on_onlineBtn_clicked()
{
    //some logic bussiness
    if(btnOnlineOne == false){
        //server start listening
        networkLayer->startServer();
        btnOnlineOne = true;

        ui->onlineBtn->setIcon(QIcon(":/icon/icon_disconnected.jpeg"));
        ui->statusbar->showMessage("Module online");
        //setting button back to dissable
    }

    else{//stopping server listening
        networkLayer->stopServer();
        isServerUp = false;
        btnOnlineOne = false;
        //UI colors on button
        ui->onlineBtn->setIcon(QIcon(":/icon/icon_connect_disconnect.png"));
        ui->statusbar->showMessage("Module offline");
    }
}

