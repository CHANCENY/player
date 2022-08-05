#include "playervideos.h"
#include "ui_playervideos.h"
#include <QRandomGenerator>
#include <QFileDialog>
#include <QDir>
#include <QtMath>
#include <QAudioOutput>
#include <QAudioFormat>
#include <QMediaMetaData>
#include <QtSql>
#include <QFile>
#include <QMessageBox>
#include <QFileInfo>



static int index = 0;
static int playlistcapacity = 0;
static int lock = 0;

static int repeatSet = 0;



playerVideos::playerVideos(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::playerVideos)
{
    ui->setupUi(this);
    this->playlistDatabase();
    this->styleSave();

    QString st = this->stylesloader();
    if(st != nullptr)
    {
        setStyleSheet(st);
    }

    QDir dir;
    dir.mkdir("music");
    ui->progressBar->setVisible(false);
     ui->saving->setVisible(false);    
     this->fetchQuickPlayList();

    // initialize the player object class
    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput;
    offlineVideos = new QVideoWidget;

    //these are images of default if when audio load does not have image
    defaultNodeImages = {":/n/nodes/itunes.png",":/n/nodes/music (1).png",":/n/nodes/music-note.png",":/n/nodes/music-notes.png"
                        ,":/n/nodes/music.png",":/n/nodes/musical-note (1).png",":/n/nodes/musical-note.png",":/n/nodes/spotify.png"};


    //insert one in displayer label at random selection
    int x = QRandomGenerator::global()->bounded(0,defaultNodeImages.size() - 1);
    ui->imageDisplayer->setPixmap(QPixmap(defaultNodeImages.at(x)));

    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(durationplaying()));
    timer->start(5000);

    timers = new QTimer(this);
    connect(timers,SIGNAL(timeout()),this,SLOT(playerFunction()));
    timers->start(1000);
}

playerVideos::~playerVideos()
{
    delete ui;
}

QString playerVideos::calculateFileSize(int sizes)
{

    if(sizes <= 1000000)
    {
      return QString::number(sizes / 1024)+" KB";
    }
    if(sizes > 1000000)
    {
       return QString::number(sizes / pow(1024,2))+" MB";
    }
    if(sizes > 1073741824)
    {
       return QString::number(sizes / pow(1024,3))+" GB";
    }
    return nullptr;
}

void playerVideos::extracteMetaData()
{
   extern Q_CORE_EXPORT int qt_ntfs_permission_lookup;
   QFileInfo info(this->musicFile[index]);
   qt_ntfs_permission_lookup++;
   QString title = info.baseName();
   QString date = info.birthTime().toString("dd-MM-yy");
   QString sizes = this->calculateFileSize(info.size());
   QString type = info.suffix();
   QString owner = info.owner();

   ui->titleMusic->setText(title);
   ui->sizeMusic->setText(sizes);
   ui->typeMusic->setText(type);
   ui->dateMusic->setText(date);
   ui->MusicSytemowner->setText(owner);

   QMediaMetaData meta = player->metaData();
   qt_ntfs_permission_lookup--;

}

void playerVideos::playlistDatabase()
{
   QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE");
   database.setDatabaseName("PlayList.db");
   if(database.open())
   {
       QSqlQuery query;
       query.prepare("CREATE TABLE musicplaylist(PlayListName varchar(40), musicName varchar(100), musicCollection varchar(250), Type varchar(10));");
       query.exec();
       query.prepare("CREATE TABLE styles(StyleName varchar(20), StyleCode varchar(50), Status varchar(5));");
       query.exec();
   }

}

void playerVideos::styleSave()
{
   QVector<QString> styles;
   styles={":/loadStyles/themes/Amoled.txt",":/loadStyles/themes/Aqua.txt",
          ":/loadStyles/themes/Buttons.txt",":/loadStyles/themes/darkorange.txt",
          ":/loadStyles/themes/Elegantdark.txt",":/loadStyles/themes/MacOs.txt",
           ":/loadStyles/themes/ManjaroMix.txt",":/loadStyles/themes/materialdark.txt",
          ":/loadStyles/themes/ubuntu.txt"};

   int flag = 0;
   QSqlQuery query;
   query.prepare("SELECT * FROM styles;");
   if(query.exec())
   {
       while(query.next())
       {
           if(query.value(0).toString() != nullptr)
           {
              flag = 1;
           }
       }
   }

   if(flag == 0)
   {
     foreach(QString line, styles)
     {
         QStringList list = line.split('/');
         query.prepare("INSERT INTO styles VALUES('"+list.last().split('.').first()+"','"+line+"','False');");
         query.exec();
     }
   }
   return;
}

QString playerVideos::loadThemeChosen()
{
  QSqlQuery query;
  QString stylespath;

  query.prepare("SELECT * FROM styles WHERE Status = 'True';");
  if(query.exec())
  {
      while(query.next())
      {
          stylespath = query.value(1).toString();
      }
  }

  if(stylespath != nullptr)
  {
      QString styles;

      QFile myfile(stylespath);
      myfile.open(QIODevice::ReadOnly);
      if(myfile.isOpen())
      {
          styles = myfile.readAll();
          myfile.close();
      }
      return styles;
  }
  return nullptr;
}

QString playerVideos::readMusicForPlayList(QString path, QByteArray data)
{
    QString pathmade = "music/"+path;

    QDir dir;
    dir.cd("music");
    QStringList files = dir.entryList();

    there:
    foreach(QString line, files)
    {
        if(line == path)
        {
            QStringList list = path.split('.');
            QString random =QString::number(QRandomGenerator::global()->bounded(0,100000));
           pathmade = "music/"+list.first()+random+"."+list.last();
           for(int i = 0; i < files.size(); i++)
           {
               if(files[i] == pathmade)
               {
                   goto there;
               }
           }
        }
    }

    QFile myfile(pathmade);
    myfile.open(QIODevice::WriteOnly);
    if(myfile.isOpen())
    {
        myfile.write(data);
        myfile.flush();
        myfile.close();
    }

    return pathmade;
}

bool playerVideos::saveMusicInDb(QString playlistname, QString musicname, QString musicpath, QString type)
{
    QString namefound = nullptr;
    QString musicnamefound = nullptr;
    QString musicpathfound = nullptr;
   QSqlQuery query;
   query.prepare("SELECT * FROM musicplaylist WHERE PlayListName ='"+playlistname+"';");
   if(query.exec())
   {
      while(query.next())
      {
          namefound = query.value(0).toString();
          musicnamefound = query.value(1).toString();
          musicpathfound = query.value(2).toString();
      }
   }

   if(namefound != nullptr)
   {
      musicnamefound +="~"+musicname;
      musicpathfound +="~"+musicpath;

      query.prepare("UPDATE musicplaylist SET musicName ='"+musicnamefound+"', musicCollection ='"+musicpathfound+"' WHERE PlayListName ='"+namefound+"';");
      if(query.exec())
      {
          return true;
      }
   }

   if(namefound == nullptr)
   {
       query.prepare("INSERT INTO musicplaylist VALUES('"+playlistname+"','"+musicname+"','"+musicpath+"','"+type+"');");
       if(query.exec())
       {
           return true;
       }
   }
   return false;
}

void playerVideos::displayWindowTitle(QString musicfile)
{
  QFileInfo info(musicfile);
  setWindowTitle(info.fileName()+" "+calculateFileSize(info.size()));
}

void playerVideos::fetchQuickPlayList()
{
  QSqlQuery query; query.prepare("SELECT * FROM musicplaylist WHERE Type = 'Auto';");
  if(query.exec())
  {
      QString line = nullptr;
      while(query.next())
      {
        line = query.value(2).toString();
      }

      if(line != nullptr)
      {
          this->musicFile = line.split("~");

          for(int i = 0; i < this->musicFile.size(); i++)
          {
              QFileInfo info(this->musicFile[i]);
              ui->listWidget->addItem(info.fileName());
              index = 0;
          }
      }
  }
  return;
}


void playerVideos::on_actionAdd_File_triggered()
{    
    // this open the window dialog to select the misic file to play one    
    QString music = QFileDialog::getOpenFileName(0,"Music file",QDir::homePath(),"*.mp3; *.mp4;*.mkv");

   if(music != nullptr)
   {

       this->musicFile.clear();
       this->musicFile.append(music);
       playlistcapacity = this->musicFile.size();

      QFileInfo info(musicFile[index]);
      setWindowTitle(info.fileName()+" "+calculateFileSize(info.size()));
      ui->listWidget->clear();
      for(int i = 0; i < this->musicFile.size(); i++)
      {
          QFileInfo in(this->musicFile[i]);
          ui->listWidget->addItem(in.fileName());
      }

   }

}


void playerVideos::on_volume_valueChanged(int value)
{
    audioOutput->setVolume(value);
    ui->displayvolume->setText(QString::number(value));
}

void playerVideos::durationplaying()
{
   // ui->progressdisplay->setValue(player->loops());
    int x = QRandomGenerator::global()->bounded(0,defaultNodeImages.size() - 1);
    ui->imageDisplayer->setPixmap(QPixmap(defaultNodeImages.at(x)));
}

void playerVideos::playerFunction()
{
   playlistcapacity = this->musicFile.size();

   if(this->musicFile.isEmpty())
   {
       return;
   }

   int status = player->mediaStatus();

   if(status == 0)
   {
       QFileInfo types(this->musicFile[index]);

       QString typefound = types.suffix();
       if(typefound == "mp4")
       {
           ui->listWidget->item(index)->setIcon(QIcon(":/ic/iconsFolder/sound.png"));
           player->setSource(QUrl(this->musicFile[index]));
           player->setVideoOutput(offlineVideos);
           player->setAudioOutput(audioOutput);
           audioOutput->setVolume(ui->volume->value());
           offlineVideos->show();
           player->play();

       }

       player->setSource(QUrl(this->musicFile[index]));      
       ui->listWidget->item(index)->setIcon(QIcon(":/ic/iconsFolder/sound.png"));
       player->setAudioOutput(audioOutput);
       audioOutput->setVolume(ui->volume->value());
       extracteMetaData();
       player->play();

       ui->play->setIcon(QIcon(":/ic/iconsFolder/play.png"));
       ui->play->setText("playing..");
       lock = 1;
       this->displayWindowTitle(this->musicFile[index]);

       durationmovertimer = new QTimer(this);
       connect(durationmovertimer,SIGNAL(timeout()),this,SLOT(durationmover()));
       durationmovertimer->start(1000);

   }
   if( status == 6)
   {
       ui->listWidget->item(index)->setIcon(QIcon(":/ic/iconsFolder/check.png"));

       QFileInfo types(this->musicFile[index]);

       QString typefound = types.suffix();
       if(typefound == "mp4")
       {
           ui->listWidget->item(index)->setIcon(QIcon(":/ic/iconsFolder/sound.png"));
           player->setSource(QUrl(this->musicFile[index]));
           player->setVideoOutput(offlineVideos);
           player->setAudioOutput(audioOutput);
           audioOutput->setVolume(ui->volume->value());
           offlineVideos->show();
           player->play();

       }

       if(repeatSet == 0)
       {
          index++;
          if(index == playlistcapacity)
          {
             index = 0;
             timers->stop();
             return;
          }
       }
       if(repeatSet == 2)
       {
           index++;
           if(index == playlistcapacity)
           {
               index = 0;
           }
       }

       if(index < playlistcapacity)
       {
           player->setSource(QUrl(this->musicFile[index]));           
           player->setAudioOutput(audioOutput);
           audioOutput->setVolume(ui->volume->value());
           extracteMetaData();
           player->play();

           ui->play->setIcon(QIcon(":/ic/iconsFolder/play.png"));
           ui->play->setText("playing..");
           this->displayWindowTitle(this->musicFile[index]);

           durationmovertimer = new QTimer(this);
           connect(durationmovertimer,SIGNAL(timeout()),this,SLOT(durationmover()));
           durationmovertimer->start(1000);

       }
   }

   if(status == 8)
   {
       index += 1;
   }
   if(status == 4)
   {
       index += 1;
   }

  ui->listWidget->item(index)->setIcon(QIcon(":/ic/iconsFolder/sound.png"));

}

void playerVideos::savePlayList()
{
    this->playListName = nullptr;


    QFile myfile("temp.txt");
    myfile.open(QIODevice::ReadOnly);
    if(myfile.isOpen())
    {
        this->playListName = myfile.readAll();
        myfile.close();
    }
    QFile::remove("temp.txt");

    if(this->playListName != nullptr)
    {
        findName->stop();
        int counter = 0;

        QVector<QString> playlistMusic = QFileDialog::getOpenFileNames(0,"Select Music files",QDir::homePath(),"*.mp3; *.mp4;*.mkv");
        if(!playlistMusic.isEmpty())
        {
            //save in db
            this->musicFile.clear();
            ui->progressBar->setVisible(true);
            ui->progressBar->setMaximum(playlistMusic.size());
            ui->progressBar->setMinimum(counter);
            ui->saving->setVisible(true);
            ui->progressBar->setTextVisible(false);

            foreach(QString music, playlistMusic)
            {
              //first read music data
                QByteArray musicdataread;

                QFile myfile(music);
                myfile.open(QIODevice::ReadOnly);
                if(myfile.isOpen())
                {
                  musicdataread = myfile.readAll();
                  myfile.close();
                }

                if(!musicdataread.isEmpty())
                {
                    QFileInfo  info(music);
                   //second save data read into db together with playlistname
                    QString savedfile = this->readMusicForPlayList(info.fileName(),musicdataread);

                    if(this->saveMusicInDb(this->playListName,info.fileName(),savedfile,"Normal"))
                    {
                       counter += 1;
                       this->musicFile.append(savedfile);
                       ui->listWidget->clear();
                       ui->listWidget->addItems(this->musicFile);
                       ui->saving->setText(info.fileName());
                       ui->progressBar->setValue(counter);
                    }
                }
            }

            if(counter == playlistMusic.size())
            {
                counter = 0;
                ui->progressBar->setValue(counter);
                ui->progressBar->setVisible(false);
                ui->progressBar->setTextVisible(false);
                ui->saving->clear();
                ui->saving->setVisible(false);
                this->playListName = nullptr;
            }
        }
    }
}

void playerVideos::getPlayList()
{
    QString musicfound = nullptr;

   QFile myfile("temp.txt");
   myfile.open(QIODevice::ReadOnly);
   if(myfile.isOpen())
   {
      musicfound = myfile.readAll();
      myfile.close();
   }

   QFile::remove("temp.txt");

   if(musicfound != nullptr)
   {
       playlistFetch->stop();
       player->stop();
       timers->stop();
       this->musicFile.clear();
       this->musicFile = musicfound.split("~");
       ui->listWidget->clear();
       foreach(QString line, this->musicFile)
       {
            QFileInfo info(line);
            ui->listWidget->addItem(info.fileName());
       }
       index = 0;
       player->setSource(QUrl(this->musicFile[index]));
       player->setAudioOutput(this->audioOutput);
       this->audioOutput->setVolume(ui->volume->value());
       player->play();
       this->displayWindowTitle(this->musicFile[index]);
       ui->play->setIcon(QIcon(":/ic/iconsFolder/play.png"));
       ui->play->setText("playing..");


       timers->start(1000);
   }
   qDebug()<<"here rowing";
}

void playerVideos::setAutoPlayList()
{
    this->playListName = nullptr;

    QFile myfile("temp.txt");
    myfile.open(QIODevice::ReadOnly);
    if(myfile.isOpen())
    {
        this->playListName = myfile.readAll();
        myfile.close();
    }
    QFile::remove("temp.txt");
    int counter = 0;
    QStringList musicAutoPlay;

    if(this->playListName != nullptr)
    {
        autoplaymaker->stop();
        musicAutoPlay = QFileDialog::getOpenFileNames(0,"Select Music",QDir::homePath(),"*.mp3; *.mp4;*.mkv");

        this->musicFile.clear();
        ui->progressBar->setVisible(true);
        ui->progressBar->setMaximum(musicAutoPlay.size());
        ui->progressBar->setMinimum(counter);
        ui->saving->setVisible(true);
        ui->progressBar->setTextVisible(false);

        if(!musicAutoPlay.isEmpty())
        {
          foreach(QString music, musicAutoPlay)
          {
              QByteArray data;

              QFile myfile(music);
              myfile.open(QIODevice::ReadOnly);
              if(myfile.isOpen())
              {
                  data = myfile.readAll();
                  myfile.close();
              }

              if(!data.isEmpty())
              {
                  QFileInfo info(music);
                QString savedfile = this->readMusicForPlayList(info.fileName(),data);

                if(this->saveMusicInDb(this->playListName,info.fileName(),savedfile,"Auto"))
                {
                   counter += 1;
                   this->musicFile.append(savedfile);
                   ui->listWidget->clear();
                   ui->listWidget->addItems(this->musicFile);
                   ui->saving->setText(info.fileName());
                   ui->progressBar->setValue(counter);
                }
              }
          }

          if(counter == musicAutoPlay.size())
          {
              counter = 0;
              ui->progressBar->setValue(counter);
              ui->progressBar->setVisible(false);
              ui->progressBar->setTextVisible(false);
              ui->saving->clear();
              ui->saving->setVisible(false);
              this->playListName = nullptr;
          }
        }
    }
}

void playerVideos::durationmover()
{

}

void playerVideos::onlineWaitToPlayer()
{
    QString line = nullptr;
    QFile myfile("online.txt");
    myfile.open(QIODevice::ReadOnly);
    if(myfile.isOpen())
    {
      line = myfile.readAll();
      myfile.close();
    }

    QFile::remove("online.txt");

    if(line != nullptr)
    {
        onlinechecker->stop();
        player->stop();
        onlinePlayer = new QMediaPlayer;
        onlineaudioOutput = new QAudioOutput;
        onlineVideos = new QVideoWidget;

        player->setSource(QUrl(line));

        if(player->hasVideo())
        {
            player->setVideoOutput(onlineVideos);
            onlineVideos->show();
            player->play();
        }
        else
        {
            player->setAudioOutput(onlineaudioOutput);
            onlineaudioOutput->setVolume(ui->volume->value());
            player->play();
        }
    }


    qDebug()<<line;

}


void playerVideos::on_play_clicked()
{
   if(this->musicFile.isEmpty())
   {
        return;
   }
   timers->start(1000);
   player->play();
   ui->play->setIcon(QIcon(":/ic/iconsFolder/play.png"));
   ui->play->setText("playing..");
   ui->play->setEnabled(false);
   ui->paused->setEnabled(true);
   ui->paused->setIcon(QIcon(":/ic/iconsFolder/pause.png"));
   ui->paused->setText("Pause");

}


void playerVideos::on_paused_clicked()
{
    if(this->musicFile.isEmpty())
    {
        return;
    }

    player->pause();
    ui->paused->setIcon(QIcon(":/ic/iconsFolder/pause.png"));
    ui->paused->setText("Paused");
    ui->play->setEnabled(true);
    ui->paused->setEnabled(false);
    ui->play->setIcon(QIcon(":/ic/iconsFolder/play.png"));
    ui->play->setText("play");
}


void playerVideos::on_nexts_clicked()
{
    if(this->musicFile.isEmpty())
    {
         return;
    }

    timers->start(1000);
    ui->listWidget->item(index)->setIcon(QIcon(":/ic/iconsFolder/check.png"));
    index += 1;

    if(this->musicFile.isEmpty())
    {
        return;
    }

    if(index >= 0 && index < playlistcapacity)
    {

        QFileInfo types(this->musicFile[index]);

        QString typefound = types.suffix();
        if(typefound == "mp4")
        {
            ui->listWidget->item(index)->setIcon(QIcon(":/ic/iconsFolder/sound.png"));
            player->setSource(QUrl(this->musicFile[index]));
            player->setVideoOutput(offlineVideos);
            player->setAudioOutput(audioOutput);
            audioOutput->setVolume(ui->volume->value());

            offlineVideos->show();
            player->play();

        }

        QFileInfo info(musicFile[index]);
        setWindowTitle(info.fileName()+" "+calculateFileSize(info.size()));
        player->setSource(QUrl(this->musicFile[index]));
        player->play();
        extracteMetaData();
    }
    else if(index >= playlistcapacity)
    {
        index -= 1;
        QMessageBox::warning(0,"warning","You are at the end");

    }
}


void playerVideos::on_previous_clicked()
{
    if(this->musicFile.isEmpty())
    {
         return;
    }

    timers->start(1000);
    ui->listWidget->item(index)->setIcon(QIcon(":/ic/iconsFolder/check.png"));
    index -= 1;

    if(this->musicFile.isEmpty())
    {
        return;
    }


    if(index >= 0 && index < playlistcapacity)
    {
        QFileInfo types(this->musicFile[index]);

        QString typefound = types.suffix();
        if(typefound == "mp4")
        {
            ui->listWidget->item(index)->setIcon(QIcon(":/ic/iconsFolder/sound.png"));
            player->setSource(QUrl(this->musicFile[index]));
            player->setVideoOutput(offlineVideos);
            player->setAudioOutput(audioOutput);
            audioOutput->setVolume(ui->volume->value());

            offlineVideos->show();
            player->play();

        }

        QFileInfo info(musicFile[index]);
        setWindowTitle(info.fileName()+" "+calculateFileSize(info.size()));
        player->setSource(QUrl(this->musicFile[index]));
        player->play();
        extracteMetaData();
    }
    else if(index < 0)
    {
        index = 0;
        QMessageBox::warning(0,"warning","Your are already at the font of playlist");
    }
}


void playerVideos::on_progressdisplay_valueChanged(int value)
{

}


void playerVideos::on_actionAdd_PlayList_triggered()
{
     QString offline = "offline";
     playlist = new PlayList(0,&offline);
     playlist->show();
     findName = new QTimer(this);
     connect(findName,SIGNAL(timeout()),this,SLOT(savePlayList()));
     findName->start(1000);
}


void playerVideos::on_actionView_PlayList_triggered()
{
  view = new ViewPlayLists(0);
  view->show();

  playlistFetch = new QTimer(this);
  connect(playlistFetch,SIGNAL(timeout()),this,SLOT(getPlayList()));
  playlistFetch->start(1000);
}


void playerVideos::on_listWidget_currentRowChanged(int currentRow)
{
    ui->listWidget->item(index)->setIcon(QIcon(":/ic/iconsFolder/check.png"));
    index = currentRow;
    player->setSource(QUrl(this->musicFile[index]));

    QFileInfo types(this->musicFile[index]);

    QString typefound = types.suffix();
    if(typefound == "mp4")
    {
        ui->listWidget->item(index)->setIcon(QIcon(":/ic/iconsFolder/sound.png"));
        player->setSource(QUrl(this->musicFile[index]));
        player->setVideoOutput(offlineVideos);
        player->setAudioOutput(audioOutput);
        audioOutput->setVolume(ui->volume->value());

        offlineVideos->show();
        player->play();

    }

    this->displayWindowTitle(this->musicFile[index]);
    player->setAudioOutput(this->audioOutput);
    this->audioOutput->setVolume(ui->volume->value());
    player->play();
    extracteMetaData();
}


void playerVideos::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{   
   int ok = QMessageBox::question(0,"Removing music","Do you want to remove this song from playlist","Yes","No","Cancel",1,-1);

   if(ok == 0)
   {


   }
}


void playerVideos::on_actionRemove_PlayList_triggered()
{
  removeplaylist = new Removing(this);
  removeplaylist->show();
}

void playerVideos::on_actionExit_Player_triggered()
{
    this->close();
}


void playerVideos::on_actionAuto_PlayList_triggered()
{
    QString offline = "offline";
    playlist = new PlayList(0,&offline);
    playlist->show();
   autoplaymaker = new QTimer(this);
   connect(autoplaymaker,SIGNAL(timeout()),this,SLOT(setAutoPlayList()));
   autoplaymaker->start(1000);

}


void playerVideos::on_repeat_clicked()
{
  if(repeatSet == 0)
  {
      repeatSet = 1;
      ui->repeat->setIcon(QIcon(":/ic/iconsFolder/repeatone.png"));
       return;

  }
  if(repeatSet == 1)
  {
      repeatSet = 2;
      ui->repeat->setIcon(QIcon(":/ic/iconsFolder/repeat all(1).png"));
       return;
  }

  if(repeatSet == 2)
  {
      repeatSet = 0;
      ui->repeat->setIcon(QIcon(":/ic/iconsFolder/repeat.png"));
       return;
  }
}


void playerVideos::on_actionPlay_3_triggered()
{
    on_play_clicked();
}


void playerVideos::on_actionPause_3_triggered()
{
    on_paused_clicked();
}


void playerVideos::on_actionPrevious_2_triggered()
{
    on_previous_clicked();
}


void playerVideos::on_actionNext_2_triggered()
{
    on_nexts_clicked();
}


void playerVideos::on_actionRepeat_triggered()
{
    on_repeat_clicked();
}


void playerVideos::on_actionStop_3_triggered()
{
    player->stop();
}


void playerVideos::on_actionSelect_theme_triggered()
{
    toThemes = new Themes(this);
    toThemes->show();
}


void playerVideos::on_actionDefault_theme_triggered()
{
    QSqlQuery query;
    query.prepare("UPDATE styles SET Status ='False' WHERE Status = 'True';");
    if(query.exec())
    {
        ui->progressBar->setMinimum(0);
        ui->progressBar->setMaximum(100);
        for(int i = 0; i < 100; i++)
        {
            ui->progressBar->setValue(i);
            this->close();
        }
    }

}

void playerVideos::on_actionMusic_link_triggered()
{
  onlinechecker = new QTimer(this);
  connect(onlinechecker,SIGNAL(timeout()),this,SLOT(onlineWaitToPlayer()));

  QString on = "online";
  playlist = new PlayList(0, &on);
  playlist->show();

  onlinechecker->start(1000);

}

