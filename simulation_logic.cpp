#include "mainwindow.h"

void MainWindow::update_neighbors_count(int col, int row, bool is_alive) {
    int arr[] = {1,0,-1,0,1,1,-1,-1,1};
    if(is_alive) {
        for(int i = 0; i < 8; i++) {
            if(col+arr[i] >= 0 && col+arr[i] < GRID_WIDTH && row+arr[i+1] >= 0 && row+arr[i+1] < GRID_HEIGHT) {
                neighbors_count[QList<int>{col+arr[i],row+arr[i+1]}]++;
            }
        }
    }
    else {
        for(int i = 0; i < 8; i++) {
            if(col+arr[i] >= 0 && col+arr[i] < GRID_WIDTH && row+arr[i+1] >= 0 && row+arr[i+1] < GRID_HEIGHT) {
                neighbors_count[QList<int>{col+arr[i],row+arr[i+1]}]--;
            }
        }
    }
}

void MainWindow::simulate() {
    /*
     *  go through flipped_cells and set the neighbors count of the cell's neighbors
     *
     *  if the cell's neighbors count got updated
     *      if the cell is alive
     *          if the neighbors count of the cell is in [2,3]
     *              remove the cell from candidates
     *          else
     *              add the cell to candidates
     *      if the cell is dead
     *          if the neighbors count of the cell is not 3
     *              remove the cell from candidates
     *          else
     *              add the cell to candidates
     *
     *  go through flipped cells to flip all the cells and to update the window
     *  go through flipped cells to see if any can be in candidates
     *  set flipped_cells to candidates
     */

    qInfo() << "start flipped cells: " << flipped_cells;
    qInfo() << "start alive cells: " << alive_cells;

    QSet<QList<int>> candidates;

    // update the candidates first from the flipped cells
    int arr[] = {1,0,-1,0,1,1,-1,-1,1};
    for(QList<int> fc: as_const(flipped_cells)) {
        int col = fc[0];
        int row = fc[1];
        for(int i = 0; i < 8; i++) {
            int neigh_col = col+arr[i];
            int neigh_row = row+arr[i+1];
            QList<int> neighbor = QList<int>{neigh_col,neigh_row};
            // check for out of bound
            if(neigh_col >= 0 && neigh_col < GRID_WIDTH && neigh_row >= 0 && neigh_row < GRID_HEIGHT) {
                // if the cell that's about to be flipped is alive then neighbors_count +1, else -1
                neighbors_count[neighbor]+=(alive_cells.contains(fc)? -1 : 1);
                candidates.insert(neighbor);
            }
        }
    }
    qInfo() << neighbors_count;

    // for(QList<int> candidate: candidates) {
    //     qInfo() << candidate << alive_cells.contains(candidate) << neighbors_count[candidate];
    // }

    // remove the candidates that won't be flipped
    candidates.removeIf([=](QList<int> candidate) {
        int count = neighbors_count[candidate];
        if((alive_cells.contains(candidate) && (count < 2 || count > 3))||
            (!alive_cells.contains(candidate) && count != 3)) {
            return true;
        }
        return false;
    });
    qInfo() << "trimmed candidates: " << candidates;

    // flip all the cells and update the window
    for(QList<int> fc: as_const(flipped_cells)) {
        if(alive_cells.contains(fc)) {
            alive_cells.remove(fc);
        }
        else {
            alive_cells.insert(fc);
        }

        //update the window
        update(QRect(fc[0]*CELL_DIMENSION,fc[1]*CELL_DIMENSION,CELL_DIMENSION,CELL_DIMENSION));

    }

    // go through flipped_cells to check if any cell can still be flipped next round
    for(const QList<int>& fc: as_const(flipped_cells)) {
        if((alive_cells.contains(fc) && (neighbors_count[fc] < 2 || neighbors_count[fc] > 3))||
            (!alive_cells.contains(fc) && neighbors_count[fc] == 3)) {
            candidates.insert(fc);
        }

    }

    flipped_cells = candidates;

    qInfo() << "end flipped cells: " << flipped_cells;
    qInfo() << "end alive cells: " << alive_cells;


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
