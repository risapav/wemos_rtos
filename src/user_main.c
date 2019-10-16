#include "esp_common.h"
#include "esp_spiffs.h"
#include "esp_system.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <lwip/api.h>
#include "gpio.h"
#include "uart.h"

#include "spiffs.h"
#include "spiffs_nucleus.h"

#include "spi_flash.h"

#include "configuration.h"

/******************************************************************************
 * FunctionName : user_rf_cal_sector_set
 * Description  : SDK just reversed 4 sectors, used for rf init data and paramters.
 *                We add this function to force users to set rf cal sector, since
 *                we don't know which sector is free in user's application.
 *                sector map for last several sectors : ABCCC
 *                A : rf cal
 *                B : rf init data
 *                C : sdk parameters
 * Parameters   : none
 * Returns      : rf cal sector
*******************************************************************************/
uint32 user_rf_cal_sector_set(void)
{
  flash_size_map size_map = system_get_flash_size_map();
  uint32 rf_cal_sec = 0;

  switch (size_map)
  {
  case FLASH_SIZE_4M_MAP_256_256:
    rf_cal_sec = 128 - 5;
    break;

  case FLASH_SIZE_8M_MAP_512_512:
    rf_cal_sec = 256 - 5;
    break;

  case FLASH_SIZE_16M_MAP_512_512:
  case FLASH_SIZE_16M_MAP_1024_1024:
    rf_cal_sec = 512 - 5;
    break;

  case FLASH_SIZE_32M_MAP_512_512:
  case FLASH_SIZE_32M_MAP_1024_1024:
    rf_cal_sec = 1024 - 5;
    break;
  case FLASH_SIZE_64M_MAP_1024_1024:
    rf_cal_sec = 2048 - 5;
    break;
  case FLASH_SIZE_128M_MAP_1024_1024:
    rf_cal_sec = 4096 - 5;
    break;
  default:
    rf_cal_sec = 0;
    break;
  }

  return rf_cal_sec;
}

void task_blink(void *ignore)
{
  //GPIO_OUTPUT(14, 0);

  //  gpio16_output_conf();
  while (true)
  {
    //GPIO_OUTPUT_SET(14, 1);
    //printf("\nblink %d", 0);
    //    gpio16_output_set(0);
    vTaskDelay(500 / portTICK_RATE_MS);
    //    gpio16_output_set(1);
    //GPIO_OUTPUT_SET(14, 0);
    //printf("\nblink %d", 1);
    vTaskDelay(500 / portTICK_RATE_MS);
  }

  vTaskDelete(NULL);
}

void httpd_task(void *pvParameters)
{
  struct netconn *client = NULL;
  struct netconn *nc = netconn_new(NETCONN_TCP);
  if (nc == NULL)
  {
    printf("Failed to allocate socket.\n");
    vTaskDelete(NULL);
  }
  netconn_bind(nc, IP_ADDR_ANY, 80);
  netconn_listen(nc);
  char buf[512];
  const char *webpage = {
      "HTTP/1.1 200 OK\r\n"
      "Content-type: text/html\r\n\r\n"
      "<html><head><title>HTTP Server</title>"
      "<style> div.main {"
      "font-family: Arial;"
      "padding: 0.01em 16px;"
      "box-shadow: 2px 2px 1px 1px #d2d2d2;"
      "background-color: #f1f1f1;}"
      "</style></head>"
      "<body><div class='main'>"
      "<h3>HTTP Server</h3>"
      "<p>URL: %s</p>"
      "<p>Uptime: %d seconds</p>"
      "<p>Free heap: %d bytes</p>"
      "<button onclick=\"location.href='/on'\" type='button'>"
      "LED On</button></p>"
      "<button onclick=\"location.href='/off'\" type='button'>"
      "LED Off</button></p>"
      "</div></body></html>"};
  /* disable LED */
  GPIO_OUTPUT(2, 0);
  GPIO_OUTPUT_SET(2, 1);
  while (1)
  {
    err_t err = netconn_accept(nc, &client);
    if (err == ERR_OK)
    {
      struct netbuf *nb;
      if ((err = netconn_recv(client, &nb)) == ERR_OK)
      {
        void *data;
        u16_t len;
        netbuf_data(nb, &data, &len);
        /* check for a GET request */
        if (!strncmp(data, "GET ", 4))
        {
          char uri[16];
          const int max_uri_len = 16;
          char *sp1, *sp2;
          /* extract URI */
          sp1 = (char *)data + 4;
          sp2 = memchr(sp1, ' ', max_uri_len);
          int len = sp2 - sp1;
          memcpy(uri, sp1, len);
          uri[len] = '\0';
          printf("uri: %s\n", uri);
          if (!strncmp(uri, "/on", max_uri_len))
            GPIO_OUTPUT_SET(2, 0);

          else if (!strncmp(uri, "/off", max_uri_len))
            GPIO_OUTPUT_SET(2, 1);

          snprintf(buf, sizeof(buf), webpage, uri,
                   xTaskGetTickCount() * portTICK_RATE_MS / 1000,
                   (int)xPortGetFreeHeapSize());
          netconn_write(client, buf, strlen(buf), NETCONN_COPY);
        }
      }
      netbuf_delete(nb);
    }
    printf("Closing connection\n");
    netconn_close(client);
    netconn_delete(client);
  }
}

/*****************************************************************************
 *
 *
 * **************************************************************************/
#define FS1_FLASH_ADDR 0x300000
#define FS1_FLASH_SIZE (1024 * 1024)
#define SECTOR_SIZE (4 * 1024)
#define LOG_BLOCK SECTOR_SIZE
#define LOG_PAGE (128)
#define FD_BUF_SIZE (32 * 4)
#define CACHE_BUF_SIZE (LOG_PAGE + 32) * 8

void spiffs_fs1_init()
{
  /*
SPIFFS_START_ADDRESS=0x300000
SPIFFS_END_ADDRESS=0x3FB000
SPIFFS_PAGE=256
SPIFFS_BLOCK=8192
SPIFFS_SIZE=$((SPIFFS_END_ADDRESS - SPIFFS_START_ADDRESS))
*/
  struct esp_spiffs_config config;
  config.phys_size = FLASH_SIZE_32M_MAP_512_512;
  config.phys_addr = FS1_FLASH_ADDR;
  config.phys_erase_block = SECTOR_SIZE;
  config.log_block_size = LOG_BLOCK;
  config.log_page_size = LOG_PAGE;
  config.fd_buf_size = FD_BUF_SIZE * 2;
  config.cache_buf_size = CACHE_BUF_SIZE;

  int ret = esp_spiffs_init(&config);
  // need to initalize
  ret = esp_spiffs_init(&config);
  printf("\nCheck initialized: %d\n", ret);
  esp_spiffs_deinit(0);
}

/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void user_init(void)
{
  UART_ConfigTypeDef uart_config;
  uart_config.baud_rate = BIT_RATE_74880; //BIT_RATE_19200;//BIT_RATE_115200;
  uart_config.data_bits = UART_WordLength_8b;
  uart_config.parity = USART_Parity_None;
  uart_config.stop_bits = USART_StopBits_1;
  uart_config.flow_ctrl = USART_HardwareFlowControl_None;
  uart_config.UART_RxFlowThresh = 120;
  uart_config.UART_InverseMask = UART_None_Inverse;
  UART_ParamConfig(UART0, &uart_config);

  UART_SetPrintPort(UART0); // select UART0
  printf(string_0);
  printf(string_1, system_get_sdk_version());
  printf(string_2, system_get_chip_id());
  printf("\nflash map: %d\n", user_rf_cal_sector_set());
  //spiffs init
  printf("Initializing SPIFFS");
  spiffs_fs1_init();
  printf("SPIFFS unmounted");
  //task create
  xTaskCreate(&task_blink, (signed char *)"startup", 2048, NULL, 1, NULL);
  xTaskCreate(&httpd_task, (signed char *)"http_server", 1024, NULL, 2, NULL);
}
