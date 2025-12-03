#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "certificados.h"
#include <MQTT.h>
#include <esp_camera.h>

WiFiClientSecure conexaoSegura;

MQTTClient mqtt(200000);

// CAMERA CONFIG
camera_config_t config = {
  .pin_pwdn = -1, .pin_reset = -1, .pin_xclk = 15, .pin_sscb_sda = 4, .pin_sscb_scl = 5,
  .pin_d7 = 16, .pin_d6 = 17, .pin_d5 = 18, .pin_d4 = 12, .pin_d3 = 10, .pin_d2 = 8,
  .pin_d1 = 9, .pin_d0 = 11, .pin_vsync = 6, .pin_href = 7, .pin_pclk = 13,
  .xclk_freq_hz = 20000000,
  .ledc_timer = LEDC_TIMER_0, .ledc_channel = LEDC_CHANNEL_0,
  .pixel_format = PIXFORMAT_JPEG,
  .frame_size = FRAMESIZE_SVGA,
  .jpeg_quality = 10,
  .fb_count = 2,
  .grab_mode = CAMERA_GRAB_LATEST
};

void tirarFotoEEnviarParaMQTT() {
  camera_fb_t* foto = esp_camera_fb_get();
  if (!foto) {
    Serial.println("Falha ao capturar foto");
    return;
  }

  bool ok = mqtt.publish("topico/foto", (const char*)foto->buf, foto->len);

  if (ok) Serial.println("Foto enviada com sucesso!");
  else    Serial.println("Falha ao enviar foto!");

  esp_camera_fb_return(foto);
}

void reconectarWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin("Projeto", "2022-11-07");
    while (WiFi.status() != WL_CONNECTED) delay(500);
  }
}

void reconectarMQTT() {
  while (!mqtt.connected()) {
    mqtt.connect("espcam_roberto", "aula", "zowmad-tavQez");
  }
}

unsigned long ultimoEnvio = 0;

void setup() {
  Serial.begin(115200);

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Erro camera: 0x%x\n", err);
    while (true);
  }

  reconectarWiFi();
  conexaoSegura.setCACert(certificado1);
  mqtt.begin("mqtt.janks.dev.br", 8883, conexaoSegura);
  reconectarMQTT();
}

void loop() {
  reconectarWiFi();
  reconectarMQTT();
  mqtt.loop();

  if (millis() - ultimoEnvio > 10000) {
    tirarFotoEEnviarParaMQTT();
    ultimoEnvio = millis();
  }
}
