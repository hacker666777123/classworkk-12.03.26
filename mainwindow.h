#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVector>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void onGenerateMatrixClicked();
    void onFindPathClicked();
    void onRandomizeMatrixClicked();
    void onClearMatrixClicked();

private:
    Ui::MainWindow *ui;

    int N;
    QVector<QVector<double> > adjacencyMatrix;

    void updateTable();
    void dijkstra(int start, int end);
    bool validateMatrix();
    void log(const QString &message);
};

#endif
