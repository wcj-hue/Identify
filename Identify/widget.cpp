#include "widget.h"
#include "ui_widget.h"
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QFileDialog>
#include <QFileDevice>
#include <QByteArray>
#include <QBuffer>
#include <QFile>
#include <QHttpMultiPart>
#include <QHttpPart>
#include <QContextMenuEvent>
#include <QJsonObject>
#include <QTime>
#include <QTimer>
#include <QDateTime>
#include <QImage>
#include <QCamera>
#include <QCameraViewfinder>
#include <QCameraImageCapture>
#include <QCameraImageCapture>

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    if (QSqlDatabase::contains("qt_sql_default_connection"))
    {
        database = QSqlDatabase::database("qt_sql_default_connection");
    }
    else
    {
        database = QSqlDatabase::addDatabase("QSQLITE");
        database.setDatabaseName("MyDataBase.db");
    }


    ui->label_4->resize(50,20);
    camera = new QCamera;
    cameraViewfinder = new QCameraViewfinder;
    cameraImageCapture = new QCameraImageCapture(camera);
    camera->setCaptureMode(QCamera::CaptureStillImage);
    camera->setViewfinder(ui->widget);
    camera->start();

    QTimer *time = new QTimer;
    connect(time,SIGNAL(timeout()),this,SLOT(gettime()));
    time->start(1);
    manager = new QNetworkAccessManager(this);
    request = "https://aip.baidubce.com/rest/2.0/ocr/v1/license_plate";
    connect(manager,SIGNAL(finished(QNetworkReply *)),SLOT(replyfinished(QNetworkReply *)));
    connect(cameraImageCapture,SIGNAL(imageCaptured(int,QImage)),this,SLOT(displayImage(int,QImage)));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButton_clicked()
{

    QUrl url(request + "?access_token=" + "24.f78a6b6522a4d454095f04a63e81bb67.2592000.1570003964.282335-17155115");
    QNetworkRequest requesturl(url);
    requesturl.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/x-www-form-urlencoded"));
    QString pic = QFileDialog::getOpenFileName();
    qDebug()<< pic;

    QImage image(pic);
    QSize laSize=ui->label_3->size();
    image.scaled(laSize,Qt::IgnoreAspectRatio);//重新调整图像大小以适应窗口
    ui->label_3->setPixmap(QPixmap::fromImage(image));//图片显示

    QByteArray arry;
    QBuffer buff(&arry);
    buff.open(QIODevice::WriteOnly);
    image.save(&buff,"jpg");
    QByteArray header = "image=";
    QByteArray postdata = header+arry.toBase64().toPercentEncoding();
    manager->post(requesturl,postdata);

}

void Widget::replyfinished(QNetworkReply * reply)
{
    //解析数据
    QByteArray arry = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(arry);
    QJsonObject result = doc.object();
    if(result.contains("error_msg"))
    {
        qDebug()<< "failed:"<<result.value("error_msg");
    }else
    {
        QJsonObject num = result.value("words_result").toObject();
        ui->lineEdit->setText(num.value("number").toString());
        qDebug()<< "failed---------------"<<num.value("number").toString()<<endl;;
    }
    QDateTime time = QDateTime::currentDateTime();
    QString mytime = time.currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    ui->label_4->setText(mytime);
    QString num = ui->lineEdit->text();

    if(ui->lineEdit->text() == "")
    {
        return ;
    }
    //打开数据库
    if (!database.open())
    {
        qDebug() << "Error: Failed to connect database." << database.lastError();
    }
    else
    {
        bool ok;
        QSqlQuery sql_query;
        //判断是否有表，没有就创建
        sql_query.exec(QString("select count(*) from sqlite_master where type='table' and name='%1'").arg("infomotion"));
        if(sql_query.next())
        {
            if(sql_query.value(0).toInt()==0)
            {
                QString create_sql = "create table infomotion (info text,time text);";
                sql_query.prepare(create_sql);
                if(!sql_query.exec())
                {
                    qDebug() << "Error: Fail to create table." << sql_query.lastError();
                }
            }
            int n = sql_query.size();
            qDebug()<<num;
            ok = sql_query.isNull(num);
            qDebug()<<ok;
            if(n==0)
            {
                sql_query.exec("insert into infomotion values('"+num+"' ,'"+mytime+"');");
            }
            else if(ok == false)
            {
                qDebug()<<"-----------1-------------";
                sql_query.exec("delete from infomotion where info='"+num+"';");
            }else
            {
                sql_query.exec("insert into infomotion values('"+num+"' ,'"+mytime+"');");
            }
            qDebug()<<"---------5---------------";
        }
    }
}
//获取时间
void Widget:: gettime()
{
    QDateTime time = QDateTime::currentDateTime();
    QString mytime = time.currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    ui->lineEdit_2->setText(mytime);
}
//图片显示及POST请求
void Widget::displayImage(int,QImage preview)
{
    qDebug()<<"------------------------";
    QSize laSize=ui->label_3->size();
    QImage image=preview.scaled(laSize,Qt::IgnoreAspectRatio);//重新调整图像大小以适应窗口
    ui->label_3->setPixmap(QPixmap::fromImage(image));//图片显示

    QUrl url(request + "?access_token=" + "24.f78a6b6522a4d454095f04a63e81bb67.2592000.1570003964.282335-17155115");
    QNetworkRequest requesturl(url);
    requesturl.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/x-www-form-urlencoded"));
    QByteArray arry;
    QBuffer buff(&arry);
    buff.open(QIODevice::WriteOnly);
    preview.save(&buff,"jpg");
    QByteArray header = "image=";
    QByteArray postdata = header+arry.toBase64().toPercentEncoding();
    manager->post(requesturl,postdata);

    gettime();

}
//拍照
void Widget::on_pushButton_2_clicked()
{
    cameraImageCapture->capture();
}
