#include <Arduino.h>
#include <Discord_WebHook.h>
#include <Ultrasonic.h>

#define DISTANCE_TRIG_PIN 12
#define DISTANCE_ECHO_PIN 13
#define MAILBOX_LENGTH_CM 10
#define DISCORD_WEBHOOK_TOKEN "https://discord.com/api/webhooks/id/token"
#define DISCORD_MESSAGE "Mailbox opened!"
#define WIFI_SSID "baz"
#define WIFI_PSK "changeme"

Ultrasonic distance(DISTANCE_TRIG_PIN, DISTANCE_ECHO_PIN);
Discord_Webhook discord;

void setup() {
  Serial.begin(115200);

  while (!Serial) {
    delay(100);
  }

  Serial.println("Connected!");
}

void loop() {
  uint32_t start = millis();

  uint32_t distanceCm = distance.read();
  Serial.print(distanceCm);
  Serial.println(" cm away");
  if (distanceCm > MAILBOX_LENGTH_CM) {
    discord.begin(DISCORD_WEBHOOK_TOKEN);
    discord.disableDebug();
    discord.addWiFi(WIFI_SSID, WIFI_PSK);
    discord.connectWiFi();
    if (!discord.send(DISCORD_MESSAGE)) {
      Serial.println("Error triggering Discord webhook!");
    }
  }

  uint32_t runtime = millis() - start;
  delay(60000 - runtime);
}
