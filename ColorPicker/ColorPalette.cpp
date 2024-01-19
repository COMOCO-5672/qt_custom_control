#include "ColorPalette.h"
#include <QPushButton>
#include <QPainter>
#include <QPaintEvent>
#include <QRegularExpression>
#include "HDBasePushButton.h"

namespace Custom_Control
{

    ColorHueBar::ColorHueBar(QWidget *parent)
        : QWidget(parent)
    {
        m_slider_ = new QSlider(this);
        m_slider_->setMaximum(359);
        m_slider_->setFixedHeight(16);
        m_slider_->setStyleSheet("QSlider::groove:horizontal {height: 12px; \
                                background-color: qlineargradient(spread:pad, x1:1, y1:0, x2:0, y2:0, \
                                    stop:0 hsv(0,255,255), \
                                    stop:0.17 hsv(59,255,255), \
                                    stop:0.33 hsv(119,255,255), \
                                    stop:0.5 hsv(179,255,255), \
                                    stop:0.67 hsv(239,255,255), \
                                    stop:0.83 hsv(299,255,255), \
                                    stop:1 hsv(359,255,255));} \
                            QSlider::handle:horizontal { \
                                background: white; \
                                width: 4px; \
                                margin:-2px 0px; \
                                border: 1px solid grey; \
                                border-radius: 2px;} \
                            QSlider::add-page:horizontal {background: transparent;} \
                            QSlider::sub-page:horizontal {background: transparent;}");

        m_slider_->setOrientation(Qt::Orientation::Horizontal);

        SetValue(m_slider_->maximum());
        connect(m_slider_, &QSlider::valueChanged, this, [this] {
            emit sig_valueChanged(Value());
            });

        m_main_hloayout_ = new QHBoxLayout(this);
        m_main_hloayout_->setContentsMargins(0, 0, 0, 0);
        m_main_hloayout_->setSpacing(0);
        m_main_hloayout_->addWidget(m_slider_);
    }

    ColorHueBar::~ColorHueBar()
    {
        if (m_slider_) {
            m_slider_->disconnect();
            m_slider_->deleteLater();
        }

        if (m_main_hloayout_) {
            m_main_hloayout_->deleteLater();
        }

    }

    void ColorHueBar::SetValue(int val) const
    {
        m_slider_->setValue(qAbs(val - m_slider_->maximum()));
    }

    int ColorHueBar::Value() const
    {
        return qAbs(m_slider_->value() - m_slider_->maximum());
    }

    ColorSVCanvas::ColorSVCanvas(QWidget *parent)
        : QWidget(parent)
        , m_margin_(5)
        , m_radius_(m_margin_ - 1)
        , m_saturation_max_(255)
        , m_value_max_(255)
        , m_hue_(0)
        , m_pos_(QPoint(-1, -1))
    {
        installEventFilter(this);
    }

    ColorSVCanvas::~ColorSVCanvas()
    {
        removeEventFilter(this);
    }

    bool ColorSVCanvas::SetHue(int hue)
    {
        if (hue < 0 || hue > 359)
            return false;

        m_hue_ = hue;

        update();
        emit sig_colorChanged(Color());

        return true;
    }

    bool ColorSVCanvas::SetSaturationValue(int saturation, int value)
    {
        return SetSaturationValue(QPoint(saturation, value));
    }

    bool ColorSVCanvas::SetSaturationValue(QPoint saturationValue)
    {
        if (!QRect(0, 0, 256, 256).contains(saturationValue))
            return false;

        m_pos_ = posFromValue(saturationValue);

        update();
        emit sig_colorChanged(Color());

        return true;
    }

    QColor ColorSVCanvas::Color()
    {
        const QPoint tmpVal = valueFromPos(m_pos_);
        QColor tmpColor;
        tmpColor.setHsv(m_hue_, tmpVal.x(), tmpVal.y());
        return tmpColor;
    }

    QRect ColorSVCanvas::AvailabilityRect() const
    {
        const QRect tmpRect(m_margin_, m_margin_, width() - m_margin_ * 2, height() - m_margin_ * 2);
        return tmpRect;
    }

    int ColorSVCanvas::Margin() const
    {
        return m_margin_;
    }

    void ColorSVCanvas::paintEvent(QPaintEvent *)
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        const QRect rect = AvailabilityRect();

        // 绘制从左到右RGB(255,255,255)到HSV(h,255,255)的渐变
        QLinearGradient linearGradientH(rect.topLeft(), rect.topRight());
        linearGradientH.setColorAt(0, Qt::white);
        QColor color;
        color.setHsv(m_hue_, m_saturation_max_, m_value_max_);
        linearGradientH.setColorAt(1, color);
        painter.fillRect(rect, linearGradientH);

        // 绘制顶部颜色值为RGBA(0,0,0,0)到最底部RGBA(0,0,0,255)的渐变
        QLinearGradient linearGradientV(rect.topLeft(), rect.bottomLeft());
        linearGradientV.setColorAt(0, QColor(0, 0, 0, 0));
        linearGradientV.setColorAt(1, QColor(0, 0, 0, 255));
        painter.fillRect(rect, linearGradientV);

        painter.setPen(QColor(Qt::darkGray));
        painter.drawEllipse(m_pos_, m_radius_, m_radius_);
    }

    void ColorSVCanvas::resizeEvent(QResizeEvent *)
    {
        if (m_pos_ == QPoint(-1, -1)) {
            SetSaturationValue(255, 255);
        }
    }

    bool ColorSVCanvas::eventFilter(QObject *obj, QEvent *ev)
    {
        if (obj == this) {
            if (ev->type() == QEvent::MouseButtonPress || ev->type() == QEvent::MouseButtonDblClick
                || ev->type() == QEvent::MouseButtonRelease) {
                if (AvailabilityRect().contains(mapFromGlobal(QCursor::pos()))) {
                    m_pos_ = mapFromGlobal(QCursor::pos());
                    update();
                    emit sig_colorChanged(Color());

                    if (ev->type() == QEvent::MouseButtonDblClick)
                        emit sig_doubleClick();

                    return true;
                }
            }
        }
        return QWidget::eventFilter(obj, ev);
    }

    QPoint ColorSVCanvas::valueFromPos(QPoint &pos) const
    {
        const QRect tmp_rect = AvailabilityRect();

        const QPoint tmp_pos = pos - tmp_rect.topLeft();
        const int saturation = tmp_pos.x() * m_saturation_max_ / tmp_rect.width();
        const int value = qAbs(tmp_pos.y() * m_value_max_ / tmp_rect.height() - m_value_max_);

        return QPoint(saturation, value);
    }

    QPoint ColorSVCanvas::posFromValue(QPoint &val) const
    {
        const QRect tmp_rect = AvailabilityRect();

        const int tmp_x = val.x() * tmp_rect.width() / m_saturation_max_;
        const int tmp_y = qAbs(val.y() - m_value_max_) * tmp_rect.height() / m_value_max_;

        return QPoint(tmp_x, tmp_y) + tmp_rect.topLeft();
    }

    ColorChecker::ColorChecker(QWidget *parent)
        : QWidget(parent)
        , m_checker_size_(6)
    {

    }

    ColorChecker::~ColorChecker()
    {

    }

    void ColorChecker::paintEvent(QPaintEvent *ev)
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        const int row_count = ev->rect().height() / m_checker_size_ + 1;
        const int col_count = ev->rect().width() / m_checker_size_ + 1;

        for (int i = 0; i < row_count; i++) {
            for (int j = 0; j < col_count; j++) {
                QRect checker_rect(m_checker_size_ * j, m_checker_size_ * i, m_checker_size_, m_checker_size_);

                QColor checker_color;
                if (((j + i % 2) % 2) == 0) {
                    checker_color = Qt::darkGray;
                }
                else {
                    checker_color = Qt::white;
                }
                painter.fillRect(checker_rect, checker_color);
            }
        }
    }

    ColorAlphaBar::ColorAlphaBar(QWidget *parent)
        : QWidget(parent)
        , m_groove_height_(12)
    {
        m_checker_ = new ColorChecker(this);

        m_slider_ = new QSlider(Qt::Horizontal, this);
        m_slider_->setMaximum(255);
        m_slider_->setValue(m_slider_->maximum());
        m_slider_->setFixedHeight(16);
        SetColor(Qt::red);

        connect(m_slider_, &QSlider::valueChanged, this, [this] {
            emit sig_colorChanged(Color());
            });

        m_v_box_layout_ = new (std::nothrow)QVBoxLayout(this);
        m_v_box_layout_->setContentsMargins(0, 0, 0, 0 );
        m_v_box_layout_->setSpacing(0);
        m_v_box_layout_->addWidget(m_slider_);
    }

    ColorAlphaBar::~ColorAlphaBar()
    {
        if (m_checker_) {
            m_checker_->deleteLater();
        }

        if (m_slider_) {
            m_slider_->disconnect();
            m_slider_->deleteLater();
        }

        if (m_v_box_layout_) {
            m_v_box_layout_->deleteLater();
        }

    }

    void ColorAlphaBar::SetColor(QColor ori_color)
    {
        m_color_ = ori_color;
        m_color_.setAlpha(255);

        QColor tmp_color(ori_color);
        tmp_color.setAlpha(0);

        m_slider_->setStyleSheet(QString("QSlider::groove:horizontal { \
                                        height: %3px; \
                                        background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, \
                                        stop:0 %1, \
                                        stop:1 %2);} \
                                    QSlider::handle:horizontal { \
                                        background: white; \
                                        width: 4px; \
                                        margin: -2px 0; \
                                        border: 1px solid grey; \
                                        border-radius: 2px;} \
                                    QSlider::add-page:horizontal {background: transparent;} \
                                    QSlider::sub-page:horizontal {background: transparent;}")
            .arg(tmp_color.name(QColor::HexArgb))
            .arg(m_color_.name())
            .arg(m_groove_height_));

        emit sig_colorChanged(Color());
    }

    QColor ColorAlphaBar::Color() const
    {
        QColor tmpColor(m_color_);
        tmpColor.setAlpha(m_slider_->value());
        return tmpColor;
    }

    void ColorAlphaBar::SetValue(int alpha) const
    {
        m_slider_->setValue(alpha);
    }

    void ColorAlphaBar::resizeEvent(QResizeEvent *)
    {
        m_checker_->setGeometry(0, (height() - m_groove_height_) / 2, width(), m_groove_height_);
    }

    ColorWorkbench::ColorWorkbench(QWidget *parent)
        : QDialog(parent, Qt::Popup)
    {
        init();
    }

    ColorWorkbench::~ColorWorkbench()
    {
        if (m_confirm_btn_) {
            m_confirm_btn_->disconnect();
            m_confirm_btn_->deleteLater();
        }

        if (m_hsv_bar_) {
            m_hsv_bar_->disconnect();
            m_hsv_bar_->deleteLater();
        }

        if (m_canvas_) {
            m_canvas_->disconnect();
            m_canvas_->deleteLater();
        }

        if (m_alpha_slider_) {
            m_alpha_slider_->disconnect();
            m_alpha_slider_->deleteLater();
        }

        if (m_line_edit_) {
            m_line_edit_->disconnect();
            m_line_edit_->deleteLater();
        }

        if (m_cancel_btn_) {
            m_cancel_btn_->disconnect();
            m_cancel_btn_->deleteLater();
        }

        if (m_handle_layout_)
            m_handle_layout_->deleteLater();

        if (m_main_layout_)
            m_main_layout_->deleteLater();

        if (m_preview_show_btn_)
            m_preview_show_btn_->deleteLater();

        if (m_checker_)
            m_checker_->deleteLater();

    }

    void ColorWorkbench::init()
    {
        initUI();
        init_connection();
    }

    void ColorWorkbench::initUI()
    {
        setFixedSize(320, 280);
        setAttribute(Qt::WA_StyledBackground);
        setAttribute(Qt::WA_Hover);
        setObjectName("workbench");
        setStyleSheet("#workbench{background-color:white; border:1px solid rgb(245,245,245); border-radius: 6px;}");

        m_preview_show_btn_ = new QPushButton();
        m_preview_show_btn_->setFixedSize(QSize(32, 32));

        m_checker_ = new ColorChecker(this);

        m_canvas_ = new ColorSVCanvas(this);
        m_canvas_->setFixedSize(300, 180);

        m_hsv_bar_ = new ColorHueBar();
        m_hsv_bar_->setFixedWidth(m_canvas_->AvailabilityRect().width() - 70);

        m_alpha_slider_ = new ColorAlphaBar(this);
        m_alpha_slider_->setFixedWidth(m_canvas_->AvailabilityRect().width() - 70);

        m_line_edit_ = new QLineEdit(this);

        m_cancel_btn_ = new HDBasePushButton(this);
        m_cancel_btn_->setObjectName("cancel");
        m_cancel_btn_->setText(tr("cancel"));
        m_cancel_btn_->setStyleSheet("#cancel{background:#b5b7be;color:#FFFFFF; border-radius: 4px;}");

        m_confirm_btn_ = new HDBasePushButton(this);
        m_confirm_btn_->setObjectName("confirm");
        m_confirm_btn_->setText(tr("confirm"));
        m_confirm_btn_->setStyleSheet("#confirm{background:#FF842F; color:#FFFFFF; border-radius: 4px;}");
        // 布局
        m_handle_layout_ = new QHBoxLayout;
        m_handle_layout_->addSpacing(m_canvas_->Margin());
        m_handle_layout_->addWidget(m_line_edit_);
        m_handle_layout_->addStretch();
        m_handle_layout_->addWidget(m_cancel_btn_);
        m_handle_layout_->addWidget(m_confirm_btn_);

        m_main_layout_ = new QGridLayout(this);
        m_main_layout_->addWidget(m_canvas_, 0, 0);

        m_huebar_layout_ = new QHBoxLayout;
        m_huebar_layout_->setContentsMargins(0, 0, 0, 0);
        m_huebar_layout_->addSpacing(0);

        m_adjust_vlayout_ = new QVBoxLayout;
        m_adjust_vlayout_->setContentsMargins(0, 0, 0, 0);
        m_adjust_vlayout_->addSpacing(0);
        m_adjust_vlayout_->addStretch(0);
        m_adjust_vlayout_->addWidget(m_hsv_bar_);
        m_adjust_vlayout_->addWidget(m_alpha_slider_);

        m_huebar_layout_->addLayout(m_adjust_vlayout_);
        m_huebar_layout_->addWidget(m_preview_show_btn_, Qt::AlignCenter);

        m_main_layout_->addLayout(m_huebar_layout_, 1, 0);
        m_main_layout_->addLayout(m_handle_layout_, 2, 0, 1, 2);
    }

    void ColorWorkbench::init_connection()
    {
        connect(m_confirm_btn_, &QPushButton::clicked, this, [this] {
            emit sig_confirmed(m_alpha_slider_->Color());
            });

        connect(m_cancel_btn_, &QPushButton::clicked, this, [this] {
            emit sig_canceled();
            });

        connect(m_hsv_bar_, &ColorHueBar::sig_valueChanged, this, [this](int val) {
            m_canvas_->SetHue(val);
            });
        connect(m_canvas_, &ColorSVCanvas::sig_colorChanged, this, [this](const QColor &color) {
            m_alpha_slider_->SetColor(color);
            });
        connect(m_canvas_, &ColorSVCanvas::sig_doubleClick, this, [this]() {
            emit sig_confirmed(m_alpha_slider_->Color());
            });
        connect(m_alpha_slider_, &ColorAlphaBar::sig_colorChanged, this, &ColorWorkbench::slot_colorDisplay);
        connect(m_line_edit_, &QLineEdit::textEdited, this, &ColorWorkbench::slot_colorEdit);
        this->installEventFilter(this);
    }

    void ColorWorkbench::SetColor(QColor color) const
    {
        m_hsv_bar_->SetValue(color.hsvHue());
        m_canvas_->SetSaturationValue(color.hsvSaturation(), color.value());
        m_alpha_slider_->SetValue(color.alpha());
    }

    QColor ColorWorkbench::GetColor() const
    {
        return m_alpha_slider_->Color();
    }

    QColor ColorWorkbench::colorFromStr(QString str)
    {
        QColor color(str);
        if (!color.isValid()) {
            QString tmpStr = str;
            QRegularExpression rx("[^\\d+^,^.]");
            if (str.contains(rx)) {
                tmpStr.remove(rx);
            }
            if (!tmpStr.isEmpty()) {
                QStringList strList = tmpStr.split(",");
                if (str.contains("rgba") && strList.count() == 4) {
                    color.setRgb(strList.at(0).toInt(), strList.at(1).toInt(), strList.at(2).toInt());
                    if (strList.at(3).toDouble() > 1) {
                        color.setAlpha(strList.at(3).toInt());
                    }
                    else {
                        color.setAlphaF(strList.at(3).toDouble());
                    }
                }
                else if (str.contains("rgb") && strList.count() == 3) {
                    color.setRgb(strList.at(0).toInt(), strList.at(1).toInt(), strList.at(2).toInt());
                }
                else if (str.contains("hsv") && strList.count() == 3) {
                    color.setHsv(strList.at(0).toInt(), strList.at(1).toInt(), strList.at(2).toInt());
                }
            }
        }

        return color;
    }

    void ColorWorkbench::setPreviewColor(const QColor &color)
    {
        if (m_preview_show_btn_) {
            m_preview_show_btn_->setStyleSheet(QString("QPushButton{border:1px solid %1; background-color:%2}")
                .arg(QColor(152, 152, 152).name())
                .arg(color.name(QColor::HexArgb)));
        }
    }

    void ColorWorkbench::resizeEvent(QResizeEvent *event)
    {
        if (m_checker_ && m_preview_show_btn_)
            m_checker_->setGeometry(m_preview_show_btn_->geometry());
    }

    bool ColorWorkbench::eventFilter(QObject *watched, QEvent *event)
    {
        if (watched == this) {
            if (event->type() == QEvent::HoverEnter) {
                this->setCursor(Qt::ArrowCursor);
                emit sig_hover(true);
            }

            if (event->type() == QEvent::HoverLeave)
                emit sig_hover(false);

        }

        if (event->type() == QEvent::Resize)
            return QWidget::eventFilter(watched, event);

        return true;
    }

    void ColorWorkbench::slot_colorDisplay(const QColor &color)
    {
        QRegularExpression reg("(\\.){0,1}0+$");// 去除末尾0
        m_line_edit_->setText(QString("rgba(%1, %2, %3, %4)")
            .arg(color.red())
            .arg(color.green())
            .arg(color.blue())
            .arg(QString::number(color.alphaF(), 'f', 2).replace(reg, "")));
        // set preview color
        setPreviewColor(color);

        emit sig_colorChanged(color);
    }

    void ColorWorkbench::slot_colorEdit(const QString &text)
    {
        const QColor color = colorFromStr(text);
        if (color.isValid()) {
            disconnect(m_alpha_slider_, &ColorAlphaBar::sig_colorChanged, this, &ColorWorkbench::slot_colorDisplay);
            SetColor(color);
            connect(m_alpha_slider_, &ColorAlphaBar::sig_colorChanged, this, &ColorWorkbench::slot_colorDisplay);
            // set preview color
            setPreviewColor(color);

            emit sig_colorChanged(color);
        }
    }

    ColorPalette::ColorPalette(QWidget *parent)
        : QLabel(parent)
    {
        m_checker_ = new ColorChecker(this);

        m_popup_ = new ColorWorkbench(this);
        connect(m_popup_, &ColorWorkbench::sig_colorChanged, this, &ColorPalette::slot_colorChanged);
        connect(m_popup_, &QDialog::finished, this, [this](int result) {
            if (result == QDialog::Accepted) {
                m_ori_color_ = m_cur_color_;
            }
            else {
                setColor(m_ori_color_);
            }
            });

        m_button_ = new QPushButton("v", this);
        m_button_->setFixedSize(30, 30);
        connect(m_button_, &QPushButton::pressed, this, &ColorPalette::slot_showPopup);

        setFixedSize(40, 40);
        setStyleSheet(QString("QLabel{border:1px solid %1; border-radius: 4px; background-color: %2;}")
            .arg(QColor(230, 230, 230).name())
            .arg(QColor(Qt::white).name()));
        setColor(QColor(255, 0, 0, 150));

        m_main_layout_ = new QHBoxLayout(this);
        m_main_layout_->setContentsMargins(0, 0, 0, 0);
        m_main_layout_->addWidget(m_button_);
    }

    ColorPalette::~ColorPalette()
    {
        if (m_popup_) {
            m_popup_->disconnect();
            m_popup_->deleteLater();
        }

        if (m_checker_) {
            m_checker_->disconnect();
            m_checker_->deleteLater();
        }

        if (m_button_) {
            m_button_->disconnect();
            m_button_->deleteLater();
        }

        if (m_main_layout_)
            m_main_layout_->deleteLater();

    }

    void ColorPalette::resizeEvent(QResizeEvent *)
    {
        m_checker_->setGeometry(m_button_->geometry());
    }

    void ColorPalette::setColor(const QColor &color)
    {
        m_cur_color_ = color;
        m_button_->setStyleSheet(QString("QPushButton{border:1px solid %1; background-color:%2}")
            .arg(QColor(152, 152, 152).name())
            .arg(color.name(QColor::HexArgb)));
    }

    void ColorPalette::slot_showPopup()
    {
        m_ori_color_ = m_cur_color_;
        m_popup_->SetColor(m_ori_color_);

        QPoint tmpPos = mapToGlobal(m_button_->geometry().center());
        tmpPos += QPoint(-m_popup_->width() / 2, m_button_->height() / 2 + 5);
        m_popup_->move(tmpPos);

        m_popup_->open();
    }

    void ColorPalette::slot_colorChanged(const QColor &color)
    {
        if (m_popup_->isVisible()) {
            setColor(color);
        }
    }


}

