//Libraries
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
//Constants
ESP8266WebServer  server(80)

// Data structures
struct settings {
  char ssid[30];
  char password[30];
  int  pc_id[30];
} user_settings = {};

void setup() {
  EEPROM.begin(sizeof(struct settings));

  WiFi.mode(WIFI_AP);
  WiFi.softAp("Setup portal", "hq");

  server.on("/", handlePortal);
  server.begin();
}

void loop() {
  server.handleClient()
}

void handlePortal() {
  server.on("/", {
    server.send(200, "text/html", wifiSettingsPage)
  });
  server.on("/save-network", {
    strncpy(user_settings.ssid, server.arg("ssid").c_str(), sizeof(user_settings.ssid));
    strncpy(user_settings.password, server.arg("password").c_str(), sizeof(user_settings.ssid));
  });
}

// Html code
String wifiSettingsPage =  R"(<!DOCTYPE html>
<html lang="ru">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>NodeMCU WiFi setup</title>
</head>
<body class="body">
  <style>
    *,
    ::before,
    ::after {
      box-sizing: border-box;
      margin: 0;
      padding: 0;
    }
    a {
      color: inherit;
      text-decoration: inherit;
    }
    h1,
    h2,
    h3 {
      margin: 0;
    }
    html {
      height: 100dvh;
      width: 100%;
    }
    .container {
      max-width: 1280px;
      padding: 0 2em;
      margin: auto;
    }
    .body {
      margin: 0;
      padding: 0;
      position: relative;
      min-height: 100dvh;
      width: 100%;
      display: flex;
      flex-direction: column;
      font-family: Arial, Helvetica, sans-serif;
      font-size: 12px;
      line-height: 120%;
      background-color: darkslategray;
      color: white;
    }
    .header {
      padding: 1.5em 2em;
      display: flex;
      justify-content: center;
      align-items: center;
      position: sticky;
      top: 0;
      left: 0;
      flex: 0 1 auto;
      max-height: max-content;
      border-bottom: 1px solid white;
      backdrop-filter: blur(3px);
    }
    .links-list {
      display: flex;
      align-items: center;
      margin: 0;
      list-style: none;
      width: 100%;
      font-size: 1rem;
    }
    .links-list li {
      height: 100%;
      padding: 0 2em;
      position: relative;
    }
    .links-list li:not(:last-child)::after {
      position: absolute;
      top: 0;
      right: 0;
      content: "";
      height: 100%;
      width: 1px;
      background-color: white;
    }
    .main {
      display: flex;
      flex-direction: column;
      flex: 1 1 100%;
    }
    .settings,
    .contacts {
      display: flex;
      flex-direction: column;
      justify-content: center;
      flex: 1 1 100%;
      width: 100%;
      min-height: 92dvh;
      margin-bottom: 100px;
    }
    .contacts {
      justify-content: flex-start;
      margin-bottom: 0;
    }
    .settings__title,
    .contacts__title {
      text-align: center;
      font-weight: normal;
      margin: 1em 0;
    }
    .settings__form {
      display: flex;
      flex-direction: column;
      flex: 0 1;
    }
    .settings__input-wrapper {
      padding: 0.5rem 0;
      display: flex;
      flex-direction: column;
      justify-content: center;
      width: 100%;
      font-size: 1rem;
      margin: auto;
    }
    .settings__input-wrapper > label {
      font-size: 0.8rem;
      color: lightgray;
    }
    .settings__input-wrapper > input {
      margin: 0.5em 0;
      padding: 0.5em 1em;
      background-color: inherit;
      border: none;
      outline: 1px solid white;
      border-radius: 4px;
      color: white;
    }
    .settings__submit-btn {
      margin: 2em 0;
      padding: 0.5em 1em;
      display: block;
      background-color: transparent;
      border: 1px solid white;
      border-radius: 4px;
      color: white;
      flex: 0 1 auto;
    }
    .contacts__title {
      margin-top: 1rem;
      margin-bottom: 3rem;
      font-size: 1.2rem;
    }
    .contacts__links-list {
      margin: 0;
      list-style: none;
      display: flex;
      flex-direction: column;
      gap: 1em;
      font-size: 1.2rem;
    }
    .contacts__link-name {
      font-size: 1rem;
      margin-right: 10px;
      color: lightgray;
    }
  </style>
  <header class="header">
    <nav class="container">
      <ul class="links-list">
        <li>
          <a href="#settings">
            Настройки
          </a>
        </li>
        <li>
          <a href="#contacts">
            Контакты
          </a>
        </li>
      </ul>
    </nav>
  </header>
  <main class="main">
    <section id="settings" class="settings container">
      <form class="settings__form" action="/save-network" method="post">
        <h2 class="settings__title">Настройка WiFi</h2>
        <div class="settings__input-wrapper">
          <label for="ssid">Название</label>
          <input autocomplete="on" tabindex="0" id="ssid" type="text">
        </div>
        <div class="settings__input-wrapper">
          <label for="password">Пароль</label>
          <input autocomplete="on" type="password" id="password" type="text">
        </div>
        <button class="settings__submit-btn" type="submit">Сохранить</button>
      </form>
    </section>
    <section id="contacts" class="contacts container">
      <h2 class="contacts__title">Контакты</h2>
      <ul class="contacts__links-list">
        <li>
          <p>
            <span class="contacts__link-name">TG:</span>
            <a href="https://telegram.me/kryst4l320" target="_blank">@kryst4l320</a>
          </p>
        </li>
        <li>
          <p>
            <span class="contacts__link-name">GitHub</span>
            <a href="https://github.com/hrustalq" target="_blank">hrustalq</a>
          </p>
        </li>
      </ul>
    </section>
  </main>
</body>
</html>
)";
