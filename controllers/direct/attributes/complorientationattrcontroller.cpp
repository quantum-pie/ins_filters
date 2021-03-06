#include "controllers/direct/attributes/complorientationattrcontroller.h"

#include <QLineEdit>

ComplOrientationAttrController::ComplOrientationAttrController(QLineEdit *static_accel_le, QLineEdit *static_magn_le,
                                                               QLineEdit *bias_gain_le)
    : static_accel_le{ static_accel_le }, static_magn_le{ static_magn_le }, bias_gain_le{ bias_gain_le }
{
    connect(static_accel_le, SIGNAL(textEdited(QString)), this, SLOT(on_static_accel_gain(QString)));
    connect(static_magn_le, SIGNAL(textEdited(QString)), this, SLOT(on_static_magn_gain(QString)));
    connect(bias_gain_le, SIGNAL(textEdited(QString)), this, SLOT(on_bias_gain(QString)));
}

void ComplOrientationAttrController::on_static_accel_gain(const QString &str)
{
    call_setter(str, &IComplementOrientationAttr::set_static_accel_gain);
}

void ComplOrientationAttrController::on_static_magn_gain(const QString &str)
{
    call_setter(str, &IComplementOrientationAttr::set_static_magn_gain);
}

void ComplOrientationAttrController::on_bias_gain(const QString &str)
{
    call_setter(str, &IComplementOrientationAttr::set_bias_gain);
}

void ComplOrientationAttrController::apply_attributes()
{
    on_static_accel_gain(static_accel_le->text());
    on_static_magn_gain(static_magn_le->text());
    on_bias_gain(bias_gain_le->text());
}

void ComplOrientationAttrController::borrow_attributes()
{
    static_accel_le->setText(QString::number(get_model()->get_static_accel_gain()));
    static_magn_le->setText(QString::number(get_model()->get_static_magn_gain()));
    bias_gain_le->setText(QString::number(get_model()->get_bias_gain()));
}
