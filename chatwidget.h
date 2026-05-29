#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QTimer>
#include "appstate.h"
#include "theme.h"

//  Один пузырь сообщения
class BubbleWidget : public QWidget {
public:
    BubbleWidget(const ChatMessage& m, int myId, QWidget* parent=nullptr)
        : QWidget(parent)
    {
        bool isMe = m.isMe(myId);
        auto* h = new QHBoxLayout(this);
        h->setContentsMargins(0,2,0,2);

        auto* bubble = new QLabel();
        bubble->setTextFormat(Qt::PlainText);
        bubble->setWordWrap(true);
        bubble->setMaximumWidth(320);
        bubble->setMinimumWidth(60);

        QString timeStr = m.time.toString("hh:mm");
        bubble->setText(m.text + "\n");

        // Время
        auto* timeLabel = new QLabel(m.fromLogin + "  " + timeStr);
        timeLabel->setStyleSheet(QString("font-size:10px; color:%1;").arg(Theme::TEXT3));

        auto* col = new QVBoxLayout();
        col->setSpacing(2);
        col->setContentsMargins(0,0,0,0);

        if (isMe) {
            bubble->setStyleSheet(QString(R"(
                QLabel {
                    background:%1; color:white;
                    border-radius:14px; border-bottom-right-radius:4px;
                    padding:10px 14px; font-size:13px;
                }
            )").arg(Theme::ACCENT));
            col->addWidget(bubble, 0, Qt::AlignRight);
            col->addWidget(timeLabel, 0, Qt::AlignRight);
            h->addStretch();
            h->addLayout(col);
        } else {
            bubble->setStyleSheet(QString(R"(
                QLabel {
                    background:%1; color:%2;
                    border-radius:14px; border-bottom-left-radius:4px;
                    padding:10px 14px; font-size:13px;
                    border:1px solid %3;
                }
            )").arg(Theme::BG3).arg(Theme::TEXT1).arg(Theme::BORDER));
            col->addWidget(bubble, 0, Qt::AlignLeft);
            col->addWidget(timeLabel, 0, Qt::AlignLeft);
            h->addLayout(col);
            h->addStretch();
        }
    }
};

// Виджет чата
class ChatWidget : public QWidget {
    Q_OBJECT
public:
    // lotId — лот, toUserId — с кем говорим (продавец или покупатель)
    ChatWidget(int lotId, int toUserId, QWidget* parent=nullptr)
        : QWidget(parent), m_lotId(lotId), m_toUserId(toUserId)
    {
        setStyleSheet(QString("QWidget { background:%1; }").arg(Theme::BG1));

        auto* root = new QVBoxLayout(this);
        root->setContentsMargins(0,0,0,0);
        root->setSpacing(0);

        // Header
        auto* hdr = new QWidget();
        hdr->setFixedHeight(52);
        hdr->setStyleSheet(QString("background:%1; border-bottom:1px solid %2;")
                           .arg(Theme::BG2).arg(Theme::BORDER));
        auto* hh = new QHBoxLayout(hdr);
        hh->setContentsMargins(16,0,16,0);

        auto* u = AppState::i().findUser(toUserId);
        auto* avatar = new QLabel(u ? u->login.left(1).toUpper() : "?");
        avatar->setFixedSize(32,32);
        avatar->setAlignment(Qt::AlignCenter);
        avatar->setStyleSheet(QString(R"(
            QLabel {
                background:%1; color:%2;
                border-radius:16px; font-size:13px; font-weight:700;
            }
        )").arg(Theme::ACCENT).arg("white"));

        auto* nameLabel = new QLabel(u ? u->login : "—");
        nameLabel->setStyleSheet(QString("font-size:13px; font-weight:600; color:%1;").arg(Theme::TEXT1));

        hh->addWidget(avatar);
        hh->addSpacing(10);
        hh->addWidget(nameLabel);
        hh->addStretch();
        root->addWidget(hdr);

        // Сообщения
        m_scroll = new QScrollArea();
        m_scroll->setWidgetResizable(true);
        m_scroll->setFrameShape(QFrame::NoFrame);
        m_scroll->setStyleSheet("QScrollArea { background:transparent; }");

        m_msgContainer = new QWidget();
        m_msgContainer->setStyleSheet("QWidget { background:transparent; }");
        m_msgLayout = new QVBoxLayout(m_msgContainer);
        m_msgLayout->setContentsMargins(12,12,12,12);
        m_msgLayout->setSpacing(4);
        m_msgLayout->addStretch();

        m_scroll->setWidget(m_msgContainer);
        root->addWidget(m_scroll, 1);

        // Ввод
        auto* inputRow = new QWidget();
        inputRow->setFixedHeight(60);
        inputRow->setStyleSheet(QString("background:%1; border-top:1px solid %2;")
                                .arg(Theme::BG2).arg(Theme::BORDER));
        auto* ir = new QHBoxLayout(inputRow);
        ir->setContentsMargins(12,10,12,10);
        ir->setSpacing(8);

        m_input = new QLineEdit();
        m_input->setPlaceholderText("Написать сообщение...");
        m_input->setStyleSheet(QString(R"(
            QLineEdit {
                background:%1; border:1px solid %2; border-radius:20px;
                padding:9px 16px; font-size:13px; color:%3;
            }
            QLineEdit:focus { border-color:%4; }
        )").arg(Theme::BG3).arg(Theme::BORDER).arg(Theme::TEXT1).arg(Theme::ACCENT));

        auto* sendBtn = new QPushButton("➤");
        sendBtn->setFixedSize(36,36);
        sendBtn->setStyleSheet(QString(R"(
            QPushButton {
                background:%1; color:white; border:none;
                border-radius:18px; font-size:14px;
            }
            QPushButton:hover { background:%2; }
        )").arg(Theme::ACCENT).arg(Theme::ACCENT2));

        connect(m_input,  &QLineEdit::returnPressed, this, &ChatWidget::sendMsg);
        connect(sendBtn,  &QPushButton::clicked,     this, &ChatWidget::sendMsg);

        ir->addWidget(m_input);
        ir->addWidget(sendBtn);
        root->addWidget(inputRow);

        connect(&AppState::i(), &AppState::dataChanged, this, &ChatWidget::refresh);
        refresh();
    }

private:
    int m_lotId, m_toUserId;
    QScrollArea*  m_scroll;
    QWidget*      m_msgContainer;
    QVBoxLayout*  m_msgLayout;
    QLineEdit*    m_input;

    void refresh() {
        auto* u = AppState::i().me();
        auto* l = AppState::i().findLot(m_lotId);
        if (!u || !l) return;

        int buyerId  = (u->id == l->sellerId) ? m_toUserId : u->id;
        int sellerId = l->sellerId;
        auto* chat = AppState::i().findChat(m_lotId, buyerId, sellerId);

        // Очистить
        QLayoutItem* item;
        while (m_msgLayout->count() > 1) {
            item = m_msgLayout->takeAt(0);
            if (item->widget()) delete item->widget();
            delete item;
        }

        if (chat) {
            // Вставляем с начала (prepend в списке, надо реверс)
            QList<ChatMessage> ordered = chat->messages;
            for (auto& m : ordered) {
                auto* bw = new BubbleWidget(m, u->id);
                m_msgLayout->insertWidget(m_msgLayout->count()-1, bw);
            }
        }

        // Скролл вниз
        QTimer::singleShot(50, this, [=]{
            m_scroll->verticalScrollBar()->setValue(
                m_scroll->verticalScrollBar()->maximum());
        });
    }

    void sendMsg() {
        QString txt = m_input->text().trimmed();
        if (txt.isEmpty()) return;
        AppState::i().sendMessage(m_lotId, m_toUserId, txt);
        m_input->clear();
    }
};
