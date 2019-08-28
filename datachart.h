#ifndef DATACHART_H
#define DATACHART_H
#include <QObject>
#include <QWidget>
#include <QStringList>
#include "thirdparty/qcustomplot.h"

//#define DATACHART_MAX_X 5.0
//#define DATACHART_MAX_Y 5.0
//#define DATACHART_SCROLL_SCALE 10.0

enum PlotPolicy {
    frame,
    append
};

class DataChart : public QWidget
{
    Q_OBJECT

    public:
        static const qreal max_x;
        static const qreal max_y;
        static const qreal scroll_scale;
        DataChart(QWidget *parent=nullptr);
        ~DataChart();
        void initGraph(const QStringList &legends);
        void addPoints(double time, const QStringList &points,
                       const PlotPolicy &policy = PlotPolicy::frame);
        void cleanup();

    public slots:
        void setHorizontalAxisRange(int value);
        void setHorizontalAxisStart(int value);

    private:
        QSharedPointer<QCPAxisTickerTime> timeTicker;
        QCustomPlot *plot;
//        const double max_x = DATACHART_MAX_X;
//        const double max_y = DATACHART_MAX_Y;

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



#endif // DATACHART_H
