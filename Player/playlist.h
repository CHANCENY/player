#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QDialog>
#include "stylesloader.h"

namespace Ui {
class PlayList;
}

class PlayList : public QDialog, public StylesLoader
{
    Q_OBJECT

public:
    explicit PlayList(QWidget *parent = nullptr, QString *playlistname = nullptr);
    ~PlayList();

private slots:
    void on_lineEdit_returnPressed();

private:
    Ui::PlayList *ui;
    QString playListName;
};

#endif // PLAYLIST_H
