#pragma once
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QTimer>
#include <QComboBox>
#include <QLineEdit>
#include <QStackedWidget>
#include <QButtonGroup>
#include <QFrame>
#include <QMouseEvent>
#include <functional>
#include "appstate.h"
#include "theme.h"
#include "widgets.h"
#include "lotdialog.h"
#include "createlotdialog.h"

//  Кнопка сайдера
class SideBtn : public QPushButton {
public:
    SideBtn(const QString& icon, const QString& label, QWidget* parent=nullptr)
        : QPushButton(parent)
    {
        setCheckable(true);
        setCursor(Qt::PointingHandCursor);
        setFixedHeight(44);
        setText(icon + "  " + label);
        setStyleSheet(QString(R"(
            QPushButton {
                background:transparent; border:none; border-radius:7px;
                padding:0 14px; text-align:left;
                color:%1; font-size:13px;
                font-family:'Segoe UI',sans-serif;
            }
            QPushButton:hover   { background:%2; color:%3; }
            QPushButton:checked { background:%4; color:%5; font-weight:600; }
        )").arg(Theme::TEXT2).arg(Theme::BG3).arg(Theme::TEXT1)
           .arg(Theme::ACCENTBG).arg(Theme::ACCENT));
    }
};

//  Главное окно ─
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget* parent=nullptr) : QMainWindow(parent) {
        setWindowTitle("BidFlow");
        setMinimumSize(1060, 680);
        setStyleSheet(Theme::appStyle());

        auto* central = new QWidget();
        setCentralWidget(central);
        auto* root = new QHBoxLayout(central);
        root->setContentsMargins(0,0,0,0);
        root->setSpacing(0);

        root->addWidget(buildSidebar());

        auto* content = new QWidget();
        content->setStyleSheet(QString("background:%1;").arg(Theme::BG1));
        auto* cv = new QVBoxLayout(content);
        cv->setContentsMargins(0,0,0,0);
        cv->setSpacing(0);
        cv->addWidget(buildTopBar());

        m_stack = new QStackedWidget();
        m_stack->setStyleSheet("background:transparent;");
        m_stack->addWidget(buildLotsPage());       // 0 — все лоты
        m_stack->addWidget(buildFavPage());        // 1 — избранное
        m_stack->addWidget(buildMyLotsPage());     // 2 — мои лоты
        m_stack->addWidget(buildMyBidsPage());     // 3 — мои ставки
        m_stack->addWidget(buildFinishedPage());   // 4 — завершённые
        m_stack->addWidget(buildProfilePage());    // 5 — профиль
        cv->addWidget(m_stack, 1);
        root->addWidget(content, 1);

        // Таймер обновления
        auto* t = new QTimer(this);
        connect(t,   &QTimer::timeout,          this, &MainWindow::refresh);
        connect(&AppState::i(), &AppState::dataChanged, this, &MainWindow::refresh);
        t->start(1000);
        refresh();
    }

signals:
    void loggedOut();

private:
    // Навигация
    QButtonGroup*   m_navGroup;
    QStackedWidget* m_stack;

    QLabel *m_lbUser, *m_lbBal;

    // Страница лотов
    QLineEdit*  m_search;
    QComboBox*  m_catFilter;
    QVBoxLayout *m_allLayout, *m_favLayout, *m_myLotLayout,
                *m_myBidLayout, *m_finLayout;

    // Профиль
    StatTile *m_stBal, *m_stFree, *m_stHold, *m_stLots, *m_stBids;

    // Sidebar
    QWidget* buildSidebar() {
        auto* sb = new QWidget();
        sb->setFixedWidth(210);
        sb->setStyleSheet(QString("QWidget { background:%1; border-right:1px solid %2; }")
                          .arg(Theme::BG2).arg(Theme::BORDER));
        auto* v = new QVBoxLayout(sb);
        v->setContentsMargins(12,20,12,20);
        v->setSpacing(4);

        // Лого
        auto* logo = new QLabel();
        logo->setText(QString("<span style='color:%1; font-size:18px;'>♦</span>"
                              "<span style='color:%2; font-size:17px; font-weight:700; letter-spacing:-0.3px;'> BidFlow</span>")
                      .arg(Theme::ACCENT).arg(Theme::TEXT1));
        logo->setTextFormat(Qt::RichText);
        logo->setStyleSheet("padding:0 6px; margin-bottom:12px; background:transparent;");
        v->addWidget(logo);
        v->addSpacing(8);

        m_navGroup = new QButtonGroup(this);
        m_navGroup->setExclusive(true);

        auto addBtn = [&](const QString& ic, const QString& label, int idx) {
            auto* btn = new SideBtn(ic, label);
            m_navGroup->addButton(btn, idx);
            v->addWidget(btn);
            if (idx==0) btn->setChecked(true);
            connect(btn, &QPushButton::clicked, this, [=]{ m_stack->setCurrentIndex(idx); });
        };
        addBtn("🏠", "Все лоты",     0);
        addBtn("★",  "Избранное",    1);
        addBtn("📦", "Мои лоты",     2);
        addBtn("💰", "Мои ставки",   3);
        addBtn("🏁", "Завершённые",  4);

        v->addStretch();
        v->addWidget(makeSep());
        v->addSpacing(8);
        addBtn("👤", "Профиль",      5);

        auto* logoutBtn = new QPushButton("Выйти");
        logoutBtn->setStyleSheet(Theme::btnGhost());
        logoutBtn->setFixedHeight(38);
        connect(logoutBtn, &QPushButton::clicked, this, [=]{
            AppState::i().logout(); emit loggedOut();
        });
        v->addSpacing(8);
        v->addWidget(logoutBtn);
        return sb;
    }

    // Top bar
    QWidget* buildTopBar() {
        auto* bar = new QWidget();
        bar->setFixedHeight(60);
        bar->setStyleSheet(QString("background:%1; border-bottom:1px solid %2;")
                           .arg(Theme::BG2).arg(Theme::BORDER));
        auto* h = new QHBoxLayout(bar);
        h->setContentsMargins(24,0,24,0);

        m_lbUser = new QLabel();
        m_lbUser->setStyleSheet(QString("font-size:12px; color:%1; font-family:'Segoe UI',sans-serif; letter-spacing:0.5px;").arg(Theme::TEXT2));

        m_lbBal = new QLabel();
        m_lbBal->setStyleSheet(QString("font-size:13px; font-weight:700; color:%1; font-family:'Segoe UI',sans-serif;").arg(Theme::ACCENT));

        auto* createBtn = new QPushButton("＋  Создать лот");
        createBtn->setStyleSheet(Theme::btnAccent());
        createBtn->setFixedHeight(36);
        connect(createBtn, &QPushButton::clicked, this, &MainWindow::createLot);

        h->addWidget(m_lbUser);
        h->addSpacing(20);
        h->addWidget(m_lbBal);
        h->addStretch();
        h->addWidget(createBtn);
        return bar;
    }

    //  Страница лотов
    QWidget* buildLotsPage() {
        auto* w = new QWidget(); w->setStyleSheet("background:transparent;");
        auto* v = new QVBoxLayout(w); v->setContentsMargins(24,20,24,0); v->setSpacing(14);

        auto* titleRow = new QHBoxLayout();
        auto* title = new QLabel("Активные аукционы");
        title->setStyleSheet(QString("font-size:17px; font-weight:700; color:%1;").arg(Theme::TEXT1));
        titleRow->addWidget(title); titleRow->addStretch();
        v->addLayout(titleRow);

        // Фильтры
        auto* fRow = new QHBoxLayout(); fRow->setSpacing(10);
        m_search = new QLineEdit(); m_search->setPlaceholderText("🔍  Поиск по названию...");
        m_search->setFixedHeight(38);
        m_search->setStyleSheet(Theme::inputStyle());
        m_catFilter = new QComboBox();
        m_catFilter->setFixedHeight(38);
        m_catFilter->setStyleSheet(Theme::inputStyle());
        m_catFilter->addItems({"Все категории","Техника","Фото","Музыка","Игры","Спорт","Часы","Антиквариат","Одежда","Другое"});
        connect(m_search,    &QLineEdit::textChanged, this, &MainWindow::refresh);
        connect(m_catFilter, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::refresh);
        fRow->addWidget(m_search, 1); fRow->addWidget(m_catFilter);
        v->addLayout(fRow);

        auto* scroll = makeScroll(m_allLayout);
        v->addWidget(scroll, 1);
        return w;
    }

    QWidget* buildFavPage()      { return makeSimplePage("Избранное",        "★", m_favLayout); }
    QWidget* buildMyLotsPage()   { return makeSimplePage("Мои лоты",         "📦", m_myLotLayout); }
    QWidget* buildMyBidsPage()   { return makeSimplePage("Мои ставки",       "💰", m_myBidLayout); }
    QWidget* buildFinishedPage() { return makeSimplePage("Завершённые лоты", "🏁", m_finLayout); }

    QWidget* makeSimplePage(const QString& title, const QString& /*icon*/, QVBoxLayout*& layoutRef) {
        auto* w = new QWidget(); w->setStyleSheet("background:transparent;");
        auto* v = new QVBoxLayout(w); v->setContentsMargins(24,20,24,0); v->setSpacing(14);
        auto* t = new QLabel(title);
        t->setStyleSheet(QString("font-size:17px; font-weight:700; color:%1;").arg(Theme::TEXT1));
        v->addWidget(t);
        auto* scroll = makeScroll(layoutRef);
        v->addWidget(scroll, 1);
        return w;
    }

    //  Профиль
    QWidget* buildProfilePage() {
        auto* w = new QWidget(); w->setStyleSheet("background:transparent;");
        auto* v = new QVBoxLayout(w); v->setContentsMargins(24,24,24,24); v->setSpacing(20);
        v->setAlignment(Qt::AlignTop);

        auto* title = new QLabel("Профиль");
        title->setStyleSheet(QString("font-size:17px; font-weight:700; color:%1;").arg(Theme::TEXT1));
        v->addWidget(title);

        // Статы
        auto* statsRow = new QHBoxLayout(); statsRow->setSpacing(12);
        m_stBal  = new StatTile("Общий баланс");
        m_stFree = new StatTile("Доступно");
        m_stHold = new StatTile("Заморожено");
        m_stLots = new StatTile("Моих лотов");
        m_stBids = new StatTile("Моих ставок");
        statsRow->addWidget(m_stBal);
        statsRow->addWidget(m_stFree);
        statsRow->addWidget(m_stHold);
        statsRow->addWidget(m_stLots);
        statsRow->addWidget(m_stBids);
        v->addLayout(statsRow);

        v->addWidget(makeSep());

        auto* topupLabel = new QLabel("Пополнить тестовый баланс");
        topupLabel->setStyleSheet(QString("font-size:13px; color:%1;").arg(Theme::TEXT2));
        v->addWidget(topupLabel);

        auto* btnRow = new QHBoxLayout(); btnRow->setSpacing(10);
        for (int amt : {5000, 10000, 25000, 50000, 100000}) {
            auto* btn = new QPushButton(QString("+%1 ₽").arg(amt));
            btn->setStyleSheet(Theme::btnGhost());
            btn->setFixedHeight(38);
            connect(btn, &QPushButton::clicked, this, [=]{
                AppState::i().topUp(amt);
            });
            btnRow->addWidget(btn);
        }
        btnRow->addStretch();
        v->addLayout(btnRow);
        v->addStretch();
        return w;
    }

    // Helpers
    QScrollArea* makeScroll(QVBoxLayout*& layoutRef) {
        auto* scroll = new QScrollArea();
        scroll->setWidgetResizable(true);
        scroll->setFrameShape(QFrame::NoFrame);
        scroll->setStyleSheet("QScrollArea { background:transparent; }");
        auto* container = new QWidget();
        container->setStyleSheet("background:transparent;");
        layoutRef = new QVBoxLayout(container);
        layoutRef->setSpacing(8);
        layoutRef->setContentsMargins(0,0,0,16);
        layoutRef->addStretch();
        scroll->setWidget(container);
        return scroll;
    }

    void fillLayout(QVBoxLayout* layout, std::function<bool(const Lot&)> filter) {
        QLayoutItem* item;
        while (layout->count() > 1) {
            item = layout->takeAt(0);
            if (item->widget()) delete item->widget();
            delete item;
        }
        int count = 0;
        for (auto& l : AppState::i().lots) {
            if (!filter(l)) continue;
            bool fav = AppState::i().isFav(l.id);
            auto* card = new LotCard(l, fav);
            connect(card, &LotCard::clicked,     this, [=](int id){ openLot(id); });
            connect(card, &LotCard::favToggled,  this, [=](int id){ AppState::i().toggleFav(id); });
            layout->insertWidget(layout->count()-1, card);
            count++;
        }
        if (!count) {
            auto* lbl = new QLabel("Ничего не найдено");
            lbl->setAlignment(Qt::AlignCenter);
            lbl->setStyleSheet(QString("color:%1; font-size:14px; padding:40px;").arg(Theme::TEXT3));
            layout->insertWidget(0, lbl);
        }
    }

    void refresh() {
        auto* u = AppState::i().me();
        if (!u) return;

        m_lbUser->setText(u->login.toUpper());
        m_lbBal->setText(QString("₽ %1").arg(u->balance,0,'f',0));

        // Статы профиля
        if (m_stBal) {
            m_stBal->setValue(QString("₽%1").arg(u->balance,0,'f',0), Theme::ACCENT);
            m_stFree->setValue(QString("₽%1").arg(u->freeBal(),0,'f',0), Theme::GREEN);
            m_stHold->setValue(QString("₽%1").arg(u->hold,0,'f',0), Theme::TEXT2);
            int myLots=0, myBids=0;
            for (auto& l : AppState::i().lots) {
                if (l.sellerId==u->id) myLots++;
                for (auto& b : l.bids) if (b.userId==u->id) { myBids++; break; }
            }
            m_stLots->setValue(QString::number(myLots));
            m_stBids->setValue(QString::number(myBids));
        }

        QString search = m_search ? m_search->text().toLower() : "";
        QString cat    = m_catFilter ? m_catFilter->currentText() : "Все категории";

        // Все лоты
        fillLayout(m_allLayout, [&](const Lot& l) {
            if (!l.isActive()) return false;
            if (cat!="Все категории" && l.category!=cat) return false;
            if (!search.isEmpty() && !l.title.toLower().contains(search)
                && !l.description.toLower().contains(search)) return false;
            return true;
        });

        // Избранное
        fillLayout(m_favLayout, [](const Lot& l){
            return AppState::i().isFav(l.id);
        });

        // Мои лоты
        fillLayout(m_myLotLayout, [&](const Lot& l){ return l.sellerId==u->id; });

        // Мои ставки
        fillLayout(m_myBidLayout, [&](const Lot& l){
            for (auto& b : l.bids) if (b.userId==u->id) return true;
            return false;
        });

        // Завершённые
        fillLayout(m_finLayout, [](const Lot& l){ return !l.isActive(); });
    }

    void openLot(int id) {
        LotDialog dlg(id, this);
        dlg.exec();
    }

    void createLot() {
        CreateLotDialog dlg(this);
        if (dlg.exec()==QDialog::Accepted) refresh();
    }
};
