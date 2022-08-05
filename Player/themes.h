#ifndef THEMES_H
#define THEMES_H

#include <QDialog>
#include <QtSql>
#include "stylesloader.h"
#include <QListWidgetItem>

namespace Ui {
class Themes;
}

class Themes : public QDialog, public StylesLoader
{
    Q_OBJECT

public:
    explicit Themes(QWidget *parent = nullptr);
    ~Themes();
    void collectionStylesReader();

private slots:
    void on_listWidget_itemClicked(QListWidgetItem *item);

private:
    Ui::Themes *ui;
    QStringList themes;
};

#endif // THEMES_H
