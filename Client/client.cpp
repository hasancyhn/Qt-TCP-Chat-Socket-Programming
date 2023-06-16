#include "client.h"
#include "ui_client.h"
#include <QtWidgets/QMessageBox>
#include <QTime>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QByteArray>
#include <QDataStream>
#include <QStandardPaths>
#include <QString>

Client::Client(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Client){
    ui->setupUi(this);
    m_socket = new QTcpSocket();

    QObject::connect(m_socket,&QTcpSocket::readyRead,this,&Client::socket_Recv_Data);       /* ???????????????????? */
    QObject::connect(m_socket,&QTcpSocket::disconnected,this,&Client::Socket_Disconnect);   /* btn_Send'i devre dışı bıraktık. */

    ui->btn_Send->setShortcut(QKeySequence(tr("ctrl+return")));                             /* Eylemi tetikleyen kısayol ayarlanır. */ /* ????????????????????????????????? */
    ui->btn_Send->setEnabled(false);
}

Client::~Client(){
    delete m_socket;                                                                        /* ???????????????????? */
    delete ui;
}


void Client::on_btn_Connect_clicked(){
    QString IP;
    qint16 port;

    if(ui->btn_Connect->text() == QString("Connect"))
    {
        IP = ui->lineEdit_IP->text();                                                       /* Kullanıcı IP adresini girdikten sonra IP adresi tutulur. */
        port = ui->lineEdit_Port->text().toInt();                                           /* Kullanıcı port adresini girdikten sonra port adresi tutulur ve integer'a çevirilir. */

        m_socket->abort();                                                                  /* İşlemi hemen durdurur ve açık olan tüm ağ bağlantılarını kapatır. Devam etmekte olan yüklemeler de iptal edilir. */
        m_socket->connectToHost(IP,port);                                                   /* Verilen bağlantı noktasında IP adresi ile bağlantı kurmaya çalışır. Port numarası, hangi ağ protokolünün kullanılacağını belirtmek için kullanılır. */

        if(!m_socket->waitForConnected())                                                   /* Soket bağlanana kadar bekler. Bağlantı kurulmuşsa, bu işlev true değerini döndürür.*/
        {                                                                                   /* Eğer bağlantı sağlanılmazsa aşağıdaki işlemler gerçekleştirilir. */
            QMessageBox::warning(this, "BİLGİ", "Bağlantı kurulamadı!", "Tamam");
        }
        else
        {
            ui->btn_Send->setEnabled(true);                                                 /* btn_Send'i tekrar işlev görür hale getirdik. */
            QMessageBox::information(this, "BİLGİ", "Bağlantı başarılı.", "Tamam");
            ui->btn_Connect->setText("Disconnect");
        }
    }
    else
    {
        m_socket->disconnectFromHost();                                                     /* Soketi kapatmaya çalışır. Yazılmayı bekleyen veriler varsa onlar yazılana kadar beklenir sonra disconnected() atılır. */

        ui->btn_Connect->setText("Connect");
        ui->btn_Send->setEnabled(false);                                                    /* btn_Send'i devre dışı bıraktık. */
    }
}


void Client::on_btn_Send_clicked(){
    if(ui->textEdit->toPlainText() == QString())
    {
        QMessageBox::critical(this, "BİLGİ", "Boş mesaj gönderemezsin!", "Tamam");
    }
    ui->textBrowser->setTextColor(Qt::gray);
    ui->textBrowser->setCurrentFont(QFont("Times New Roman",10));
    ui->textBrowser->append("From Client: "+QTime::currentTime().toString());               /* Mesajın gönderilme zamanını saatini, dakikasını saniyesini ekrana yazdırır. */

    ui->textBrowser->setTextColor(Qt::black);
    ui->textBrowser->setCurrentFont(QFont("Times New Roman",16));
    ui->textBrowser->append(ui->textEdit->toPlainText().toUtf8());

    m_socket->write(ui->textEdit->toPlainText().toUtf8());                                  /* Kullanıcıdan alınan veriler m_socket'e yazılır. */
    m_socket->flush();                                                                      /* Verinin gerçekten yazıldığını doğrulamak için flush() parametresini kullandık. */
    ui->textEdit->clear();
}

void Client::socket_Recv_Data(){
    QByteArray data_tmp;
    data_tmp = m_socket->readAll();                                                         /* m_socket'in içindeki bütün verileri okuyup data_tmp değişkenine aktarıldı. */
    if(!data_tmp.isEmpty())                                                                 /* data_tmp değişkeninin içinde herhangi bir değer varsa aşağıdaki işlemler gerçekleştirilir. */
    {
        QString str = QString(data_tmp);                                                    /* data_tmp değişkenini String'e çevirdik. */

        ui->textBrowser->setTextColor(Qt::gray);
        ui->textBrowser->setCurrentFont(QFont("Times New Roman",10));
        ui->textBrowser->append("From Client :"+QTime::currentTime().toString());

        ui->textBrowser->setTextColor(Qt::black);
        ui->textBrowser->setCurrentFont(QFont("Times New Roman",16));
        ui->textBrowser->append(str);
    }
}

void Client::Socket_Disconnect(){
    ui->btn_Send->setEnabled(false);
    ui->btn_Connect->setText("Connect");
    QMessageBox::warning(this, "BİLGİ", "Bağlantı koptu!", "Tamam");
}
