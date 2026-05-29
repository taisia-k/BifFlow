#pragma once
#include <QString>
#include <QDateTime>
#include <QList>

//  Bid
struct Bid {
    int      id;
    int      lotId;
    int      userId;
    QString  userLogin;
    double   amount;
    QDateTime time;
};

//  ChatMessage
struct ChatMessage {
    int     fromUserId;
    QString fromLogin;
    QString text;
    QDateTime time;
    bool isMe(int myId) const { return fromUserId == myId; }
};

// Чат
struct Chat {
    int lotId;
    int userA;   // всегда buyer
    int userB;   // всегда seller
    QList<ChatMessage> messages;

    bool involves(int uid) const { return userA == uid || userB == uid; }
    int  other(int uid)   const { return uid == userA ? userB : userA; }
};

// Лот
enum class LotStatus { Active, Finished };

struct Lot {
    int       id;
    QString   title;
    QString   description;
    QString   category;
    int       sellerId;
    QString   sellerLogin;
    double    startPrice;
    double    currentPrice;
    double    minStep;
    int       leaderId   = -1;
    QString   leaderLogin;
    QDateTime endTime;
    LotStatus status     = LotStatus::Active;
    QList<Bid>  bids;

    bool isActive()    const { return status == LotStatus::Active && secondsLeft() > 0; }
    int  secondsLeft() const { return (int)QDateTime::currentDateTime().secsTo(endTime); }
};

// User
struct User {
    int     id;
    QString login;
    QString password;
    double  balance = 50000.0;
    double  hold    = 0.0;

    double freeBal() const { return balance - hold; }
};
