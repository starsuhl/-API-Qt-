#include "widget.h"
#include "ui_widget.h"
#include <QFileDialog>
#include<QtMultimedia/QMediaPlayer>
#include<QtMultimedia/QMediaPlaylist>
#include <QFile>
#include <QTime>
#include <QSqlQuery>
#include <ctime>
#include <QMessageBox>
#include <QInputDialog>
#include <QPalette>
#include <QPixmap>
#include <QBitmap>
#include <QPainter>
#include<QVariant>
#include<QByteArray>
#include<QNetworkAccessManager>
#include<QJsonParseError>
#include<QJsonDocument>
#include<QJsonObject>
#include<QJsonArray>
#include<QUrl>
#include<QMapIterator>
#include<QRegExp>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    db = QSqlDatabase::addDatabase("QSQLITE");
    ui->listWidget->installEventFilter(this);
    ui->listWidget_3->installEventFilter(this);
    ui->tableWidget->installEventFilter(this);

    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList()<<"song"<<"singer"<<"album"<<"duration");
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    //ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setHighlightSections(false);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->verticalSlider->hide();
    ui->listWidget->hide();
    ui->widget->hide();
    voice_show=false;
    show_list=false;
    db.setDatabaseName("music.db");
    if(db.open()){
        qDebug()<<"open success!";
    }else{
        qDebug()<<"open failed!";
    }
    play_flag=0;
    music_table=0;
    row=0;
    page_size=1;
    m_menu = new QMenu;
    favor_menu = new QMenu;
    table_menu = new QMenu;

    m_next = new QAction("Next",this);
    m_pre = new QAction("Pre",this);

    m_menu->addAction(m_next);
    m_menu->addAction(m_pre);

    table_addto_favo = new QAction("Add_Favo",this);

    table_menu->addAction(table_addto_favo);

    favo_add = new QAction("Add New",this);
    favo_rename = new QAction("Rename",this);
    favo_delete = new QAction("Delete",this);

    favor_menu->addAction(favo_add);
    favor_menu->addAction(favo_rename);
    favor_menu->addAction(favo_delete);

    init_my_favo();

    ui->verticalSlider->setValue(20);

    player = new QMediaPlayer(this);
    playlist = new QMediaPlaylist(this);
    playlist->setPlaybackMode(QMediaPlaylist::Sequential);
    ui->widget->setStyleSheet("background-color: rgb(155, 170, 155);");
    player->setPlaylist(playlist);
    song_seq=0;

    mylabel = new My_Label(this);
    mylabel->setGeometry(QRect(240,500,71,61));

    connect(player,SIGNAL(positionChanged(qint64)),this,SLOT(positionChange(qint64)));
    connect(player,SIGNAL(durationChanged(qint64)),this,SLOT(total_time(qint64)));
   // connect(playlist,SIGNAL(currentIndexCahnged(int)),this,SLOT(onPlaylistChanged(int)));
    connect(m_next,SIGNAL(triggered()),this,SLOT(on_toolButton_3_clicked()));
    connect(m_pre,SIGNAL(triggered()),this,SLOT(on_toolButton_4_clicked()));

    connect(favo_add,SIGNAL(triggered()),this,SLOT(on_toolButton_7_clicked()));
    connect(favo_rename,SIGNAL(triggered()),this,SLOT(my_table_rename()));
    connect(favo_delete,SIGNAL(triggered()),this,SLOT(my_table_delete()));
    connect(table_addto_favo,SIGNAL(triggered()),this,SLOT(on_toolButton_5_clicked()));

    connect(mylabel,SIGNAL(clicked()),this,SLOT(lrc_ui()));

    network_manager = new QNetworkAccessManager();
    network_request = new QNetworkRequest();
    network_manager_2 = new QNetworkAccessManager();
    network_request_2 = new QNetworkRequest();
    network_manager_3 = new QNetworkAccessManager();
    network_request_3 = new QNetworkRequest();
    connect(network_manager, &QNetworkAccessManager::finished, this, &Widget::replyFinished,Qt::DirectConnection);
    connect(network_manager_2, &QNetworkAccessManager::finished, this, &Widget::replyFinished2);
    connect(network_manager_3, &QNetworkAccessManager::finished,this, &Widget::replyFinished3);

    connect(this,SIGNAL(pre_sig()),this,SLOT(on_toolButton_clicked()));
    connect(this,SIGNAL(pause_sig()),this,SLOT(on_toolButton_2_clicked()));
    connect(this,SIGNAL(next_sig()),this,SLOT(on_toolButton_3_clicked()));

}

Widget::~Widget()
{
    delete ui;
}

void Widget::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    QPixmap pix;
    pix.load(":/Image/xie.jpg");
    p.save();
    QBrush brush(QColor(0,0,0));
    p.setBrush(brush);
    p.drawPixmap(0,0,this->width(),this->height(),pix);
    p.restore();
}

void Widget::my_table_delete()
{
    QListWidgetItem* item = ui->listWidget_3->currentItem();
    QString name = item->text();
    ui->listWidget_3->removeItemWidget(item);
    QString opt="drop table " + name + ";";
    QSqlQuery query;
    query.exec(opt);
}

void Widget::my_table_rename()
{
    QString new_name = QInputDialog::getText(this,"Input name","Name:");
    QString opt = "create table " + new_name +" (song_name char,song_path char);";
    QListWidgetItem* item = ui->listWidget_3->currentItem();
    QString old_name = item->text();
    QSqlQuery query;
    query.exec(opt);
    opt = "select * from " + old_name + ";";
    query.exec(opt);
    while(query.next()){
        opt="insert into " + new_name + "values (\"" + query.value(0).toString() + "\",\"" + query.value(1).toString() + "\");";
    }
    opt = "drop table " + old_name + ";";
    item->setText(new_name);
}

void Widget::create_table()
{/*
    QString opt = "create table song (song_name char,song_path char,is_net int);";
    QSqlQuery query;
    query.exec(opt);
*/
}

void Widget::init_my_favo()
{
    ui->toolButton_2->setIcon(QIcon(":/Image/pause.png"));
    ui->toolButton_17->setIcon(QIcon(":/Image/pre.png"));
    ui->toolButton_16->setIcon(QIcon(":/Image/pause.png"));
    ui->toolButton_15->setIcon(QIcon(":/Image/next.png"));
    QStringList tables=db.tables();
    for(int i=0;i<tables.size();i++){
        if(!(tables.at(i) == "song")){
            ui->listWidget_3->addItem(tables.at(i));
        }
    }
}


/*
void Widget::on_pushButton_clicked()
{
    filename = QFileDialog::getOpenFileName(this,
                                            "Open",
                                            QDir::currentPath(),"Media File (*.mp3)");
    player->setMedia(QUrl::fromLocalFile(filename));
    player->play();
}
*/
void Widget::positionChange(qint64 pos)
{
 /*   moved=pos;
    QTime moveTime(0,(moved/60000)%60,(moved/1000)%60);
    ui->label->setText(moveTime.toString("mm:ss"));*/
    ui->horizontalSlider->setValue(pos);
    ui->label->setText(setTime(player->position()));
    int position = pos/10;
    QMap<int,QString>::iterator iter = lrcMap.begin();
    while (iter != lrcMap.end())
            {
                if(position-50<=iter.key()&& position+50>=iter.key())
                {
                        int j=0;
                        if(iter != lrcMap.begin())
                        {
                            iter--;
                            ui->label_5->setText(iter.value());
                            j++;
                        }
                        if(iter != lrcMap.begin())
                        {
                            iter--;
                            ui->label_9->setText(iter.value());
                            j++;
                        }

                        if(iter != lrcMap.begin())
                        {
                            iter--;
                            ui->label_10->setText(iter.value());
                            j++;
                        }
                        for(;j>0;j--)
                        {
                            iter++;
                        }
                   //中间
                   ui->label_11->setText(iter.value());
                   iter++;
                   if(iter != lrcMap.end())
                   {
                       ui->label_12->setText(iter.value());
                   }
                   else
                   {
                       ui->label_12->setText("");
                       return;
                   }
                   iter++;
                   if(iter != lrcMap.end())
                   {
                       ui->label_13->setText(iter.value());
                   }
                   else
                   {
                       ui->label_13->setText("");
                       return;
                   }
                   iter++;
                   if(iter != lrcMap.end())
                   {
                       ui->label_16->setText(iter.value());
                   }
                   else
                   {
                       ui->label_16->setText("");
                       return;
                   }
                   iter++;
                   if(iter != lrcMap.end())
                   {
                       ui->label_17->setText(iter.value());
                   }
                   else
                   {
                       ui->label_17->setText("");
                       return;
                   }
                   iter++;
                   if(iter != lrcMap.end())
                   {
                       ui->label_14->setText(iter.value());
                   }
                   else
                   {
                       ui->label_14->setText("");
                       return;
                   }
                   iter++;
                   if(iter != lrcMap.end())
                   {
                       ui->label_15->setText(iter.value());
                   }
                   else
                   {
                       ui->label_15->setText("");
                       return;
                   }
                }
                iter++;
            }
}

QString Widget::setTime(int time)
{
    int h,m,s;
    time /= 1000;  //获得的时间是以毫秒为单位的
    h = time/3600;
    m = (time-h*3600)/60;
    s = time-h*3600-m*60;
    return QString("%1:%2:%3").arg(h).arg(m).arg(s);
}

void Widget::total_time(qint64 duration)
{
    ui->horizontalSlider->setRange(0,duration);
    ui->horizontalSlider->setEnabled(duration>0);
    int playtime=player->duration();
    ui->label_4->setText(setTime(playtime));
}

void Widget::on_horizontalSlider_sliderMoved(int position)
{
    player->setPosition(position);
}

void Widget::on_toolButton_2_clicked()
{
    switch (player->state()) {
    case QMediaPlayer::PlayingState:
        player->pause();//break;
        ui->toolButton_2->setIcon(QIcon(":/Image/pause.png"));
        ui->toolButton_16->setIcon(QIcon(":/Image/pause.png"));
        break;
    default:
        player->play();//break;
        ui->toolButton_2->setIcon(QIcon(":/Image/play.png"));
        ui->toolButton_16->setIcon(QIcon(":/Image/pause.png"));
        break;
    }
}

void Widget::on_pushButton_3_clicked()//show
{
    ui->listWidget->clear();
    QString str=QString("select * from song");
    QSqlQuery query;
    query.exec(str);

    while(query.next()){
        QString str=query.value(0).toString();
        ui->listWidget->addItem(str);
    }
}

void Widget::handle_path(QString &name)
{
    song_list = name.split("/");
    filename=song_list.last();
}

void Widget::handle_Music_data(QString table_name,QString& path)
{
    handle_path(path);
    QString opt =QString("insert into song values (\"") + filename + "\",\"" + path + "\");";

    QString opt_=QString("insert into ")+table_name + " values (\"" + filename + "\",\""+path+"\");";
    QSqlQuery query;
    query.exec(opt_);
}


void Widget::on_pushButton_2_clicked()//import
{
    QString son_name = QFileDialog::getOpenFileName(this,
                                 "Open",
                                 QDir::currentPath(),"Media File (*.mp3)");
    handle_path(son_name);
    local_music=true;
    handle_Music_data("song",son_name);
    ui->listWidget->addItem(filename);
}

void Widget::on_verticalSlider_sliderMoved(int position)
{
    player->setVolume(position);
}

void Widget::search_music(QString table_name,QString name)
{
    QString str=QString("select * from song where song_name=\"")+name+"\";";
    QString str_=QString("select * from ")+table_name+" where song_name=\"" + name + "\";";
    QSqlQuery query;
    query.exec(str_);
    while(query.next())//
    {
        filepath = query.value(1).toString();
    }
}

bool Widget::eventFilter(QObject* watched,QEvent* event)
{
    if(watched == ui->listWidget){
        if(event->type() == QEvent::ContextMenu){
            m_menu->exec(QCursor::pos());
            return true;
        }
    }else if(watched == ui->listWidget_3){
        if(event->type() == QEvent::ContextMenu){
            favor_menu->exec(QCursor::pos());
            return true;
        }
    }else if(watched == ui->tableWidget){
        if(event->type() == QEvent::ContextMenu){
            table_menu->exec(QCursor::pos());
            return true;
        }
    }
    return QObject::eventFilter(watched,event);
}
/*
void Widget::contextMenuEvent(QContextMenuEvent* event)
{
    m_menu->exec(event->globalPos());
}*/

void Widget::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    ui->label_6->setText(item->text());
    ui->label_8->setText(item->text());

    if(music_list.key(item->text()) == "song"){
        QString name = item->text();
        QStringList tables=db.tables();
        for(int i=0;i<tables.size();i++){
            search_music(tables.at(i),name);
        }

        //search_music(table_name,name);
        player->setMedia(QUrl::fromLocalFile(filepath));
        player->play();
    }else{
        network_request_2->setUrl(QUrl(music_list.key(item->text())));
        qDebug()<<music_list.key(item->text()) + "search";
        network_request_2->setRawHeader("Cookie","kg_mid=233");
        network_request_2->setHeader(QNetworkRequest::CookieHeader, 233);
        network_manager_2->get(*network_request_2);
    }
}

void Widget::on_listWidget_2_itemClicked(QListWidgetItem *item)
{
    local_music=true;
    if(item->text() == "Local Music"){
        //ui->tableWidget->clear();
        ui->tableWidget->clearContents();
        for(int i=0;i<row;i++){
            ui->tableWidget->removeRow(i);
        }
        row=0;
        QString str=QString("select * from song");
        QSqlQuery query;
        query.exec(str);
        while(query.next()){
            QString str=query.value(0).toString();
            //ui->listWidget_4->addItem(str);
            int rou_index=ui->tableWidget->rowCount();
            ui->tableWidget->setRowCount(rou_index+1);
            ui->tableWidget->setItem(row,0,new QTableWidgetItem(str));
            row++;
        }
    }

}

void Widget::on_toolButton_3_clicked()  // next
{
    if(play_flag == 2){
        int lentgh=ui->listWidget->count();
        qsrand(time(nullptr));
        song_seq = qrand()%lentgh;
    }else if(play_flag == 1){
        if(song_seq+1 == ui->listWidget->count()){
            song_seq=0;
        }else{
            song_seq++;
        }
    }else if(play_flag == 3){
        //song_seq=song_seq;
    }
    QListWidgetItem* item=ui->listWidget->item(song_seq);
    item->setSelected(true);
    ui->label_6->setText(item->text());
    ui->label_8->setText(item->text());
        if(music_list.key(item->text()) == "song"){
            QString name = item->text();
            QStringList tables=db.tables();
            for(int i=0;i<tables.size();i++){
                search_music(tables.at(i),name);
            }

            //search_music(table_name,name);
            player->setMedia(QUrl::fromLocalFile(filepath));
            player->play();
        }else{
            network_request_2->setUrl(QUrl(music_list.key(item->text())));
            qDebug()<<music_list.key(item->text());
            network_request_2->setRawHeader("Cookie","kg_mid=233");
            network_request_2->setHeader(QNetworkRequest::CookieHeader, 233);
            network_manager_2->get(*network_request_2);
        }
}

void Widget::on_toolButton_4_clicked()// play mode
{
    play_flag++;
    if(play_flag == 4){
        play_flag=0;
    }else if(play_flag == 1){
        ui->toolButton_4->setIcon(QIcon(":/Image/seq.png"));
    }else if(play_flag == 2){
        ui->toolButton_4->setIcon(QIcon(":/Image/random.png"));
    }else if(play_flag == 3){
        ui->toolButton_4->setIcon(QIcon(":/Image/single.png"));
    }

}

void Widget::on_toolButton_clicked() // pre
{
    if(play_flag == 2){
        int lentgh=ui->listWidget->count();
        qsrand(time(nullptr));
        song_seq = qrand()%lentgh;
    }else if(play_flag == 1){
        if(song_seq == 0){
            song_seq=ui->listWidget->count()-1;
        }else{
            song_seq--;
        }
    }else if(play_flag == 3){
        //song_seq=song_seq;
    }
    QListWidgetItem* item=ui->listWidget->item(song_seq);
    item->setSelected(true);
    ui->label_6->setText(item->text());
    ui->label_8->setText(item->text());
        if(music_list.key(item->text()) == "song"){
            QString name = item->text();
            QStringList tables=db.tables();
            for(int i=0;i<tables.size();i++){
                search_music(tables.at(i),name);
            }

            //search_music(table_name,name);
            player->setMedia(QUrl::fromLocalFile(filepath));
            player->play();
        }else{
            network_request_2->setUrl(QUrl(music_list.key(item->text())));
            qDebug()<<music_list.key(item->text());
            network_request_2->setRawHeader("Cookie","kg_mid=233");
            network_request_2->setHeader(QNetworkRequest::CookieHeader, 233);
            network_manager_2->get(*network_request_2);
        }
}

void Widget::Favor_rename()
{

}

void Widget::handle_net_song(QString table_name,QString song_name,QString hash)
{
    QString opt_=QString("insert into ")+table_name + " values (\"" +
            song_name + "\",\""+hash+"\");";
    QSqlQuery query;
    query.exec(opt_);
}

void Widget::on_toolButton_5_clicked() // Favorite
{
    //QListWidgetItem* item = ui->listWidget->item(song_seq);
    QTableWidgetItem* item = ui->tableWidget->currentItem();
    //ui->listWidget_3->addItem(item);
    QString name = item->text();

    if(!local_music){
        handle_net_song("Favorite",name,music_list.key(name));
    }
}



void Widget::on_toolButton_7_clicked()
{
    QString str = QInputDialog::getText(this,"Input Table name","Name:");
    QString table = QString("create table ") + str + QString("(song_name char , song_path char);");
    QSqlQuery query;
    if(!str.isEmpty()){
        ui->listWidget_3->addItem(str);
        query.exec(table);
    }
}

void Widget::on_listWidget_3_itemDoubleClicked(QListWidgetItem *item)
{
    local_music=false;
    QString name=ui->listWidget_3->currentItem()->text();
    //search_music(name,ui->listWidget_4->currentItem()->text());
    ui->tableWidget->clearContents();
    for(int i=0;i<row;i++){
        ui->tableWidget->removeRow(i);
    }
    row=0;
    QString str=QString("select * from ")+name+";";
    QSqlQuery query;
    query.exec(str);
    while(query.next()){
        QString str=query.value(0).toString();
        int rou_index=ui->tableWidget->rowCount();
        ui->tableWidget->setRowCount(rou_index+1);
        ui->tableWidget->setItem(row,0,new QTableWidgetItem(str));
        row++;
    }
}


bool Widget::judgde_exist(QString name)
{
    if(music_list.contains(name)){
        return true;
    }else{
        return false;
    }
}

void Widget::on_tableWidget_itemDoubleClicked(QTableWidgetItem *item)
{
    int c_row = item->row();
    if(local_music){
        QList<QTableWidgetItem*> listItem=ui->tableWidget->selectedItems();
        QString name = listItem.at(0)->text();
        QStringList tables=db.tables();
        //qDebug()<<c_row;
        for(int i=0;i<tables.size();i++){
            search_music(tables.at(i),name);
        }
        if(!judgde_exist(item->text())){
            ui->listWidget->addItem(item->text());
            music_list.insert(item->text(),"song");
        }
        //music_list.insert(item->text(),"local");
        player->setMedia(QUrl::fromLocalFile(filepath));
        player->play();
    }
    else if(my_favo){
        QList<QTableWidgetItem*> listItem=ui->tableWidget->selectedItems();
        QString name = listItem.at(0)->text();
        search_music("Favorite",name);
        network_request_2->setUrl(QUrl(filepath));
        network_request_2->setRawHeader("Cookie","kg_mid=233");
        network_request_2->setHeader(QNetworkRequest::CookieHeader, 233);
        network_manager_2->get(*network_request_2);
    }
    else{
        QString KGAPISTR1 =QString("https://www.kugou.com/yy/index.php?r=play/getdata"
        "&hash=%1&album_id=%2&_=1497972864535").arg(JI.m_Hash.at(c_row)).arg(JI.m_Album_id.at(c_row));
        if(!judgde_exist(item->text())){
            ui->listWidget->addItem(ui->tableWidget->item(c_row,0)->text());
            music_list.insert(KGAPISTR1,ui->tableWidget->item(c_row,0)->text());
            qDebug()<<ui->tableWidget->item(c_row,0)->text();
            qDebug()<<KGAPISTR1;
        }
        ui->label_6->setText(JI.m_Songname_original.at(c_row));
        network_request_2->setUrl(QUrl(KGAPISTR1));
        network_request_2->setRawHeader("Cookie","kg_mid=233");
        network_request_2->setHeader(QNetworkRequest::CookieHeader, 233);
        network_manager_2->get(*network_request_2);
    }

}

void Widget::on_toolButton_8_clicked()
{
    if(show_list){
        show_list=false;
        ui->listWidget->hide();
    }else{
        show_list=true;
        ui->listWidget->show();
    }
}

void Widget::search(QString str, int page=1, int pagesize=12)
{
    QString KGAPISTR1 = QString("http://mobilecdn.kugou.com/api/v3/search/song?format=json"
                                   "&keyword=%1&page=%2&pagesize=%3").arg(str).arg(page).arg(pagesize);
        network_request->setUrl(QUrl(KGAPISTR1));
        network_manager->get(*network_request);
}
void Widget::on_toolButton_6_clicked()
{
    local_music=false;
    ui->tableWidget->clearContents();
    music_list.clear();
    JI.clear_info();
    search(ui->lineEdit->text());
}

QString Widget::getcontains(QJsonObject Object,QString strInfo)
{
    if (Object.contains(strInfo))
        {
            QJsonValue value = Object.take(strInfo);
            if (value.isString())
            {
                return value.toString();
            }
        }
        return "";
}

JsonInfo Widget::parseJson(QString json)
{
    QByteArray byte_array;
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(byte_array.append(json), &json_error);
        if (json_error.error == QJsonParseError::NoError)
        {
            if (parse_doucment.isObject())
            {
                QJsonObject rootObj = parse_doucment.object();
                if (rootObj.contains("data"))
                {
                    QJsonValue valuedata = rootObj.value("data");
                    if (valuedata.isObject())
                    {
                        QJsonObject valuedataObject = valuedata.toObject();
                        if (valuedataObject.contains("info"))
                        {
                            QJsonValue valueArray = valuedataObject.value("info");
                            if (valueArray.isArray())
                            {
                                QJsonArray array = valueArray.toArray();
                                int size = array.size();
                                ui->tableWidget->setRowCount(size);//设置列数
                                for (int i = 0; i < size; i++)
                                {
                                    QJsonValue value = array.at(i);
                                    if (value.isObject())
                                    {
                                        QJsonObject object = value.toObject();
                                        //收集数据
                                        JI.m_Songname_original.append(getcontains(object,"songname_original"));
                                        JI.m_Singername.append(getcontains(object,"singername"));
                                        JI.m_Album_name.append(getcontains(object,"album_name"));
                                        JI.m_Hash.append(getcontains(object,"hash"));
                                        JI.m_Album_id.append(getcontains(object,"album_id"));
                                        if (object.contains("duration"))//时长
                                        {

                                           QJsonValue AlbumID_value = object.take("duration").toInt();
                                           JI.m_Duration.append(AlbumID_value.toInt());
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        else
        {
            qDebug() << json_error.errorString();
        }
        for(int i=0;i<JI.m_Songname_original.size();i++){
            ui->tableWidget->setItem(i,0,new QTableWidgetItem(JI.m_Songname_original.at(i)));
            ui->tableWidget->setItem(i,1,new QTableWidgetItem(JI.m_Singername.at(i)));
            ui->tableWidget->setItem(i,2,new QTableWidgetItem(JI.m_Album_name.at(i)));
            QString time = QString("%1:%2").arg(JI.m_Duration.at(i)/60).arg(JI.m_Duration.at(i)%60);
            ui->tableWidget->setItem(i,3,new QTableWidgetItem(time));
        }
        return JI;
}

void Widget::replyFinished(QNetworkReply *reply)
{
    //qDebug()<<"老二1先出来";
    //获取响应的信息，状态码为200表示正常
    QVariant status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    if(reply->error() == QNetworkReply::NoError)
    {
        qDebug()<<"test";
        QByteArray bytes = reply->readAll();  //获取字节
        QString result(bytes);                //转化为字符串
        JI = parseJson(result);
    }
    else
    {
        qDebug()<<"搜索信息处理错误";
    }
    //qDebug()<<"老二2先出来";
    IsExecute = 0;
}

void Widget::parseJsonSongInfo(QString json)
{
    qDebug()<<"data:"<<json;
    QByteArray byte_array;
    QJsonParseError json_error;
    QJsonDocument parse_document = QJsonDocument::fromJson(byte_array.append(json),&json_error);
    if(json_error.error == QJsonParseError::NoError){
        if(parse_document.isObject()){
            QJsonObject rootObj = parse_document.object();
            if(rootObj.contains("data")){
                QJsonValue valuedata = rootObj.value("data");
                if(valuedata.isObject()){
                    QJsonObject valuedataObject = valuedata.toObject();
                    //play;
                    QString url = getcontains(valuedataObject,"play_url");
                    playlist->addMedia(QUrl::fromLocalFile(url));
                    playlist->setCurrentIndex(playlist->mediaCount()-1);
                    player->play();

                    network_request_3->setUrl(QUrl(getcontains(valuedataObject,"img")));
                    network_manager_3->get(*network_request_3);

                    QString lrc = getcontains(valuedataObject,"lyrics");
                    QStringList lrclist = lrc.split("\n");
                    for(int i=3;i<lrclist.size()-1;i++){
                        QString ss1 = lrclist.at(i);
                        QRegExp ipRegExp = QRegExp("\\[\\d\\d\\S\\d\\d\\S\\d\\d\\]");
                        bool match = ipRegExp.indexIn(lrclist.at(i));
                        if(match == false){
                            int s_1 = ss1.mid(1,2).toInt();
                            int s_2 = ss1.mid(4,2).toInt();
                            int s_3 = ss1.mid(7,2).toInt();      //厘秒
                            int lrctime = (s_1*60+s_2)*100+s_3;   //规定写法
                            QString lrcstr = ss1.mid(10);
                            lrcMap.insert(lrctime,lrcstr);

                        }
                    }
                }
            }
        }
    }else{
        qDebug()<<json_error.error;
    }
}
void Widget::replyFinished2(QNetworkReply *reply)
{
    //清空缓存
    //lrcMap.clear();
    //获取响应的信息，状态码为200表示正常
        QVariant status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

        //无错误返回
        if(reply->error() == QNetworkReply::NoError)
        {
            QByteArray bytes = reply->readAll();  //获取字节
            QString result(bytes);  //转化为字符串
            parseJsonSongInfo(result);
        }
        else
            //处理错误
        {
            qDebug()<<"处理错误2";
        }
}

void Widget::replyFinished3(QNetworkReply* reply)
{
    QVariant status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
            //无错误返回
            if(reply->error() == QNetworkReply::NoError)
            {
                QByteArray bytes = reply->readAll();  //获取字节
                QPixmap pixmap;
                QSize picSize(45,45);
                QSize Size(120,120);
                pixmap.loadFromData(bytes);

                //ui->label_5->setPixmap(pixmap.scaled(picSize));
                mylabel->setPixmap(pixmap.scaled(picSize));
                ui->label_7->setPixmap(pixmap.scaled(Size));
            }
            else
            {
                //处理错误
                qDebug()<<"处理错误3";
            }
}

void Widget::on_toolButton_13_clicked()
{
    if(voice_show){
        voice_show=false;
        ui->verticalSlider->hide();
    }else{
        voice_show=true;
        ui->verticalSlider->show();
    }
}

void Widget::on_toolButton_14_clicked()
{
    ui->widget->hide();
    mylabel->show();
}

void Widget::lrc_ui()
{
    ui->widget->show();
    mylabel->hide();
}

void Widget::on_toolButton_16_clicked()
{
    emit pause_sig();
}

void Widget::on_toolButton_17_clicked()
{
    emit pre_sig();
}

void Widget::on_toolButton_15_clicked()
{
    emit next_sig();
}

void Widget::on_pushButton_clicked()
{
    page_size++;
    local_music=false;
    ui->tableWidget->clearContents();
    music_list.clear();
    JI.clear_info();
    search(ui->lineEdit->text(),page_size);
}

void Widget::on_pushButton_5_clicked()
{
    if(page_size!=0)
        page_size--;
    local_music=false;
    ui->tableWidget->clearContents();
    music_list.clear();
    JI.clear_info();
    search(ui->lineEdit->text(),page_size);
}
