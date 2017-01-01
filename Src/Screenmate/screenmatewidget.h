#pragma once

#include <QWidget>

#include <QTimer>
#include <QList>
#include <QPoint>
#include <QMouseEvent>
#include <QTimerEvent>
#include <QKeyEvent>
#include <QPair>
#include <QPixmap>
#include <QBitmap>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QSignalMapper>

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
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);

    /// Слоты для элементов меню
//    void onIsTrainingModeAct();
    void onGarlandAct();
    void onStarsAct();
    void onAnimationAct();
    void onTransparentActSelected(int level);
    void onExitAct();
    ///

private:
    /**
     * @brief The ConstructMode enum режимы конструирования траектории
     */
    enum ConstructMode { Predefined, BasedOnFixedPoints, Random };

    /**
     * @brief The MovementDirection enum направления движения
     */
    enum MovementDirection { toLeft, toRight };

    /**
     * @brief readSettings чтение настроек из файла настроек в соответствующие поля класса
     * @param filename имя файла с настройками
     */
    void readSettings(const QString &filename);

    /**
     * @brief initSprites загрузка спрайтов из ресурса в контейнер (поле класса)
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
     * @brief createTrayIcon
     * @param contextMenu указатель на контекстное меню
     */
    void createTrayIcon(QMenu *contextMenu);

    /**
     * @brief createTrayIconMenu создание меню
     * @return указатель на меню
     */
    QMenu *createTrayIconMenu();

    /**
     * @brief constructTrajectory конструирование траектории
     * @param points
     * @return
     */
    QList<QPoint> constructTrajectory(const QStringList &points = QStringList());

    QTimer *timerMove_;

    QPoint dragPosition_;

    QList<QPair<QPixmap, QBitmap> > sprites_;           // контейнер для спрайтов (направление toLeft)
    QList<QPair<QPixmap, QBitmap> > mirroredSprites_;   // контейнер для спрайтов (направление toRight)

    QPixmap currSprite_;

    MovementDirection direction_;

    QSystemTrayIcon *trayIcon_;

    // значения ключей из файла настроек
    bool isTrainingMode_;               // тренировочный режим

    int moveSpeed_;                 // скорость перемещения
    int drawSpeed_;                 // скорость отрисовки

    QList<QPoint> trajectory_;      // траектория
    ConstructMode constructMode_;   // режим конструирования траектории
    //

    QSignalMapper *signalMapper_;
};
