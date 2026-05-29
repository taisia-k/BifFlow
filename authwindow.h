#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QResizeEvent>
#include "appstate.h"
#include "theme.h"

class AuthWindow : public QWidget {
    Q_OBJECT
public:
    AuthWindow(QWidget* parent=nullptr) : QWidget(parent) {
        setWindowTitle("BidFlow");

        setStyleSheet(QString("QWidget { background:%1; }").arg(Theme::BG0));

        card = new QWidget(this);
        card->setFixedSize(400, 500);
        card->setStyleSheet(QString(R"(
            QWidget {
                background: %1;
                border-radius: 14px;
                color: %2;
            }
            QLineEdit {
                background: %3;
                border: 1px solid %4;
                border-radius: 8px;
                padding: 12px 15px;
                font-size: 13px;
                color: %2;
                font-family: 'Segoe UI', sans-serif;
            }
            QLineEdit:focus { border-color: %5; }
            QPushButton#go {
                background: %5;
                color: #0d0d0b;
                border: none;
                border-radius: 8px;
                padding: 13px;
                font-size: 13px;
                font-weight: 700;
                font-family: 'Segoe UI', sans-serif;
                letter-spacing: 0.3px;
            }
            QPushButton#go:hover   { background: %6; }
            QPushButton#go:pressed { background: %7; }
            QLabel { background: transparent; }
        )").arg(Theme::BG2).arg(Theme::TEXT1).arg(Theme::BG3)
           .arg(Theme::BORDER).arg(Theme::ACCENT).arg(Theme::ACCENT2).arg(Theme::ACCENTD));

        auto* sh = new QGraphicsDropShadowEffect(card);
        sh->setBlurRadius(60);
        sh->setOffset(0, 12);
        sh->setColor(QColor(0, 0, 0, 220));
        card->setGraphicsEffect(sh);

        auto* root = new QVBoxLayout(card);
        root->setContentsMargins(40, 44, 40, 36);
        root->setSpacing(0);

        // Лого
        auto* logoRow = new QHBoxLayout();
        logoRow->setAlignment(Qt::AlignCenter);
        auto* diamond = new QLabel("♦");
        diamond->setStyleSheet(QString("font-size:22px; color:%1;").arg(Theme::ACCENT));
        auto* logoText = new QLabel("BidFlow");
        logoText->setStyleSheet(QString(
            "font-size:22px; font-weight:700; color:%1;"
            "letter-spacing:-0.5px; font-family:'Georgia',serif;"
        ).arg(Theme::TEXT1));
        logoRow->addWidget(diamond);
        logoRow->addSpacing(6);
        logoRow->addWidget(logoText);
        root->addLayout(logoRow);
        root->addSpacing(6);

        auto* sub = new QLabel("Аукционная платформа");
        sub->setAlignment(Qt::AlignCenter);
        sub->setStyleSheet(QString(
            "font-size:11px; color:%1; letter-spacing:1.5px;"
            "font-family:'Segoe UI',sans-serif;"
        ).arg(Theme::TEXT3));
        root->addWidget(sub);
        root->addSpacing(38);

        // Разделитель с золотой линией
        auto* divRow = new QHBoxLayout();
        auto* lineL = new QFrame(); lineL->setFrameShape(QFrame::HLine);
        lineL->setStyleSheet(QString("background:%1; border:none; max-height:1px;").arg(Theme::BORDER));
        auto* diamond2 = new QLabel("♦");
        diamond2->setAlignment(Qt::AlignCenter);
        diamond2->setStyleSheet(QString("font-size:8px; color:%1; padding:0 8px;").arg(Theme::BORDER2));
        auto* lineR = new QFrame(); lineR->setFrameShape(QFrame::HLine);
        lineR->setStyleSheet(QString("background:%1; border:none; max-height:1px;").arg(Theme::BORDER));
        divRow->addWidget(lineL, 1);
        divRow->addWidget(diamond2);
        divRow->addWidget(lineR, 1);
        root->addLayout(divRow);
        root->addSpacing(28);

        //  Табы
        QString tabCss = QString(R"(
            QPushButton {
                background: transparent; border: none; border-radius: 0;
                border-bottom: 1px solid transparent;
                padding: 8px 4px; color: %1;
                font-size: 12px; font-family: 'Segoe UI', sans-serif;
                letter-spacing: 0.5px;
            }
            QPushButton:checked { color: %2; border-bottom: 1px solid %2; }
            QPushButton:hover:!checked { color: %3; }
        )").arg(Theme::TEXT3).arg(Theme::ACCENT).arg(Theme::TEXT2);

        btnLogin = new QPushButton("ВХОД");
        btnReg   = new QPushButton("РЕГИСТРАЦИЯ");
        btnLogin->setStyleSheet(tabCss); btnReg->setStyleSheet(tabCss);
        btnLogin->setCheckable(true);   btnReg->setCheckable(true);
        btnLogin->setChecked(true);

        auto* tabRow = new QHBoxLayout();
        tabRow->setSpacing(20);
        tabRow->addWidget(btnLogin);
        tabRow->addWidget(btnReg);
        tabRow->addStretch();
        root->addLayout(tabRow);
        root->addSpacing(20);

        stack = new QStackedWidget();
        stack->setStyleSheet("background: transparent;");
        stack->addWidget(buildLoginForm());
        stack->addWidget(buildRegForm());
        root->addWidget(stack);

        connect(btnLogin, &QPushButton::clicked, this, [=]{
            stack->setCurrentIndex(0); btnReg->setChecked(false); btnLogin->setChecked(true);
        });
        connect(btnReg, &QPushButton::clicked, this, [=]{
            stack->setCurrentIndex(1); btnLogin->setChecked(false); btnReg->setChecked(true);
        });
    }

    void resizeEvent(QResizeEvent*) override { centerCard(); }
    void showEvent(QShowEvent* e) override { QWidget::showEvent(e); centerCard(); }

signals:
    void loginSuccess();

private:
    QWidget* card;
    QPushButton *btnLogin, *btnReg;
    QStackedWidget* stack;
    QLineEdit *leLogin, *lePass, *leRLogin, *leRPass, *leRPass2;
    QLabel *errL, *errR;

    void centerCard() {
        card->move((width()-card->width())/2, (height()-card->height())/2);
    }

    QWidget* buildLoginForm() {
        auto* w = new QWidget(); w->setStyleSheet("background:transparent;");
        auto* v = new QVBoxLayout(w); v->setContentsMargins(0,0,0,0); v->setSpacing(10);

        leLogin = new QLineEdit(); leLogin->setPlaceholderText("Логин");
        lePass  = new QLineEdit(); lePass->setPlaceholderText("Пароль");
        lePass->setEchoMode(QLineEdit::Password);
        connect(leLogin, &QLineEdit::returnPressed, this, [=]{ lePass->setFocus(); });
        connect(lePass,  &QLineEdit::returnPressed, this, &AuthWindow::doLogin);

        errL = new QLabel(); errL->setFixedHeight(16);
        errL->setStyleSheet(QString("font-size:11px; color:%1; font-family:'Segoe UI',sans-serif;").arg(Theme::RED));

        auto* go = new QPushButton("Войти"); go->setObjectName("go");
        connect(go, &QPushButton::clicked, this, &AuthWindow::doLogin);

        // Тихая подсказка
        auto* hint = new QLabel("taisia  ·  egor  ·  elena   —   пароль: 1234");
        hint->setAlignment(Qt::AlignCenter);
        hint->setStyleSheet(QString(
            "font-size:10px; color:%1; letter-spacing:0.5px;"
            "font-family:'Segoe UI',sans-serif;"
        ).arg(Theme::TEXT3));

        v->addWidget(leLogin); v->addWidget(lePass); v->addWidget(errL);
        v->addSpacing(4); v->addWidget(go);
        v->addSpacing(18); v->addWidget(hint);
        v->addStretch();
        return w;
    }

    QWidget* buildRegForm() {
        auto* w = new QWidget(); w->setStyleSheet("background:transparent;");
        auto* v = new QVBoxLayout(w); v->setContentsMargins(0,0,0,0); v->setSpacing(10);

        leRLogin = new QLineEdit(); leRLogin->setPlaceholderText("Логин");
        leRPass  = new QLineEdit(); leRPass->setPlaceholderText("Пароль");
        leRPass->setEchoMode(QLineEdit::Password);
        leRPass2 = new QLineEdit(); leRPass2->setPlaceholderText("Повторите пароль");
        leRPass2->setEchoMode(QLineEdit::Password);
        connect(leRPass2, &QLineEdit::returnPressed, this, &AuthWindow::doReg);

        errR = new QLabel(); errR->setFixedHeight(16);
        errR->setStyleSheet(QString("font-size:11px; color:%1; font-family:'Segoe UI',sans-serif;").arg(Theme::RED));

        auto* go = new QPushButton("Зарегистрироваться"); go->setObjectName("go");
        connect(go, &QPushButton::clicked, this, &AuthWindow::doReg);

        v->addWidget(leRLogin); v->addWidget(leRPass); v->addWidget(leRPass2);
        v->addWidget(errR); v->addSpacing(4); v->addWidget(go);
        v->addStretch();
        return w;
    }

    void doLogin() {
        errL->clear();
        if (AppState::i().login(leLogin->text().trimmed(), lePass->text())) {
            lePass->clear(); emit loginSuccess();
        } else errL->setText("Неверный логин или пароль");
    }

    void doReg() {
        errR->clear();
        if (leRLogin->text().trimmed().isEmpty()) { errR->setText("Введите логин"); return; }
        if (leRPass->text().isEmpty())             { errR->setText("Введите пароль"); return; }
        if (leRPass->text() != leRPass2->text())  { errR->setText("Пароли не совпадают"); return; }
        auto& s = AppState::i();
        if (!s.registerUser(leRLogin->text().trimmed(), leRPass->text())) {
            errR->setText("Логин занят"); return;
        }
        s.login(leRLogin->text().trimmed(), leRPass->text());
        emit loginSuccess();
    }
};
