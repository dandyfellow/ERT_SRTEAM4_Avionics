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
    qmc5883l_data_t get_data() const {return data;}

    static void testing();
private:
    qmc5883l_t  dev;
    qmc5883l_data_t data;
};




#endif //HMC5883L_H
