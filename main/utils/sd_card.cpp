#include <sd_card.h>

FILE* sd_card::file_ptr = nullptr;

void sd_card::open_new_sd_log_file() {
    char filename[64];
    int file_number = 0;

    while (file_number < 1000) {
        snprintf(filename, sizeof(filename), "/sdcard/log_sd_%d.csv", file_number);

        FILE* test = fopen(filename, "r");
        if (test == nullptr) {
            // File doesn't exist, create new
            file_ptr = fopen(filename, "w");
            if (file_ptr == nullptr) {
                ESP_LOGE("CSV_LOG", "Failed to create file: %s", filename);
                return;
            }

            // Write CSV header
            fprintf(file_ptr,
                "time,ax,ay,az,gx,gy,gz,mx,my,mz,pressure,temperature,altitude,"
                "pitch,yaw,roll,max_altitude_reached,deploy_main_para_parachute\n"
            );
            ESP_LOGI("CSV_LOG", "Created new log file: %s", filename);
            return;
        } else {
            // File exists, try next
            fclose(test);
            file_number++;
        }
    }

    ESP_LOGE("CSV_LOG", "Too many log files, clean up SD card.");
}

void sd_card::write_to_sd_csv(const IMUData10Axis& data) {
    if (file_ptr == nullptr) return;

    fprintf(file_ptr,
            "%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,"
            "%.3f,%.3f,%.3f,%.2f,%.2f,%.2f,"
            "%.3f,%.3f,%.3f,%.3f,%d,%d\n",
            data.time,
            data.ax, data.ay, data.az,
            data.gx, data.gy, data.gz,
            data.mx, data.my, data.mz,
            data.pressure, data.temperature, data.altitude,
            data.pitch, data.yaw, data.roll,
            data.max_altitude_reached ? 1 : 0,
            data.deploy_main_para_parachute ? 1 : 0
    );

    fflush(file_ptr); // optional, ensures write immediately
}