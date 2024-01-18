#include "mainwindow.h"

void MainWindow::update_neighbors_count(int col, int row, bool is_alive) {
    int arr[] = {1,0,-1,0,1,1,-1,-1,1};
    // QList<int> cell {col,row};
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

void MainWindow::simulate() {

    qInfo() << "start alive cells:" << alive_cells;
    qInfo() << "start flipped cells:" << flipped_cells;
    qInfo() << "start neighbors count:" << neighbors_count;

    // need a temp set because I can't insert to flipped_cells while iterating through it since flipped_cells is a set
    QSet<QList<int>> temp = flipped_cells;

    for(QList<int> cell: as_const(temp)) {
        int col = cell[0];
        int row = cell[1];
        update_neighbors_count(col,row,alive_cells.contains(cell));
            if(alive_cells.contains(cell)) {
                alive_cells.remove(cell);
            }
            else {
                alive_cells.insert(cell);
            }

            // update(QRect(cell[0]*CELL_DIMENSION,cell[1]*CELL_DIMENSION,CELL_DIMENSION,CELL_DIMENSION));
    }


    qInfo() << "end alive cells:" << alive_cells;
    qInfo() << "end flipped cells:" << flipped_cells;
    qInfo() << "end neighbors count:" << neighbors_count;

    // qInfo() << "neighbors count:" << neighbors_count;

    // // go through flipped cells to flip the cells and update the mainwindow
    // for(QList<int> fc: as_const(flipped_cells)) {
    //     if(alive_cells.contains(fc)) {
    //         alive_cells.remove(fc);
    //     }
    //     else {
    //         alive_cells.insert(fc);
    //     }

    //     //update the window
    //     update(QRect(fc[0]*CELL_DIMENSION,fc[1]*CELL_DIMENSION,CELL_DIMENSION,CELL_DIMENSION));
    // }
    // qInfo() << "alive cells:" << alive_cells;

    // /*EVERYTHING BEYOND THIS IS TO UPDATE THE FLIPPED CELLS FOR THE NEXT ITERATION*/

    // // go through flipped cells to see if any cell still stays in flipped cells next iteration
    // for(const QList<int>& fc: as_const(flipped_cells)) {
    //     int count = neighbors_count[fc];
    //     if((alive_cells.contains(fc) && (count < 2 || count > 3))||
    //         (!alive_cells.contains(fc) && count == 3))
    //     {
    //         candidates.insert(fc);
    //     }
    // }

    // qInfo() << "original candidates:" << candidates;
    // // remove the candidates that won't be flipped
    // candidates.removeIf([=](QList<int> candidate) {
    //     int count = neighbors_count[candidate];
    //     if((alive_cells.contains(candidate) && (count >= 2 && count <= 3))||
    //         (!alive_cells.contains(candidate) && count != 3))
    //     {
    //         return true;
    //     }
    //     return false;
    // });
    // qInfo() << "trimmed candidates:" << candidates;

    // flipped_cells = candidates;

}

void MainWindow::simulate_all() {
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this, timer]() {
        simulate();
        if(!simulation_toggle) {
            timer->stop();
        }
    });
    timer->start(300);
}
