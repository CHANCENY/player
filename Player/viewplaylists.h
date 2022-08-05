#ifndef VIEWPLAYLISTS_H
#define VIEWPLAYLISTS_H

#include <QDialog>
#include "stylesloader.h"

namespace Ui {
class ViewPlayLists;
}

class ViewPlayLists : public QDialog, public StylesLoader
{
    Q_OBJECT

public:
    explicit ViewPlayLists(QWidget *parent = nullptr);
    ~ViewPlayLists();

private slots:
    void on_listWidget_currentTextChanged(const QString &currentText);

    void on_pushButton_clicked();

private:
    Ui::ViewPlayLists *ui;
    QString pathMusic;
};

#endif // VIEWPLAYLISTS_H
