#include "mainwindow.h"

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if(event->key() == Qt::Key_Q) {
        simulate();
    }
}
