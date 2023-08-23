#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>

#include <QList>
#include <QMessageBox>
#include <QDateTime>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void findFreePorts();
    bool initSerialPort();
    void sendMsg(QString msg);
    void sendMsg2(QString msg);

public slots:
    void recvMsg();
    void recvMsg2();
    void handleError(QSerialPort::SerialPortError error);

private slots:
    void on_open_Com_stateChanged(int arg1);

    void on_btn_Send_clicked();

    void on_pbt_apply_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort *serialPort;
    QString data_from_serialPort;
    bool is_data_recevied = false;
};

#endif // MAINWINDOW_H
