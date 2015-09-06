#pragma once

#include <QWidget>

#include <QTimer>
#include <QList>
#include <QPoint>
#include <QMouseEvent>
#include <QPair>
#include <QPixmap>
#include <QBitmap>
#include <QSettings>

namespace Ui {
class ScreenmateWidget;
}

class ScreenmateWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ScreenmateWidget(QWidget *parent = 0);
    ~ScreenmateWidget();

    enum Character { Bat, External };
    enum ConstructionMode { Predefined, BasedOnFixedPoints, Random };

protected:
    void paintEvent(QPaintEvent* event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private slots:
    void onTimerDraw();
    void onTimerMove();

private:
    Ui::ScreenmateWidget *ui;

    QSettings *settings_;

    QTimer* timerDraw_;
    QTimer* timerMove_;

    QList<QPoint> trajectory_;
    QPoint dragPosition_;

    QList<QPair<QPixmap, QBitmap> > sprites_;
    QPixmap currSprite_;

    bool isTraining_;

    void initSettings(const QString &fileName);
    void initSprites(Character character = Bat);
    void constructTrajectory(ConstructionMode mode = Predefined);

    void saveTrajectory(const QPoint &pos);

    int qrand(int low, int high);
};
