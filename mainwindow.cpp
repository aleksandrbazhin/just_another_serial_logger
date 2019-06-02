#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QScrollBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QSysInfo>
#include <QSerialPortInfo>
#include <QTextStream>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    serial(new QSerialPort(this)),
    datachart(new DataChart)
{
    this->ui->setupUi(this);
    this->initChart();

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
            this->recording_start_time = QDateTime::currentMSecsSinceEpoch();
            this->appendRow(this->ui->dataPlainTextEdit, "-------------connect--------------");
            this->appendRow(this->ui->timePlainTextEdit, "-connect-");
        }
    } else {
        this->ui->connectButton->setText("Connect");
        this->serial_connected = false;
//        this->ui->sendLineEdit->setEnabled(false);
        this->closeSerialPort();
        this->ui->recordButton->setEnabled(false);
        this->recording_started = false;
        this->resetRecording();
        this->appendRow(this->ui->dataPlainTextEdit, "------------disconnect--------------");
        this->appendRow(this->ui->timePlainTextEdit, "disconnect");
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
    QString data_string(data);
    if (!data_string.endsWith("\n")) {
        this->temp_receive.append(data_string);
        return;
    } else {
        data_string.prepend(this->temp_receive);
        this->temp_receive = "";
    }

    if (!this->data_header_received)  {
        this->recording_start_time = QDateTime::currentMSecsSinceEpoch();
        QStringList received_headers = this->getEntriesAt(data_string, 0);
        QString header = "time," + received_headers.join(",") + "\r\n";
        this->data_to_save += header;
        this->data_header_received = true;
        this->datachart->initGraph(received_headers);
    }

    qreal time_sec =
            (QDateTime::currentMSecsSinceEpoch() - this->recording_start_time) / 1000.0;
    QStringList received_values = this->getEntriesAt(data_string, 1);

    if (this->recording_started) {
        QString row = QString::number(time_sec) + "," + received_values.join(",") + "\r\n";
        this->data_to_save += row;
    }

    this->appendRow(this->ui->dataPlainTextEdit, data_string.trimmed());
    this->appendRow(this->ui->timePlainTextEdit, QString::number(time_sec));
    this->datachart->addPoints(time_sec, received_values);
}

void MainWindow::resetRecording()
{
    this->recording_start_time = QDateTime::currentMSecsSinceEpoch();
    this->data_to_save = "";
    this->ui->recordButton->setText("Start record");
    this->data_header_received = false;
}

void MainWindow::startStopRecording()
{
    if (!this->recording_started) {
        this->ui->recordButton->setText("Stop and save");
        this->ui->dataPlainTextEdit->clear();
        this->ui->timePlainTextEdit->clear();
        this->recording_started = true;
        this->data_header_received = false;
    } else {
        this->recording_started = false;
        this->appendRow(this->ui->dataPlainTextEdit, "-----------recording end-------------");
        this->appendRow(this->ui->timePlainTextEdit, "--r end--");
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

void MainWindow::appendRow(QPlainTextEdit *edit, const QString &text)
{
    edit->appendPlainText(text);
    QScrollBar *bar = edit->verticalScrollBar();
    bar->setValue(bar->maximum());
}

void MainWindow::initChart()
{
    this->datachart->setTitle("Data");
    this->datachart->legend()->hide();
    this->datachart->setAnimationOptions(QChart::AllAnimations);
    this->ui->chartGraphicsView->setChart(this->datachart);
//    this->ui->chartGraphicsView->set
    this->ui->chartGraphicsView->setRenderHint(QPainter::Antialiasing, true);
}

QStringList MainWindow::getEntriesAt(const QString &data_string,
                                     int data_position,
                                     int data_length,
                                     QString data_sep,
                                     QString entry_sep)
{
    QStringList entries = data_string.split(data_sep);
    QStringList values;
    for (QString entry: entries) {
        QStringList data_el = entry.trimmed().split(entry_sep);
        if (data_el.count() == data_length) {
            values.append(data_el.at(data_position));
        }
    }
    return values;
}


