#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTextStream>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    findFreePorts();
    on_pbt_apply_clicked();

}

MainWindow::~MainWindow()
{
    delete ui;
}

//尋找空置的通訊埠
void MainWindow::findFreePorts(){
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    for (int i = 0; i < ports.size(); ++i){
        if (ports.at(i).isNull()){
            ports.removeAt(i);
            continue;
        }
        ui->portName->addItem(ports.at(i).portName());
    }
    if (!ports.size()){
        ui->comLog->insertPlainText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "can't seek com port usable！");
        return;
    }
    ui->baudRate->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    ui->baudRate->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    ui->baudRate->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    ui->baudRate->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
    ui->baudRate->addItem(tr("Custom"));
    ui->dataBits->addItem(QStringLiteral("5"), QSerialPort::Data5);
    ui->dataBits->addItem(QStringLiteral("6"), QSerialPort::Data6);
    ui->dataBits->addItem(QStringLiteral("7"), QSerialPort::Data7);
    ui->dataBits->addItem(QStringLiteral("8"), QSerialPort::Data8);
    ui->dataBits->setCurrentIndex(3);
    ui->parity->addItem(tr("None"), QSerialPort::NoParity);
    ui->parity->addItem(tr("Even"), QSerialPort::EvenParity);
    ui->parity->addItem(tr("Odd"), QSerialPort::OddParity);
    ui->parity->addItem(tr("Mark"), QSerialPort::MarkParity);
    ui->parity->addItem(tr("Space"), QSerialPort::SpaceParity);
    ui->parity->setCurrentIndex(1);
    ui->stopBits->addItem(QStringLiteral("1"), QSerialPort::OneStop);
#ifdef Q_OS_WIN
    ui->stopBits->addItem(tr("1.5"), QSerialPort::OneAndHalfStop);
#endif
    ui->stopBits->addItem(QStringLiteral("2"), QSerialPort::TwoStop);
    ui->flowControl->addItem(tr("None"), QSerialPort::NoFlowControl);
    ui->flowControl->addItem(tr("RTS/CTS"), QSerialPort::HardwareControl);
    ui->flowControl->addItem(tr("XON/XOFF"), QSerialPort::SoftwareControl);
}
//初始化通訊埠
bool MainWindow::initSerialPort(){

    serialPort->setPortName(ui->portName->currentText());
    serialPort->setBaudRate(ui->baudRate->currentText().toInt());

    if (ui->dataBits->currentText().toInt() == 8){
        serialPort->setDataBits(QSerialPort::Data8);
    }else if (ui->dataBits->currentText().toInt() == 7){
        serialPort->setDataBits(QSerialPort::Data7);

    }else if (ui->dataBits->currentText().toInt() == 6){
        serialPort->setDataBits(QSerialPort::Data6);

    }else if (ui->dataBits->currentText().toInt() == 5){
        serialPort->setDataBits(QSerialPort::Data5);
    }
    if (ui->stopBits->currentText().toInt() == 1){
        serialPort->setStopBits(QSerialPort::OneStop);

    }else if (ui->stopBits->currentText().toInt() == 2){
        serialPort->setStopBits(QSerialPort::TwoStop);
    }
    if(ui->parity->currentText() == "None"){
        serialPort->setParity(QSerialPort::NoParity);
    }else if (ui->parity->currentText() == "Even"){
        serialPort->setParity(QSerialPort::EvenParity);
    }else if (ui->parity->currentText() == "Odd"){
        serialPort->setParity(QSerialPort::OddParity);
    }else if (ui->parity->currentText() == "Mark"){
        serialPort->setParity(QSerialPort::MarkParity);
    }else if (ui->parity->currentText() == "Space"){
        serialPort->setParity(QSerialPort::SpaceParity);
    }
    if(ui->flowControl->currentText() == "None")
    {
        serialPort->setFlowControl(QSerialPort::NoFlowControl);
    }else if(ui->flowControl->currentText() == "RTS/CTS"){
        serialPort->setFlowControl(QSerialPort::HardwareControl);
    }else if(ui->flowControl->currentText() == "XON/XOFF"){
        serialPort->setFlowControl(QSerialPort::SoftwareControl);
    }
    if(serialPort->open(QIODevice::ReadWrite))
    {
        ui->comLog->insertPlainText(QDateTime::currentDateTime().toString("hh:mm:ss") + " Serial Port connect success！！" + char(10));
    }else{
        ui->comLog->insertPlainText(QDateTime::currentDateTime().toString("hh:mm:ss") + " Serial Port connect failure！！" + serialPort->errorString() + char(10));
        if (serialPort->isOpen())
        {
            serialPort->close();
        }
    }
    return true;
}
//向通訊埠發送訊息
void MainWindow::sendMsg(QString msg){
    if(serialPort->isOpen())
    {
        serialPort->write( msg.toLatin1() + char(10));
        serialPort->flush();
        ui->comLog->insertPlainText(QDateTime::currentDateTime().toString("hh:mm:ss") + " SEND：" + msg + "\n");
    }
}
//向通訊埠發送訊息2
void MainWindow::sendMsg2(QString msg){
    if(serialPort->isOpen())
    {
      //  serialPort->write(QByteArray::fromHex(msg.toLatin1()));
        serialPort->write( msg.toLatin1() + "\n");
        serialPort->flush();
        ui->comLog->insertPlainText(QDateTime::currentDateTime().toString("hh:mm:ss") + " SEND：" + msg );
    }
}
//接受來自通訊埠的訊息
void MainWindow::recvMsg()
{
    if(serialPort->isOpen())
    {
        while (serialPort->bytesAvailable()) {
            data_from_serialPort += serialPort->readAll();
            if(data_from_serialPort.at(data_from_serialPort.length()-1)==char(10))
            {
                is_data_recevied = true;
            }
            if(is_data_recevied==true)
            {
                ui->comLog->insertPlainText(QDateTime::currentDateTime().toString("hh:mm:ss") + " RECV： " + data_from_serialPort + "\n");
                data_from_serialPort = "";
                is_data_recevied = false;
            }
        }

    }


}
//接受來自通訊埠的訊息2
void MainWindow::recvMsg2()
{
    QByteArray msg = serialPort->readAll();
    QTextStream in(&msg);
    QString line = in.readLine();//過濾多餘的資料
   // ui->comLog->insertPlainText(QDateTime::currentDateTime().toString("hh:mm:ss") + " RECV： " + msg.toHex().data() + "\n");
   // ui->comLog->insertPlainText(QDateTime::currentDateTime().toString("hh:mm:ss") + " RECV： " + line + "\n");
    QString str = QDateTime::currentDateTime().toString("hh:mm:ss") + "  RECV： " + line +  "\n";
    ui->comLog->append(str);
}

void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), serialPort->errorString());

    }
}

void MainWindow::on_open_Com_stateChanged(int arg1)
{
    if(arg1==2)
    {
        initSerialPort();
        ui->btn_Send->setEnabled(true);
    }else{
        serialPort->close();

        ui->btn_Send->setEnabled(false);
        ui->open_Com->setChecked(false);
    }
}


void MainWindow::on_btn_Send_clicked()
{
    sendMsg2(ui->message->toPlainText().trimmed());
}

//com port write parameter
void MainWindow::on_pbt_apply_clicked()
{
    serialPort = new QSerialPort(this);
    connect(serialPort, &QSerialPort::errorOccurred, this, &MainWindow::handleError);
    connect(this , &MainWindow::destroyed, this, &QSerialPort::deleteLater);
    connect(serialPort, SIGNAL(readyRead()), this, SLOT(recvMsg2()));
}

