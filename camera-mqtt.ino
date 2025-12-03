#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "certificados.h"
#include <MQTT.h>
#include <esp_camera.h>

// CAMERA CONFIG
camera_config_t config = {
 .pin_pwdn = -1, .pin_reset = -1,
 .pin_xclk = 15, .pin_sscb_sda = 4, .pin_sscb_scl = 5,
 .pin_d7 = 16, .pin_d6 = 17, .pin_d5 = 18, .pin_d4 = 12,
 .pin_d3 = 10, .pin_d2 = 8, .pin_d1 = 9, .pin_d0 = 11,
 .pin_vsync = 6, .pin_href = 7, .pin_pclk = 13,
 .xclk_freq_hz = 20000000,
 .ledc_timer = LEDC_TIMER_0, .ledc_channel = LEDC_CHANNEL_0,
 .pixel_format = PIXFORMAT_JPEG, .frame_size = FRAMESIZE_SVGA,
 .jpeg_quality = 10, .fb_count = 2, .grab_mode = CAMERA_GRAB_LATEST
};

WiFiClientSecure conexaoSegura;

void reconectarWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin("Projeto", "2022-11-07");
    Serial.print("Conectando ao WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(1000);
    }

    Serial.print("conectado!\nEndereço IP: ");
    Serial.println(WiFi.localIP());
  }
}

MQTTClient mqtt(1000);  // tamanho máximo das mensagens (1000 bytes)
void reconectarMQTT() {
  if (!mqtt.connected()) {
    Serial.print("Conectando MQTT...");
    while (!mqtt.connected()) {
      mqtt.connect("IDENTIFICADOR ÚNICO SEU", "aula", "zowmad-tavQez");
      Serial.print(".");
      delay(1000);
    }
    Serial.println(" conectado!");

    mqtt.subscribe("topico1");                 // qos = 0
    mqtt.subscribe("topico2/+/parametro", 1);  // qos = 1
  }
}

void recebeuMensagem(String topico, String conteudo) {
  Serial.println(topico + ": " + conteudo);
}

void setup() {
  Serial.begin(115200);
  delay(500);
  esp_err_t err = esp_camera_init(&config); 
  reconectarWiFi();
  conexaoSegura.setCACert(certificado1);
  mqtt.begin("mqtt.janks.dev.br", 8883, conexaoSegura);
  mqtt.onMessage(recebeuMensagem);
  reconectarMQTT();
}
void loop() {
  reconectarWiFi();
  reconectarMQTT();
  mqtt.loop();
}
