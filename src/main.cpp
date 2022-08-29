#include <Arduino.h>
#include <Discord_WebHook.h>
#include <Ultrasonic.h>

#define VCC_PIN 34
#define DISTANCE_TRIG_PIN 12
#define DISTANCE_ECHO_PIN 13
#define MAILBOX_LENGTH_CM 10
#define DISCORD_WEBHOOK_TOKEN "https://discord.com/api/webhooks/id/token"
#define DISCORD_MESSAGE "Mailbox opened!"
#define WIFI_SSID "baz"
#define WIFI_PSK "changeme"

Ultrasonic distance(DISTANCE_TRIG_PIN, DISTANCE_ECHO_PIN);
Discord_Webhook discord;

bool openTrigger = false;

void setup() {
  pinMode(VCC_PIN, INPUT);

#ifdef DEBUG
  Serial.begin(115200);

  while (!Serial) {
    delay(100);
  }

  Serial.println("Connected!");
#endif
}

void loop() {
  uint32_t distanceCm = distance.read();

#ifdef DEBUG
  Serial.print(distanceCm);
  Serial.println(" cm away");
#endif

  if (openTrigger && distanceCm < MAILBOX_LENGTH_CM) {
#ifdef DEBUG
    Serial.println("Triggering Discord webhook!");
#endif
    discord.begin(DISCORD_WEBHOOK_TOKEN);
#ifndef DEBUG
    discord.disableDebug();
#endif
    discord.addWiFi(WIFI_SSID, WIFI_PSK);
    discord.connectWiFi();
    if (!discord.send(DISCORD_MESSAGE)) {
#ifdef DEBUG
      Serial.println("Error triggering Discord webhook!");
#endif
    }
    openTrigger = false;
  } else if (!openTrigger && distanceCm >= MAILBOX_LENGTH_CM) {
    openTrigger = true;
  }

  int volts = (analogRead(VCC_PIN) / 4095.0) * 3.3;
#ifdef DEBUG
  Serial.print(volts);
  Serial.println(" V");
#endif

  if (volts < 2.5) {
#ifdef DEBUG
    Serial.println("Battery critically low, sleeping forever!");
#endif
    ESP.deepSleep(0);
  }
}
