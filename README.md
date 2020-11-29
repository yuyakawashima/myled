# ロボットシステム学 課題１ 
## 目的
作成したデバイスドライバを利用し，LEDの操作を行う．
## 使用したもの
・Raspberry Pi 4  
・ブレッドボード  
・抵抗(200Ω)×2  
・ジャンパー線  
・LED(赤，青)
## 動作方法
１．GPIO23と25(16，22番ピン)にLEDのアノードを接続し，カソードをGNDに接続する．  
２．myledディレクトリに入り以下のコマンドを実行する．
```
$make
$sudo insmod myled.ko
$sudo chmod 666 /dev/myled0
$echo 0 > /dev/myled0		//LED消灯
$echo 1 > /dev/myled0		//赤色LED点灯
$echo 2 > /dev/myled0		//青色LED点灯
$echo 3 > /dev/myled0		//337拍子のリズムで点灯(3回で終了)
$echo 4 > /dev/myled0		//4拍のリズムで点滅(10回で終了)
```
３．終了時以下のコマンドを実行する．
```
$sudo rmmod myled
```
### 実行動画
https://www.youtube.com/watch?v=OcpkBpW5xU0
