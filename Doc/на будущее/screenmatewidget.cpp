#include "screenmatewidget.h"

#include <QApplication>

#include <QSettings>
#include <QPainter>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDesktopWidget>
#include <qmath.h>

ScreenmateWidget::ScreenmateWidget(QWidget *parent) :
    QWidget(parent),
    direction_(toLeft)
{
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    const QString fileName = QString("%1/%2.conf")
            .arg(QApplication::applicationDirPath())
            .arg(QApplication::applicationName());

    readSettings(fileName);
    initSprites(5, true);

    if (!isTraining_) {
        onTimerMove();
        timerMove_ = new QTimer(this);
        connect(timerMove_, SIGNAL(timeout()), this, SLOT(onTimerMove()));
        timerMove_->start(moveSpeed_);
    }

    timerEvent(0);
    startTimer(drawSpeed_);
}

void ScreenmateWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.drawPixmap(0, 0, currSprite_);
}

void ScreenmateWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        qApp->exit();
    }

    if (!isTraining_) {
        return;
    }

    if (event->button() == Qt::LeftButton) {
        dragPosition_ = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void ScreenmateWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!isTraining_) {
        return;
    }

    if (event->buttons() & Qt::LeftButton) {
        QPoint pos = event->globalPos() - dragPosition_;
        move(pos);
        static QPoint oldPos = pos;
        if (pos.x() > oldPos.x()) {
            direction_ = toRight;
            oldPos = pos;

        } else if (pos.x() < oldPos.x()) {
            direction_ = toLeft;
            oldPos = pos;
        }

        event->accept();
    }
}

void ScreenmateWidget::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);

    static int i = 0;

    if (direction_ == toLeft) {
        currSprite_ = sprites_.at(i).first;
        setMask(sprites_.at(i).second);
    } else if (direction_ == toRight) {
        currSprite_ = mirroredSprites_.at(i).first;
        setMask(mirroredSprites_.at(i).second);
    }

    i = (i == sprites_.size() - 1) ? 0 : i + 1;
    emit update();
}

void ScreenmateWidget::onTimerMove()
{
    static int i = 0;

    int x = trajectory_.at(i).x();
    int y = trajectory_.at(i).y();
    i = (i == trajectory_.size() - 1) ? 0 : i + 1;
    move(x, y);

    static int oldX = x;
    if (x > oldX) {
        direction_ = toRight;
        oldX = x;

    } else if (x < oldX) {
        direction_ = toLeft;
        oldX = x;
    }

}

void ScreenmateWidget::readSettings(const QString &filename)
{
    QSettings settings(filename, QSettings::IniFormat);
    isTraining_ = settings.value("Training/isTraining", false).toBool();

    moveSpeed_ = settings.value("Sprite/moveSpeed", 150).toInt();
    drawSpeed_ = settings.value("Sprite/drawSpeed", 250).toInt();

    QStringList points = settings.value("Sprite/trajectory").toString()
            .split(";", QString::SkipEmptyParts);
    QString constructMode = settings.value("Sprite/constructMode", "random").toString();

    if (constructMode.compare("predefined", Qt::CaseSensitive) == 0) {
        constructMode_ = Predefined;

    } else if (constructMode.compare("basedOnFixedPoints", Qt::CaseSensitive) == 0) {
        constructMode_ = BasedOnFixedPoints;

    } else if (constructMode.compare("random", Qt::CaseSensitive) == 0) {
        constructMode_ = Random;
    }

    trajectory_ = constructTrajectory(points);
}

void ScreenmateWidget::writeSettings(const QString &filename)
{

}

QList<QPoint> ScreenmateWidget::constructTrajectory(const QStringList &points)
{
    QList<QPoint> trajectory;

    switch (constructMode_) {
    case Predefined:
        Q_FOREACH(QString point, points) {
            int x = point.section(',', 0, 0).toInt();
            int y = point.section(',', 1, 1).toInt();
            trajectory.append(QPoint(x, y));
        }
        break;
    case BasedOnFixedPoints:
        break;
    case Random:
        QDesktopWidget desktop;
        QRect desktopRect = desktop.screenGeometry();

        const int coef = 100.0;
        for (int x = 0; x < desktopRect.width(); ++x) {
            int y = 800 + qRound(coef * qAbs(qSin(5.0 * x)));
            trajectory.append(QPoint(x, y));
        }
    }
    return trajectory;
}

void ScreenmateWidget::initSprites(
        int count, bool useHeuristicCalcSpritesCount, const QColor &maskColor)
{
    QPixmap pixmap("://characters/bat.png");
    int spriteCount = 0;
    if (!useHeuristicCalcSpritesCount) {
        spriteCount = count;
    } else {
        QImage image = pixmap.toImage();

        bool inSprite = false;
        for (int x = 0; x < image.width(); ++x) {
            for (int y = 0; y < image.height(); ++y) {
                if (image.pixel(x, y) != maskColor.rgb()) {
                    inSprite = true;
                    goto abortLineScan;
                }
            }
            if (inSprite) {
                inSprite = false;
                spriteCount++;
            }
            abortLineScan: ;
        }
    }

    int spriteWidth = pixmap.width() / spriteCount;
    int spriteHeigth = pixmap.height();

    for (int i = 0; i < pixmap.width(); i += spriteWidth) {
        QPixmap sprite = pixmap.copy(i, 0, spriteWidth, spriteHeigth);
        QBitmap mask = sprite.createHeuristicMask();
        sprites_.append(QPair<QPixmap, QBitmap>(sprite, mask));

        QPixmap mirroredSprite = QPixmap::fromImage(sprite.toImage().mirrored(true, false));
        QBitmap mirroredMask = mirroredSprite.createHeuristicMask();
        mirroredSprites_.append(QPair<QPixmap, QBitmap>(mirroredSprite, mirroredMask));
    }
}
