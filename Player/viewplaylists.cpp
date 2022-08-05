#include "viewplaylists.h"
#include "ui_viewplaylists.h"
#include <QtSql>
#include <QFile>

ViewPlayLists::ViewPlayLists(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ViewPlayLists)
{
    ui->setupUi(this);

    QString st = this->stylesloader();
    if(st != nullptr)
    {
        setStyleSheet(st);
    }

    QStringList list;
    QSqlQuery query;
    query.prepare("SELECT * FROM musicplaylist;");
    if(query.exec())
    {
        while(query.next())
        {
            list.append(query.value(0).toString());
        }
    }

    if(list.isEmpty())
    {
        ui->listWidget->addItem("No PlayList Found");
        ui->listWidget->setEnabled(false);
    }
    else
    {
        ui->listWidget->clear();
        ui->listWidget->addItems(list);
    }
}

ViewPlayLists::~ViewPlayLists()
{
    delete ui;
}

void ViewPlayLists::on_listWidget_currentTextChanged(const QString &currentText)
{
    QString musicnames;
    QSqlQuery query; query.prepare("SELECT * FROM musicplaylist WHERE PlayListName = '"+currentText+"';");
    if(query.exec())
    {
      while(query.next())
      {
          musicnames = query.value(1).toString();
          pathMusic = query.value(2).toString();
      }
    }

    if(musicnames != nullptr)
    {
        ui->listWidget_2->clear();
        ui->listWidget_2->addItems(musicnames.split("~"));
    }
    else
    {
        ui->listWidget_2->addItem("No Music file found on this playlist!");
    }
}


void ViewPlayLists::on_pushButton_clicked()
{
   QFile myfile("temp.txt");
   myfile.open(QIODevice::WriteOnly|QFile::Text);
   if(myfile.isOpen())
   {
       myfile.write(this->pathMusic.toLocal8Bit());
       myfile.flush();
       myfile.close();
       this->close();
   }
}

