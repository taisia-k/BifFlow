#pragma once
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QLabel>
#include "appstate.h"
#include "theme.h"

class CreateLotDialog : public QDialog {
    Q_OBJECT
public:
    CreateLotDialog(QWidget* parent=nullptr) : QDialog(parent) {
        setWindowTitle("Создать лот");
        setFixedSize(500, 560);
        setStyleSheet(Theme::appStyle() + Theme::inputStyle() + R"(
            QDialog { background:#0e0e1a; }
            QLabel  { color:#8888aa; font-size:12px; background:transparent; }
            QLabel#h { font-size:17px; font-weight:700; color:#f0f0f8; }
            QLabel#err { color:#f87171; font-size:12px; }
        )");

        auto* root = new QVBoxLayout(this);
        root->setContentsMargins(32,28,32,28);
        root->setSpacing(16);

        auto* h = new QLabel("Новый лот"); h->setObjectName("h");
        root->addWidget(h);

        auto* form = new QFormLayout();
        form->setSpacing(12);
        form->setLabelAlignment(Qt::AlignRight);
        form->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

        leName = new QLineEdit(); leName->setPlaceholderText("Название лота");
        leDesc = new QTextEdit(); leDesc->setPlaceholderText("Описание (материал, состояние, комплектация...)");
        leDesc->setFixedHeight(72);

        cbCat = new QComboBox();
        cbCat->addItems({"Техника","Фото","Музыка","Игры","Спорт","Часы","Антиквариат","Одежда","Другое"});

        spStart = new QDoubleSpinBox();
        spStart->setRange(1, 10000000); spStart->setValue(5000);
        spStart->setSuffix(" ₽"); spStart->setDecimals(0);

        spStep = new QDoubleSpinBox();
        spStep->setRange(1, 1000000); spStep->setValue(500);
        spStep->setSuffix(" ₽"); spStep->setDecimals(0);

        // Длительность: часы + минуты
        auto* durRow = new QHBoxLayout();
        spHours = new QSpinBox();
        spHours->setRange(0, 72); spHours->setValue(1); spHours->setSuffix(" ч");
        spMins  = new QSpinBox();
        spMins->setRange(0, 59);  spMins->setValue(0);  spMins->setSuffix(" мин");
        auto* durHint = new QLabel("минимум 1 минута");
        durHint->setStyleSheet(QString("color:%1; font-size:11px;").arg(Theme::TEXT3));
        durRow->addWidget(spHours);
        durRow->addSpacing(8);
        durRow->addWidget(spMins);
        durRow->addSpacing(8);
        durRow->addWidget(durHint);
        durRow->addStretch();

        form->addRow("Название:", leName);
        form->addRow("Описание:", leDesc);
        form->addRow("Категория:", cbCat);
        form->addRow("Стартовая цена:", spStart);
        form->addRow("Шаг ставки:", spStep);
        form->addRow("Длительность:", durRow);
        root->addLayout(form);

        lbErr = new QLabel(); lbErr->setObjectName("err"); lbErr->setFixedHeight(16);
        root->addWidget(lbErr);
        root->addStretch();

        auto* btnRow = new QHBoxLayout();
        auto* cancel = new QPushButton("Отмена");
        cancel->setStyleSheet(Theme::btnGhost());
        connect(cancel, &QPushButton::clicked, this, &QDialog::reject);

        auto* create = new QPushButton("Создать лот");
        create->setStyleSheet(Theme::btnAccent());
        connect(create, &QPushButton::clicked, this, &CreateLotDialog::doCreate);

        btnRow->addWidget(cancel);
        btnRow->addStretch();
        btnRow->addWidget(create);
        root->addLayout(btnRow);
    }

private:
    QLineEdit       *leName;
    QTextEdit       *leDesc;
    QComboBox       *cbCat;
    QDoubleSpinBox  *spStart, *spStep;
    QSpinBox        *spHours, *spMins;
    QLabel          *lbErr;

    void doCreate() {
        lbErr->clear();
        if (leName->text().trimmed().isEmpty()) { lbErr->setText("Введите название"); return; }
        int durSecs = spHours->value()*3600 + spMins->value()*60;
        if (durSecs < 60) { lbErr->setText("Минимальная длительность — 1 минута"); return; }
        AppState::i().createLot(
            leName->text().trimmed(),
            leDesc->toPlainText().trimmed(),
            cbCat->currentText(),
            spStart->value(),
            spStep->value(),
            durSecs
        );
        accept();
    }
};
