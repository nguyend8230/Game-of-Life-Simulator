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
    qInfo() << "flipped cells: " << flipped_cells;
    qInfo() << "alive cells: " << alive_cells;

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
                // if the cell that's about to be flipped is alive then neighbors_count -1, else +1
                neighbors_count[neighbor]+=(alive_cells.contains(fc)? -1 : 1);
                candidates.insert(neighbor);
            }
        }    
    }

    qInfo() << "neighbors count:" << neighbors_count;

    // go through flipped cells to flip the cells and update the mainwindow
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
    qInfo() << "alive cells:" << alive_cells;

    /*EVERYTHING BEYOND THIS IS TO UPDATE THE FLIPPED CELLS FOR THE NEXT ITERATION*/

    // go through flipped cells to see if any cell still stays in flipped cells next iteration
    for(const QList<int>& fc: as_const(flipped_cells)) {
        int count = neighbors_count[fc];
        if((alive_cells.contains(fc) && (count < 2 || count > 3))||
            (!alive_cells.contains(fc) && count == 3))
        {
            candidates.insert(fc);
        }
    }

    qInfo() << "original candidates:" << candidates;
    // remove the candidates that won't be flipped
    candidates.removeIf([=](QList<int> candidate) {
        int count = neighbors_count[candidate];
        if((alive_cells.contains(candidate) && (count >= 2 && count <= 3))||
            (!alive_cells.contains(candidate) && count != 3))
        {
            return true;
        }
        return false;
    });
    qInfo() << "trimmed candidates:" << candidates;

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
