#include "datachart.h"
#include <QtCharts/QAbstractAxis>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QDebug>


DataChart::DataChart(QGraphicsItem *parent, Qt::WindowFlags wFlags):
    QChart(QChart::ChartTypeCartesian, parent, wFlags),
    axisX(new QValueAxis()),
    axisY(new QValueAxis())
{
    this->addAxis(this->axisX, Qt::AlignBottom);
    this->addAxis(this->axisY, Qt::AlignLeft);
    this->axisX->setRange(0, this->max_x);
    this->axisY->setRange(0, this->max_y);
    this->axisX->setLabelsVisible(false);
    this->axisX->setGridLineVisible(false);
    this->setAnimationOptions(QChart::NoAnimation);
}

DataChart::~DataChart()
{

}

void DataChart::initGraph(const QStringList &legend)
{
    for (auto one_series: this->chart_series) {
        one_series->clear();
    }
    this->series().clear();
    this->chart_series.clear();
    for (QString title: legend) {
        QLineSeries *series = new QLineSeries(this);
        series->setName(title);
        this->chart_series.append(series);
        this->addSeries(series);
        series->attachAxis(this->axisX);
        series->attachAxis(this->axisY);

    }
}

void DataChart::addPoints(qreal time, const QStringList &points)
{
    qDebug() << points;
    if (this->chart_series.length() != points.length()) {
        return;
    }
    if (time > this->max_x) {
        this->max_x = time;
        this->axisX->setMax(this->max_x);
    }
    for (int i = 0; i < points.length(); i++) {
        qreal y = points[i].toDouble();
        if (y > this->max_y) {
            this->max_y = y + 50; // some random number large enough
            this->axisY->setMax(this->max_y);
        }
        this->chart_series[i]->append(time, y);
    }
}
