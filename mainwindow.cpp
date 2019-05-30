#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QScrollBar>
#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    serial(new QSerialPort(this))
{
    ui->setupUi(this);

    connect(this->ui->connectButton, &QPushButton::pressed, this, &MainWindow::handleConnectButton);
    connect(this->ui->recordButton, &QPushButton::pressed, this, &MainWindow::startRecording);
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
    }
}

bool MainWindow::openSerialPort()
{
    this->serial->setPortName(this->ui->portLineEdit->text());
//    this->serial->setBaudRate(this->ui->baudLineEdit->text().toInt());
    this->serial->setBaudRate(QSerialPort::Baud9600);
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

    this->ui->dataPlainTextEdit->insertPlainText(data);
    QScrollBar *bar = this->ui->dataPlainTextEdit->verticalScrollBar();
    bar->setValue(bar->maximum());

    if (this->recording_started) {
        QString data_string(data);
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
        QString msec = QString::number(
                    QDateTime::currentMSecsSinceEpoch() - this->recording_start_time);
        this->ui->timePlainTextEdit->insertPlainText(msec);
        this->ui->timePlainTextEdit->insertPlainText("\r\n");

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

void MainWindow::startRecording()
{
    if (!this->recording_started) {
        this->recording_started = true;
        this->recording_start_time = QDateTime::currentMSecsSinceEpoch();
        this->ui->recordButton->setText("Stop and save");
        this->ui->dataPlainTextEdit->clear();
        this->ui->timePlainTextEdit->clear();

    } else {
        this->recording_started = false;
        this->data_header_received = false;
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                   "test",
                                   tr("CSV (*.сsv)"));

        if (!fileName.isEmpty()) {
            QFile file(fileName);
            if (!file.open(QIODevice::WriteOnly)) {
                QMessageBox::information(this, tr("Unable to open file"),
                    file.errorString());
            }

            QDataStream out(&file);
            out << this->data_to_save.trimmed().toLatin1();

        } else {
            QMessageBox msgBox;
            msgBox.setText("something happened");
            msgBox.exec();
        }
        this->data_to_save = "";
        this->ui->recordButton->setText("Start record");
    }
}






