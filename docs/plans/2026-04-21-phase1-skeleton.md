# Qt Serial Monitor Phase 1 — Skeleton Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Build a runnable Qt 6.11 app with correct layout, live sine wave chart, scrolling log, and Connect/Disconnect toggle.

**Architecture:** MainWindow assembles ControlPanel (left), ChartWidget + LogPanel (right via QSplitter), and a status bar. DataGenerator drives both chart and log via Qt signals at 10 Hz.

**Tech Stack:** Qt 6.11, C++17, MSVC2022, CMake 3.16+, Qt6::Widgets + Qt6::Charts + Qt6::SerialPort

---

### Task 1: CMakeLists.txt

**Files:**
- Create: `CMakeLists.txt`
- Create: `src/main.cpp`

**Step 1: Write CMakeLists.txt**

```cmake
cmake_minimum_required(VERSION 3.16)
project(QtSerialMonitor VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Charts SerialPort)

add_executable(QtSerialMonitor
    src/main.cpp
    src/MainWindow.cpp
    src/ControlPanel.cpp
    src/ChartWidget.cpp
    src/LogPanel.cpp
    src/DataGenerator.cpp
)

target_link_libraries(QtSerialMonitor PRIVATE
    Qt6::Widgets
    Qt6::Charts
    Qt6::SerialPort
)
```

**Step 2: Write src/main.cpp**

```cpp
#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setStyle("Fusion");

    MainWindow w;
    w.resize(1000, 640);
    w.setWindowTitle("Serial Monitor");
    w.show();

    return app.exec();
}
```

**Step 3: Build to verify CMake resolves Qt6**

```bash
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH="C:/Qt/6.11.0/msvc2019_64"
cmake --build . --config Debug
```

Expected: fails with "cannot find MainWindow.h" — that's fine, confirms Qt6 is found.

---

### Task 2: DataGenerator

**Files:**
- Create: `src/DataGenerator.h`
- Create: `src/DataGenerator.cpp`

**Step 1: Write DataGenerator.h**

```cpp
#pragma once
#include <QObject>
#include <QTimer>
#include <QElapsedTimer>

class DataGenerator : public QObject
{
    Q_OBJECT
public:
    explicit DataGenerator(QObject *parent = nullptr);
    void start();
    void stop();
    bool isRunning() const;

signals:
    void dataReady(double value);

private slots:
    void tick();

private:
    QTimer m_timer;
    QElapsedTimer m_elapsed;
    double m_freq = 1.0;
    double m_amp  = 1.0;
};
```

**Step 2: Write DataGenerator.cpp**

```cpp
#include "DataGenerator.h"
#include <cmath>

DataGenerator::DataGenerator(QObject *parent) : QObject(parent)
{
    connect(&m_timer, &QTimer::timeout, this, &DataGenerator::tick);
    m_timer.setInterval(100); // 10 Hz
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
    double t = m_elapsed.elapsed() / 1000.0;
    double value = m_amp * std::sin(2.0 * M_PI * m_freq * t);
    emit dataReady(value);
}
```

---

### Task 3: ChartWidget

**Files:**
- Create: `src/ChartWidget.h`
- Create: `src/ChartWidget.cpp`

**Step 1: Write ChartWidget.h**

```cpp
#pragma once
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

class ChartWidget : public QChartView
{
    Q_OBJECT
public:
    explicit ChartWidget(QWidget *parent = nullptr);

public slots:
    void addPoint(double value);
    void clear();

private:
    QLineSeries  *m_series;
    QValueAxis   *m_axisX;
    QValueAxis   *m_axisY;
    qint64        m_sampleCount = 0;
    static constexpr int MAX_POINTS = 200;
};
```

**Step 2: Write ChartWidget.cpp**

```cpp
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
}

void ChartWidget::clear()
{
    m_series->clear();
    m_sampleCount = 0;
    m_axisX->setRange(0, MAX_POINTS);
}
```

---

### Task 4: LogPanel

**Files:**
- Create: `src/LogPanel.h`
- Create: `src/LogPanel.cpp`

**Step 1: Write LogPanel.h**

```cpp
#pragma once
#include <QWidget>
#include <QPlainTextEdit>
#include <QPushButton>

class LogPanel : public QWidget
{
    Q_OBJECT
public:
    explicit LogPanel(QWidget *parent = nullptr);

public slots:
    void appendEntry(const QString &timestamp, double value);
    void clearLog();

private:
    QPlainTextEdit *m_text;
    QPushButton    *m_clearBtn;
};
```

**Step 2: Write LogPanel.cpp**

```cpp
#include "LogPanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

LogPanel::LogPanel(QWidget *parent) : QWidget(parent)
{
    m_text = new QPlainTextEdit();
    m_text->setReadOnly(true);
    m_text->setMaximumBlockCount(1000);
    m_text->setFont(QFont("Consolas", 9));
    m_text->setStyleSheet("background:#FFFFFF; border:1px solid #E5E7EB;");

    m_clearBtn = new QPushButton("Clear Log");
    m_clearBtn->setFixedWidth(90);

    auto *topRow = new QHBoxLayout();
    topRow->addWidget(new QLabel("Log"));
    topRow->addStretch();
    topRow->addWidget(m_clearBtn);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 4, 0, 0);
    layout->addLayout(topRow);
    layout->addWidget(m_text);

    connect(m_clearBtn, &QPushButton::clicked, this, &LogPanel::clearLog);
}

void LogPanel::appendEntry(const QString &timestamp, double value)
{
    m_text->appendPlainText(QString("%1    %2").arg(timestamp).arg(value, 0, 'f', 3));
}

void LogPanel::clearLog()
{
    m_text->clear();
}
```

---

### Task 5: ControlPanel

**Files:**
- Create: `src/ControlPanel.h`
- Create: `src/ControlPanel.cpp`

**Step 1: Write ControlPanel.h**

```cpp
#pragma once
#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>

class ControlPanel : public QWidget
{
    Q_OBJECT
public:
    explicit ControlPanel(QWidget *parent = nullptr);

signals:
    void connectToggled(bool connect);

private slots:
    void onConnectClicked();

private:
    QComboBox   *m_portCombo;
    QComboBox   *m_baudCombo;
    QComboBox   *m_bitsCombo;
    QComboBox   *m_parityCombo;
    QComboBox   *m_stopCombo;
    QPushButton *m_connectBtn;
    QLabel      *m_statusDot;
    bool         m_connected = false;

    QWidget *makeSection(const QString &title, QWidget *content);
};
```

**Step 2: Write ControlPanel.cpp**

```cpp
#include "ControlPanel.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QFrame>
#include <QLabel>

ControlPanel::ControlPanel(QWidget *parent) : QWidget(parent)
{
    setFixedWidth(200);
    setStyleSheet("background:#FFFFFF;");

    // Connection section
    m_portCombo   = new QComboBox(); m_portCombo->addItem("Virtual");
    m_baudCombo   = new QComboBox(); m_baudCombo->addItems({"9600","19200","38400","57600","115200"});
    m_bitsCombo   = new QComboBox(); m_bitsCombo->addItems({"8","7","6","5"});
    m_parityCombo = new QComboBox(); m_parityCombo->addItems({"None","Even","Odd"});
    m_stopCombo   = new QComboBox(); m_stopCombo->addItems({"1","2"});

    auto *form = new QFormLayout();
    form->setSpacing(6);
    form->addRow("Port",   m_portCombo);
    form->addRow("Baud",   m_baudCombo);
    form->addRow("Bits",   m_bitsCombo);
    form->addRow("Parity", m_parityCombo);
    form->addRow("Stop",   m_stopCombo);

    auto *connWidget = new QWidget();
    connWidget->setLayout(form);

    // Signal section (placeholder for Phase 2 sliders)
    auto *sigLabel = new QLabel("Sine Wave (10 Hz)");
    sigLabel->setStyleSheet("color:#6B7280; font-size:11px;");
    auto *sigWidget = new QWidget();
    auto *sigLayout = new QVBoxLayout(sigWidget);
    sigLayout->addWidget(sigLabel);

    // Status + connect button
    m_statusDot = new QLabel("● Disconnected");
    m_statusDot->setStyleSheet("color:#6B7280; font-size:11px;");

    m_connectBtn = new QPushButton("Connect");
    m_connectBtn->setStyleSheet(
        "QPushButton { background:#2563EB; color:white; border-radius:4px; padding:6px; }"
        "QPushButton:hover { background:#1D4ED8; }"
    );

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(12, 12, 12, 12);
    root->setSpacing(12);
    root->addWidget(makeSection("CONNECTION", connWidget));
    root->addWidget(makeSection("SIGNAL", sigWidget));
    root->addStretch();
    root->addWidget(m_statusDot);
    root->addWidget(m_connectBtn);

    connect(m_connectBtn, &QPushButton::clicked, this, &ControlPanel::onConnectClicked);
}

void ControlPanel::onConnectClicked()
{
    m_connected = !m_connected;
    if (m_connected) {
        m_connectBtn->setText("Disconnect");
        m_connectBtn->setStyleSheet(
            "QPushButton { background:#DC2626; color:white; border-radius:4px; padding:6px; }"
            "QPushButton:hover { background:#B91C1C; }"
        );
        m_statusDot->setText("● Connected");
        m_statusDot->setStyleSheet("color:#16A34A; font-size:11px;");
    } else {
        m_connectBtn->setText("Connect");
        m_connectBtn->setStyleSheet(
            "QPushButton { background:#2563EB; color:white; border-radius:4px; padding:6px; }"
            "QPushButton:hover { background:#1D4ED8; }"
        );
        m_statusDot->setText("● Disconnected");
        m_statusDot->setStyleSheet("color:#6B7280; font-size:11px;");
    }
    emit connectToggled(m_connected);
}

QWidget *ControlPanel::makeSection(const QString &title, QWidget *content)
{
    auto *container = new QWidget();
    auto *layout    = new QVBoxLayout(container);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(6);

    auto *header = new QLabel(title);
    header->setStyleSheet("color:#6B7280; font-size:10px; font-weight:bold; letter-spacing:1px;");
    layout->addWidget(header);

    auto *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("color:#E5E7EB;");
    layout->addWidget(line);

    layout->addWidget(content);
    return container;
}
```

---

### Task 6: MainWindow

**Files:**
- Create: `src/MainWindow.h`
- Create: `src/MainWindow.cpp`

**Step 1: Write MainWindow.h**

```cpp
#pragma once
#include <QMainWindow>
#include <QLabel>
#include "ControlPanel.h"
#include "ChartWidget.h"
#include "LogPanel.h"
#include "DataGenerator.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onConnectToggled(bool connect);
    void onDataReady(double value);

private:
    ControlPanel  *m_control;
    ChartWidget   *m_chart;
    LogPanel      *m_log;
    DataGenerator *m_gen;

    QLabel *m_statSamples;
    QLabel *m_statMin;
    QLabel *m_statMax;
    QLabel *m_statAvg;

    qint64 m_count = 0;
    double m_min   =  1e9;
    double m_max   = -1e9;
    double m_sum   = 0.0;
};
```

**Step 2: Write MainWindow.cpp**

```cpp
#include "MainWindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QStatusBar>
#include <QDateTime>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setStyleSheet("QMainWindow { background:#F5F6FA; }");

    m_control = new ControlPanel();
    m_chart   = new ChartWidget();
    m_log     = new LogPanel();
    m_gen     = new DataGenerator(this);

    // Right side: chart + log in vertical splitter
    auto *rightSplitter = new QSplitter(Qt::Vertical);
    rightSplitter->addWidget(m_chart);
    rightSplitter->addWidget(m_log);
    rightSplitter->setStretchFactor(0, 65);
    rightSplitter->setStretchFactor(1, 35);
    rightSplitter->setStyleSheet("QSplitter::handle { background:#E5E7EB; height:2px; }");

    // Main horizontal layout
    auto *central = new QWidget();
    auto *hLayout = new QHBoxLayout(central);
    hLayout->setContentsMargins(0, 0, 0, 0);
    hLayout->setSpacing(0);
    hLayout->addWidget(m_control);

    // Divider line
    auto *divider = new QFrame();
    divider->setFrameShape(QFrame::VLine);
    divider->setStyleSheet("color:#E5E7EB;");
    hLayout->addWidget(divider);
    hLayout->addWidget(rightSplitter, 1);

    setCentralWidget(central);

    // Status bar
    m_statSamples = new QLabel("Samples: 0");
    m_statMin     = new QLabel("Min: —");
    m_statMax     = new QLabel("Max: —");
    m_statAvg     = new QLabel("Avg: —");
    statusBar()->addWidget(m_statSamples);
    statusBar()->addWidget(new QLabel(" | "));
    statusBar()->addWidget(m_statMin);
    statusBar()->addWidget(new QLabel(" | "));
    statusBar()->addWidget(m_statMax);
    statusBar()->addWidget(new QLabel(" | "));
    statusBar()->addWidget(m_statAvg);
    statusBar()->setStyleSheet("QStatusBar { background:#FFFFFF; border-top:1px solid #E5E7EB; }");

    connect(m_control, &ControlPanel::connectToggled, this, &MainWindow::onConnectToggled);
    connect(m_gen,     &DataGenerator::dataReady,     this, &MainWindow::onDataReady);
}

void MainWindow::onConnectToggled(bool connect)
{
    if (connect) {
        m_count = 0; m_min = 1e9; m_max = -1e9; m_sum = 0.0;
        m_chart->clear();
        m_gen->start();
    } else {
        m_gen->stop();
    }
}

void MainWindow::onDataReady(double value)
{
    m_chart->addPoint(value);

    QString ts = QDateTime::currentDateTime().toString("HH:mm:ss.zzz");
    m_log->appendEntry(ts, value);

    ++m_count;
    m_min  = std::min(m_min, value);
    m_max  = std::max(m_max, value);
    m_sum += value;

    m_statSamples->setText(QString("Samples: %1").arg(m_count));
    m_statMin->setText(QString("Min: %1").arg(m_min, 0, 'f', 3));
    m_statMax->setText(QString("Max: %1").arg(m_max, 0, 'f', 3));
    m_statAvg->setText(QString("Avg: %1").arg(m_sum / m_count, 0, 'f', 3));
}
```

---

### Task 7: Build and verify

**Step 1: Build**

```bash
cd build
cmake --build . --config Debug
```

Expected: compiles with 0 errors.

**Step 2: Run**

```bash
./Debug/QtSerialMonitor.exe
```

Expected:
- Window opens 1000×640, light gray background
- Left panel shows CONNECTION + SIGNAL sections, Connect button
- Right side shows empty chart + empty log
- Click Connect → button turns red "Disconnect", status turns green, sine wave appears in chart, log starts filling

**Step 3: Report result to user for Phase 2 go-ahead**
