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
    void onAddPointClicked();
    void onRemovePointClicked();
    void onCalculateClicked();
    void onClearAllClicked();

private:
    Ui::MainWindow *ui;

    struct Point {
        double x;
        double y;
        Point(double x = 0, double y = 0) : x(x), y(y) {}
    };

    QVector<Point> points;

    bool isSelfIntersecting();
    double calculatePerimeter();
    bool canDrawWithoutLifting();
    double crossProduct(const Point &a, const Point &b, const Point &c);
    bool onSegment(const Point &p, const Point &a, const Point &b);
    bool segmentsIntersect(const Point &a1, const Point &a2, const Point &b1, const Point &b2);
    double distance(const Point &a, const Point &b);
    void updateTable();
};

#endif
