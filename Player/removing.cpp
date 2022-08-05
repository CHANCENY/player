#include "removing.h"
#include "ui_removing.h"
#include <QtSql>
#include <QMessageBox>
#include <QFile>

Removing::Removing(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Removing)
{
    ui->setupUi(this);

    QString st = this->stylesloader();
    if(st != nullptr)
    {
        setStyleSheet(st);
    }

    QStringList list;
    QSqlQuery query; query.prepare("SELECT * FROM musicplaylist;");
    if(query.exec())
    {
        while(query.next())
        {
           list.append(query.value(0).toString());
        }
    }

    if(!list.isEmpty())
    {
        ui->listWidget->addItems(list);
    }
    else
    {
        ui->listWidget->addItem("No Playlist Found!");
        ui->listWidget->setEnabled(false);
    }

    // QSqlQuery query; query.prepare("SELECT * FROM musicplaylist WHERE PlayListName ");

}

Removing::~Removing()
{
    delete ui;
}

void Removing::on_listWidget_itemClicked(QListWidgetItem *item)
{
    QString current = ui->listWidget->currentItem()->text();
    QString musicpath = nullptr;

    if(current != nullptr)
    {
      int ok = QMessageBox::question(0,"Deleting Playlist","Are you sure you want to delete this playlist ?","Yes","No","Cancel",1,-1);

      if(ok == 0)
      {
          QSqlQuery query; query.prepare("SELECT * FROM musicplaylist WHERE PlayListName='"+current+"';");
          if(query.exec())
          {
              while(query.next())
              {
                  musicpath = query.value(2).toString();
              }
          }

          QStringList path = musicpath.split("~");
          int flag = 0;

          for(int i = 0; i < path.size(); i++)
          {
              QFile::remove(path[i]);
              flag++;
          }

          if(flag == path.size())
          {
              query.prepare("DELETE FROM musicplaylist WHERE PlayListName ='"+current+"';");
              if(query.exec())
              {
                  flag = -1;
              }
          }

          if(flag == -1)
          {
              query.prepare("SELECT * FROM musicplaylist;");
              if(query.exec())
              {
                  QStringList listed;
                  while(query.next())
                  {
                     listed.append(query.value(0).toString());
                  }

                  if(!listed.isEmpty())
                  {
                      ui->listWidget->clear();
                      ui->listWidget->addItems(listed);
                  }
                  else
                  {
                      ui->listWidget->clear();
                      ui->listWidget->addItem("PlayLists Not Found!");
                  }
              }
          }
      }
    }
}

