#ifndef REMOVING_H
#define REMOVING_H

#include <QDialog>
#include <QListWidgetItem>
#include "stylesloader.h"

namespace Ui {
class Removing;
}

class Removing : public QDialog, public StylesLoader
{
    Q_OBJECT

public:
    explicit Removing(QWidget *parent = nullptr);
    ~Removing();


private slots:
    void on_listWidget_itemClicked(QListWidgetItem *item);

private:
    Ui::Removing *ui;
};

#endif // REMOVING_H
