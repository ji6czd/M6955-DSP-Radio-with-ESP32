# M6955-DSP-Radio-with-ESP32
A program to control M6955 DSP radio module by ESP32.

## なにものか

AITENDOで販売しているDSPラジオモジュールのM6955を使ったラジオになる予定のプログラム。せっかくESP32を使うのだから、なにかインターネットに繋がっている状態で楽しいことがしたい。

## 用意するもの

* M6955とかそれに類するもの
* M5StackとかESP32 DevKitとか、そういうマイコン
* アンテナとかケースとかユニバーサル基板とかブレッドボードとか

## 結線

* IO21, IO22でI2C接続
* IO23をP_ONに接続
* IO25を圧電ブザーとかに接続（M5Stackならつながってる）
* IO16, IO17, IO18, IO19をタクトスイッチに接続
