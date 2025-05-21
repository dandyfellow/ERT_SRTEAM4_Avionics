#ifndef CONSTANTES_H
#define CONSTANTES_H

constexpr double MAIN_PARA_DEPLOY_HEIGHT = 150; // meters //height where it deploys
constexpr double MAIN_PARA_DEPLOY_HEIGHT_TOLERANCE = 4; // distance [m] the baro needs to drop before max height is declared to be reached
constexpr double MAIN_PARA_DEPLOY_HEIGHT_WAIT =  1; // seconds (height to wait before deploying the main parachute if it hasn't reached 150m)
constexpr double SAMPLE_FREQ = 20; // Hz | t[ms]= 1000/SAMPLE_FREQ
constexpr double MAIN_PARA_DEPLOY_VELOCITY = -1; // [m/s]


#endif //CONSTANTES_H
