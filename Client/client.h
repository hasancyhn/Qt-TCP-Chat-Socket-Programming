#ifndef CLIENT_H
#define CLIENT_H

#include <QWidget>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class Client; }
QT_END_NAMESPACE

class Client : public QWidget{
    Q_OBJECT

public:
    Client(QWidget *parent = nullptr);
    ~Client();

private slots:
    void on_btn_Connect_clicked();  // Bağlanma butonu
    void on_btn_Send_clicked();     // Gönder butonu
    void socket_Recv_Data();        // Veri almak için
    void Socket_Disconnect();       // Bağlantıyı kesmek için

private:
    Ui::Client *ui;
    QTcpSocket *m_socket;           // MainWindow içindeki her yerden erişebilmek için buraya tanımladık.
};
#endif // CLIENT_H
