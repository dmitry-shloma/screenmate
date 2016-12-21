#include "screenmatewidget.h"

#include <QApplication>

#include <QSettings>
#include <QPainter>
#include <QDir>
#include <QFile>
#include <QTextStream>

ScreenmateWidget::ScreenmateWidget(QWidget *parent) :
    QWidget(parent),
    direction_(toLeft)
{
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    const QString fileName = QString("%1%2%3.conf")
            .arg(QApplication::applicationDirPath())
            .arg(QDir::separator())
            .arg(QApplication::applicationName());

    readSettings(fileName);
    initSprites(5);

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
        saveTrajectory(pos);
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
}

void ScreenmateWidget::readSettings(const QString &filename)
{
    QSettings settings(filename, QSettings::IniFormat);
    isTraining_ = settings.value("Training/isTraining", false).toBool();

    moveSpeed_ = settings.value("Sprite/moveSpeed", 15).toInt();
    drawSpeed_ = settings.value("Sprite/drawSpeed", 250).toInt();

    QStringList points = settings.value("Trajectory/points").toString()
            .split(";", QString::SkipEmptyParts);
    Q_FOREACH(QString point, points) {
        int x = point.section(',', 0, 0).toInt();
        int y = point.section(',', 1, 1).toInt();
        trajectory_.append(QPoint(x, y));
    }
    QString modeStr = settings.value("Trajectory/mode", "predefined").toString();
    if (modeStr.compare("predefined", Qt::CaseSensitive)) {
        mode_ = Predefined;
    } else if (modeStr.compare("basedOnFixedPoints", Qt::CaseSensitive)) {
        mode_ = BasedOnFixedPoints;
    } else if (modeStr.compare("random", Qt::CaseSensitive)) {
        mode_ = Random;
    }
}

void ScreenmateWidget::saveTrajectory(const QPoint &pos)
{
    static QString fileName = QString("%1/savedTrajectory.log")
            .arg(QApplication::applicationDirPath());

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
        return;
    }

    QTextStream out(&file);
    QString str = QString("%1,%2;").arg(pos.x()).arg(pos.y());
    out << str;
    file.close();
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
