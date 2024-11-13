/*==================[ Inclusiones ]============================================*/
#include "../include/wifi.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "esp_log.h"

/*==================[ Definiciones ]===================================*/

// ---CREAR UN SERVIDOR WEB COMO ACESS POINT---

/*==================[Prototipos de funciones]======================*/

esp_err_t root_get_handler(httpd_req_t *req);
esp_err_t toggle_agitador_handler(httpd_req_t *req);
esp_err_t PH_get_handler(httpd_req_t *req);
static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

/*==================[Variables]==============================*/

static const char *TAG          = "wifi_server";
static httpd_handle_t server    = NULL;

bool flag_agitador_wifi = false;

extern float Vout_PH;
char PH_string[6];

extern QueueHandle_t S_Agitador;

/*==================[Implementaciones]=================================*/

// Función para manejar la solicitud desde la página web
esp_err_t root_get_handler(httpd_req_t *req) {
    // Generar HTML con estilo CSS y JavaScript para actualizar el voltaje
    char PH_string[2048];
    snprintf(PH_string, sizeof(PH_string),
        "<html>"
        "<head>"
        "<style>"
        "body { font-family: Arial, sans-serif; text-align: center; margin: 0; padding: 0; }"
        "h2 { font-size: 2em; }"
        "p { font-size: 1.5em; }"
        "button { "
        "    display: inline-block; "
        "    width: 80%%; "
        "    padding: 15px; "
        "    font-size: 1.2em; "
        "    margin: 10px auto; "
        "    background-color: #333; "
        "    color: white; "
        "    border: none; "
        "    border-radius: 8px; "
        "    cursor: pointer; "
        "}"
        "button:hover { background-color: #555; }"
        "#PH-box { "
        "    margin-top: 20px; "
        "    font-size: 1.5em; "
        "    padding: 10px; "
        "    border: 2px solid #333; "
        "    display: inline-block; "
        "    border-radius: 8px; "
        "}"
        "@media (min-width: 600px) {"
        "    button { width: 300px; }"
        "}"
        "</style>"
        "<script>"
        "function updatePH() {"
        "    fetch('/Vout_PH').then(response => response.text()).then(data => {"
        "        document.getElementById('PH-box').innerText = 'PH: ' + data;"
        "    });"
        "}"
        "setInterval(updatePH, 100);" // Tiempo de Act en ms 
        "</script>"
        "</head>"
        "<body>"
        "<h2>ESP32 Servidor WEB</h2>"
        "<p>Usando Modo Estacion</p>"
        "<p>Estado Agitador: %s</p>"
        "<form action=\"/flag_agitador_wifi\"><button>%s</button></form>"
        "<div id='PH-box'>PH: -- </div>"
        "</body>"
        "</html>",
        flag_agitador_wifi ? "ON" : "OFF", flag_agitador_wifi ? "OFF" : "ON");

    // Enviar la respuesta HTML
    httpd_resp_send(req, PH_string, strlen(PH_string));
    return ESP_OK;
}

// Función para alternar el estado del Agitador
esp_err_t toggle_agitador_handler(httpd_req_t *req) {
    flag_agitador_wifi = !flag_agitador_wifi;
    xQueueSend(S_Agitador, &flag_agitador_wifi, portMAX_DELAY);
    return root_get_handler(req);
}

// Manejador para enviar el valor de PH
esp_err_t PH_get_handler(httpd_req_t *req) {
    // Crear la respuesta con el voltaje actual
    snprintf(PH_string, sizeof(PH_string), "%.02f", Vout_PH);
    httpd_resp_send(req, PH_string, strlen(PH_string));
    return ESP_OK;
}

// Iniciar servidor web y registrar manejadores de URI
httpd_handle_t start_webserver(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.stack_size = 8192;  // Asignar más memoria de pila (valor en bytes)

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t uri_get = {
            .uri = "/", .method = HTTP_GET, .handler = root_get_handler, .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &uri_get);

        httpd_uri_t uri_flag_agitador_wifi = {
            .uri = "/flag_agitador_wifi", .method = HTTP_GET, .handler = toggle_agitador_handler, .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &uri_flag_agitador_wifi);
    
        httpd_uri_t uri_root = {
            .uri = "/", .method = HTTP_GET, .handler = root_get_handler, .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &uri_root);

        httpd_uri_t uri_Vout_PH = {
            .uri = "/Vout_PH", .method = HTTP_GET, .handler = PH_get_handler, .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &uri_Vout_PH);
    }
    return server;
}

// // Manejo de eventos Wi-Fi
// static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
//     if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
//         esp_wifi_connect();
//     } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
//         esp_wifi_connect();
//         ESP_LOGI(TAG, "Intentando reconectar...");
//     } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
//         ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
//         ESP_LOGI(TAG, "Conectado con IP: " IPSTR, IP2STR(&event->ip_info.ip));
//         start_webserver();  // Iniciar el servidor web al obtener IP
//     }
// }

// // Inicializar Wi-Fi
// void wifi_init(void) {
//     esp_netif_init();
//     esp_event_loop_create_default();
//     esp_netif_create_default_wifi_sta();

//     wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
//     esp_wifi_init(&cfg);

//     esp_event_handler_instance_t instance_any_id;
//     esp_event_handler_instance_t instance_got_ip;
//     esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, &instance_any_id);
//     esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, &instance_got_ip);

//     wifi_config_t wifi_config = {
//         .sta = {
//             .ssid = WIFI_SSID,
//             .password = WIFI_PASS,
//         },
//     };
//     esp_wifi_set_mode(WIFI_MODE_STA);
//     esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
//     esp_wifi_start();
// }

// Manejo de eventos Wi-Fi
static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_START) {
        ESP_LOGI(TAG, "Access Point iniciado");
    }
}

// Inicializar Wi-Fi en modo Access Point
void wifi_init_softap(void) {
    // Inicializa la pila TCP/IP subyacente
    esp_netif_init();
    // Crea un loop de evento por defecto 
    esp_event_loop_create_default();
    // Crea AP WIFI predeterminado. En caso de cualquier error de inicio, esta API se cancela.
    esp_netif_t *netif = esp_netif_create_default_wifi_ap();

    // Configurar IP estática
    esp_netif_ip_info_t ip_info;
    IP4_ADDR(&ip_info.ip, 192, 168, 4, 1);        // Dirección IP           ESCRIBIR ESTO ---> http://192.168.4.1/
    IP4_ADDR(&ip_info.gw, 192, 168, 4, 1);        // Puerta de enlace
    IP4_ADDR(&ip_info.netmask, 255, 255, 255, 0); // Máscara de subred
    esp_netif_dhcps_stop(netif);                 // Detener DHCP para usar IP fija
    esp_netif_set_ip_info(netif, &ip_info);
    esp_netif_dhcps_start(netif);                // Reiniciar DHCP

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    // Inicializa el Wifi del ESP32
    esp_wifi_init(&cfg);

    esp_event_handler_instance_t instance_any_id;
    // Registra una instancia del controlador de eventos en el bucle predeterminado.
    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, &instance_any_id);

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = WIFI_SSID,
            .ssid_len = strlen(WIFI_SSID),
            .password = WIFI_PASS,
            .max_connection = 4,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK,
        },
    };

    if (strlen(WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    esp_wifi_set_mode(WIFI_MODE_AP);
    esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config);
    esp_wifi_start();
}