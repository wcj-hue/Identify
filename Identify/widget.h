#ifndef WIDGET_H
#define WIDGET_H
#include <QNetworkAccessManager>
#include <QWidget>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QCamera>
#include <QCameraViewfinder>
#include <QCameraImageCapture>


#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_pushButton_clicked();
    void replyfinished(QNetworkReply * reply);
    void gettime();
    void displayImage(int,QImage);
    void on_pushButton_2_clicked();

private:
    Ui::Widget *ui;
    QNetworkAccessManager *manager;
    QString request;

    QCamera *camera ;
    QCameraViewfinder *cameraViewfinder ;
    QCameraImageCapture *cameraImageCapture ;
    QCameraImageCapture *qCameraImg;

    QSqlDatabase database;
};

#endif // WIDGET_H
