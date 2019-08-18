#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QMessageBox>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QPlainTextEdit>
#include <QVector>
#include "datachart.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void handleConnectButton();
    void disconnectUiUpdate();
    void handleSerialError(QSerialPort::SerialPortError error);
    void discoverPorts();
    void resetUI();
    void scrollRaw(int to);
    void scrollParsed(int to);
    void showSettingsDialog();
    void handleSendButton();

private:
    void initUi();
    bool openSerialPort();
    void closeSerialPort();
    void connectUiUpdate();
    void readData();
    void resetRecording();
    void startStopRecording();
    void appendRow(QPlainTextEdit *edit, const QString &text);
    void saveRecorded();
    void sendDataToPort(const QString &data_string);

    QStringList getEntriesAt(const QString &data_string,
                             int data_position = 1,
                             int data_length = 2,
                             QString data_sep = ",",
                             QString entry_sep = " ");

    Ui::MainWindow *ui;
    QSerialPort *serial = nullptr;
    bool serial_connected = false;
    bool recording_started = false;
    qint64 recording_start_time = 0;
    bool data_header_received = false;
    QString temp_receive = "";
    QString data_to_save = "";
    bool throw_away_first = true;
    bool thrown_first_away = false;

};

#endif // MAINWINDOW_H
