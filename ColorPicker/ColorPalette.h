#pragma once

#include <QWidget>
#include <QColor>
#include <QSlider>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>

#include "HDBasePushButton.h"

namespace Custom_Control
{
    class ColorHueBar : public QWidget
    {
        Q_OBJECT

    public:
        explicit ColorHueBar(QWidget *parent = nullptr);
        ~ColorHueBar() override;

        void SetValue(int val) const;
        int Value() const;

    signals:
        void sig_valueChanged(int val);

    private:
        QSlider *m_slider_ { nullptr };
        QHBoxLayout *m_main_hloayout_ { nullptr };
    };

    class ColorSVCanvas : public QWidget
    {
        Q_OBJECT
    public:
        explicit ColorSVCanvas(QWidget *parent = nullptr);
        ~ColorSVCanvas() override;

        bool SetHue(int hue);
        bool SetSaturationValue(int saturation, int value);
        bool SetSaturationValue(QPoint saturationValue);

        QColor Color();
        QRect AvailabilityRect() const;
        int Margin() const;

    signals:
        void sig_colorChanged(const QColor &color);
        void sig_doubleClick();

    protected:
        void paintEvent(QPaintEvent *ev) Q_DECL_OVERRIDE;
        void resizeEvent(QResizeEvent *ev) Q_DECL_OVERRIDE;
        bool eventFilter(QObject *obj, QEvent *ev) Q_DECL_OVERRIDE;

    private:
        QPoint valueFromPos(QPoint &pos) const;
        QPoint posFromValue(QPoint &val) const;

    private:
        int m_margin_;
        int m_radius_;
        int m_saturation_max_;
        int m_value_max_;

        int m_hue_;
        QPoint m_pos_ = { 0 ,0 };
    };

    class ColorChecker : public QWidget
    {
        Q_OBJECT

    public:
        explicit ColorChecker(QWidget *parent = nullptr);
        ~ColorChecker() override;

    protected:
        void paintEvent(QPaintEvent *ev) Q_DECL_OVERRIDE;

    private:
        int m_checker_size_;
    };

    class ColorAlphaBar : public QWidget
    {
        Q_OBJECT
    public:
        explicit ColorAlphaBar(QWidget *parent = nullptr);
        ~ColorAlphaBar() override;

        void SetColor(QColor color);
        QColor Color() const;

        void SetValue(int alpha) const;

    signals:
        void sig_colorChanged(const QColor &color);

    protected:
        void resizeEvent(QResizeEvent *ev) Q_DECL_OVERRIDE;

    private:
        ColorChecker *m_checker_ { nullptr };
        QSlider *m_slider_ { nullptr };
        QColor m_color_;
        QVBoxLayout *m_v_box_layout_ { nullptr };
        int m_groove_height_;
    };

    class ColorWorkbench : public QDialog
    {
        Q_OBJECT
    public:
        explicit ColorWorkbench(QWidget *parent = nullptr);
        ~ColorWorkbench() override;

        void SetColor(QColor color) const;

        QColor GetColor() const;

    signals:
        void sig_colorChanged(const QColor &color);

        void sig_confirmed(const QColor &color);
        void sig_canceled();

        void sig_hover(bool is_hover);

    private:
        QColor colorFromStr(QString str);
        void setPreviewColor(const QColor& color);
        void init();
        void initUI();
        void init_connection();

    protected:
        void resizeEvent(QResizeEvent* event) override;
        bool eventFilter(QObject* watched, QEvent* event) override;

    private slots:
        void slot_colorDisplay(const QColor &color);
        void slot_colorEdit(const QString &text);

    private:
        ColorSVCanvas *m_canvas_ { nullptr };
        ColorHueBar *m_hsv_bar_ { nullptr };
        ColorAlphaBar *m_alpha_slider_ { nullptr };
        ColorChecker *m_checker_ { nullptr };

        QLineEdit *m_line_edit_ { nullptr };
        HDAbsPushButton *m_cancel_btn_ { nullptr };
        HDAbsPushButton *m_confirm_btn_ { nullptr };

        QHBoxLayout *m_handle_layout_ { nullptr };
        QHBoxLayout *m_huebar_layout_ { nullptr };
        QVBoxLayout *m_adjust_vlayout_ { nullptr };
        QGridLayout *m_main_layout_ { nullptr };

        QPushButton *m_preview_show_btn_ { nullptr };
    };

    class ColorPalette : public QLabel
    {
        Q_OBJECT
    public:
        explicit ColorPalette(QWidget *parent = nullptr);
        ~ColorPalette() override;

    protected:
        void resizeEvent(QResizeEvent *ev) Q_DECL_OVERRIDE;

    private:
        void setColor(const QColor &color);

    private slots:
        void slot_showPopup();
        void slot_colorChanged(const QColor &color);

    private:
        QPushButton *m_button_ { nullptr };
        ColorChecker *m_checker_ { nullptr };
        ColorWorkbench *m_popup_ { nullptr };

        QColor m_cur_color_;
        QColor m_ori_color_;
        QHBoxLayout *m_main_layout_ { nullptr };
    };
}
