#ifndef SD_CARD_H
#define SD_CARD_H

#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "dirent.h"
#include <Tools.h>

class sd_card {
public:
    static void open_new_sd_log_file();
    static void write_to_sd_csv(const IMUData10Axis& data);

    static FILE* file_ptr;
};



#endif //SD_CARD_H
