#ifndef WIDGET_H
#define WIDGET_H
class My_Label;
class QLabel;
#include <QWidget>
#include <QMediaPlayer>
#include <QSqlDatabase>
#include <QList>
#include <QMouseEvent>
#include <QListWidgetItem>
#include <QTableWidgetItem>
#include <QMenu>
#include <QAction>
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimedia/QMediaPlaylist>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSignalMapper>
#include <QVector>
#include <QMap>
#include <QByteArray>
#include "info.h"
#include "my_label.h"
QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
public:

    My_Label* mylabel;

    void handle_path(QString&);
    void handle_Music_data(QString,QString&);
    void search_music(QString,QString);
    bool judgde_exist(QString);
    QString setTime(int time);
    void init_my_favo();
    void search(QString str,int page,int pagesize); //搜索
    JsonInfo parseJson(QString json);
    QString getcontains(QJsonObject Object,QString strInfo);
    void parseJsonSongInfo(QString json);
    void create_table();
    void handle_net_song(QString,QString,QString);



signals:
    void pre_sig();
    void pause_sig();
    void next_sig();

private slots:
 //   void on_pushButton_clicked();
    void positionChange(qint64);
    void total_time(qint64);

    //void contextMenuEvent(QContextMenuEvent *event);
    //void mouseDoubleClickEvent(QMouseEvent *event);
    //void mousePressEvent(QMouseEvent *event);
    bool eventFilter(QObject* watched,QEvent* event);

    void on_horizontalSlider_sliderMoved(int position);

    void on_toolButton_2_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_verticalSlider_sliderMoved(int position);

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_listWidget_2_itemClicked(QListWidgetItem *item);

    void on_toolButton_3_clicked();

    void on_toolButton_clicked();

    void on_toolButton_5_clicked();

    void on_toolButton_4_clicked();

    void on_toolButton_7_clicked();

    void Favor_rename();

    void on_listWidget_3_itemDoubleClicked(QListWidgetItem *item);


    void paintEvent(QPaintEvent *event);

    void on_tableWidget_itemDoubleClicked(QTableWidgetItem *item);

    void on_toolButton_8_clicked();

    void replyFinished(QNetworkReply *reply);
    void replyFinished2(QNetworkReply *reply);
    void replyFinished3(QNetworkReply *reply);

    void on_toolButton_6_clicked();

    void on_toolButton_13_clicked();

    void on_toolButton_14_clicked();

    void lrc_ui();

    void on_toolButton_16_clicked();

    void on_toolButton_17_clicked();

    void on_toolButton_15_clicked();

    void on_pushButton_clicked();

    void on_pushButton_5_clicked();

    void my_table_rename();
    void my_table_delete();

private:
    Ui::Widget *ui;
    QSqlDatabase db;
    QMediaPlayer* player;
    QMediaPlaylist* playlist;
    QNetworkRequest* network_request;
    QNetworkRequest* network_request_2;
    QNetworkRequest* network_request_3;
    QNetworkAccessManager* network_manager;
    QNetworkAccessManager* network_manager_2;
    QNetworkAccessManager* network_manager_3;
    QVector<QString> song_name;
    QVector<QString> singername,albumname,album_ID,imge,vector_list;
    QVector<int> duration;
    QMap<int,QString> lrcMap;
    QMap<QString,QString> music_list;

   // QSignalMapper* myMapper;
    JsonInfo JI;

    QMenu* m_menu,*favor_menu,*table_menu;
    QAction* m_next,*m_pre,*favo_rename,*favo_delete,*favo_add,*table_addto_favo;
    QStringList song_list;
    QString filename,filepath;
    bool show_list,local_music,voice_show,my_favo;
    int moved,song_seq,row,page_size;
    int play_flag,music_table;
    int IsExecute = 1;
};
#endif // WIDGET_H
