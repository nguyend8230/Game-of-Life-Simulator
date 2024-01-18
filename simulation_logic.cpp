#include "mainwindow.h"

void MainWindow::update_neighbors(int col, int row, bool is_alive) {
    int arr[] = {1,0,-1,0,1,1,-1,-1,1};
    for(int i = 0; i < 8; i++) {
        int neigh_col = col+arr[i];
        int neigh_row = row+arr[i+1];
        QList<int> neighbor {neigh_col, neigh_row};
        if(neigh_col >= 0 && neigh_col < GRID_WIDTH && neigh_row >= 0 && neigh_row < GRID_HEIGHT) {
            neighbors_count[neighbor]+=(is_alive? -1: 1);
            flipped_cells.insert(neighbor);
        }
    }
}

void MainWindow::clean_flipped_cells() {
    flipped_cells.removeIf([=](QList<int> cell) {
        int count = neighbors_count[cell];
        if((alive_cells.contains(cell) && (count >= 2 && count <= 3))||
            (!alive_cells.contains(cell) && count != 3))
        {
            return true;
        }
        return false;
    });
}

void MainWindow::flip_cell(QList<int> cell) {
    int col = cell[0];
    int row = cell[1];
    update_neighbors(col,row,alive_cells.contains(cell));
    if(alive_cells.contains(cell)) {
        alive_cells.remove(cell);
    }
    else {
        alive_cells.insert(cell);
    }
    update(QRect(col*CELL_DIMENSION,row*CELL_DIMENSION,CELL_DIMENSION,CELL_DIMENSION));
}

void MainWindow::simulate() {
    qInfo() << "start alive cells:" << alive_cells;
    qInfo() << "start flipped cells:" << flipped_cells;
    qInfo() << "start neighbors count:" << neighbors_count << Qt::endl;

    if(flipped_cells.empty()) {
        simulation_toggle = false;
        return;
    }

    // need a temp set because I can't insert to flipped_cells while iterating through it since flipped_cells is a set
    QSet<QList<int>> temp = flipped_cells;

    for(const QList<int>& cell: as_const(temp)) {
        flip_cell(cell);
    }

    qInfo() << "end alive cells:" << alive_cells;
    qInfo() << "end flipped cells:" << flipped_cells;
    qInfo() << "end neighbors count:" << neighbors_count << Qt::endl;

    clean_flipped_cells();
}

void MainWindow::simulate_all() {
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=]() {
        if(simulation_toggle) {
            simulate();
        }
        else {
            timer->stop();
        }
    });
    timer->start(300);
}
