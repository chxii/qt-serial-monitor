#include "SidebarNav.h"
#include <QPainter>
#include <QPainterPath>
#include <QHBoxLayout>
#include <QSpacerItem>

SidebarNav::SidebarNav(QWidget *parent) : QWidget(parent)
{
    setFixedWidth(200);
    setAttribute(Qt::WA_StyledBackground, true);
    setObjectName("SidebarNav");

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(12, 16, 12, 16);
    root->setSpacing(4);

    // App title
    auto *title = new QLabel("Serial Monitor");
    title->setStyleSheet("font-size:15px; font-weight:700; color:#101828; padding:0 4px 8px 4px;");
    root->addWidget(title);

    auto *divider = new QFrame();
    divider->setFrameShape(QFrame::HLine);
    divider->setStyleSheet("background:#EAECF0; max-height:1px; border:none;");
    root->addWidget(divider);
    root->addSpacing(8);

    // Nav items area
    m_navLayout = new QVBoxLayout();
    m_navLayout->setSpacing(2);
    m_navLayout->setContentsMargins(0,0,0,0);
    root->addLayout(m_navLayout);

    root->addStretch();

    // User info area
    auto *userRow = new QHBoxLayout();
    userRow->setSpacing(8);

    m_userAvatar = new QLabel("S");
    m_userAvatar->setFixedSize(32, 32);
    m_userAvatar->setAlignment(Qt::AlignCenter);
    m_userAvatar->setStyleSheet(
        "background:#2563EB; color:white; border-radius:16px;"
        "font-size:13px; font-weight:600;"
    );

    auto *nameCol = new QVBoxLayout();
    nameCol->setSpacing(0);
    m_userName = new QLabel("User");
    m_userName->setStyleSheet("font-size:13px; font-weight:600; color:#101828;");
    m_userRole = new QLabel("Engineer");
    m_userRole->setStyleSheet("font-size:11px; color:#667085;");
    nameCol->addWidget(m_userName);
    nameCol->addWidget(m_userRole);

    userRow->addWidget(m_userAvatar);
    userRow->addLayout(nameCol);
    userRow->addStretch();
    root->addLayout(userRow);
}

void SidebarNav::addNavItem(const QString &text, int pageIndex)
{
    NavItem item;
    item.text      = text;
    item.pageIndex = pageIndex;
    item.btn       = new QPushButton(text);
    item.btn->setCheckable(false);
    item.btn->setFlat(true);
    item.btn->setCursor(Qt::PointingHandCursor);
    item.btn->setStyleSheet(
        "QPushButton { text-align:left; padding:8px 12px; border-radius:6px;"
        "color:#667085; font-size:13px; background:transparent; border:none; }"
        "QPushButton:hover { background:#F4F5F7; color:#101828; }"
    );

    int idx = m_items.size();
    connect(item.btn, &QPushButton::clicked, this, [this, idx]() {
        selectItem(m_items[idx]);
        emit pageChanged(m_items[idx].pageIndex);
    });

    m_navLayout->addWidget(item.btn);
    m_items.append(item);

    if (m_items.size() == 1)
        selectItem(m_items[0]);
}

void SidebarNav::setUserInfo(const QString &name, const QString &role)
{
    m_userName->setText(name);
    m_userRole->setText(role);
    if (!name.isEmpty())
        m_userAvatar->setText(name.at(0).toUpper());
}

void SidebarNav::setCurrentIndex(int pageIndex)
{
    for (auto &item : m_items) {
        if (item.pageIndex == pageIndex) {
            selectItem(item);
            return;
        }
    }
}

void SidebarNav::selectItem(NavItem &target)
{
    for (auto &item : m_items) {
        if (item.pageIndex == target.pageIndex) {
            item.btn->setStyleSheet(
                "QPushButton { text-align:left; padding:8px 12px; border-radius:6px;"
                "color:#2563EB; font-size:13px; background:#EFF6FF; border:none; font-weight:600; }"
            );
        } else {
            item.btn->setStyleSheet(
                "QPushButton { text-align:left; padding:8px 12px; border-radius:6px;"
                "color:#667085; font-size:13px; background:transparent; border:none; }"
                "QPushButton:hover { background:#F4F5F7; color:#101828; }"
            );
        }
    }
    m_currentPage = target.pageIndex;
}

void SidebarNav::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.fillRect(rect(), QColor("#FFFFFF"));
    // Right border
    p.setPen(QPen(QColor("#EAECF0"), 1));
    p.drawLine(width() - 1, 0, width() - 1, height());
}
