//
// Created by Fayorg on 20/05/2025.
//

#ifndef SPIFFSSTORE_H
#define SPIFFSSTORE_H

#include <string>

using namespace std;

class SPIFFSStore : public DataStore {
    public:
        bool init() override;

        string getStoreName() override {
            return "SPIFFS";
        }

        bool logEvent(const string& event) override;
        bool logFlightData() override;

    private:
        FILE* logs;
        FILE* telemetry;
};



#endif //SPIFFSSTORE_H
