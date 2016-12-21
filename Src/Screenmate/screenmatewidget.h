#pragma once

#include <QWidget>

#include <QTimer>
#include <QList>
#include <QPoint>
#include <QMouseEvent>
#include <QTimerEvent>
#include <QPair>
#include <QPixmap>
#include <QBitmap>

class ScreenmateWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ScreenmateWidget(QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent* event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void timerEvent(QTimerEvent *event);

private slots:
    void onTimerMove();

private:
    /**
     * @brief readSettings чтение настроек из файла настроек в соответствующие поля класса
     * @param filename имя файла с настройками
     */
    void readSettings(const QString &filename);

    /**
     * @brief initSprites загрузка спрайтов из ресурса в контейнер, являющийся полем класса
     * @param count количество спрайтов
     * @param useHeuristicCalcSpritesCount использовать эвристику при подсчете количества спрайтов.
     * Eсли true, то @param count игнорируется
     * @param maskColor цвет маски. Игнорируется, если @param useHeuristicCalcSpritesCount
     * установлен в false
     */
    void initSprites(
            int count,
            bool useHeuristicCalcSpritesCount = false,
            const QColor &maskColor = Qt::white);

    /**
     * @brief saveTrajectory
     * @param pos
     */
    void saveTrajectory(const QPoint &pos);

    enum ConstructMode { Predefined, BasedOnFixedPoints, Random };
    enum MovementDirection { toLeft, toRight };

    QTimer *timerMove_;

    QPoint dragPosition_;

    QList<QPair<QPixmap, QBitmap> > sprites_;           // спрайты для направления toLeft
    QList<QPair<QPixmap, QBitmap> > mirroredSprites_;   // спрайты для направления toRight

    QPixmap currSprite_;

    MovementDirection direction_;

    // значения ключей из файла настроек
    bool isTraining_;

    int moveSpeed_;
    int drawSpeed_;

    QList<QPoint> trajectory_;
    ConstructMode mode_;
    //
};
