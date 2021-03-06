#include "controllers/direct/attributes/kalmanorientationattrcontroller.h"

#include <QLineEdit>

KalmanOrientationAttrController::KalmanOrientationAttrController(QLineEdit * proc_gyro_le, QLineEdit * proc_gyro_bias_le,
                                                                 QLineEdit * meas_accel_std_le, QLineEdit * meas_magn_std_le,
                                                                 QLineEdit * init_qs_std_le, QLineEdit * init_qx_std_le, QLineEdit * init_qy_std_le, QLineEdit * init_qz_std_le,
                                                                 QLineEdit * init_bias_std_le)
    : proc_gyro_le{ proc_gyro_le }, proc_gyro_bias_le{ proc_gyro_bias_le },
      meas_accel_std_le{ meas_accel_std_le }, meas_magn_std_le{ meas_magn_std_le },
      init_qs_std_le{ init_qs_std_le }, init_qx_std_le{ init_qx_std_le },
      init_qy_std_le{ init_qy_std_le }, init_qz_std_le{ init_qz_std_le },
      init_bias_std_le{ init_bias_std_le }
{
    connect(proc_gyro_le, SIGNAL(textEdited(QString)), this, SLOT(on_proc_gyro_std(QString)));
    connect(proc_gyro_bias_le, SIGNAL(textEdited(QString)), this, SLOT(on_proc_gyro_bias_std(QString)));
    connect(meas_accel_std_le, SIGNAL(textEdited(QString)), this, SLOT(on_meas_accel_std(QString)));
    connect(meas_magn_std_le, SIGNAL(textEdited(QString)), this, SLOT(on_meas_magn_std(QString)));
    connect(init_bias_std_le, SIGNAL(textEdited(QString)), this, SLOT(on_init_bias_std(QString)));
    connect(init_qs_std_le, SIGNAL(textEdited(QString)), this, SLOT(on_init_qs_std(QString)));
    connect(init_qx_std_le, SIGNAL(textEdited(QString)), this, SLOT(on_init_qx_std(QString)));
    connect(init_qy_std_le, SIGNAL(textEdited(QString)), this, SLOT(on_init_qy_std(QString)));
    connect(init_qz_std_le, SIGNAL(textEdited(QString)), this, SLOT(on_init_qz_std(QString)));
}

void KalmanOrientationAttrController::on_proc_gyro_std(const QString & str)
{
    call_setter(str, &IKalmanOrientationAttr::set_proc_gyro_std);
}

void KalmanOrientationAttrController::on_proc_gyro_bias_std(const QString & str)
{
    call_setter(str, &IKalmanOrientationAttr::set_proc_gyro_bias_std);
}

void KalmanOrientationAttrController::on_meas_accel_std(const QString & str)
{
    call_setter(str, &IKalmanOrientationAttr::set_meas_accel_std);
}

void KalmanOrientationAttrController::on_meas_magn_std(const QString & str)
{
    call_setter(str, &IKalmanOrientationAttr::set_meas_magn_std);
}

void KalmanOrientationAttrController::on_init_qs_std(const QString & str)
{
    call_setter(str, &IKalmanOrientationAttr::set_init_qs_std);
}

void KalmanOrientationAttrController::on_init_qx_std(const QString & str)
{
    call_setter(str, &IKalmanOrientationAttr::set_init_qx_std);
}

void KalmanOrientationAttrController::on_init_qy_std(const QString & str)
{
    call_setter(str, &IKalmanOrientationAttr::set_init_qy_std);
}

void KalmanOrientationAttrController::on_init_qz_std(const QString & str)
{
    call_setter(str, &IKalmanOrientationAttr::set_init_qz_std);
}

void KalmanOrientationAttrController::on_init_bias_std(const QString & str)
{
    call_setter(str, &IKalmanOrientationAttr::set_init_bias_std);
}

void KalmanOrientationAttrController::apply_attributes()
{
    on_init_bias_std(init_bias_std_le->text());
    on_init_qs_std(init_qs_std_le->text());
    on_init_qx_std(init_qx_std_le->text());
    on_init_qy_std(init_qy_std_le->text());
    on_init_qz_std(init_qz_std_le->text());
    on_meas_accel_std(meas_accel_std_le->text());
    on_meas_magn_std(meas_magn_std_le->text());
    on_proc_gyro_bias_std(proc_gyro_bias_le->text());
    on_proc_gyro_std(proc_gyro_le->text());
}

void KalmanOrientationAttrController::borrow_attributes()
{
    init_bias_std_le->setText(QString::number(get_model()->get_init_bias_std()));
    init_qs_std_le->setText(QString::number(get_model()->get_init_qs_std()));
    init_qx_std_le->setText(QString::number(get_model()->get_init_qx_std()));
    init_qy_std_le->setText(QString::number(get_model()->get_init_qy_std()));
    init_qz_std_le->setText(QString::number(get_model()->get_init_qz_std()));
    meas_accel_std_le->setText(QString::number(get_model()->get_meas_accel_std()));
    meas_magn_std_le->setText(QString::number(get_model()->get_meas_magn_std()));
    proc_gyro_bias_le->setText(QString::number(get_model()->get_proc_gyro_bias_std()));
    proc_gyro_le->setText(QString::number(get_model()->get_proc_gyro_std()));
}
