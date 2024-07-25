#include "sd_card_helper.h"
#include "esp_log.h"
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>

#define TAG "SD_CARD_HELPER"

esp_err_t sd_card_write_file(const char* path, const char* data) {
    FILE* f = fopen(path, "w");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return ESP_FAIL;
    }
    fprintf(f, "%s", data);
    fclose(f);
    return ESP_OK;
}

esp_err_t sd_card_read_file(const char* path, char* buffer, size_t buffer_size) {
    FILE* f = fopen(path, "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return ESP_FAIL;
    }
    size_t bytes_read = fread(buffer, 1, buffer_size - 1, f);
    buffer[bytes_read] = '\0';
    fclose(f);
    return ESP_OK;
}

esp_err_t sd_card_append_file(const char* path, const char* data) {
    FILE* f = fopen(path, "a");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for appending");
        return ESP_FAIL;
    }
    fprintf(f, "%s", data);
    fclose(f);
    return ESP_OK;
}

esp_err_t sd_card_delete_file(const char* path) {
    if (unlink(path) == 0) {
        return ESP_OK;
    } else {
        ESP_LOGE(TAG, "Failed to delete file");
        return ESP_FAIL;
    }
}