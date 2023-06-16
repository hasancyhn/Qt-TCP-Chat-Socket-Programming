#include "server.h"
#include "ui_server.h"
#include <QtWidgets/QMessageBox>
#include <QTime>

server::server(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::server){
    ui->setupUi(this);
    ui->btn_send->setEnabled(false);

    m_server = new QTcpServer();
    connect(m_server,&QTcpServer::newConnection,this,&::server::server_New_Connect);    /* Bağlantıyı kuruyoruz newConnection argümanı ve socket_New_Connect fonksiyonuyla bağlantı kurararak. */
}

server::~server(){
    m_server->close();                                                                  /* m_server'i kapatır. */
    m_server->deleteLater();                                                            /* Potansiyel olarak çok fazla alan/bellek tüketildiğinde deleteLater() ile silerek tekrar alan kazanırız. */
    delete ui;
}

void server::on_btn_listen_clicked(){
    if(ui->btn_listen->text() ==QString("Listen"))
    {
        qint16 port = ui->lineEdit_Port->text().toInt();                                /* Kullanıcıdan port numarası alınır ve integer'e çevrilir. */
                                                                                        /* Integer değerleri farklı	 platformlarda farklı boyutlarda olabilirler, qint boyutlarının tanınabilir	olmasını sağlar. */
        if(!m_server->listen(QHostAddress::Any,port))                                   /* Bu adrese bağlı bir soket hem IPv4 hem de IPv6 arabirimlerini dinleyecektir. Kullanıcıdan da port adresi alınacak. */
        {
            QMessageBox::critical(this,"Error !", m_server->errorString(), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
            return;
        }
        ui->btn_listen->setText("Cancel Listening");
    }
    else
    {
        if(m_socket->state() == QAbstractSocket::ConnectedState)                        /* state(), işlemin mevcut durumunu döndürür. ConnectedState sayesinde bağlantı kurulur. */
        {
            m_socket->disconnectFromHost();                                             /* Soketi kapatmaya çalışır. Yazılmayı bekleyen veriler varsa onlar yazılana kadar beklenir sonra disconnected() atılır. */
        }
        m_server->close();
        ui->btn_listen->setText("Listen");
        ui->btn_send->setEnabled(false);
    }
}


void server::on_btn_send_clicked(){
    if(ui->textEdit->toPlainText() == QString())
    {
        QMessageBox::critical(this, "BİLGİ", "Boş mesaj gönderemezsin!", "Tamam");
    }
    m_socket->write(ui->textEdit->toPlainText().toUtf8());                              /* Kullanıcıdan alınan veriler tutulur ve yazılır. */

    ui->textBrowser->setTextColor(Qt::gray);
    ui->textBrowser->setCurrentFont(QFont("Times New Roman",10));
    ui->textBrowser->append("From Server: "+QTime::currentTime().toString());

    ui->textBrowser->setTextColor(Qt::black);
    ui->textBrowser->setCurrentFont(QFont("Times New Roman",16));
    ui->textBrowser->append(ui->textEdit->toPlainText().toUtf8());

    m_socket->flush();                                                                  /* Verinin gerçekten yazıldığını doğrulamak için flush() parametresini kullandık. */
    ui->textEdit->clear();
}

void server::server_New_Connect(){
    m_socket = m_server->nextPendingConnection();                                               /* Bir sonraki bekleyen bağlantıyı bağlar bir QTcpSocket nesnesi olarak döndürür. */

    QObject::connect(m_socket,&QTcpSocket::readyRead,this,&server::socket_Recv_Data);           /* Verileri okuyoruz readyRead argümanı ve socket_Recv_Data fonksiyonuyla bağlantı kurararak. */
    QObject::connect(m_socket,&QTcpSocket::disconnected,this,&server::socket_Disconnect);       /* Bağlantıyı koparıyoruz disconnected argümanı ve socket_Disconnect fonksiyonuyla bağlantı kurararak. */

    ui->textBrowser->setTextColor(Qt::gray);
    ui->textBrowser->setCurrentFont(QFont("Times New Roman",10));
    ui->textBrowser->append(tr("Client Connect ! %1\n").arg(QTime::currentTime().toString()));
    ui->btn_send->setEnabled(true);
}

void server::socket_Recv_Data(){
    QByteArray data_tmp;
    data_tmp = m_socket->readAll();
    if(!data_tmp.isEmpty())                                                                     /* data_tmp değişkeninin içinde herhangi bir değer varsa aşağıdaki işlemler gerçekleştirilir. */
    {
        QString str = QString(data_tmp);

        ui->textBrowser->setTextColor(Qt::gray);
        ui->textBrowser->setCurrentFont(QFont("Times New Roman",10));
        ui->textBrowser->append("From Client: "+QTime::currentTime().toString());

        ui->textBrowser->setTextColor(Qt::black);
        ui->textBrowser->setCurrentFont(QFont("Times New Roman",16));
        ui->textBrowser->append(str);

        QTextCursor cursor = ui->textBrowser->textCursor();                 /* Automatically pull down to the bottom. */
        cursor.movePosition(QTextCursor::End);                              /* İmleç belgenin sonuna gider. */
        ui->textBrowser->setTextCursor(cursor);                             /* İmleç görünür hale getirildi. */
    }
}

void server::socket_Disconnect(){
    ui->btn_send->setEnabled(false);
    ui->textBrowser->setTextColor(Qt::gray);
    ui->textBrowser->setCurrentFont(QFont("Times New Roman",10));
    ui->textBrowser->append(tr("Client Left ! %1\n").arg(QTime::currentTime().toString()));
}
