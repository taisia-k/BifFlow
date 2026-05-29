#pragma once
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QTabWidget>
#include <QTimer>
#include <QFrame>
#include "appstate.h"
#include "theme.h"
#include "chatwidget.h"

class LotDialog : public QDialog {
    Q_OBJECT
public:
    LotDialog(int lotId, QWidget* parent=nullptr) : QDialog(parent), m_lotId(lotId) {
        setModal(true);
        setFixedSize(640, 660);
        setWindowTitle("Лот");
        setStyleSheet(Theme::appStyle() + Theme::inputStyle() + QString(R"(
            QDialog { border-radius:0; }
            QTabWidget::pane { border:none; }
            QTabBar::tab {
                background:transparent; color:%1;
                padding:10px 20px; font-size:11px;
                font-family:'Segoe UI',sans-serif;
                letter-spacing:0.5px;
                border-bottom:1px solid transparent;
            }
            QTabBar::tab:selected { color:%2; border-bottom:1px solid %2; }
            QTabBar::tab:hover:!selected { color:%3; }
            QListWidget {
                background:%4; border:1px solid %5;
                border-radius:8px; padding:4px;
            }
            QListWidget::item {
                padding:9px 12px; border-bottom:1px solid %6;
                color:%1; font-size:12px; border-radius:5px;
                font-family:'Segoe UI',sans-serif;
            }
            QListWidget::item:hover { background:%6; }
        )").arg(Theme::TEXT2).arg(Theme::ACCENT).arg(Theme::TEXT2)
           .arg(Theme::BG2).arg(Theme::BORDER).arg(Theme::BG3));

        auto* root = new QVBoxLayout(this);
        root->setContentsMargins(0,0,0,0);
        root->setSpacing(0);

        //  Заголовок
        auto* hdr = new QWidget();
        hdr->setStyleSheet(QString("background:%1; border-bottom:1px solid %2;")
                           .arg(Theme::BG2).arg(Theme::BORDER));
        auto* hh = new QVBoxLayout(hdr);
        hh->setContentsMargins(28,22,28,18);
        hh->setSpacing(8);

        auto* topRow = new QHBoxLayout();
        m_catTag = new QLabel();
        m_catTag->setStyleSheet(QString(R"(
            QLabel { background:%1; color:%2; border-radius:5px;
                     padding:3px 10px; font-size:10px; font-weight:700; letter-spacing:1px; }
        )").arg(Theme::BG3).arg(Theme::ACCENT2));

        m_favBtn = new QPushButton();
        m_favBtn->setFixedSize(32,32);
        m_favBtn->setCursor(Qt::PointingHandCursor);
        updateFavBtn();
        connect(m_favBtn, &QPushButton::clicked, this, [=]{
            AppState::i().toggleFav(m_lotId); updateFavBtn();
        });

        topRow->addWidget(m_catTag);
        topRow->addStretch();
        topRow->addWidget(m_favBtn);
        hh->addLayout(topRow);

        m_title = new QLabel();
        m_title->setWordWrap(true);
        m_title->setStyleSheet(QString("font-size:18px; font-weight:700; color:%1;").arg(Theme::TEXT1));
        hh->addWidget(m_title);

        m_seller = new QLabel();
        m_seller->setStyleSheet(QString("font-size:12px; color:%1;").arg(Theme::TEXT3));
        hh->addWidget(m_seller);

        root->addWidget(hdr);

        // Основное тело
        auto* body = new QWidget();
        auto* bv = new QVBoxLayout(body);
        bv->setContentsMargins(28,20,28,20);
        bv->setSpacing(16);

        // Цена + таймер
        auto* priceRow = new QHBoxLayout();

        auto* priceBox = new QWidget();
        priceBox->setStyleSheet(QString("background:%1; border-radius:8px; border:1px solid %2;")
                                .arg(Theme::BG3).arg(Theme::BORDER));
        auto* pv = new QVBoxLayout(priceBox);
        pv->setContentsMargins(16,12,16,12); pv->setSpacing(2);
        auto* pLbl = new QLabel("ТЕКУЩАЯ СТАВКА");
        pLbl->setStyleSheet(QString("font-size:9px; color:%1; letter-spacing:1.5px; font-family:'Segoe UI',sans-serif;").arg(Theme::TEXT3));
        m_price = new QLabel();
        m_price->setStyleSheet(QString("font-size:24px; font-weight:700; color:%1; font-family:'Segoe UI',sans-serif;").arg(Theme::ACCENT));
        pv->addWidget(pLbl); pv->addWidget(m_price);

        auto* timerBox = new QWidget();
        timerBox->setStyleSheet(QString("background:%1; border-radius:8px; border:1px solid %2;")
                                .arg(Theme::BG3).arg(Theme::BORDER));
        auto* tv = new QVBoxLayout(timerBox);
        tv->setContentsMargins(16,12,16,12); tv->setSpacing(2);
        auto* tLbl = new QLabel("ДО ОКОНЧАНИЯ");
        tLbl->setStyleSheet(QString("font-size:9px; color:%1; letter-spacing:1.5px; font-family:'Segoe UI',sans-serif;").arg(Theme::TEXT3));
        m_timer = new QLabel();
        m_timer->setStyleSheet(QString("font-size:24px; font-weight:700; color:%1; font-family:'Segoe UI',sans-serif;").arg(Theme::ACCENT));
        tv->addWidget(tLbl); tv->addWidget(m_timer);

        priceRow->addWidget(priceBox, 1);
        priceRow->addSpacing(10);
        priceRow->addWidget(timerBox, 1);
        bv->addLayout(priceRow);

        m_leaderLine = new QLabel();
        m_leaderLine->setStyleSheet(QString("font-size:13px; color:%1;").arg(Theme::TEXT2));
        bv->addWidget(m_leaderLine);

        bv->addWidget(makeSep2());

        // Форма ставки
        auto* bidRow = new QHBoxLayout();
        m_bidInput = new QLineEdit(); m_bidInput->setPlaceholderText("Ваша ставка, ₽");
        m_bidBtn   = new QPushButton("Сделать ставку");
        m_bidBtn->setStyleSheet(Theme::btnAccent());
        m_bidBtn->setFixedHeight(44);
        connect(m_bidBtn,  &QPushButton::clicked,      this, &LotDialog::doBid);
        connect(m_bidInput, &QLineEdit::returnPressed, this, &LotDialog::doBid);
        bidRow->addWidget(m_bidInput, 1);
        bidRow->addSpacing(8);
        bidRow->addWidget(m_bidBtn);
        bv->addLayout(bidRow);

        m_errLabel = new QLabel(); m_errLabel->setFixedHeight(16);
        m_errLabel->setStyleSheet(QString("font-size:12px; color:%1;").arg(Theme::RED));
        bv->addWidget(m_errLabel);

        m_balLabel = new QLabel();
        m_balLabel->setStyleSheet(QString("font-size:11px; color:%1;").arg(Theme::TEXT3));
        bv->addWidget(m_balLabel);

        bv->addWidget(makeSep2());

        // Табы: история  описание  чат
        auto* tabs = new QTabWidget();
        tabs->addTab(makeBidHistory(), "История ставок");
        tabs->addTab(makeDescTab(),    "Описание");

        // Чат (только если не свой лот)
        auto* u = AppState::i().me();
        auto* l = AppState::i().findLot(m_lotId);
        if (u && l && u->id != l->sellerId) {
            m_chatWidget = new ChatWidget(m_lotId, l->sellerId);
            tabs->addTab(m_chatWidget, "💬 Чат с продавцом");
        } else if (u && l && u->id == l->sellerId) {
            // Продавец видит все чаты
            m_chatWidget = nullptr;
            tabs->addTab(makeSellerChats(l), "💬 Чаты покупателей");
        }

        bv->addWidget(tabs, 1);
        root->addWidget(body, 1);

        // Таймер обновления
        m_ticker = new QTimer(this);
        connect(m_ticker, &QTimer::timeout, this, &LotDialog::refresh);
        m_ticker->start(1000);
        connect(&AppState::i(), &AppState::dataChanged, this, &LotDialog::refresh);
        refresh();
    }

private:
    int m_lotId;
    QLabel *m_catTag, *m_title, *m_seller, *m_price, *m_timer,
           *m_leaderLine, *m_errLabel, *m_balLabel;
    QPushButton *m_favBtn, *m_bidBtn;
    QLineEdit   *m_bidInput;
    QListWidget *m_bidList = nullptr;
    QLabel      *m_descLabel = nullptr;
    QTimer      *m_ticker;
    ChatWidget  *m_chatWidget = nullptr;

    void refresh() {
        auto* l = AppState::i().findLot(m_lotId);
        auto* u = AppState::i().me();
        if (!l||!u) return;

        m_catTag->setText(l->category.toUpper());
        m_title->setText(l->title);
        m_seller->setText("Продавец: " + l->sellerLogin + "  ·  Старт: ₽" + QString::number((int)l->startPrice) + "  ·  Шаг: ₽" + QString::number((int)l->minStep));
        m_price->setText(QString("₽ %1").arg(l->currentPrice,0,'f',0));
        m_balLabel->setText(QString("Ваш баланс: ₽%1  ·  Доступно: ₽%2  ·  Заморожено: ₽%3")
                            .arg(u->balance,0,'f',0).arg(u->freeBal(),0,'f',0).arg(u->hold,0,'f',0));

        bool active = l->isActive();
        if (active) {
            int s = l->secondsLeft();
            int h2=s/3600, m=(s%3600)/60, sec=s%60;
            m_timer->setText(h2>0
                ? QString("%1:%2:%3").arg(h2,2,10,QChar('0')).arg(m,2,10,QChar('0')).arg(sec,2,10,QChar('0'))
                : QString("00:%1:%2").arg(m,2,10,QChar('0')).arg(sec,2,10,QChar('0')));
            m_timer->setStyleSheet(s<300
                ? QString("font-size:24px; font-weight:700; color:%1; font-family:'Segoe UI',sans-serif;").arg(Theme::RED)
                : QString("font-size:24px; font-weight:700; color:%1; font-family:'Segoe UI',sans-serif;").arg(Theme::ACCENT));

            bool isSeller = (l->sellerId==u->id);
            bool canBid   = !isSeller;
            m_bidBtn->setEnabled(canBid);
            m_bidInput->setEnabled(canBid);
            if (l->leaderId==u->id)
                m_leaderLine->setText("✅  Вы лидируете!");
            else if (l->leaderId!=-1)
                m_leaderLine->setText("Лидер: " + l->leaderLogin);
            else
                m_leaderLine->setText("Ставок пока нет — будьте первым");
            if (isSeller) m_errLabel->setText("Это ваш лот — ставки недоступны");
        } else {
            m_timer->setText("Завершён");
            m_timer->setStyleSheet(QString("font-size:24px; font-weight:700; color:%1; font-family:'Segoe UI',sans-serif;").arg(Theme::TEXT3));
            m_bidBtn->setEnabled(false); m_bidInput->setEnabled(false);
            if (l->leaderId==u->id)
                m_leaderLine->setText("🏆  Вы победили! Товар ваш.");
            else if (l->leaderId!=-1)
                m_leaderLine->setText("Победитель: " + l->leaderLogin);
            else
                m_leaderLine->setText("Аукцион завершён без ставок");
        }

        // История
        if (m_bidList) {
            m_bidList->clear();
            for (auto& b : l->bids) {
                QString s = QString("  %1  ·  ₽%2  ·  %3")
                            .arg(b.userLogin,-12)
                            .arg(b.amount,10,'f',0)
                            .arg(b.time.toString("hh:mm:ss"));
                auto* item = new QListWidgetItem(s);
                if (!m_bidList->count()) // первый = лидер
                    item->setForeground(QColor(Theme::GREEN));
                m_bidList->addItem(item);
            }
            if (l->bids.isEmpty())
                m_bidList->addItem("  Ставок пока нет");
        }
        if (m_descLabel) m_descLabel->setText(l->description);
    }

    void doBid() {
        m_errLabel->clear();
        bool ok; double amt = m_bidInput->text().toDouble(&ok);
        if (!ok||amt<=0) { m_errLabel->setText("Введите корректную сумму"); return; }
        QString err = AppState::i().placeBid(m_lotId, amt);
        if (!err.isEmpty()) m_errLabel->setText(err);
        else m_bidInput->clear();
    }

    void updateFavBtn() {
        bool fav = AppState::i().isFav(m_lotId);
        m_favBtn->setText(fav ? "★" : "☆");
        m_favBtn->setStyleSheet(QString(R"(
            QPushButton { background:transparent; border:1px solid %1;
                          border-radius:16px; font-size:16px; color:%2; }
            QPushButton:hover { background:%3; }
        )").arg(Theme::BORDER).arg(fav?Theme::ACCENT:Theme::TEXT3).arg(Theme::BG3));
    }

    QWidget* makeBidHistory() {
        auto* w = new QWidget(); w->setStyleSheet("background:transparent;");
        auto* v = new QVBoxLayout(w); v->setContentsMargins(0,8,0,0);
        m_bidList = new QListWidget();
        m_bidList->setFont(QFont("Consolas,monospace",12));
        v->addWidget(m_bidList);
        return w;
    }

    QWidget* makeDescTab() {
        auto* w = new QWidget(); w->setStyleSheet("background:transparent;");
        auto* v = new QVBoxLayout(w); v->setContentsMargins(0,12,0,0);
        m_descLabel = new QLabel();
        m_descLabel->setWordWrap(true);
        m_descLabel->setAlignment(Qt::AlignTop);
        m_descLabel->setStyleSheet(QString("font-size:13px; color:%1; line-height:1.6;").arg(Theme::TEXT2));
        v->addWidget(m_descLabel);
        v->addStretch();
        return w;
    }

    QWidget* makeSellerChats(Lot* l) {
        // Продавец видит всех кто писал
        auto* w = new QWidget(); w->setStyleSheet("background:transparent;");
        auto* v = new QVBoxLayout(w); v->setContentsMargins(0,8,0,0);
        bool any=false;
        for (auto& c : AppState::i().chats) {
            if (c.lotId==m_lotId) {
                int buyerId = c.userA;
                auto* cw = new ChatWidget(m_lotId, buyerId);
                v->addWidget(cw);
                any=true;
                break; // один чат на лот
            }
        }
        if (!any) {
            auto* lbl = new QLabel("Пока никто не написал");
            lbl->setStyleSheet(QString("color:%1; font-size:13px;").arg(Theme::TEXT3));
            lbl->setAlignment(Qt::AlignCenter);
            v->addWidget(lbl);
            v->addStretch();
        }
        return w;
    }

    QFrame* makeSep2() {
        auto* f = new QFrame(); f->setFrameShape(QFrame::HLine);
        f->setStyleSheet(QString("background:%1; border:none; max-height:1px;").arg(Theme::BORDER));
        return f;
    }
};
