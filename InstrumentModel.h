#include <cinttypes>
#include <cmath>

#include "mathFuncs.h"

#define EARTH_ROTATIONS_PER_UT1_DAY 1.002737811906
#define SECONDS_PER_DAY 86400.0
namespace LFAST
{
    const double SiderealRate_radpersec = (M_2_PI / SECONDS_PER_DAY) * EARTH_ROTATIONS_PER_UT1_DAY;
    // const double SiderealRate_radpersec = (0.000072921); //(15.041067 / 3600.0 * M_PI / 180.0)
}

template <class T>
class InstrumentParam
{
private:
    T val = 0;
    T sVal = 0;
    T cVal = 1;

public:
    InstrumentParam() {}
    void updateValue(T _val)
    {
        val = _val;
        cVal = std::cos(_val);
        sVal = std::sin(_val);
    }
    T cosine() { return cVal; }
    T sin() { return sVal; }
    T value() { return val; }
};

// struct InstrumentStates
// {
//     double
// }

class InstrumentModel
{
protected:
    InstrumentParam<double> apparentSiderealTime;
    InstrumentParam<double> geodeticLongitude;
    InstrumentParam<double> geodeticLatitude;
    InstrumentParam<double> geocentricApparentRA;
    InstrumentParam<double> geocentricApparentDEC;
    InstrumentParam<double> currentHourAngle;

    InstrumentParam<double> azAngle_rad;
    InstrumentParam<double> altAngle_rad;
    InstrumentParam<double> parAngle_rad;

    double azRate_radpersec = 0;
    double altRate_radpersec = 0;
    double parAngle_radpersec = 0;

    void updateHorizontalStates_geocentric();
    // double getAltitudeAngle();
    // double getAzimuthAngle();
    double getHourAngle_rad();

public:
    InstrumentModel(){};

    double getApparentSiderealTime() { return rad2hrs(apparentSiderealTime.value()); };

    void setApparentSiderealTime(char *t);
    void setApparentSiderealTime(double t);

    void setRaDec(double ra, double dec);
    void setRaDec(char *ra, char *dec);

    void getHorizontalStates_geocentric(double *altAngle, double *azAngle, double *parAngle, double *altRate, double *azRate, double *parRate);

    void setGeodeticLatitude(char *lat);
    void setGeodeticLatitude(double lat);

    double getDeclination() { return rad2deg(geocentricApparentDEC.value()); }
    double getRightAscension() { return rad2deg(geocentricApparentRA.value()); }
    double getSiderealTime(){ return rad2hrs(apparentSiderealTime.value()); }
    double getLatitude(){return rad2deg(geodeticLatitude.value());}
    double getLongitude(){return rad2deg(geodeticLongitude.value());}
    

    double getHourAngle();
    double getNorthPolarDistance();

};

void printDeets(InstrumentModel*);
int f_scansexa(const char *str0, double *dp);