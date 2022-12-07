#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QVariantMap>
#include <QDebug>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr, const QVariantMap &variant=QVariantMap());
    ~Dialog();

private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
