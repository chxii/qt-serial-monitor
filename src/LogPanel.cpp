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
