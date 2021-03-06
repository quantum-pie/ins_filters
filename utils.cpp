/*
 * utils.cpp
 *
 *      Author: Ermakov_P
 */
#include "utils.h"
#include "packets.h"
#include "qcustomplot.h"

#include <cmath>
#include <limits>

#include <QString>
#include <QDateTime>

namespace utils
{

//! Multiplier to convert speed units from m/s to knots.
static const double ms2knots { 1.94384 };

//! How many degrees in PI radians.
static const double degrees_per_pi { 180.0 };

//! Multiplier to convert floating-point value into 16.16 fixed-point value.
static const uint16_t fixed16d16_mult { std::numeric_limits<uint16_t>::max() };

double radians_to_degrees(double radians)
{
    return radians / M_PI * degrees_per_pi;
}

double degrees_to_radians(double degrees)
{
    return degrees / degrees_per_pi * M_PI;
}

double fix_angle(double radians)
{
	if(radians >= M_PI)
	{
		return radians - 2 * M_PI;
	}
	else if(radians < - M_PI)
	{
		return radians + 2 * M_PI;
	}
	else
	{
		return radians;
	}
}

int32_t float_to_fixed(float val)
{
    return static_cast<int32_t>(val * fixed16d16_mult);
}

int32_t double_to_fixed(double val)
{
    return static_cast<int32_t>(val * fixed16d16_mult);
}

int32_t angle_to_fixed(double radians)
{
    return static_cast<int32_t>(radians * fixed16d16_mult / M_PI);
}

double fixed_to_double(int32_t fixed)
{
    return static_cast<double>(fixed) / fixed16d16_mult;
}

double fixed_to_angle(int32_t fixed)
{
    return static_cast<double>(fixed) * M_PI / fixed16d16_mult;
}

double ms_to_knots(double ms)
{
	return ms * ms2knots;
}

QString double_view(double val, std::size_t digits)
{
    return QString::number(val, 'f', digits);
}

QString gps_time_string(const Timestamp & ts)
{
    QDateTime dt;
    dt.setDate(QDate(ts.year, ts.month, ts.day));
    dt.setTime(QTime(ts.hour, ts.minute, ts.second, ts.ssecond * 10));
    return dt.toString(Qt::DefaultLocaleShortDate);
}

std::string time_string(const Timestamp & ts)
{
    std::ostringstream oss;
    oss << static_cast<unsigned>(ts.day) << "-" <<
            static_cast<unsigned>(ts.month) << "-" <<
            ts.year << "_" <<
            static_cast<unsigned>(ts.hour) << "-" <<
            static_cast<unsigned>(ts.minute) << "-" <<
            static_cast<unsigned>(ts.second);

    return oss.str();
}

}
