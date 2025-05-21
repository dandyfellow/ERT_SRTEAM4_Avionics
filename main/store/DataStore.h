//
// Created by Elie "Fayorg" Baier on 20/05/2025.
//

#ifndef DATASTORE_H
#define DATASTORE_H

#include <vector>
#include <string>
#include <algorithm>

extern "C" {
    #include "esp_log.h"
}

using namespace std;

class DataStore {
    public:
      static vector<DataStore*> stores;

      DataStore() {
          stores.push_back(this);
      }

      virtual ~DataStore() {
          auto it = find(stores.begin(), stores.end(), this);
          if (it != stores.end()) {
              stores.erase(it);
          }
      }

      // Get the name of the store (mostly used for logging)
      virtual string getStoreName();

      // This is to log an event (logs) in the store.
      virtual bool logEvent(const string& event);

      // This is used to log telemetry in the store
      virtual bool logFlightData();

      // Initialize all stores
      static void initialize() {
          for (auto store : stores) {
              store->init();
              if (!store->init()) {
                  ESP_LOGE("DataStore", "Failed to initialize store: %s", store->getStoreName().c_str());
              } else {
                  ESP_LOGI("DataStore", "Initialized store: %s", store->getStoreName().c_str());
              }
          }
      }

    private:
        bool init();
};

#endif //DATASTORE_H
