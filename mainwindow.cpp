#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QScrollBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QSysInfo>
#include <QSerialPortInfo>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    serial(new QSerialPort(this))
{
    this->ui->setupUi(this);

    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()) {
        this->ui->portComboBox->addItem(serialPortInfo.portName());
    }

    connect(this->ui->connectButton, &QPushButton::pressed, this, &MainWindow::handleConnectButton);
    connect(this->ui->recordButton, &QPushButton::pressed, this, &MainWindow::startStopRecording);
    connect(this->serial, &QSerialPort::readyRead, this, &MainWindow::readData);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::handleConnectButton()
{
    if (!this->serial_connected) {
        if (this->openSerialPort()) {
            this->ui->connectButton->setText("Disconnect");
            this->serial_connected = true;
//            this->ui->sendLineEdit->setEnabled(true);
            this->ui->recordButton->setEnabled(true);
        }
    } else {
        this->ui->connectButton->setText("Connect");
        this->serial_connected = false;
//        this->ui->sendLineEdit->setEnabled(false);
        this->closeSerialPort();
        this->ui->recordButton->setEnabled(false);
        this->recording_started = false;
        this->resetRecording();
    }
}

bool MainWindow::openSerialPort()
{
    this->serial->setPortName(this->ui->portComboBox->currentText());
    this->serial->setBaudRate(this->ui->baudComboBox->currentText().toInt());
    this->serial->setDataBits(QSerialPort::Data8);
    this->serial->setParity(QSerialPort::NoParity);
    this->serial->setStopBits(QSerialPort::OneStop);
    if (this->serial->open(QIODevice::ReadWrite)) {
        return true;
    } else {
        QMessageBox::critical(this, tr("Error"), this->serial->errorString());
        return false;

    }
}

void MainWindow::closeSerialPort()
{

    if (this->serial->isOpen())
        this->serial->close();
}

void MainWindow::readData()
{
    char data[256];
    this->serial->readLine(data, sizeof(data));
//    QByteArray data = this->serial->readAll();




    QString data_string(data);
    if (!data_string.endsWith("\n")) {
        this->temp_receive.append(data_string);
        return;
    } else {
        data_string.prepend(this->temp_receive);
        this->temp_receive = "";
    }

    this->ui->dataPlainTextEdit->appendPlainText(data_string.trimmed());
    QScrollBar *bar = this->ui->dataPlainTextEdit->verticalScrollBar();
    bar->setValue(bar->maximum());


    if (this->recording_started) {


        QStringList values = data_string.split(",");
        if (!this->data_header_received)  {
            QString header = "time,";
            for (QString s: values) {
                QStringList data_el = s.trimmed().split(" ");
                if (data_el.count() == 2) {
                    header += data_el.at(0) + ",";
                }
            }
            header += "\r\n";
            this->data_to_save += header;
            this->data_header_received = true;
        }
        QString msec = QString::number((
                    QDateTime::currentMSecsSinceEpoch() - this->recording_start_time) / 1000.0);

        this->ui->timePlainTextEdit->appendPlainText(msec);

        QString row = msec + ",";
        for (QString s: values) {
            QStringList data_el = s.trimmed().split(" ");
            if (data_el.count() == 2) {
                row += data_el.at(1) + ",";
            }
        }
        row += "\r\n";
        this->data_to_save += row;
    }
    QScrollBar *bar2 = this->ui->timePlainTextEdit->verticalScrollBar();
    bar2->setValue(bar2->maximum());
}

void MainWindow::resetRecording()
{
    this->data_to_save = "";
    this->ui->recordButton->setText("Start record");

    this->data_header_received = false;
}

void MainWindow::startStopRecording()
{
    if (!this->recording_started) {
        this->recording_start_time = QDateTime::currentMSecsSinceEpoch();
        this->ui->recordButton->setText("Stop and save");
        this->ui->dataPlainTextEdit->clear();
        this->ui->timePlainTextEdit->clear();
        this->recording_started = true;
    } else {
        this->recording_started = false;
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                   "experiment",
                                   tr("CSV as .txt (*.txt)"));
        if (!fileName.isEmpty()) {
            QFile file(fileName);
            if (!file.open(QIODevice::WriteOnly)) {
                QMessageBox::information(this, tr("Unable to open file"),
                    file.errorString());
            }

            QTextStream out(&file);
            out << this->data_to_save;

        }
        this->resetRecording();
    }
}





