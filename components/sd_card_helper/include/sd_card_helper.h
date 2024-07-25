#ifndef SD_CARD_HELPER_H
#define SD_CARD_HELPER_H

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t sd_card_write_file(const char* path, const char* data);
esp_err_t sd_card_read_file(const char* path, char* buffer, size_t buffer_size);
esp_err_t sd_card_append_file(const char* path, const char* data);
esp_err_t sd_card_delete_file(const char* path);

#ifdef __cplusplus
}
#endif

#endif // SD_CARD_HELPER_H