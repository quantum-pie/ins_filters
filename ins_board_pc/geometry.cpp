/*
 * geometry.cpp
 *
 *      Author: bigaw
 */

#include "eigenaux.h"
#include "geometry.h"

namespace geom
{

Vector3D great_circle_destination(const Vector3D & geo, double start_bearing, double distance, const Ellipsoid & e)
{
	double delta = distance / e.re;

	double slat = std::sin(geo[0]);
	double clat = std::cos(geo[0]);
	double sdel = std::sin(delta);
	double cdel = std::cos(delta);
	double sbea = std::sin(start_bearing);
	double cbea = std::cos(start_bearing);

	Vector3D geo_dest;
	geo_dest[0] = std::asin(slat * cdel + clat * sdel * cbea);
	geo_dest[1] += std::atan2(sbea * sdel * clat, cdel - slat * std::sin(geo_dest[0]));
	geo_dest[2] = geo[2];

	return geo_dest;
}

Vector3D cartesian_to_geodetic(const Vector3D & pos, const Ellipsoid & e)
{
	Vector3D geo;

    double x = pos[0];
    double y = pos[1];
    double z = pos[2];

    double zp = std::abs(z);
    double w2 = x * x + y * y;
    double w = std::sqrt(w2);
    double r2 = w2 + z * z;
    double r = std::sqrt(r2);

    geo[1] = std::atan2(y, x);

    double s2 = z * z / r2;
    double c2 = w2 / r2;
    double u = e.a2 / r;
    double v = e.a3 - e.a4 / r;

    double s, c, ss;
    if(c2 > 0.3)
    {
        s = zp / r * (1.0 + c2 * (e.a1 + u + s2 * v) / r);
        geo[0] = std::asin(s);
        ss = s * s;
        c = std::sqrt(1.0 - ss);
    }
    else
    {
        c = w / r * (1.0 - s2 * (e.a5 - u - c2 * v) / r);
        geo[0] = std::acos(c);
        ss = 1.0 - c * c;
        s = std::sqrt(ss);
    }

    double g = 1.0 - e.epssq * ss;
    double rg = e.a / std::sqrt(g);
    double rf = e.a6 * rg;

    u = w - rg * c;
    v = zp - rf * s;

    double f = c * u + s * v;
    double m = c * v - s * u;
    double p = m / (rf / g + f);

    geo[0] = geo[0] + p;
    geo[2] = f + m * p / 2.0;

    if(z < 0.0)
    {
        geo[0] *= -1.0;
    }

    return geo;
}

Vector3D geodetic_to_cartesian(const Vector3D & geo, const Ellipsoid & e)
{
	Vector3D pos;

	double lat = geo[0];
	double lon = geo[1];
	double alt = geo[2];

	double clat = std::cos(lat);
	double slat = std::sin(lat);
	double clon = std::cos(lon);
	double slon = std::sin(lon);

	double n = e.a / std::sqrt(1 - e.epssq * slat * slat);

	pos[0] = (n + alt) * clat * clon;
	pos[1] = (n + alt) * clat * slon;
	pos[2] = (n * (1 - e.epssq) + alt) * slat;

	return pos;
}

Vector3D ecef_to_enu(const Vector3D & ecef, const Vector3D & geo)
{
	return geodetic_to_dcm(geo) * ecef;
}

Vector3D enu_to_ecef(const Vector3D & enu, const Vector3D & geo)
{
	return geodetic_to_dcm(geo).transpose() * enu;
}

Matrix3D geodetic_to_dcm(const Vector3D & geo)
{
    double lat, lon;
    lat = geo[0];
    lon = geo[1];

    double clat = std::cos(lat);
    double slat = std::sin(lat);
    double clon = std::cos(lon);
    double slon = std::sin(lon);

    Matrix3D DCM;

    DCM << -slon, clon, 0,
            -clon * slat, -slon * slat, clat,
            clon * clat, slon * clat, slat;

    return DCM;
}

Matrix3D dcm_lat_partial(const Vector3D & geo)
{
    double lat, lon;
    lat = geo[0];
    lon = geo[1];

    double clat = std::cos(lat);
    double slat = std::sin(lat);
    double clon = std::cos(lon);
    double slon = std::sin(lon);

    Matrix3D RES;

    RES << 0, 0, 0,
            -clon * clat, -slon * clat, -slat,
            -clon * slat, -slon * slat, clat;

    return RES;
}

Matrix3D dcm_lon_partial(const Vector3D & geo)
{
    double lat, lon;
    lat = geo[0];
    lon = geo[1];

    double clat = std::cos(lat);
    double slat = std::sin(lat);
    double clon = std::cos(lon);
    double slon = std::sin(lon);

    Matrix3D RES;

    RES << -clon, -slon, 0,
            slon * slat, -clon * slat, 0,
            -slon * clat, clon * clat, 0;

    return RES;
}

Matrix3D dgeo_dpos(const Vector3D & geo, const Ellipsoid & e)
{
    double lat, lon, alt;
    lat = geo[0];
    lon = geo[1];
    alt = geo[2];

    double clat = std::cos(lat);
    double slat = std::sin(lat);
    double clon = std::cos(lon);
    double slon = std::sin(lon);

    double bracket = std::pow(1 - e.epssq * slat * slat, 1.5);
    double norm_rad = e.a / std::sqrt(1 - e.epssq * slat * slat);
    double common_mult = bracket / (e.a * (e.epssq - 1) - alt * bracket);
    double common_mult_2 = 1 / (norm_rad + alt);

    Matrix3D RES;

    RES(0, 0) = common_mult / (slat * clon);
    RES(0, 1) = common_mult / (slat * slon);
    RES(0, 2) = -common_mult / clat;

    RES(1, 0) = -common_mult_2 / (clat * slon);
    RES(1, 1) = common_mult_2 / (clat * clon);
    RES(1, 2) = 0.0;

    RES(2, 0) = 1 / (clat * clon);
    RES(2, 1) = 1 / (clat * slon);
    RES(2, 2) = 1 / slat;

    return RES;
}

double ground_speed(const Vector3D & ecef_vel, const Vector3D & geo)
{
	Vector3D enu_vel = ecef_to_enu(ecef_vel, geo);
	return std::sqrt(enu_vel[0] * enu_vel[0] + enu_vel[1] * enu_vel[1]);
}

double track_angle(const Vector3D & ecef_vel, const Vector3D & geo)
{
	Vector3D mean_enu_vel = ecef_to_enu(ecef_vel, geo);
	return std::atan2(mean_enu_vel[0], mean_enu_vel[1]);
}

}