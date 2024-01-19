#pragma once

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>

namespace Custom_Control
{
    class ColorSpy :public QWidget {
        Q_OBJECT
    public:
        explicit ColorSpy(QWidget *parent = nullptr);
        ~ColorSpy() override;

        QColor GetColor();

        void StartTimer();
        void StopTimer();

    signals:
        void sig_pickerColor(QColor color);
        void sig_timerPickerColor(QColor color);

    private slots:
        void slot_showColorValue();
    private:
        void init();
        void initUI();
        void init_connection();

        void uinit_connection();

    protected:
        void showEvent(QShowEvent *event) override;
        void hideEvent(QHideEvent *event) override;
        bool eventFilter(QObject *watched, QEvent *event) override;

    private:
        QTimer *m_timer_ { nullptr };

        QHBoxLayout *m_hlayout_ { nullptr };

        QGridLayout *m_grid_layout_ { nullptr };

        QLabel m_show_lab_;
        QLabel m_hex_lab_ { "hex" };
        QLabel m_rgb_lab_ { "rgb" };
        QLabel m_position_lab_ { "position" };

        QLineEdit *m_hex_edit_ { nullptr };
        QLineEdit *m_rgb_edit_ { nullptr };
        QLineEdit *m_position_edit_ { nullptr };

        QColor m_color_ { "#FFFFFF" };

    };
}
