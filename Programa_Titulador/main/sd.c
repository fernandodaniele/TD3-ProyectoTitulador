/*
Módulo para la lectura/escritura del archivos en una tarjeta de memoria SD
*/
<<<<<<< HEAD
#include "../include/sd.h"
=======
#include "sd.h"
>>>>>>> dde2b5c5b43463e871f1baeb1bdac8bf1ca93d7d
#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "driver/sdspi_host.h"
#include "driver/spi_common.h"
#include "sdmmc_cmd.h"
#include "sdkconfig.h"
#include "driver/sdmmc_host.h"

#define MOUNT_POINT "/sdcard"
#define SPI_DMA_CHANNEL    1 // DMA channel to be used by the SPI peripheral
// Pines para el modo SPI
#define PIN_NUM_MISO 2
#define PIN_NUM_MOSI 15
#define PIN_NUM_CLK  14
#define PIN_NUM_CS   13

<<<<<<< HEAD
static const char *TAG_SD = "example";
=======
static const char *TAG = "example";
>>>>>>> dde2b5c5b43463e871f1baeb1bdac8bf1ca93d7d

void inicializarSD()
{
    esp_err_t ret;
    // Options for mounting the filesystem.
    // If format_if_mount_failed is set to true, SD card will be partitioned and
    // formatted in case when mounting fails.
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };
    sdmmc_card_t* card;
    const char mount_point[] = MOUNT_POINT;
    // Note: esp_vfs_fat_sdmmc/sdspi_mount is all-in-one convenience functions.
    // Please check its source code and implement error recovery when developing
    // production applications.
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };
    ret = spi_bus_initialize(host.slot, &bus_cfg, SPI_DMA_CHANNEL);
    if (ret != ESP_OK) {
<<<<<<< HEAD
        ESP_LOGE(TAG_SD, "Failed to initialize bus.");
=======
        ESP_LOGE(TAG, "Failed to initialize bus.");
>>>>>>> dde2b5c5b43463e871f1baeb1bdac8bf1ca93d7d
        return;
    }

    // This initializes the slot without card detect (CD) and write protect (WP) signals.
    // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = PIN_NUM_CS;
    slot_config.host_id = host.slot;

    ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
<<<<<<< HEAD
            ESP_LOGE(TAG_SD, "Failed to mount filesystem. "
                "If you want the card to be formatted, set the EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
        } else {
            ESP_LOGE(TAG_SD, "Failed to initialize the card (%s). "
=======
            ESP_LOGE(TAG, "Failed to mount filesystem. "
                "If you want the card to be formatted, set the EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
>>>>>>> dde2b5c5b43463e871f1baeb1bdac8bf1ca93d7d
                "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
        }
        return;
    }
}

int escribeSD(char* dato)
{
    FILE* f = fopen("/sdcard/titular.txt", "a");
    if (f == NULL) {
        return 0;
    }
    fprintf(f, "%s",dato);
    fclose(f);
    return 1;
}

int escribeSDInt(int dato)
{
    FILE* f = fopen("/sdcard/titular.txt", "a");
    if (f == NULL) {
        return 0;
    }
    fprintf(f, "%d",dato);
    fclose(f);
    return 1;
}

int escribeSDFloat(float dato)
{
    FILE* f = fopen("/sdcard/titular.txt", "a");
    if (f == NULL) {
        return 0;
    }
    fprintf(f, "%.2f",dato);
    fclose(f);
    return 1;
}

int leeSD(char* dato)
{
    // Abre el archivo para lectura
    FILE* f = fopen("/sdcard/titular.txt", "r");
    if (f == NULL) {
        return 0;
    }
    fgets(dato, 10, f);
    fclose(f);
    return 1;
}

void desmontarSD()
{
    // Desmonta particion y deshabilita SPI
    esp_vfs_fat_sdmmc_unmount();
}