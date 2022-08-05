#include "playlist.h"
#include "ui_playlist.h"
#include <QFile>


PlayList::PlayList(QWidget *parent, QString *playlistname) :
    QDialog(parent),
    ui(new Ui::PlayList)
{
    ui->setupUi(this);

    this->playListName = *playlistname;

    this->setStyleSheet(" QLineEdit { "
                            " border-radius: 8px; "
                            " color:rgb(0, 0, 0); "
                            " background-color: rgb(255, 255, 255); } "

                            " QLineEdit:focus { "
                            " border:4px outset; "
                            " border-radius: 8px; "
                            " border-color: rgb(41, 237, 215); "
                            " color:rgb(0, 0, 0); "
                            " background-color: rgb(150, 150, 150); } "
                       );
    QString st = this->stylesloader();
    if(st != nullptr)
    {
        setStyleSheet(st);
    }
}

PlayList::~PlayList()
{
    delete ui;
}

void PlayList::on_lineEdit_returnPressed()
{
    if(this->playListName == "online")
    {
        QString line = ui->lineEdit->text();
        if(line != nullptr)
        {
            QFile my("online.txt");
            my.open(QIODevice::WriteOnly);
            if(my.isOpen())
            {
                my.write(line.toLocal8Bit());
                my.flush();
                my.close();
            }
            this->close();
        }
    }
    else
    {
            QString line = ui->lineEdit->text();
            if(line != nullptr)
            {
                QFile my("temp.txt");
                my.open(QIODevice::WriteOnly);
                if(my.isOpen())
                {
                    my.write(line.toLocal8Bit());
                    my.flush();
                    my.close();
                }
                this->close();
            }
    }
}

