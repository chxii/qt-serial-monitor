#define _USE_MATH_DEFINES
#include "DataGenerator.h"
#include <cmath>
#include <QRandomGenerator>

DataGenerator::DataGenerator(QObject *parent) : QObject(parent)
{
    connect(&m_timer, &QTimer::timeout, this, &DataGenerator::tick);
    m_timer.setInterval(50); // 20 Hz — fills screen faster
}

void DataGenerator::start()
{
    m_elapsed.start();
    m_timer.start();
}

void DataGenerator::stop()
{
    m_timer.stop();
}

bool DataGenerator::isRunning() const
{
    return m_timer.isActive();
}

void DataGenerator::tick()
{
    double value;
    if (m_sine) {
        double t = m_elapsed.elapsed() / 1000.0;
        value = m_amp * std::sin(2.0 * M_PI * m_freq * t);
    } else {
        value = m_amp * (QRandomGenerator::global()->generateDouble() * 2.0 - 1.0);
    }
    emit dataReady(value);
}
