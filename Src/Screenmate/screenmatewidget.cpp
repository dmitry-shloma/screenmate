#include "screenmatewidget.h"

#include <QApplication>

#include <QSettings>
#include <QPainter>
#include <QDir>
#include <QFile>
#include <QTextStream>

ScreenmateWidget::ScreenmateWidget(QWidget *parent) :
    QWidget(parent)
{
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint); // установка флагов окна

    const QString fileName = QString("%1%2%3.conf")
            .arg(QApplication::applicationDirPath())
            .arg(QDir::separator())
            .arg(QApplication::applicationName());

    readSettings(fileName);
    initSprites();

    if (!isTraining_) {
        onTimerMove();
        timerMove_ = new QTimer(this);
        connect(timerMove_, SIGNAL(timeout()), this, SLOT(onTimerMove()));
        timerMove_->start(moveSpeed_);
    }

    onTimerDraw();
    timerDraw_ = new QTimer(this);
    connect(timerDraw_, SIGNAL(timeout()), this, SLOT(onTimerDraw()));
    timerDraw_->start(drawSpeed_);
}

ScreenmateWidget::~ScreenmateWidget()
{
    delete timerMove_;
    delete timerDraw_;
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
        QApplication::exit();
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
        saveTrajectory(pos);
        event->accept();
    }
}

void ScreenmateWidget::onTimerDraw()
{
    static int i = 0;

    currSprite_ = sprites_.at(i).first;
    setMask(sprites_.at(i).second);
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

    QStringList points = settings.value("Trajectory/points").toString().split(";", QString::SkipEmptyParts);
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
    static QString fileName = QString("%1%2savedTrajectory.log")
            .arg(QApplication::applicationDirPath())
            .arg(QDir::separator());

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
        return;
    }

    QTextStream out(&file);
    QString str = QString("%1,%2;").arg(pos.x()).arg(pos.y());
    out << str;
    file.close();
}

void ScreenmateWidget::initSprites()
{
    QPixmap pixmap("://characters/bat.png");
    int count = /*calcSpritesCountEurestics(pixmap)*/5;
    int spriteWidth = pixmap.width() / count;
    int spriteHeigth = pixmap.height();

    for (int i = 0; i < pixmap.width(); i += spriteWidth) {
        QPixmap sprite = pixmap.copy(i, 0, spriteWidth, spriteHeigth);
        QBitmap mask = sprite.createHeuristicMask();
        sprites_.append(QPair<QPixmap, QBitmap>(sprite, mask));
    }
}

int ScreenmateWidget::calcSpritesCountEurestics(const QPixmap &pixmap, const QColor maskColor)
{
    QImage image = pixmap.toImage();

    bool inSprite = false;
    int count = 0;

    for (int x = 0; x < image.width(); ++x) {
        for (int y = 0; y < image.height(); ++y) {
            if (image.pixel(x, y) != maskColor.rgb()) {
                inSprite = true;
            }
        }
        if (!inSprite) {
            count++;
        }
    }

    return count;
}
