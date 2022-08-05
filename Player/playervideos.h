#ifndef PLAYERVIDEOS_H
#define PLAYERVIDEOS_H

#include <QMainWindow>
#include <QVector>
#include <QMediaPlayer>
#include "playlist.h"
#include "viewplaylists.h"
#include <QListWidgetItem>
#include "removing.h"
#include <QTimer>
#include "stylesloader.h"
#include "themes.h"
#include <QVideoWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class playerVideos; }
QT_END_NAMESPACE

class playerVideos : public QMainWindow, public StylesLoader
{
    Q_OBJECT

public:
    playerVideos(QWidget *parent = nullptr);
    ~playerVideos();
    QString calculateFileSize(int sizes);
    void extracteMetaData();
    void playlistDatabase();
    void styleSave();
    QString loadThemeChosen();
    QString readMusicForPlayList(QString path,QByteArray data);
    bool saveMusicInDb(QString playlistname, QString musicname, QString musicpath, QString type);
    void displayWindowTitle(QString musicfile);
    void fetchQuickPlayList();

private slots:
    void on_actionAdd_File_triggered();


    void on_volume_valueChanged(int value);

    void on_play_clicked();

    void on_paused_clicked();

    void on_nexts_clicked();

    void on_previous_clicked();

    void on_progressdisplay_valueChanged(int value);

    void on_actionAdd_PlayList_triggered();

    void on_actionView_PlayList_triggered();

    void on_listWidget_currentRowChanged(int currentRow);

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_actionRemove_PlayList_triggered();

    void on_actionExit_Player_triggered();

    void on_actionAuto_PlayList_triggered();

    void on_repeat_clicked();

    void on_actionPlay_3_triggered();

    void on_actionPause_3_triggered();

    void on_actionPrevious_2_triggered();

    void on_actionNext_2_triggered();

    void on_actionRepeat_triggered();

    void on_actionStop_3_triggered();

    void on_actionSelect_theme_triggered();

    void on_actionDefault_theme_triggered();

    void on_actionMusic_link_triggered();

public slots:
    void durationplaying();
    void playerFunction();
    void savePlayList();
    void getPlayList();
    void setAutoPlayList();
    void durationmover();
    void onlineWaitToPlayer();

private:
    Ui::playerVideos *ui;
    QVector<QString> defaultNodeImages;
    QMediaPlayer *player;
    QAudioOutput *audioOutput;
    QVector<QString>  musicFile;

    PlayList *playlist;
    QString playListName;
    ViewPlayLists *view;
    Removing *removeplaylist;

    QTimer *timer;
    QTimer *findName;
    QTimer *playlistFetch;
    QTimer *timers;
    QTimer *autoplaymaker;
    QTimer *durationmovertimer;
    Themes *toThemes;
    QTimer *onlinechecker;
    QMediaPlayer *onlinePlayer;
    QAudioOutput *onlineaudioOutput;
    QVideoWidget *onlineVideos;

    QVideoWidget *offlineVideos;

};
#endif // PLAYERVIDEOS_H
