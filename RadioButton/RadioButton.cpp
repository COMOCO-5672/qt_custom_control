#include "RadioButton.h"

#include <QPainter>

namespace Custom_Control
{
    RadioButton::RadioButton(QWidget *parent) :QRadioButton(parent)
    {

    }

    RadioButton::~RadioButton()
    {

    }

    void RadioButton::SetBackgroundColor(const QColor &color)
    {
        m_background_color_ = color;
    }

    QColor RadioButton::GetBackgroundColor()
    {
        return m_background_color_;
    }

    void RadioButton::SetForegroundColor(QColor &color)
    {
        m_foreground_color_ = color;
    }

    QColor RadioButton::GetForegroundColor()
    {
        return m_foreground_color_;
    }

    void RadioButton::SetBorderColor(QColor &color)
    {
        m_border_color_ = color;
    }

    QColor RadioButton::GetBorderColor()
    {
        return m_border_color_;
    }

    void RadioButton::SetThickness(int thickness)
    {
        m_thickness_ = thickness;
    }

    int RadioButton::GetThickness() const
    {
        return m_thickness_;
    }

    void RadioButton::SetRadius(int radius)
    {
        m_border_radius_ = radius;
    }

    int RadioButton::GetRadius() const
    {
        return m_border_radius_;
    }

    void RadioButton::paintEvent(QPaintEvent *event)
    {
        Q_UNUSED(event);
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        // TODO:Style control through files
        painter.setBrush(m_background_color_);
        
        // hide border
        painter.drawRoundedRect(0, 0, this->width(), this->height(), m_border_radius_, m_border_radius_);

        if (this->isChecked()) {

            painter.setPen(QPen(m_foreground_color_, m_thickness_));
            const int cal_width = this->width();
            const int cal_height = this->height();

            QPointF p[3] = { QPointF(cal_width * 0.2, cal_height * 0.5)
                , QPointF(cal_width * 0.4,cal_height * 0.7)
            , QPointF(cal_width * 0.8, cal_height * 0.3) };
            painter.drawPolyline(p, 3);
        }

    }

}
