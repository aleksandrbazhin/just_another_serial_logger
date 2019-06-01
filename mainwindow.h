#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QMessageBox>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QPlainTextEdit>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void handleConnectButton();
    bool openSerialPort();
    void closeSerialPort();
    void readData();
    void resetRecording();
    void startStopRecording();
    void appendRow(QPlainTextEdit *edit, const QString &text);

    Ui::MainWindow *ui;
    QSerialPort *serial = nullptr;
    bool serial_connected = false;
    bool recording_started = false;
    qint64 recording_start_time = 0;
    bool data_header_received = false;
    QString temp_receive = "";
    QString data_to_save = "";


};

#endif // MAINWINDOW_H
