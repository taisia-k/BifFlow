#pragma once
#include <QString>

namespace Theme {

// Палитра
constexpr auto BG0      = "#080807";
constexpr auto BG1      = "#0d0d0b";
constexpr auto BG2      = "#131310";
constexpr auto BG3      = "#1c1c18";
constexpr auto BORDER   = "#2a2a22";
constexpr auto BORDER2  = "#3a3a2e";


constexpr auto ACCENT   = "#c9a84c";
constexpr auto ACCENT2  = "#e2c06a";
constexpr auto ACCENTD  = "#a07830";
constexpr auto ACCENTBG = "#1e1a0e";

constexpr auto GREEN    = "#5cb88a";
constexpr auto RED      = "#c97070";
constexpr auto SILVER   = "#9090a0";

// Текст
constexpr auto TEXT1    = "#f2ede4";
constexpr auto TEXT2    = "#7a7060";
constexpr auto TEXT3    = "#44403a";


inline QString appStyle() {
    return QString(R"(
        QWidget {
            background: %1;
            color: %2;
            font-family: 'Georgia', 'Palatino Linotype', 'Times New Roman', serif;
            font-size: 13px;
        }
        QScrollBar:vertical {
            background: transparent; width: 4px; margin: 0;
        }
        QScrollBar::handle:vertical {
            background: %3; border-radius: 2px; min-height: 24px;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
        QScrollBar:horizontal { height: 4px; background: transparent; }
        QScrollBar::handle:horizontal { background: %3; border-radius: 2px; }
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0; }
        QToolTip {
            background: %4; color: %2;
            border: 1px solid %5; border-radius: 6px; padding: 6px 10px;
        }
    )").arg(BG1).arg(TEXT1).arg(BORDER2).arg(BG3).arg(BORDER2);
}

// Инпуты
inline QString inputStyle() {
    return QString(R"(
        QLineEdit, QTextEdit, QComboBox, QSpinBox, QDoubleSpinBox {
            background: %1;
            border: 1px solid %2;
            border-radius: 8px;
            padding: 10px 14px;
            font-size: 13px;
            color: %3;
            font-family: 'Segoe UI', sans-serif;
            selection-background-color: %4;
        }
        QLineEdit:focus, QTextEdit:focus { border-color: %5; }
        QComboBox::drop-down { border: none; width: 24px; }
        QComboBox::down-arrow { width: 8px; height: 8px; }
        QComboBox QAbstractItemView {
            background: %1; border: 1px solid %2;
            border-radius: 8px; padding: 4px; color: %3;
        }
        QComboBox QAbstractItemView::item { padding: 8px 12px; border-radius: 5px; }
        QComboBox QAbstractItemView::item:selected { background: %4; color: %3; }
        QSpinBox::up-button, QSpinBox::down-button,
        QDoubleSpinBox::up-button, QDoubleSpinBox::down-button { width: 0; }
    )").arg(BG3).arg(BORDER).arg(TEXT1).arg(BORDER2).arg(ACCENT);
}

// Кнопки
inline QString btnAccent() {
    return QString(R"(
        QPushButton {
            background: %1;
            color: #0d0d0b;
            border: none;
            border-radius: 8px;
            padding: 11px 24px;
            font-size: 13px;
            font-weight: 700;
            font-family: 'Segoe UI', sans-serif;
            letter-spacing: 0.3px;
        }
        QPushButton:hover   { background: %2; }
        QPushButton:pressed { background: %3; }
        QPushButton:disabled { background: %4; color: %5; }
    )").arg(ACCENT).arg(ACCENT2).arg(ACCENTD).arg(BORDER2).arg(TEXT3);
}

inline QString btnGhost() {
    return QString(R"(
        QPushButton {
            background: transparent;
            color: %1;
            border: 1px solid %2;
            border-radius: 8px;
            padding: 11px 24px;
            font-size: 13px;
            font-family: 'Segoe UI', sans-serif;
        }
        QPushButton:hover { border-color: %3; color: %4; background: %5; }
        QPushButton:pressed { background: %2; }
    )").arg(TEXT2).arg(BORDER2).arg(ACCENT).arg(ACCENT).arg(ACCENTBG);
}

// Логотип
inline QString logoHtml() {
    return QString("<span style='color:%1;'>♦</span>"
                   "<span style='color:%2; font-weight:700; letter-spacing:-0.5px;'> BidFlow</span>")
           .arg(ACCENT).arg(TEXT1);
}

}
