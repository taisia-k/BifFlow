#pragma once
#include <QObject>
#include <QTimer>
#include "models.h"

class AppState : public QObject {
    Q_OBJECT
public:
    static AppState& i() { static AppState s; return s; }

    QList<User>  users;
    QList<Lot>   lots;
    QList<Chat>  chats;
    int currentUserId = -1;

    // Lookup
    User* me()               { return findUser(currentUserId); }
    User* findUser(int id)   { for (auto& u : users) if (u.id==id) return &u; return nullptr; }
    User* byLogin(const QString& l) { for (auto& u : users) if (u.login==l) return &u; return nullptr; }
    Lot*  findLot(int id)    { for (auto& l : lots)  if (l.id==id) return &l; return nullptr; }

    bool isFav(int lotId) const { return favIds.contains(lotId); }
    void toggleFav(int lotId) {
        if (favIds.contains(lotId)) favIds.remove(lotId);
        else favIds.insert(lotId);
        emit dataChanged();
    }

    //  Auth
    bool registerUser(const QString& login, const QString& pass) {
        if (byLogin(login)) return false;
        User u; u.id=nextUid++; u.login=login; u.password=pass;
        users.append(u); return true;
    }
    bool login(const QString& login, const QString& pass) {
        auto* u = byLogin(login);
        if (!u || u->password != pass) return false;
        currentUserId = u->id; return true;
    }
    void logout() { currentUserId = -1; }

    //  Lots
    bool createLot(const QString& title, const QString& desc,
                   const QString& cat, double start, double step, int durSecs) {
        auto* u = me(); if (!u) return false;
        Lot l;
        l.id=nextLid++; l.title=title; l.description=desc; l.category=cat;
        l.sellerId=u->id; l.sellerLogin=u->login;
        l.startPrice=start; l.currentPrice=start; l.minStep=step;
        l.endTime=QDateTime::currentDateTime().addSecs(durSecs);
        lots.append(l); return true;
    }

    // Bids
    QString placeBid(int lotId, double amount) {
        auto* l = findLot(lotId); auto* u = me();
        if (!l||!u)          return "Ошибка";
        if (!l->isActive())  return "Аукцион завершён";
        if (l->sellerId==u->id) return "Нельзя ставить на свой лот";
        double minBid = l->currentPrice + l->minStep;
        if (amount < minBid) return QString("Минимальная ставка: ₽%1").arg(minBid,0,'f',0);

        double myOldHold = (l->leaderId==u->id) ? l->currentPrice : 0.0;
        if (u->freeBal() + myOldHold < amount) return "Недостаточно средств";

        // Снять резерв у предыдущего лидера
        if (l->leaderId != -1 && l->leaderId != u->id) {
            auto* prev = findUser(l->leaderId);
            if (prev) prev->hold -= l->currentPrice;
        }
        // Обновить резерв текущего
        u->hold += (amount - myOldHold);

        l->currentPrice = amount;
        l->leaderId     = u->id;
        l->leaderLogin  = u->login;

        Bid b; b.id=nextBid++; b.lotId=lotId;
        b.userId=u->id; b.userLogin=u->login;
        b.amount=amount; b.time=QDateTime::currentDateTime();
        l->bids.prepend(b);

        emit dataChanged(); return "";
    }

    //  Chat
    Chat* findChat(int lotId, int uid1, int uid2) {
        for (auto& c : chats)
            if (c.lotId==lotId && c.involves(uid1) && c.involves(uid2)) return &c;
        return nullptr;
    }
    Chat* getOrCreateChat(int lotId, int buyerId, int sellerId) {
        auto* c = findChat(lotId, buyerId, sellerId);
        if (c) return c;
        Chat ch; ch.lotId=lotId; ch.userA=buyerId; ch.userB=sellerId;
        chats.append(ch);
        return &chats.last();
    }
    void sendMessage(int lotId, int toUserId, const QString& text) {
        auto* u = me(); if (!u||text.trimmed().isEmpty()) return;
        auto* l = findLot(lotId); if (!l) return;
        int buyerId  = (u->id == l->sellerId) ? toUserId : u->id;
        int sellerId = l->sellerId;
        auto* c = getOrCreateChat(lotId, buyerId, sellerId);
        ChatMessage m; m.fromUserId=u->id; m.fromLogin=u->login;
        m.text=text.trimmed(); m.time=QDateTime::currentDateTime();
        c->messages.append(m);
        emit dataChanged();
    }

    // Expire
    void checkExpired() {
        bool changed=false;
        for (auto& l : lots) {
            if (l.status==LotStatus::Active && l.secondsLeft()<=0) {
                l.status=LotStatus::Finished;
                if (l.leaderId!=-1) {
                    auto* w = findUser(l.leaderId);
                    if (w) { w->balance -= l.currentPrice; w->hold -= l.currentPrice; }
                }
                changed=true;
            }
        }
        if (changed) emit dataChanged();
    }

    void topUp(double amt) { auto* u=me(); if(u){u->balance+=amt; emit dataChanged();} }

signals:
    void dataChanged();

private:
    int nextUid=1, nextLid=1, nextBid=1;
    QSet<int> favIds;

    AppState() {
        seed();
        auto* t = new QTimer(this);
        connect(t, &QTimer::timeout, this, &AppState::checkExpired);
        t->start(1000);
    }

    void seed() {
        registerUser("taisia","1234");
        registerUser("egor",  "1234");
        registerUser("elena", "1234");

        // Лоты от Таисии (id=1)
        currentUserId=1;
        createLot("Leica M6 TTL",
                  "Легендарная плёночная дальномерка. Состояние 9/10, все функции исправны, затвор выверен.",
                  "Фото", 85000, 2000, 3600);
        createLot("Catan: первое немецкое издание 1995",
                  "Полный комплект, все компоненты целые. Раритет для коллекционеров.",
                  "Игры", 12000, 500, 1800);

        // Лоты от Егора (id=2)
        currentUserId=2;
        createLot("MacBook Pro 14",
                  "Конфигурация 18GB/512GB, Space Black. Гарантия Apple до 2026 года. Как новый.",
                  "Техника", 180000, 5000, 5400);
        createLot("Rolex Datejust 36 ref. 16200",
                  "Стальной корпус, циферблат Rhodium. Полный комплект - коробка, документы.",
                  "Часы", 650000, 10000, 14400);

        // Лот от Елены (id=3)
        currentUserId=3;
        createLot("Sony A7 IV + FE 24-70 f/2.8 GM II",
                  "Комплект боди + объектив. Пробег ~5000 кадров. Подходит для профессиональной съёмки.",
                  "Фото", 220000, 5000, 9000);

        currentUserId=-1;
    }
};
