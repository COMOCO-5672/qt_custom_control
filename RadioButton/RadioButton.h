#pragma once

#include <QRadioButton>
#include "Logger.h"

namespace Custom_Control
{
    class RadioButton : public QRadioButton
    {
        Q_OBJECT
    public:
        explicit RadioButton(QWidget *parent = nullptr);
        ~RadioButton() override;

        void SetBackgroundColor(const QColor &color);
        QColor GetBackgroundColor();

        void SetForegroundColor(QColor &color);
        QColor GetForegroundColor();

        void SetBorderColor(QColor &color);
        QColor GetBorderColor();

        void SetThickness(int thickness);
        int GetThickness() const;

        void SetRadius(int radius);
        int GetRadius() const;

    protected:
        void paintEvent(QPaintEvent *event) override;

    private:
        QColor m_background_color_ { "#000000" };
        QColor m_foreground_color_ { "#FFFFFF" };
        QColor m_border_color_ { "#FFFFFF" };

        int m_thickness_ = 2;
        int m_border_radius_ = 3;

        DEFINE_LOGGER("Radiobutton");
    };
}
