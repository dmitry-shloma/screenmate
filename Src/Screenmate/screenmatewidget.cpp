#include "screenmatewidget.h"
#include "ui_screenmatewidget.h"

#include <QPainter>
#include <QTime>
#include <QDir>
#include <QFile>
#include <QTextStream>

ScreenmateWidget::ScreenmateWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScreenmateWidget)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    QString fileName = QString("%1%2%3.conf")
            .arg(QApplication::applicationDirPath())
            .arg(QDir::separator())
            .arg(QApplication::applicationName());
    initSettings(fileName);

    // TODO: реализовать чтение из conf файла имени персонажа (если встроенный)
    // или пути к его спрайтам (если внешний)
    // character=Bat
    // Рассмотреть возможность использования QMetaEnum для сравнения считанного значения
    // с существующим enum

    QString str = settings_->value("Sprite/character").toString();
    if (QString::compare(str, "Bat", Qt::CaseInsensitive) != 0) {
        initSprites(Character::External);
    } else {
        initSprites();
    }

    // TODO: реализовать чтение из conf файла режима конструктирования траектории
    // trajectoryConstructMode=Predefined
    // Рассмотреть возможность использования QMetaEnum для сравнения считанного значения
    // с существующим enum

    constructTrajectory();

    isTraining_ = settings_->value("Training/isTraining", true).toBool();

    int moveSpeed = settings_->value("Sprite/moveSpeed", 15).toInt();
    int drawSpeed = settings_->value("Sprite/drawSpeed", 250).toInt();

    if (!isTraining_) {
        onTimerMove();
        timerMove_ = new QTimer(this);
        connect(timerMove_, SIGNAL(timeout()), this, SLOT(onTimerMove()));
        timerMove_->start(moveSpeed);
    }

    onTimerDraw();
    timerDraw_ = new QTimer(this);
    connect(timerDraw_, SIGNAL(timeout()), this, SLOT(onTimerDraw()));
    timerDraw_->start(drawSpeed);
}

ScreenmateWidget::~ScreenmateWidget()
{
    delete timerMove_;
    delete timerDraw_;
    delete settings_;

    delete ui;
}

void ScreenmateWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.drawPixmap(0, 0, currSprite_);
}

void ScreenmateWidget::mousePressEvent(QMouseEvent *event)
{
    setFocus();

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
    QBitmap currMask = sprites_.at(i).second;
    setMask(currMask);

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

void ScreenmateWidget::initSettings(const QString &fileName)
{
    settings_ = new QSettings(fileName, QSettings::IniFormat);
}

void ScreenmateWidget::constructTrajectory(ConstructionMode mode)
{
    // if ConstructionMode is BasedOnFixedPoints or Random
    qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));
    //

    QString pointsRaw;
    QStringList points;

    switch (mode) {
    case Predefined:
        pointsRaw = settings_->value("Trajectory/predefinedPoints").toString();
        points = pointsRaw.split(";", QString::SkipEmptyParts);

        break;

    case BasedOnFixedPoints:
        pointsRaw = settings_->value("Trajectory/fixedPoints").toString();

        // TODO: need realization

        break;

    case Random:
        // TODO: need realization
        // NOTE: реализовать траекторию, составленную из случайных точек

        break;
    }

    // construct trajectory
    Q_FOREACH(QString point, points) {
        int x = point.section(',', 0, 0).toInt();
        int y = point.section(',', 1, 1).toInt();

        trajectory_.append(QPoint(x, y));
    }
    //
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

int ScreenmateWidget::qrand(int low, int high)
{
    int value = low + ::qrand() % ((high + 1) - low);
    return value;
}

void ScreenmateWidget::initSprites(Character character)
{
    QPixmap pixmap;
    int spriteWidth;
    int spriteHeigth;
    int spriteCount;

    switch (character) {
    case Bat:
        pixmap = QPixmap("://characters/bat.png");
        spriteCount = 5;
        spriteWidth = pixmap.width() / spriteCount;
        spriteHeigth = pixmap.height();
        break;

    case External:
        pixmap = QPixmap(settings_->value("Sprite/character").toString() + ".png");
        spriteCount = settings_->value("Sprite/count").toInt();
        spriteWidth = pixmap.width() / spriteCount;
        spriteHeigth = pixmap.height();
        break;
    }

    for (int i = 0; i < pixmap.width(); i += spriteWidth) {
        QPixmap sprite = pixmap.copy(i, 0, spriteWidth, spriteHeigth);
        QBitmap mask = sprite.createHeuristicMask();
        sprites_.append(QPair<QPixmap, QBitmap>(sprite, mask));
    }
}
