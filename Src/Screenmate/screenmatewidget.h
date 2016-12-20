#pragma once

#include <QWidget>

#include <QTimer>
#include <QList>
#include <QPoint>
#include <QMouseEvent>
#include <QPair>
#include <QPixmap>
#include <QBitmap>

class ScreenmateWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ScreenmateWidget(QWidget *parent = 0);
    ~ScreenmateWidget();

    enum ConstructMode { Predefined, BasedOnFixedPoints, Random };

protected:
    void paintEvent(QPaintEvent* event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private slots:
    void onTimerDraw();
    void onTimerMove();

private:
    /**
     * @brief readSettings чтение значений ключей из файла настроек в переменные
     * @param filename имя файла с настройками
     */
    void readSettings(const QString &filename);

    /**
     * @brief initSprites инициализация спрайтов
     */
    void initSprites();

    int calcSpritesCountEurestics(const QPixmap &pixmap, const QColor maskColor = Qt::white);

    /**
     * @brief saveTrajectory
     * @param pos
     */
    void saveTrajectory(const QPoint &pos);

    QTimer *timerDraw_;
    QTimer *timerMove_;

    QPoint dragPosition_;

    QList<QPair<QPixmap, QBitmap> > sprites_;
    QPixmap currSprite_;

    // значения ключей из файла настроек
    bool isTraining_;

    int moveSpeed_;
    int drawSpeed_;

    QList<QPoint> trajectory_;
    ConstructMode mode_;
    //
};
