#include "InstrumentModel.h"
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"


void InstrumentModel::setRaDec(char *ra, char *dec)
{
    double ra_hrs, dec_deg;
    f_scansexa(ra, &ra_hrs);
    f_scansexa(dec, &dec_deg);
    setRaDec(ra_hrs, dec_deg);
}
void InstrumentModel::setRaDec(double ra, double dec)
{
    geocentricApparentRA.updateValue(hrs2rad(ra));
    geocentricApparentDEC.updateValue(deg2rad(dec));
    updateHorizontalStates_geocentric();
}

void InstrumentModel::setApparentSiderealTime(char *t)
{
    double lst;
    f_scansexa(t, &lst);
    setApparentSiderealTime(lst);
}

void InstrumentModel::setApparentSiderealTime(double t)
{
    apparentSiderealTime.updateValue(hrs2rad(t));
    updateHorizontalStates_geocentric();
};
void InstrumentModel::setGeodeticLatitude(char *lat_deg)
{
    double lat_num;
    f_scansexa(lat_deg, &lat_num);
    setGeodeticLatitude(lat_num);
}
void InstrumentModel::setGeodeticLatitude(double lat_deg)
{
    geodeticLatitude.updateValue(deg2rad(lat_deg));
    updateHorizontalStates_geocentric();
}

void InstrumentModel::updateHorizontalStates_geocentric()
{
    currentHourAngle.updateValue(getHourAngle_rad());
    double cH = currentHourAngle.cosine();
    double sH = currentHourAngle.sin();
    double cDEC = geocentricApparentDEC.cosine();
    double sDEC = geocentricApparentDEC.sin();
    double cLAT = geodeticLatitude.cosine();
    double sLAT = geodeticLatitude.sin();

    //
    // Angular Positions
    //

    // Altitude Angle
    double term1 = sDEC * sLAT;
    double term2 = cDEC * cLAT * cH;
    altAngle_rad.updateValue(std::asin(term1 + term2));

    // Azimuth Angle
    double azNum = -1.0 * sH * cDEC;
    double azDen = sDEC * cLAT - cDEC * sLAT * cH;
    azAngle_rad.updateValue(std::atan2(azNum, azDen));

    // Parallactic Angle
    double num = sH * cLAT;
    double den = sLAT * cDEC - sDEC * cLAT * cH;
    parAngle_rad.updateValue(std::atan2(num, den));

    //
    // Angular Rates
    // Note: For now disregarding non-sidereal targets (proper motions are ignored).
    //
    double sAlt = altAngle_rad.sin();
    double cAlt = altAngle_rad.cosine();
    double sAz = azAngle_rad.sin();
    double cAz = azAngle_rad.cosine();
    double sPAR = parAngle_rad.sin();
    double cPAR = parAngle_rad.cosine();

    // Altitude Rate
    double altArg1 = cLAT * sAz;
    // double altArg2 = (proper motion derivative of non-sidereal target);
    altRate_radpersec = LFAST::SiderealRate_radpersec * altArg1;

    // Azimuth Rate
    double azArg1 = cDEC * cPAR / cAlt;
    // double azArg2 = (proper motion derivative of non-sidereal target);
    azRate_radpersec = LFAST::SiderealRate_radpersec * azArg1;

    // Parallactic Rate
    double parArg1 = -1.0 * cLAT * cAz / cAlt;
    // double parArg2 = (proper motion derivative of non-sidereal target);
    azRate_radpersec = LFAST::SiderealRate_radpersec * parArg1;
}

double InstrumentModel::getHourAngle()
{
    double ha_rad = apparentSiderealTime.value() - geocentricApparentRA.value();
    return rad2hrs(ha_rad);
}

double InstrumentModel::getHourAngle_rad()
{
    double ha_rad = getApparentSiderealTime();
    // while (ha_rad < 0)
    // {
    //     ha_rad += (M_2_PI);
    // }
    // while (ha_rad > M_PI)
    // {
    //     ha_rad = ha_rad - (M_2_PI);
    // }
    return (ha_rad - geocentricApparentRA.value());
}

double InstrumentModel::getNorthPolarDistance()
{
    return rad2deg(M_PI_2 - geocentricApparentDEC.value());
}
void InstrumentModel::getHorizontalStates_geocentric(double *altAngle, double *azAngle, double *parAngle, double *altRate, double *azRate, double *parRate)
{
    updateHorizontalStates_geocentric();
    printDeets(this);
    if (altAngle != nullptr)
        *altAngle = altAngle_rad.value();
    if (azAngle != nullptr)
        *azAngle = azAngle_rad.value();
    if (parAngle != nullptr)
        *parAngle = parAngle_rad.value();
}

// Copied from indi library (indicom.h)
int f_scansexa(const char *str0, /* input string */
               double *dp)       /* cracked value, if return 0 */
{
    // locale_char_t *orig = indi_locale_C_numeric_push();

    double a = 0, b = 0, c = 0;
    char str[128];
    // char *neg;
    uint8_t isNegative = 0;
    int r = 0;

    /* copy str0 so we can play with it */
    strncpy(str, str0, sizeof(str) - 1);
    str[sizeof(str) - 1] = '\0';

    /* remove any spaces */
    char *i = str;
    char *j = str;
    while (*j != 0)
    {
        *i = *j++;
        if (*i != ' ')
            i++;
    }
    *i = 0;

    // This has problem process numbers in scientific notations e.g. 1e-06
    /*neg = strchr(str, '-');
    if (neg)
        *neg = ' ';
    */
    if (str[0] == '-')
    {
        isNegative = 1;
        str[0] = ' ';
    }

    r = sscanf(str, "%lf%*[^0-9]%lf%*[^0-9]%lf", &a, &b, &c);

    // indi_locale_C_numeric_pop(orig);

    if (r < 1)
        return (-1);
    *dp = a + b / 60.0 + c / 3600.0;
    if (isNegative)
        *dp *= -1.0;

    return (0);
}

#include <iostream>
#include <iomanip>

void printDeets(InstrumentModel* mdl)
{

    std::cout << std::setprecision(10);
    std::cout << "############################" << std::endl;
    std::cout << "LST: " << mdl->getSiderealTime() << std::endl;

    std::cout << "Right Ascension: " << mdl->getRightAscension() << std::endl;
    std::cout << "Declination: " << mdl->getDeclination() << std::endl;
    std::cout << "Latitude: " << mdl->getLatitude() << std::endl
              << std::endl;

    std::cout << "Hour Angle: " << mdl->getHourAngle() << std::endl;

    // std::cout << "Altitude: " << altAngle_rad.value() << std::endl;
    // std::cout << "Azimuth: " << azAngle_rad.value() << std::endl;
    // std::cout << "Parallactic: " << parAngle_rad.value() << std::endl
            //   << std::endl;

    std::cout << "############################" << std::endl;
}