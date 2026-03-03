#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* WIFI_SSID = "";
const char* WIFI_PASS = "";

ESP8266WebServer server(80);

const int NUM_SAMPLES = 50;
const float OFFSET = 17.0;
// Conversion factor from ADC steps to volts.
// Calculated as: SCALE = RealVoltage / (ADCreading - OFFSET)
// Example: 1.592 V / (552 - 17) ≈ 0.002975 V per ADC count
const float SCALE  = 0.002975;

float readADC(int samples)
{
  long sum = 0;
  for (int i = 0; i < samples; i++)
  {
    sum += analogRead(A0);
    delay(2);
  }
  return sum / (float)samples;
}

float readVoltage()
{
  float adc = readADC(NUM_SAMPLES);
  float corrected = adc - OFFSET;

  float voltage = corrected * SCALE;
  if (voltage < 0) voltage = 0;

  return voltage;
}

void handleRoot()
{
  float voltage = readVoltage();

  String html = "<!DOCTYPE html><html><head>";
  html += "<meta http-equiv='refresh' content='1'/>";
  html += "<style>body{font-family:Arial;text-align:center;margin-top:50px;}</style>";
  html += "</head><body>";
  html += "<h1>ESP8266 Voltage Monitor</h1>";
  html += "<h2>";
  html += String(voltage, 3);
  html += " V</h2>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

void setup()
{
  Serial.begin(115200);
  delay(200);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(300);
    Serial.print(".");
  }

  Serial.println("\nConnected");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.begin();
}

void loop()
{
  server.handleClient();
}