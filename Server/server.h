#ifndef SERVER_H
#define SERVER_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class server; }
QT_END_NAMESPACE

class server : public QWidget{
    Q_OBJECT

public:
    server(QWidget *parent = nullptr);
    ~server();

private slots:
    void on_btn_listen_clicked();
    void on_btn_send_clicked();
    void server_New_Connect();
    void socket_Recv_Data();
    void socket_Disconnect();

private:
    Ui::server *ui;
    QTcpServer *m_server;
    QTcpSocket *m_socket;
};
#endif // SERVER_H
