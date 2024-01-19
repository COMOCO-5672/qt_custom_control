#include "ColorSpy.h"
#include <QTimer>
#include <QScreen>
#include <QApplication>
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
#include <QDesktopWidget>
#else
#include <QScreen>
#endif
#include <QKeyEvent>

namespace Custom_Control
{
    ColorSpy::ColorSpy(QWidget *parent)
        :QWidget(parent)
    {
        this->setWindowFlags(this->windowFlags() | Qt::FramelessWindowHint);
        init();
    }

    ColorSpy::~ColorSpy()
    {
        uinit_connection();
        if (m_timer_)
            m_timer_->deleteLater();
    }

    QColor ColorSpy::GetColor()
    {
        return m_color_;
    }

    void ColorSpy::StartTimer()
    {
        if (m_timer_)
            m_timer_->start(20);

        installEventFilter(this);

    }

    void ColorSpy::StopTimer()
    {
        if (m_timer_)
            m_timer_->stop();

        removeEventFilter(this);
    }

    void ColorSpy::init()
    {
        initUI();
        init_connection();
    }

    void ColorSpy::initUI()
    {
        setObjectName("color_spy");
        setStyleSheet("background-color:white; border:1px solid rgb(0,0,0); border-radius: 6px;");

        if (!m_hlayout_)
            m_hlayout_ = new (std::nothrow)QHBoxLayout();

        m_hlayout_->addWidget(&m_show_lab_);
        m_hlayout_->setContentsMargins(10, 10, 10, 10);
        m_show_lab_.setMinimumSize(QSize(100, 100));
        m_show_lab_.setMaximumSize(QSize(500, 500));
        m_show_lab_.setStyleSheet("border:1px solid rgb(0,0,0)");

        if (!m_grid_layout_)
            m_grid_layout_ = new (std::nothrow)QGridLayout();

        if (!m_hex_edit_)
            m_hex_edit_ = new (std::nothrow)QLineEdit();

        if (!m_rgb_edit_)
            m_rgb_edit_ = new (std::nothrow) QLineEdit();

        if (!m_position_edit_)
            m_position_edit_ = new (std::nothrow) QLineEdit();

        m_grid_layout_->addWidget(&m_hex_lab_, 0, 0);
        m_grid_layout_->addWidget(m_hex_edit_, 1, 0);

        m_grid_layout_->addWidget(&m_rgb_lab_, 0, 1);
        m_grid_layout_->addWidget(m_rgb_edit_, 1, 1);

        m_grid_layout_->addWidget(&m_position_lab_, 0, 2);
        m_grid_layout_->addWidget(m_position_edit_, 1, 2);

        m_hlayout_->addLayout(m_grid_layout_);

        this->setLayout(m_hlayout_);
    }

    void ColorSpy::init_connection()
    {
        if (!m_timer_) {
            m_timer_ = new (std::nothrow) QTimer(this);
            connect(m_timer_, SIGNAL(timeout()), this, SLOT(slot_showColorValue()));
        }
    }

    void ColorSpy::uinit_connection()
    {
        if (m_timer_)
            m_timer_->disconnect();
    }

    bool ColorSpy::eventFilter(QObject *watched, QEvent *event)
    {
        if (event->type() == QEvent::KeyPress) {
            const auto key = static_cast<QKeyEvent *>(event);
            if (key && key->key() == Qt::Key_Escape)
                this->close();
        }

        if (event->type() == QEvent::MouseButtonPress) {
            const auto mouse_key_ev = static_cast<QMouseEvent *>(event);
            if (mouse_key_ev && mouse_key_ev->button() == Qt::LeftButton) {
                emit sig_pickerColor(GetColor());
                this->close();
            }

            if (mouse_key_ev && mouse_key_ev->button() == Qt::RightButton)
                this->close();
        }


        return QWidget::eventFilter(watched, event);
    }

    void ColorSpy::showEvent(QShowEvent *event)
    {
        QWidget::showEvent(event);
        StartTimer();
    }

    void ColorSpy::hideEvent(QHideEvent *event)
    {
        QWidget::hideEvent(event);
        StopTimer();
    }

    void ColorSpy::slot_showColorValue()
    {
        // get mouse position
        const int x = QCursor::pos().x();
        const int y = QCursor::pos().y();

        if (m_position_edit_) {
            m_position_edit_->setText(tr("x:%1 y:%2").arg(x).arg(y));
        }

        QScreen *screen = QApplication::primaryScreen();
        QPixmap pixmap = !screen ? QPixmap() : screen->grabWindow(0, x, y, 2, 2);

        int red, green, blue;
        if (pixmap.isNull())
            return;

        QImage image = pixmap.toImage();

        if (image.isNull())
            return;

        QColor color = image.pixel(0, 0);
        red = color.red();
        green = color.green();
        blue = color.blue();

        const QString hRed = QString::number(red, 16).toUpper();
        const QString hGreen = QString::number(green, 16).toUpper();
        const QString hBlue = QString::number(blue, 16).toUpper();

        if (m_hex_edit_ && m_rgb_edit_) {
            m_hex_edit_->setText(tr("#%1%2%3").arg(hRed).arg(hGreen).arg(hBlue));
            m_rgb_edit_->setText(tr("R:%1 G:%2 B:%3").arg(red).arg(green).arg(blue));
        }

        const int width = m_show_lab_.width();
        const int height = m_show_lab_.height();
        QPixmap labelPix(width, height);
        labelPix.fill(color);
        m_color_ = color;
        m_show_lab_.setPixmap(labelPix);

        emit sig_timerPickerColor(m_color_);
    }


}
