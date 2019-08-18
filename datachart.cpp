#include "datachart.h"
#include <QDebug>
#include <QGridLayout>

DataChart::DataChart(QWidget *parent):
    QWidget(parent),
    plot(new QCustomPlot(this))
{
    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(this->plot, 0, 0);
//    this->plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    this->plot->plotLayout()->insertRow(0);
    QCPTextElement *title = new QCPTextElement(this->plot,
                                "Parsed data", QFont("sans", 10));
    this->plot->plotLayout()->addElement(0, 0, title);

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%m:%s");
    this->plot->xAxis->setTicker(timeTicker);
}

DataChart::~DataChart()
{

}


void DataChart::initGraph(const QStringList &legend)
{
    this->plot->clearGraphs();
    for (QString title: legend) {
        this->plot->addGraph();
    }
    for (int i = 0; i < this->plot->graphCount(); i++) {
            this->plot->graph(i)->setPen(
                        QPen(this->colors[i % this->colors.length()]));

        this->plot->graph(i)->setName(legend[i]);
    }
    this->plot->legend->setVisible(true);
    this->plot->xAxis->setRange(0, this->max_x, Qt::AlignRight);

}

void DataChart::addPoints(double time, const QStringList &points, const PlotPolicy &policy)
{
    if (this->plot->graphCount() == points.length()) {
        for (int i = 0; i < points.length(); i++) {
            double y = points[i].toDouble();
            this->plot->graph(i)->addData(time, y);
        }
        double plotXRange = this->max_x;
        if (policy == PlotPolicy::append) {
            plotXRange = this->max_x > time ? this->max_x : time;
        }
        this->plot->xAxis->setRange(time, plotXRange, Qt::AlignRight);
        this->plot->yAxis->rescale(true);
        this->plot->replot(QCustomPlot::rpQueuedReplot);
    }
}

void DataChart::cleanup()
{
    for (int i = 0; i < this->plot->graphCount(); i++) {
        this->plot->graph(i)->data()->clear();
    }
}
