#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent, const QVariantMap&variant) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    //code here
    ui->lineTemp->setText(variant["temp"].toString());
    ui->lineFeels->setText(variant["feels_like"].toString());
    ui->lineCountry->setText(variant["country"].toString());
    ui->lineLatitud->setText(variant["lat"].toString());
    ui->lineHumidity->setText(variant["humidity"].toString());
    ui->lineIcon->setText(variant["icon"].toString());
    ui->lineLongitud->setText(variant["lo"].toString());
    ui->linePlace->setText(variant["place"].toString());
    ui->linePressure->setText(variant["pressure"].toString());
    //UI
    setWindowTitle("Places");
}

Dialog::~Dialog()
{
    qDebug()<<"Destructing Dialog";
    delete ui;
}
