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
     *  set flipped_cells to candidates
     */

    QSet<QList<int>> candidates;
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
                neighbors_count[neighbor]+=(alive_cells.contains(fc)? 1 : -1);

                candidates.insert(neighbor);
            }
        }

    }

    // remove the dead cells with no neighbors from neighbors_count
    for(const QList<int>& candidate: candidates) {
        if(!alive_cells.contains(candidate) && !neighbors_count[candidate]) {
            neighbors_count.remove(candidate);
        }
    }

    // remove the candidates that won't be flipped
    candidates.removeIf([=](QList<int> candidate) {
        int count = neighbors_count[candidate];
        if((alive_cells.contains(candidate) && (count < 2 || count > 3)) || (!alive_cells.contains(candidate) && count != 3)) {
            return true;
        }
        return false;
    });

    flipped_cells = candidates;

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
