echo %cd%
adb shell su -c "rm /data/local/tmp/main/*"
adb push "F:\IHTools\GotHookExample\libs\armeabi-v7a" "/data/local/tmp/main"
adb shell su -c "chmod 777 /data/local/tmp/main/*"
adb shell su -c "/data/local/tmp/main/main"