#include <QApplication>
#include <QStackedWidget>
#include "include/authwindow.h"
#include "include/mainwindow.h"
#include "include/appstate.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setStyle("Fusion");

    auto* stack = new QStackedWidget();
    stack->setMinimumSize(900, 640);

    auto* auth = new AuthWindow();
    auto* main = new MainWindow();

    stack->addWidget(auth);   // 0
    stack->addWidget(main);   // 1
    stack->setCurrentIndex(0);
    stack->resize(1100, 700);
    stack->setWindowTitle("BidFlow");
    stack->show();

    QObject::connect(auth, &AuthWindow::loginSuccess, [=]{
        stack->setCurrentIndex(1);
    });
    QObject::connect(main, &MainWindow::loggedOut, [=]{
        stack->setCurrentIndex(0);
    });

    return app.exec();
}
