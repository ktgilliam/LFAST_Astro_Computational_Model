#include "InstrumentModel.h"
#include <gtest/gtest.h>
#include <math.h>
#include <cmath>
#include <iostream>
#include <iomanip>
#include "mathFuncs.h"

// cd build && clear && ctest --output-on-failure --timeout 5 .
const double max_error = 1.0e-5;
TEST(InstrumentModel_tests, test_fsCanSexa1)
{
    char testStr[] = "12:34:56";

    double result;

    f_scansexa(testStr, &result);

    EXPECT_NEAR(result, 12.58222, 0.00001);
}

TEST(InstrumentModel_tests, test_fsCanSexa2)
{
    char testStr[] = "110:34:56";

    double result;

    f_scansexa(testStr, &result);

    EXPECT_NEAR(result, 110.58222222222221, 0.00001);
}

TEST(InstrumentModel_tests, hrs_2_rad1)
{
    double hours = 1.2345;
    double rightAnswer_rads = hours * M_2_PI / 24.0;
    double rads = hrs2rad(hours);
    EXPECT_EQ(rads, rightAnswer_rads);
    double hoursAgain = rad2hrs(rads);
    EXPECT_EQ(hoursAgain, hours);
}

TEST(InstrumentModel_tests, hrs_2_rad2)
{
    InstrumentModel mdl;
    char lst[] = "4:38:47.26";
    mdl.setApparentSiderealTime(lst);

    char RA[] = "18:21:25.365";
    char DEC[] = "0:0:0";
    mdl.setRaDec(RA, DEC);

    double hourAngleTruth;
    f_scansexa("-13:42:38.10", &hourAngleTruth);
    auto hourAngleResult = mdl.getHourAngle();
    EXPECT_NEAR(hourAngleTruth, hourAngleResult, max_error);

    std::cout << "Hour Angle (truth): " << hourAngleTruth << std::endl;
    std::cout << "Hour Angle (result): " << hourAngleResult << std::endl;
}

TEST(InstrumentModel_tests, test1)
{
    double lat_deg = 32.0;
    InstrumentModel mdl;
    mdl.setApparentSiderealTime(0.0);
    mdl.setGeodeticLatitude(lat_deg);
    mdl.setRaDec(0.0, 90.0);

    double alt;
    mdl.getHorizontalStates_geocentric(&alt, 0, 0, 0, 0, 0);
    std::cout << "#####" << alt << std::endl;
    double alt_deg = rad2deg(alt);
    EXPECT_NEAR(lat_deg, alt_deg, max_error);
}

TEST(InstrumentModel_tests, test_hour_angle)
{
    InstrumentModel mdl;
    double tgtRA = 3.5; // hours
    double lst = 8.5;   // hours

    mdl.setApparentSiderealTime(lst);
    mdl.setRaDec(tgtRA, 0.0);
    double ha = mdl.getHourAngle();

    double diff = lst - tgtRA;
    EXPECT_NEAR(ha, diff, 0.00001);
}

TEST(InstrumentModel_tests, test_polarDistance)
{
    InstrumentModel mdl;
    char draRA[] =  "0:0:0";
    char draDEC[] = "72:43:55.44";
    mdl.setRaDec(draRA, draDEC);

    double npdTruth;
    f_scansexa("17:16:04.56", &npdTruth);
    double npd = mdl.getNorthPolarDistance();

    EXPECT_NEAR(npd, npdTruth, 0.00001);
}

// TEST(InstrumentModel_tests, test_chi_dra_Coords)
// {
//     InstrumentModel mdl;

//     char lat[] = "33:49:03.85";
//     mdl.setGeodeticLatitude(lat);

//     char lst[] = "4:38:47.26";
//     mdl.setApparentSiderealTime(lst);

//     char draRA[] = "18:21:25.365";
//     char draDEC[] = "72:43:55.44";
//     mdl.setRaDec(draRA, draDEC);

//     double altTruth, azTruth;
//     f_scansexa("18:0:32.7", &altTruth);
//     f_scansexa("172:13:56.4", &azTruth);

//     double alt, az;
//     mdl.getHorizontalStates_geocentric(&alt, &az, 0, 0, 0, 0);

//     EXPECT_NEAR(altTruth, rad2deg(alt), 0.00001);
//     EXPECT_NEAR(azTruth, rad2hrs(alt), 0.00001);
//     // SUCCEED();
// }
