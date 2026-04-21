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
