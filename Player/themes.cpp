#include "themes.h"
#include "ui_themes.h"
#include <QMessageBox>

Themes::Themes(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Themes)
{
    ui->setupUi(this);
    ui->progressBar->setVisible(false);

    QString st = this->stylesloader();

    if(st != nullptr)
    {
        setStyleSheet(st);
    }

    this->collectionStylesReader();
}

Themes::~Themes()
{
    delete ui;
}

void Themes::collectionStylesReader()
{
    QSqlQuery query;
    query.prepare("SELECT * FROM  styles;");
    if(query.exec())
    {
        while(query.next())
        {
            ui->listWidget->addItem(query.value(0).toString());
            themes.append(query.value(0).toString());
        }
    }
}

void Themes::on_listWidget_itemClicked(QListWidgetItem *item)
{
    QSqlQuery query;
    int counter = 0;
    ui->progressBar->setVisible(true);
    ui->progressBar->setTextVisible(false);
    ui->progressBar->setMaximum(this->themes.size());
    ui->progressBar->setMinimum(counter);

    foreach(QString line, this->themes)
    {
        query.prepare("UPDATE styles SET Status = 'False' WHERE StyleName ='"+line+"';");
        if(query.exec())
        {
           counter++;
        }
        ui->progressBar->setValue(counter);
    }

    ui->progressBar->setValue(counter -1);

    query.prepare("UPDATE styles SET Status = 'True' WHERE StyleName = '"+ui->listWidget->currentItem()->text()+"';");
    if(query.exec())
    {
        ui->progressBar->setValue(counter);
        QMessageBox::information(this,"Update Notification","Theme change successfully please restart");
        this->close();
    }
}

