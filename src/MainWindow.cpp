#include "MainWindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QStatusBar>
#include <QDateTime>
#include <QWidget>
#include <QFrame>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QToolBar>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    m_control = new ControlPanel();
    m_chart   = new ChartWidget();
    m_log     = new LogPanel();
    m_gen     = new DataGenerator(this);

    m_gen->setFreq(m_control->freq());
    m_gen->setAmp(m_control->amp());
    m_gen->setMode(m_control->isSine());

    // ── Toolbar ──────────────────────────────────────────────
    auto *toolbar = addToolBar("Main");
    toolbar->setMovable(false);
    toolbar->setStyleSheet(
        "QToolBar { background:#FFFFFF; border-bottom:1px solid #EAECF0; padding:4px 12px; spacing:8px; }"
    );

    auto *titleLabel = new QLabel("Serial Monitor");
    titleLabel->setStyleSheet("font-size:15px; font-weight:700; color:#101828;");
    toolbar->addWidget(titleLabel);

    auto *spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    toolbar->addWidget(spacer);

    auto *exportBtn = new QPushButton("Export CSV");
    exportBtn->setProperty("class", "primary");
    exportBtn->setFixedHeight(32);
    exportBtn->setCursor(Qt::PointingHandCursor);
    toolbar->addWidget(exportBtn);

    // ── Right area ───────────────────────────────────────────
    auto *rightSplitter = new QSplitter(Qt::Vertical);
    rightSplitter->addWidget(m_chart);
    rightSplitter->addWidget(m_log);
    rightSplitter->setStretchFactor(0, 65);
    rightSplitter->setStretchFactor(1, 35);
    rightSplitter->setHandleWidth(1);
    rightSplitter->setStyleSheet("QSplitter::handle { background:#EAECF0; }");

    auto *rightWrap = new QWidget();
    rightWrap->setStyleSheet("background:#F4F5F7;");
    auto *rightLayout = new QVBoxLayout(rightWrap);
    rightLayout->setContentsMargins(16, 16, 16, 16);
    rightLayout->setSpacing(0);
    rightLayout->addWidget(rightSplitter);

    // ── Central ──────────────────────────────────────────────
    auto *central = new QWidget();
    auto *hLayout = new QHBoxLayout(central);
    hLayout->setContentsMargins(0, 0, 0, 0);
    hLayout->setSpacing(0);
    hLayout->addWidget(m_control);
    hLayout->addWidget(rightWrap, 1);
    setCentralWidget(central);

    // ── Status bar ───────────────────────────────────────────
    m_statSamples = new QLabel("Samples: 0");
    m_statRate    = new QLabel("Rate: — Hz");
    m_statMin     = new QLabel("Min: —");
    m_statMax     = new QLabel("Max: —");
    m_statAvg     = new QLabel("Avg: —");
    for (auto *l : {m_statSamples, m_statRate, m_statMin, m_statMax, m_statAvg})
        l->setStyleSheet("color:#667085; font-size:12px; padding:0 8px;");

    statusBar()->addWidget(m_statSamples);
    statusBar()->addWidget(m_statRate);
    statusBar()->addWidget(m_statMin);
    statusBar()->addWidget(m_statMax);
    statusBar()->addWidget(m_statAvg);

    // ── Connections ──────────────────────────────────────────
    connect(m_control,  &ControlPanel::connectToggled, this, &MainWindow::onConnectToggled);
    connect(m_control,  &ControlPanel::paramsChanged,  this, &MainWindow::onParamsChanged);
    connect(m_gen,      &DataGenerator::dataReady,     this, &MainWindow::onDataReady);
    connect(exportBtn,  &QPushButton::clicked,         this, &MainWindow::exportCsv);
}

void MainWindow::onConnectToggled(bool connect)
{
    if (connect) {
        m_count = 0; m_min = 1e9; m_max = -1e9; m_sum = 0.0;
        m_rateCount = 0;
        m_samples.clear();
        m_rateTimer.start();
        m_chart->clear();
        m_gen->start();
    } else {
        m_gen->stop();
    }
}

void MainWindow::onParamsChanged(double freq, double amp, bool sine)
{
    m_gen->setFreq(freq);
    m_gen->setAmp(amp);
    m_gen->setMode(sine);
}

void MainWindow::onDataReady(double value)
{
    QString ts = QDateTime::currentDateTime().toString("HH:mm:ss.zzz");

    m_chart->addPoint(value);
    m_log->appendEntry(ts, value);
    m_samples.append({ts, value});

    ++m_count;
    ++m_rateCount;
    m_min  = std::min(m_min, value);
    m_max  = std::max(m_max, value);
    m_sum += value;

    // Update rate every second
    qint64 elapsed = m_rateTimer.elapsed();
    if (elapsed >= 1000) {
        double rate = m_rateCount * 1000.0 / elapsed;
        m_statRate->setText(QString("Rate: %1 Hz").arg(rate, 0, 'f', 1));
        m_rateCount = 0;
        m_rateTimer.restart();
    }

    m_statSamples->setText(QString("Samples: %1").arg(m_count));
    m_statMin->setText(QString("Min: %1").arg(m_min, 0, 'f', 3));
    m_statMax->setText(QString("Max: %1").arg(m_max, 0, 'f', 3));
    m_statAvg->setText(QString("Avg: %1").arg(m_sum / m_count, 0, 'f', 3));
}

void MainWindow::exportCsv()
{
    if (m_samples.isEmpty()) {
        QMessageBox::information(this, "Export CSV", "No data to export. Connect and record some data first.");
        return;
    }

    QString path = QFileDialog::getSaveFileName(
        this, "Export CSV",
        QDateTime::currentDateTime().toString("'serial_data_'yyyyMMdd_HHmmss'.csv'"),
        "CSV Files (*.csv)"
    );
    if (path.isEmpty()) return;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Export CSV", "Could not write file: " + path);
        return;
    }

    QTextStream out(&file);
    out << "Timestamp,Value\n";
    for (const auto &s : m_samples)
        out << s.timestamp << "," << QString::number(s.value, 'f', 6) << "\n";

    QMessageBox::information(this, "Export CSV",
        QString("Exported %1 samples to:\n%2").arg(m_samples.size()).arg(path));
}
