#ifndef DATACHART_H
#define DATACHART_H
#include <Qt>
#include <QChart>
#include <QStringList>


QT_CHARTS_BEGIN_NAMESPACE
class QLineSeries;
class QValueAxis;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

class DataChart : public QChart
{
    Q_OBJECT

public:
    DataChart(QGraphicsItem *parent = nullptr, Qt::WindowFlags wFlags = nullptr);
    virtual ~DataChart();

public slots:
    void initGraph(const QStringList &legends);
    void addPoints(qreal time, const QStringList &points);

private:
    QList<QLineSeries*> chart_series;
    bool drawing_initialized = false;

    QStringList titles;
    QValueAxis *axisX;
    QValueAxis *axisY;

    qreal max_x = 5.0;
    qreal max_y = 128.0;
};

#endif // DATACHART_H
