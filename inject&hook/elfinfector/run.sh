#!/bin/bash -v
    adb shell "su -c 'chmod 777 /data/data/com.estoty.game2048/lib/'" &&
    adb shell "su -c 'chmod 777 /data/data/com.estoty.game2048/lib/*'" &&
    adb shell "su -c 'mv /data/data/com.estoty.game2048/lib/libcocos2dcpp.so.bak /data/data/com.estoty.game2048/lib/libcocos2dcpp.so'" &&
    adb push ./libs/armeabi/elfinfector /data/data/com.estoty.game2048/lib/ &&
    adb push ./libhello.so /data/data/com.estoty.game2048/lib/ &&
    adb shell "su -c '/data/data/com.estoty.game2048/lib/elfinfector /data/data/com.estoty.game2048/lib/libcocos2dcpp.so'" &&
    adb shell "su -c 'chmod 777 /data/data/com.estoty.game2048/lib/*'"
