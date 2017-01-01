#include "screenmatewidget.h"

#include <QApplication>

#include <QSettings>
#include <QPainter>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDesktopWidget>
#include <qmath.h>
#include <QAction>
#include <QDebug>

ScreenmateWidget::ScreenmateWidget(QWidget *parent) :
    QWidget(parent),
    direction_(toLeft)
{
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setWindowTitle(QApplication::applicationName());

    const QString fileName = QString("%1/%2.conf")
            .arg(QApplication::applicationDirPath())
            .arg(QApplication::applicationName());

    readSettings(fileName);
    initSprites(5, true);

    if (!isTrainingMode_) {
        onTimerMove();
        timerMove_ = new QTimer(this);
        connect(timerMove_, SIGNAL(timeout()), this, SLOT(onTimerMove()));
        timerMove_->start(moveSpeed_);
    }

    timerEvent(0);
    startTimer(drawSpeed_);

    QMenu *contextMenu = createTrayIconMenu();
    createTrayIcon(contextMenu);
}

void ScreenmateWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.drawPixmap(0, 0, currSprite_);
}

void ScreenmateWidget::mousePressEvent(QMouseEvent *event)
{
    if (!isTrainingMode_) {
        return;
    }

    if (event->button() == Qt::LeftButton) {
        dragPosition_ = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void ScreenmateWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!isTrainingMode_) {
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

void ScreenmateWidget::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Context:
        break;
    case QSystemTrayIcon::DoubleClick:
        QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(0);
        trayIcon_->showMessage("title", "msg", icon, 1000);
        break;
    }
}

void ScreenmateWidget::onGarlandAct()
{
    qDebug() << Q_FUNC_INFO;

}

void ScreenmateWidget::onStarsAct()
{
    qDebug() << Q_FUNC_INFO;
}

void ScreenmateWidget::onAnimationAct()
{
    qDebug() << Q_FUNC_INFO;
}

void ScreenmateWidget::onTransparentActSelected(int level)
{
    qDebug() << level;
}

void ScreenmateWidget::onExitAct()
{
    qApp->exit();
}

void ScreenmateWidget::readSettings(const QString &filename)
{
    QSettings settings(filename, QSettings::IniFormat);
    isTrainingMode_ = settings.value("General/isTrainingMode", true).toBool();

    moveSpeed_ = settings.value("Sprite/moveSpeed", 15).toInt();
    drawSpeed_ = settings.value("Sprite/drawSpeed", 150).toInt();

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

void ScreenmateWidget::createTrayIcon(QMenu *contextMenu)
{
    trayIcon_ = new QSystemTrayIcon(this);
    connect(trayIcon_, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            SLOT(onTrayIconActivated(QSystemTrayIcon::ActivationReason)));
    QIcon icon = QIcon::fromTheme("edit-undo");
    trayIcon_->setIcon(icon);
    trayIcon_->setToolTip(QApplication::applicationName());
    trayIcon_->setContextMenu(contextMenu);
    trayIcon_->setVisible(true);
}

QMenu *ScreenmateWidget::createTrayIconMenu()
{
    QMenu *optionsMenu = new QMenu(tr("&Options"), this);
    optionsMenu->addAction("&Garland", this, SLOT(onGarlandAct()));
    optionsMenu->addAction("&Stars", this, SLOT(onStarsAct()));
    optionsMenu->addAction("&Animation", this, SLOT(onAnimationAct()));

    Q_FOREACH(QAction *action, optionsMenu->actions()) {
        action->setCheckable(true);
    }

    signalMapper_ = new QSignalMapper(this);
    QMenu *transparentMenu = new QMenu(tr("&Transparent"), this);
    for (int i = 0; i <= 100; i+=10) {
        QAction *transparentAct = new QAction(QString("%0%").arg(i), this);
        transparentMenu->addAction(transparentAct);
        connect(transparentAct, SIGNAL(triggered()), signalMapper_, SLOT(map()));
        signalMapper_->setMapping(transparentAct, i);
    }
    connect(signalMapper_, SIGNAL(mapped(int)), SLOT(onTransparentActSelected(int)));

    QAction *exitAct = new QAction(tr("&Exit"), this);
    connect(exitAct, SIGNAL(triggered()), SLOT(onExitAct()));

    QMenu *trayIconMenu_ = new QMenu(this);
    trayIconMenu_->addMenu(optionsMenu);
    trayIconMenu_->addMenu(transparentMenu);
    trayIconMenu_->addSeparator();
    trayIconMenu_->addAction(exitAct);

    return trayIconMenu_;
}
