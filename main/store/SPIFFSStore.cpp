//
// Created by Fayorg on 20/05/2025.
//

#include "SPIFFSStore.h"

extern "C" {
    #include "esp_spiffs.h"
    #include "esp_vfs.h"
    #include "esp_log.h"
}

bool SPIFFSStore::init() {
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true
    };

    return esp_vfs_spiffs_register(&conf) == ESP_OK;
}

bool SPIFFSStore::logEvent(const string& event) {
    FILE* f = fopen("/spiffs/events.txt", "a");
    if (f == NULL) {
        ESP_LOGE("SPIFFSStore", "Failed to open file for appending");
        return false;
    }

    fprintf(f, "%s\n", event.c_str());
    fclose(f);
    return true;
}

bool SPIFFSStore::logFlightData() {
    FILE* f = fopen("/spiffs/telemetry.txt", "a");
    if (f == NULL) {
        ESP_LOGE("SPIFFSStore", "Failed to open file for appending");
        return false;
    }

    fprintf(f, "Telemetry data\n");
    fclose(f);
    return true;
}