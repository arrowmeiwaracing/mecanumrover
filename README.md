# mecanumrover
This is a mecanum rover control application.
use for esp8266(esp-wroom-02)-arduino sketch.
mecanum rover is this URL.
http://www.thingiverse.com/thing:1622028

rover link websocket.
rover(esp8266 running web server port:80 and websocket server port:81)
 -<wifi>-your wifi rooter-<wifi>-cliant(tested android chrome)

esp8266-arduino Ver 2.0.0 over.
add library:
https://github.com/Links2004/arduinoWebSockets
https://github.com/me-no-dev/ESPAsyncTCP
https://github.com/bblanchon/ArduinoJson

for Japanese.
これはメカナムホイールローバーのコントロール用アプリケーションです。
esp8266(esp-wroom-02)arduino用のsketchです。
esp8266用arduino環境(Ver 2.0.0以上)とライブラリが必要です。
1) link2004さんのwebsocketライブラリ
https://github.com/Links2004/arduinoWebSockets
2) me-no-devさんの非同期TCPライブラリ（上記のWebsocketライブラリで必須）
https://github.com/me-no-dev/ESPAsyncTCP
3) bblanchonさんのJSONライブラリ
https://github.com/bblanchon/ArduinoJson

WIFIのSSID,PASSWORDをesp8266ファイルシステム上にwificonfig.txt
というJSON形式のファイルで与えることで、スケッチ内にハードコード
しなくてよいようになっています。
https://github.com/esp8266
そのため、上記サイトの「arduino-esp8266fs-plugin」をarduino開発環境に
インストールする必要があります。

その他：
・WIFI経由のOTAでファームウエアを更新するため、sketchには
　OTA用のコードが記述されています。
・mDNSでサーバを起動していますので、mDNS対応したPCでは
  http://esp8266.local
　で接続できます。mDNSはWindows10では標準対応しています。
　androidでは現在（chrome 51.0）未対応なので、IPアドレスを
　指定して接続します。
  http://256.256.256.256
・クライアントはタッチイベントが拾える環境が必要です。

以上

## License
MIT
