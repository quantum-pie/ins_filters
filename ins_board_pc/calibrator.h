#ifndef CALIBRATOR_H
#define CALIBRATOR_H

#include <QVector3D>

class Calibrator
{
public:
    Calibrator();
    void reset();
    void update(double x, double y, double z);
    void calibrate(double & x, double & y, double & z);
    QVector3D calibrate(const QVector3D & vec);
    void save();

    double get_x_bias();
    double get_y_bias();
    double get_z_bias();
    double get_x_scale();
    double get_y_scale();
    double get_z_scale();

private:
    void update_borders(double & lower, double & upper, double val);
    void load();

    double x_min, x_max, y_min, y_max, z_min, z_max;
    double x_bias, y_bias, z_bias, x_scale, y_scale, z_scale;
};

#endif // CALIBRATOR_H
