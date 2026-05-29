#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>
#include "theme.h"
#include "models.h"

//  Разделитель
inline QFrame* makeSep() {
    auto* f = new QFrame();
    f->setFrameShape(QFrame::HLine);
    f->setStyleSheet(QString("background:%1; border:none; max-height:1px;").arg(Theme::BORDER));
    return f;
}

//  Тег категории
class TagLabel : public QLabel {
public:
    explicit TagLabel(const QString& text, QWidget* parent=nullptr) : QLabel(text, parent) {
        setStyleSheet(QString(R"(
            QLabel {
                background: %1;
                color: %2;
                border: 1px solid %3;
                border-radius: 4px;
                padding: 2px 8px;
                font-size: 9px;
                font-weight: 700;
                font-family: 'Segoe UI', sans-serif;
                letter-spacing: 1.5px;
            }
        )").arg(Theme::ACCENTBG).arg(Theme::ACCENT).arg(Theme::BORDER2));
    }
};

//  Карточка лота
class LotCard : public QFrame {
    Q_OBJECT
public:
    LotCard(const Lot& l, bool fav, QWidget* parent=nullptr)
        : QFrame(parent), m_lotId(l.id)
    {
        setObjectName("lotcard");
        setCursor(Qt::PointingHandCursor);
        setFixedHeight(108);
        setStyleSheet(QString(R"(
            QFrame#lotcard {
                background: %1;
                border: 1px solid %2;
                border-radius: 10px;
            }
            QFrame#lotcard:hover {
                background: %3;
                border-color: %4;
            }
        )").arg(Theme::BG2).arg(Theme::BORDER)
           .arg(Theme::BG3).arg(Theme::BORDER2));

        auto* h = new QHBoxLayout(this);
        h->setContentsMargins(20, 13, 18, 13);
        h->setSpacing(14);

        //  Левая колонка
        auto* left = new QVBoxLayout();
        left->setSpacing(5);

        auto* topRow = new QHBoxLayout();
        topRow->setSpacing(8);
        topRow->setContentsMargins(0,0,0,0);
        m_tag = new TagLabel(l.category);
        topRow->addWidget(m_tag);
        topRow->addStretch();
        left->addLayout(topRow);

        m_title = new QLabel(l.title);
        m_title->setStyleSheet(QString(
            "font-size:14px; font-weight:700; color:%1;"
            "font-family:'Georgia','Palatino Linotype',serif;"
        ).arg(Theme::TEXT1));
        m_title->setMaximumWidth(500);
        left->addWidget(m_title);

        m_seller = new QLabel("от " + l.sellerLogin);
        m_seller->setStyleSheet(QString(
            "font-size:11px; color:%1; font-family:'Segoe UI',sans-serif;"
        ).arg(Theme::TEXT3));
        left->addWidget(m_seller);

        //  Правая колонка
        auto* right = new QVBoxLayout();
        right->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        right->setSpacing(4);
        right->setContentsMargins(0,0,0,0);

        // Цена + звезда в одну строку
        auto* priceRow = new QHBoxLayout();
        priceRow->setSpacing(10);
        m_favBtn = new QPushButton();
        m_favBtn->setFixedSize(24,24);
        m_favBtn->setCursor(Qt::PointingHandCursor);
        connect(m_favBtn, &QPushButton::clicked, this, [=]{ emit favToggled(m_lotId); });
        priceRow->addWidget(m_favBtn);

        m_price = new QLabel();
        m_price->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        m_price->setStyleSheet(QString(
            "font-size:17px; font-weight:700; color:%1;"
            "font-family:'Segoe UI',sans-serif;"
        ).arg(Theme::ACCENT));
        priceRow->addWidget(m_price);
        right->addLayout(priceRow);

        m_timer = new QLabel();
        m_timer->setAlignment(Qt::AlignRight);
        m_timer->setStyleSheet(QString("font-size:11px; color:%1; font-family:'Segoe UI',sans-serif;").arg(Theme::ACCENT));

        m_leader = new QLabel();
        m_leader->setAlignment(Qt::AlignRight);
        m_leader->setStyleSheet(QString("font-size:11px; color:%1; font-family:'Segoe UI',sans-serif;").arg(Theme::TEXT3));

        right->addWidget(m_timer);
        right->addWidget(m_leader);

        h->addLayout(left, 1);
        h->addLayout(right);

        updateData(l, fav);
    }

    void updateData(const Lot& l, bool fav) {
        m_tag->setText(l.category.toUpper());
        m_title->setText(l.title);
        m_price->setText(QString("₽ %1").arg(l.currentPrice, 0, 'f', 0));

        // Звёздочка
        m_favBtn->setText(fav ? "★" : "☆");
        m_favBtn->setStyleSheet(QString(R"(
            QPushButton {
                background:transparent; border:none;
                font-size:15px; color:%1;
                border-radius:12px; padding:0;
            }
            QPushButton:hover { color:%2; }
        )").arg(fav ? Theme::ACCENT : Theme::TEXT3).arg(Theme::ACCENT));

        if (l.isActive()) {
            int s = l.secondsLeft();
            int h2=s/3600, m=(s%3600)/60, sec=s%60;
            QString tstr = h2>0
                ? QString("%1ч %2м").arg(h2).arg(m,2,10,QChar('0'))
                : QString("%1:%2").arg(m,2,10,QChar('0')).arg(sec,2,10,QChar('0'));
            m_timer->setText("◷  " + tstr);
            m_timer->setStyleSheet(s < 300
                ? QString("font-size:11px; color:%1; font-weight:700; font-family:'Segoe UI',sans-serif;").arg(Theme::RED)
                : QString("font-size:11px; color:%1; font-family:'Segoe UI',sans-serif;").arg(Theme::ACCENT));
            m_leader->setText(l.leaderId!=-1 ? "Лидер: " + l.leaderLogin : "Ставок нет");
            m_leader->setStyleSheet(QString("font-size:11px; color:%1; font-family:'Segoe UI',sans-serif;").arg(Theme::TEXT3));
        } else {
            m_timer->setText("Завершён");
            m_timer->setStyleSheet(QString("font-size:11px; color:%1; font-family:'Segoe UI',sans-serif;").arg(Theme::TEXT3));
            if (l.leaderId!=-1) {
                m_leader->setText("Победил: " + l.leaderLogin);
                m_leader->setStyleSheet(QString("font-size:11px; color:%1; font-weight:600; font-family:'Segoe UI',sans-serif;").arg(Theme::GREEN));
            } else {
                m_leader->setText("Не продан");
                m_leader->setStyleSheet(QString("font-size:11px; color:%1; font-family:'Segoe UI',sans-serif;").arg(Theme::TEXT3));
            }
        }
    }

    int lotId() const { return m_lotId; }

signals:
    void clicked(int lotId);
    void favToggled(int lotId);

protected:
    void mousePressEvent(QMouseEvent* e) override {
        if (e->button() == Qt::LeftButton) emit clicked(m_lotId);
    }

private:
    int m_lotId;
    QPushButton* m_favBtn;
    QLabel *m_tag, *m_title, *m_seller, *m_price, *m_timer, *m_leader;
};

// Плитка статистики (профиль)
class StatTile : public QFrame {
public:
    StatTile(const QString& label, QWidget* parent=nullptr) : QFrame(parent) {
        setStyleSheet(QString(R"(
            QFrame {
                background: %1;
                border: 1px solid %2;
                border-radius: 10px;
            }
        )").arg(Theme::BG2).arg(Theme::BORDER));
        auto* v = new QVBoxLayout(this);
        v->setContentsMargins(16,14,16,14);
        v->setSpacing(4);
        m_val = new QLabel("—");
        m_val->setStyleSheet(QString(
            "font-size:19px; font-weight:700; color:%1;"
            "font-family:'Segoe UI',sans-serif;"
        ).arg(Theme::TEXT1));
        m_lbl = new QLabel(label);
        m_lbl->setStyleSheet(QString(
            "font-size:10px; color:%1; letter-spacing:0.5px;"
            "font-family:'Segoe UI',sans-serif;"
        ).arg(Theme::TEXT3));
        v->addWidget(m_val);
        v->addWidget(m_lbl);
    }
    void setValue(const QString& v, const QString& color=Theme::TEXT1) {
        m_val->setText(v);
        m_val->setStyleSheet(QString(
            "font-size:19px; font-weight:700; color:%1;"
            "font-family:'Segoe UI',sans-serif;"
        ).arg(color));
    }
private:
    QLabel *m_val, *m_lbl;
};
