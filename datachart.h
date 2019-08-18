#ifndef DATACHART_H
#define DATACHART_H
#include <QObject>
#include <QWidget>
#include <QStringList>
#include "thirdparty/qcustomplot.h"

enum PlotPolicy {
    frame,
    append
};

class DataChart : public QWidget
{
    Q_OBJECT

    public:
        DataChart(QWidget *parent=nullptr);
        ~DataChart();
        void initGraph(const QStringList &legends);
        void addPoints(double time, const QStringList &points,
                       const PlotPolicy &policy = PlotPolicy::frame);
        void cleanup();

    private:
        QSharedPointer<QCPAxisTickerTime> timeTicker;
        QCustomPlot *plot;
        double max_x = 5.0;
        double max_y = 5.0;

        const QVector<QColor> colors = {
            QColor(57, 106, 177),
            QColor(218, 124, 48),
            QColor(62, 150, 81),
            QColor(204, 37, 41),
            QColor(83, 81, 84),
            QColor(107, 76, 154),
            QColor(146, 36, 40),
            QColor(148, 139, 61),
        };
};

;

#endif // DATACHART_H
