//
// Created by maxwc on 07/04/2025.
//

#ifndef HMC5883L_H
#define HMC5883L_H

#include "Sensor.h"

extern "C" {
    #include "../../components/qmc5883l/qmc5883l.h"
}

class HMC5883L : public Sensor {
public:
    HMC5883L();
    ~HMC5883L() override;
    bool init() override; //tries to init at most 10 times
    bool read() override;
    void display() override;
    void calibrate() override;

    qmc5883l_data_t get_data() const {return data;}
    float get_mag_x() const {return data.x;}
    float get_mag_y() const {return data.y;}
    float get_mag_z() const {return data.z;}

    static void testing();
private:
    qmc5883l_t  dev;
    qmc5883l_data_t data;
};




#endif //HMC5883L_H
