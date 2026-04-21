#include "ChartWidget.h"
#include <QtCharts/QChart>

ChartWidget::ChartWidget(QWidget *parent) : QChartView(parent)
{
    setRenderHint(QPainter::Antialiasing);

    m_series = new QLineSeries();
    QPen pen(QColor("#2563EB"));
    pen.setWidth(2);
    m_series->setPen(pen);

    m_axisX = new QValueAxis();
    m_axisX->setRange(0, MAX_POINTS);
    m_axisX->setLabelFormat("%g");
    m_axisX->setTitleText("Samples");

    m_axisY = new QValueAxis();
    m_axisY->setRange(-1.5, 1.5);
    m_axisY->setTitleText("Value");

    QChart *c = new QChart();
    c->addSeries(m_series);
    c->addAxis(m_axisX, Qt::AlignBottom);
    c->addAxis(m_axisY, Qt::AlignLeft);
    m_series->attachAxis(m_axisX);
    m_series->attachAxis(m_axisY);
    c->legend()->hide();
    c->setBackgroundBrush(QColor("#FFFFFF"));
    c->setMargins(QMargins(8, 8, 8, 8));

    setChart(c);
    setBackgroundBrush(QColor("#FFFFFF"));
}

void ChartWidget::addPoint(double value)
{
    m_series->append(m_sampleCount, value);
    ++m_sampleCount;

    if (m_series->count() > MAX_POINTS) {
        m_series->remove(0);
        m_axisX->setRange(m_sampleCount - MAX_POINTS, m_sampleCount);
    }

    // Dynamic Y-axis: scan visible points, add 15% padding
    const auto &pts = m_series->points();
    if (pts.isEmpty()) return;
    double yMin = pts[0].y(), yMax = pts[0].y();
    for (const auto &p : pts) {
        if (p.y() < yMin) yMin = p.y();
        if (p.y() > yMax) yMax = p.y();
    }
    double pad = (yMax - yMin) * 0.15;
    if (pad < 0.05) pad = 0.05; // minimum padding when signal is flat
    m_axisY->setRange(yMin - pad, yMax + pad);
}

void ChartWidget::clear()
{
    m_series->clear();
    m_sampleCount = 0;
    m_axisX->setRange(0, MAX_POINTS);
}
