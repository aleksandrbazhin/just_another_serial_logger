#include <QScrollBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QSysInfo>
#include <QSerialPortInfo>
#include <QTextStream>
#include <QDebug>
#include <QStringBuilder>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "portscombobox.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    serial(new QSerialPort(this)),
    datachart(new DataChart)
{
    this->ui->setupUi(this);
    this->initChart();
    this->discoverPorts();

    connect(this->ui->portComboBox, SIGNAL(popupShowed()), this, SLOT(discoverPorts()));
    connect(this->ui->connectButton, &QPushButton::pressed, this, &MainWindow::handleConnectButton);
    connect(this->ui->recordButton, &QPushButton::pressed, this, &MainWindow::startStopRecording);
    connect(this->ui->resetUIButton, &QPushButton::pressed, this, &MainWindow::resetUI);
    connect(this->ui->parsedPlainTextEdit->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(scrollRaw(int)));
    connect(this->ui->rawPlainTextEdit->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(scrollParsed(int)));

    connect(this->serial, &QSerialPort::errorOccurred, this, &MainWindow::handleSerialError);
    connect(this->serial, &QSerialPort::readyRead, this, &MainWindow::readData);
    connect(this->serial, &QSerialPort::aboutToClose, this, &MainWindow::disconnectUiUpdate);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::handleConnectButton()
{
    if (!this->serial_connected) {
        if (this->openSerialPort()) {
            this->connectUiUpdate();
        }
    } else {
        this->data_header_received = false;
        this->closeSerialPort();
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

void MainWindow::connectUiUpdate()
{
    this->ui->connectButton->setText("Disconnect");
    this->serial_connected = true;
//            this->ui->sendLineEdit->setEnabled(true);
    this->ui->recordButton->setEnabled(true);
    this->recording_start_time = QDateTime::currentMSecsSinceEpoch();
    this->appendRow(this->ui->rawPlainTextEdit, "-------------connect--------------");
    this->appendRow(this->ui->parsedPlainTextEdit, "");
}

void MainWindow::disconnectUiUpdate()
{
    this->ui->connectButton->setText("Connect");
    this->serial_connected = false;
//        this->ui->sendLineEdit->setEnabled(false);
    this->ui->recordButton->setEnabled(false);
    if (this->recording_started) {
        this->startStopRecording();
    }
    this->appendRow(this->ui->rawPlainTextEdit, "------------disconnect--------------");
    this->appendRow(this->ui->parsedPlainTextEdit, "");
}

void MainWindow::handleSerialError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        this->closeSerialPort();
    }
}

void MainWindow::discoverPorts()
{
    this->ui->portComboBox->clear();
    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()) {
        this->ui->portComboBox->addItem(serialPortInfo.portName());
    }
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
        QString header = "time," % received_headers.join(",");
        this->appendRow(this->ui->rawPlainTextEdit, "-----------headers parsed-----------");
        this->appendRow(this->ui->parsedPlainTextEdit, header);

        this->data_to_save = this->data_to_save % header % "\r\n";
        this->data_header_received = true;
        this->datachart->initGraph(received_headers);
    }

    qreal time_sec =
            (QDateTime::currentMSecsSinceEpoch() - this->recording_start_time) / 1000.0;
    QStringList received_values = this->getEntriesAt(data_string, 1);

    QString row = QString::number(time_sec) % "," % received_values.join(",");
    this->appendRow(this->ui->rawPlainTextEdit, data_string.trimmed());
    this->appendRow(this->ui->parsedPlainTextEdit, row);
    if (this->recording_started) {
        this->data_to_save = this->data_to_save % row % "\r\n";
    }

    this->datachart->addPoints(time_sec, received_values);
}


void MainWindow::resetUI()
{
    this->ui->rawPlainTextEdit->clear();
    this->ui->parsedPlainTextEdit->clear();
    this->ui->chartGraphicsView->chart()->resetMatrix();
    this->ui->chartGraphicsView->chart()->resetTransform();
}

void MainWindow::scrollRaw(int to)
{
    this->ui->rawPlainTextEdit->verticalScrollBar()->setValue(to);
}

void MainWindow::scrollParsed(int to)
{
    this->ui->parsedPlainTextEdit->verticalScrollBar()->setValue(to);
}

void MainWindow::resetRecording()
{
    this->recording_start_time = QDateTime::currentMSecsSinceEpoch();
    this->data_to_save = "";
    this->data_header_received = false;
}

void MainWindow::startStopRecording()
{
    if (!this->recording_started) {
        this->recording_started = true;
        this->resetUI();
        this->resetRecording();
        this->ui->recordButton->setText("Stop and save");
    } else {
        this->saveRecorded();
        this->recording_started = false;
//        this->appendRow(this->ui->rawPlainTextEdit, "-----------recording end-------------");
//        this->appendRow(this->ui->parsedPlainTextEdit, "-----------recording end------------");

        this->resetUI();
        this->ui->recordButton->setText("Start record");
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
    this->ui->chartGraphicsView->setRenderHint(QPainter::Antialiasing, true);
}

void MainWindow::saveRecorded()
{
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


