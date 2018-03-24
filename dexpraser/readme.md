### 解析DEX结构的一个项目

![DEX文件图](![](http://p19ocs37q.bkt.clouddn.com/15219024261896.jpg)

在[android源码](https://android.googlesource.com/platform/dalvik/+/android-4.4.2_r2/libdex/DexFile.h)
中的/dalvik/libdex/DexFile.h找到关于dexfile的定义




```
/*
 * Structure representing a DEX file.
 *
 * Code should regard DexFile as opaque, using the API calls provided here
 * to access specific structures.
 */
typedef struct DexFile {
    /* directly-mapped "opt" header */
    const DexOptHeader* pOptHeader;

    /* pointers to directly-mapped structs and arrays in base DEX */
    const DexHeader*    pHeader;
    const DexStringId*  pStringIds;
    const DexTypeId*    pTypeIds;
    const DexFieldId*   pFieldIds;
    const DexMethodId*  pMethodIds;
    const DexProtoId*   pProtoIds;
    const DexClassDef*  pClassDefs;
    const DexLink*      pLinkData;

    /* mapped in "auxillary" section */
    const DexClassLookup* pClassLookup;

    /* points to start of DEX file data */
    const u1*           baseAddr;

    /* track memory overhead for auxillary structures */
    int                 overhead;

    /* additional app-specific data structures associated with the DEX */
    void*               auxData;
} DexFile;
```

先创建java 源文件如下：

```
public class Hello
{
public static void main(String[] argc)
{
System.out.println(“Hello, Android!\n”);
}
}
```

在当前工作路径下 ， 编译方法如下 ：

* 1. 编译成 java class 文件

执行命令 ： javac Hello.java
编译完成后 ，目录下生成 Hello.class 文件 。可以使用命令 java Hello 来测试下 ，会输出代码中的 “Hello， Android！” 的字符串 。

* 2. 编译成 dex 文件

编译工具在 Android SDK 的路径如下 ，其中 19.0.1 是Android SDK build_tools 的版本 ，请按照在本地安装的 build_tools 版本来 。建议该路径加载到 PATH 路径下 ，否则引用 dx 工具时需要使用绝对路径 ：./build-tools/19.0.1/dx
执行命令 ： `dx –dex –output=Hello.dex Hello.class`
编译正常会生成 Hello.dex 文件 。

* 3. 使用 ADB 运行测试

测试命令和输出结果如下 ：

```$ adb root
$ adb push Hello.dex /sdcard/
$ adb shell
root@maguro:/ # dalvikvm -cp /sdcard/Hello.dex Hello
Hello, Android!
```

用python解析Hello.dex后的输出结果
```
[+] magic:	0x6465780a30333500
[+] checksum:	0x14a36056
[+] signature:	0xc6e6d38bb345f70e58b9b303c2beaf3036a156b2
[+] file_size:	0x000002e4
[+] header_size:	0x00000070
[+] endian_tag:	0x12345678
[+] link_size:	0x00000000
[+] link_off:	0x00000000
[+] map_off:	0x00000244
[+] string_ids_size:	0x0000000e
[+] string_ids_off:	0x00000070
[+] type_ids_size:	0x00000007
[+] type_ids_off:	0x000000a8
[+] proto_ids_size:	0x00000003
[+] proto_ids_off:	0x000000c4
[+] field_ids_size:	0x00000001
[+] field_ids_off:	0x000000e8
[+] method_ids_size:	0x00000004
[+] method_ids_off:	0x000000f0
[+] class_defs_size:	0x00000001
[+] class_defs_off:	0x00000110
[+] data_size:	0x000001b4
[+] data_off:	0x00000130

[+] #DexMapItem 0
u2_type  : 0000   kDexTypeHeaderItem
u2_uused : 0000
u4_size  : 1
u4_off   : 0x0


[+] #DexMapItem 1
u2_type  : 0001   kDexTypeStringIdItem
u2_uused : 0000
u4_size  : 14
u4_off   : 0x70


[+] #DexMapItem 2
u2_type  : 0002   kDexTypeTypeIdItem
u2_uused : 0000
u4_size  : 7
u4_off   : 0xa8


[+] #DexMapItem 3
u2_type  : 0003   kDexTypeProtoIdItem
u2_uused : 0000
u4_size  : 3
u4_off   : 0xc4


[+] #DexMapItem 4
u2_type  : 0004   kDexTypeFieldIdItem
u2_uused : 0000
u4_size  : 1
u4_off   : 0xe8


[+] #DexMapItem 5
u2_type  : 0005   kDexTypeMethodIdItem
u2_uused : 0000
u4_size  : 4
u4_off   : 0xf0


[+] #DexMapItem 6
u2_type  : 0006   kDexTypeClassDefItem
u2_uused : 0000
u4_size  : 1
u4_off   : 0x110


[+] #DexMapItem 7
u2_type  : 2001   kDexTypeCodeItem
u2_uused : 0000
u4_size  : 2
u4_off   : 0x130


[+] #DexMapItem 8
u2_type  : 1001   kDexTypeTypeList
u2_uused : 0000
u4_size  : 2
u4_off   : 0x168


[+] #DexMapItem 9
u2_type  : 2002   kDexTypeStringDataItem
u2_uused : 0000
u4_size  : 14
u4_off   : 0x176


[+] #DexMapItem 10
u2_type  : 2003   kDexTypeDebugInfoItem
u2_uused : 0000
u4_size  : 2
u4_off   : 0x228


[+] #DexMapItem 11
u2_type  : 2000   kDexTypeClassDataItem
u2_uused : 0000
u4_size  : 1
u4_off   : 0x234


[+] #DexMapItem 12
u2_type  : 1000   kDexTypeMapList
u2_uused : 0000
u4_size  : 1
u4_off   : 0x244


[+] #DexString 
<init>
Hello, Androie!

Hello.java
LHello;
Ljava/io/PrintStream;
Ljava/lang/Object;
Ljava/lang/String;
Ljava/lang/System;
V
VL
[Ljava/lang/String;
main
out
println

[+] #DexTypeid 
u4_descriptoridx0 : LHello;
u4_descriptoridx1 : Ljava/io/PrintStream;
u4_descriptoridx2 : Ljava/lang/Object;
u4_descriptoridx3 : Ljava/lang/String;
u4_descriptoridx4 : Ljava/lang/System;
u4_descriptoridx5 : V
u4_descriptoridx6 : [Ljava/lang/String;

[+] #DexPtotoId 
#unit_shorty_id 0  V
u4_descriptoridx0 : Ljava/lang/Object;
u4_descriptoridx0 : 0


#unit_shorty_id 1  VL
u4_descriptoridx1 : Ljava/lang/Object;
u4_descriptoridx1 : 360
DexTypeItemList_Size :1
DexTypeItem  _Ljava/lang/String;


#unit_shorty_id 2  VL
u4_descriptoridx2 : Ljava/lang/Object;
u4_descriptoridx2 : 368
DexTypeItemList_Size :1
DexTypeItem  _[Ljava/lang/String;



[+] #KDextypeFieldIdItem 
Ljava/lang/System;
Ljava/io/PrintStream;
out


[+] #KDextypeMethodIdItem 
LHello;
V
<init>

LHello;
VL
main

Ljava/io/PrintStream;
VL
println

Ljava/lang/Object;
V
<init>


[+] #KDextypeClassDefItem 0
classIdx  :     LHello;
accessflag:     00000001
superclassIdx:  Ljava/lang/Object;
interfacesOff:  0
sourceFileIdx:  Hello.java
annotationsOff:  0
classDataOff:  564
staticValuesOff:  0

[+] # DexClassData 
staticfieldsSize     00
instancefieldsSize   00
directmethodsSize    02
virtualmethodsSize   00
###  directmethods   0
     [+] MethodIdx:  0x0
     [+] accessFlags 0x10001
     [+] codeOff     0x130
                   [+] # DexCode 
                    [+]registersSize     0100
                    [+]insSize           0100
                    [+]insSoutsSize      0100
                    [+]triesSize         0000
                    [+]debugInfoOff      28020000
                    [+]insnsSize         04000000
                              [+]insns [0]       7010
                              [+]insns [1]       0300
                              [+]insns [2]       0000
                              [+]insns [3]       0e00
###  directmethods   1
     [+] MethodIdx:  0x1
     [+] accessFlags 0x9
     [+] codeOff     0x148
                   [+] # DexCode 
                    [+]registersSize     0300
                    [+]insSize           0100
                    [+]insSoutsSize      0200
                    [+]triesSize         0000
                    [+]debugInfoOff      2d020000
                    [+]insnsSize         08000000
                              [+]insns [0]       6200
                              [+]insns [1]       0000
                              [+]insns [2]       1a01
                              [+]insns [3]       0100
                              [+]insns [4]       6e20
                              [+]insns [5]       0200
                              [+]insns [6]       1000
                              [+]insns [7]       0e00
----------------------------------------
```

参考：

http://www.wjdiankong.cn/archives/579

https://github.com/guanchao/dexParser


