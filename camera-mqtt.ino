#include "esp_camera.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <Base64.h>

const char* ssid = "YOUR_WIFI";
const char* password = "YOUR_PASS";

const char* mqtt_server = "test.mosquitto.org";
const char* topic_photo = "projeto/camera/foto";
const char* topic_data  = "projeto/camera/dados";

WiFiClient espClient;
PubSubClient client(espClient);

// CAMERA CONFIGURATION ----------------
camera_config_t config = {
  .pin_pwdn       = 32,
  .pin_reset      = -1,
  .pin_xclk       = 0,
  .pin_sscb_sda   = 26,
  .pin_sscb_scl   = 27,

  .pin_d7         = 35,
  .pin_d6         = 34,
  .pin_d5         = 39,
  .pin_d4         = 36,
  .pin_d3         = 21,
  .pin_d2         = 19,
  .pin_d1         = 18,
  .pin_d0         = 5,
  .pin_vsync      = 25,
  .pin_href       = 23,
  .pin_pclk       = 22,

  .xclk_freq_hz   = 20000000,
  .ledc_timer     = LEDC_TIMER_0,
  .ledc_channel   = LEDC_CHANNEL_0,
  .pixel_format   = PIXFORMAT_JPEG,
  .frame_size     = FRAMESIZE_QVGA,
  .jpeg_quality   = 10,
  .fb_count       = 1
};
// --------------------------------------

void setup_wifi() {
  delay(100);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void reconnect() {
  while (!client.connected()) {
    client.connect("ESP32CAM");
  }
}

void sendPhoto() {
  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) {
    return;
  }

  // Convert JPEG to Base64
  String imageBase64 = base64::encode(fb->buf, fb->len);

  // Send over MQTT
  client.publish(topic_photo, imageBase64.c_str());

  // Send metadata
  client.publish(topic_data, "{\"status\":\"ok\",\"source\":\"esp32cam\"}");

  esp_camera_fb_return(fb);
}

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);

  // Start camera
  esp_camera_init(&config);

  // Connect WiFi
  setup_wifi();

  client.setServer(mqtt_server, 1883);

  reconnect();
}

void loop() {
  if (Serial1.available()) {
    String msg = Serial1.readStringUntil('\n');
    Serial.println("Recebido: " + msg);
  }

  if (!client.connected()) reconnect();
  client.loop();

  sendPhoto();
  delay(3000); // 3 seconds per photo
}
