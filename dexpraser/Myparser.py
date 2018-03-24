#!/usr/bin/env python
#coding:utf-8

# BSD 2-Clause License
#  解析dex头文件
#  混淆dex的字符串
#

import binascii

#type字段是个枚举类型 这里用键值对来表示这些内容
MAP_ITEM_TYPE_CODES = {
    0x0000 : "kDexTypeHeaderItem",
    0x0001 : "kDexTypeStringIdItem",
    0x0002 : "kDexTypeTypeIdItem",
    0x0003 : "kDexTypeProtoIdItem",
    0x0004 : "kDexTypeFieldIdItem",
    0x0005 : "kDexTypeMethodIdItem",
    0x0006 : "kDexTypeClassDefItem",
    0x1000 : "kDexTypeMapList",
    0x1001 : "kDexTypeTypeList",
    0x1002 : "kDexTypeAnnotationSetRefList",
    0x1003 : "kDexTypeAnnotationSetItem",
    0x2000 : "kDexTypeClassDataItem",
    0x2001 : "kDexTypeCodeItem",
    0x2002 : "kDexTypeStringDataItem",
    0x2003 : "kDexTypeDebugInfoItem",
    0x2004 : "kDexTypeAnnotationItem",
    0x2005 : "kDexTypeEncodedArrayItem",
    0x2006 : "kDexTypeAnnotationsDirectoryItem",
}

filepath='Hello.dex'



# #这里存储全局的DexStringid
# DexStringIdList=[]
#
# #这里存储全局的DexStringid
# DexTypeIdList = []
#
# #这里存储全局的DexProtoid
# DexProtoidList= []
#
# #这里存储全局的KDexTypeFieldItemList
# KDexTypeFieldItemList= []
#
# #这里存储全局的KDexTypeCalssDefItemList
# KDexTypeCalssDefItemList= []

#存储dex头的信息
class DexHeader(object):
    def __init__(self, ):
        super(DexHeader, self).__init__()
        self.f = None
        self.magic = None
        self.checksum = None
        self.signature = None
        self.file_size = None
        self.header_size = None
        self.endian_tag = None
        self.link_size = None
        self.link_off = None
        self.map_off = None
        self.string_ids_size = None
        self.string_ids_off = None
        self.type_ids_size = None
        self.type_ids_off = None
        self.proto_ids_size = None
        self.proto_ids_off = None
        self.field_ids_size = None
        self.field_ids_off = None
        self.method_ids_size = None
        self.method_ids_off = None
        self.class_defs_size = None
        self.class_defs_off = None
        self.data_size = None
        self.data_off = None
        self.DexStringIdList=[]
        self.DexTypeIdList=[]
        self.DexProtoidList=[]
        self.KDexTypeFieldItemList=[]
        self.KDexTypeCalssDefItemList=[]

class DexProtoIdObj(object):
    def __init__(self):
        super(DexProtoIdObj, self).__init__()
        self.shortyIdx=None
        self.returnTypeIdx=None
        self.parametersOff=None
        self.dexTypeList = None

class KDexTypeFieldItem(object):
    def __init__(self):
        super(KDexTypeFieldItem, self).__init__()
        self.classIdx = None
        self.typeIdx = None
        self.nameIdx = None

class KDexTypeMethodItem(object):
    def __init__(self):
        super(KDexTypeMethodItem, self).__init__()
        self.classIdx = None
        self.protoIdx = None
        self.nameIdx = None

class DexClassDef(object):
    def __init__(self):
        super(DexClassDef, self).__init__()
        self.classIdx = None
        self.accessFlags = None
        self.superclassIdx = None
        self.interfacesOff = None
        self.sourceFileIdx = None
        self.annotationsOff = None
        self.classDataOff = None
        self.staticValuesOff = None

class DexClassDataHandler(object):
    def __init__(self):
        super(DexClassDataHandler, self).__init__()
        self.staticfieldsSize = None
        self.instancefieldsSize = None
        self.directmethodsSize = None
        self.virtualmethodsSize = None

class DexField(object):
    def __init__(self):
        super(DexField, self).__init__()
        self.fliedIdx = None
        self.accessFlags = None

class DexCode(object):
    """docstring for DexCode"""
    def __init__(self):
        super(DexCode, self).__init__()
        self.registersSize = None
        self.insSize = None
        self.outsSize = None
        self.triesSize = None
        self.debugInfoOff = None
        self.insnsSize = None
        self.insns = None


#初始化dexheader
def init_header(self):
    with open(filepath,'rb') as f:
       self.magic=f.read(8)#以str形式  这里输出要转为16
       self.checksum=f.read(4)
       self.signature=f.read(20)
       self.file_size=f.read(4)
       self.header_size=f.read(4)#
       self.endian_tag=f.read(4)#
       self.link_size=f.read(4)#
       self.link_off=f.read(4)#
       self.map_off=f.read(4)     #
       self.string_ids_size=f.read(4)#
       self.string_ids_off=f.read(4)#
       self.type_ids_size=f.read(4)#
       self.type_ids_off=f.read(4)#
       self.proto_ids_size=f.read(4)#
       self.proto_ids_off=f.read(4)#
       self.field_ids_size=f.read(4)#
       self.field_ids_off=f.read(4)#
       self.method_ids_size=f.read(4)#
       self.method_ids_off=f.read(4)#
       self.class_defs_size=f.read(4)#
       self.class_defs_off=f.read(4)#
       self.data_size=f.read(4)#
       self.data_off=f.read(4)#

#输出dexheader
def print_header(self):
        print '[+] magic:\t0x' + binascii.b2a_hex(self.magic)
        print '[+] checksum:\t0x' + binascii.b2a_hex(self.checksum[::-1])
        print '[+] signature:\t0x' + binascii.b2a_hex(self.signature)
        print '[+] file_size:\t0x' + binascii.b2a_hex(self.file_size[::-1])
        print '[+] header_size:\t0x' + binascii.b2a_hex(self.header_size[::-1])
        print '[+] endian_tag:\t0x' + binascii.b2a_hex(self.endian_tag[::-1])
        print '[+] link_size:\t0x' + binascii.b2a_hex(self.link_size[::-1])
        print '[+] link_off:\t0x' + binascii.b2a_hex(self.link_off[::-1])
        print '[+] map_off:\t0x' + binascii.b2a_hex(self.map_off[::-1])
        print '[+] string_ids_size:\t0x' + binascii.b2a_hex(self.string_ids_size[::-1])
        print '[+] string_ids_off:\t0x' + binascii.b2a_hex(self.string_ids_off[::-1])
        print '[+] type_ids_size:\t0x' + binascii.b2a_hex(self.type_ids_size[::-1])
        print '[+] type_ids_off:\t0x' + binascii.b2a_hex(self.type_ids_off[::-1])
        print '[+] proto_ids_size:\t0x' + binascii.b2a_hex(self.proto_ids_size[::-1])
        print '[+] proto_ids_off:\t0x' + binascii.b2a_hex(self.proto_ids_off[::-1])
        print '[+] field_ids_size:\t0x' + binascii.b2a_hex(self.field_ids_size[::-1])
        print '[+] field_ids_off:\t0x' + binascii.b2a_hex(self.field_ids_off[::-1])
        print '[+] method_ids_size:\t0x' + binascii.b2a_hex(self.method_ids_size[::-1])
        print '[+] method_ids_off:\t0x' + binascii.b2a_hex(self.method_ids_off[::-1])
        print '[+] class_defs_size:\t0x' + binascii.b2a_hex(self.class_defs_size[::-1])
        print '[+] class_defs_off:\t0x' + binascii.b2a_hex(self.class_defs_off[::-1])
        print '[+] data_size:\t0x' + binascii.b2a_hex(self.data_size[::-1])
        print '[+] data_off:\t0x' + binascii.b2a_hex(self.data_off[::-1])

#解析DexMapList
def print_DexMapList(self):
    with open(filepath, 'rb') as f:
        #直接从str 转成16进制整数
        f.seek(int(binascii.b2a_hex(self.map_off[::-1]),16), 0)
        size = f.read(4)
        for i in range(0,int(binascii.b2a_hex(size[::-1]),16)):
            print "\n[+]"+" #DexMapItem "+str(i)
            u2_type=f.read(2)
            print "u2_type  : "+binascii.b2a_hex(u2_type[::-1])+"   "+ MAP_ITEM_TYPE_CODES[int(binascii.b2a_hex(u2_type[::-1]),16)]
            u2_uused=f.read(2)
            print "u2_uused : "+binascii.b2a_hex(u2_uused[::-1])
            u4_size=f.read(4)
            print "u4_size  : " + str(int(binascii.b2a_hex(u4_size[::-1]),16))
            u4_off=f.read(4)
            print "u4_off   : " + hex(int(binascii.b2a_hex(u4_off[::-1]),16))+"\n"

#初始化字符串 为MUTF-8
def init_DexString_id(x):
    with open(filepath, 'rb+') as f:
        #print int(binascii.b2a_hex(x[::-1]), 16) 位移
        f.seek(int(binascii.b2a_hex(x[::-1]), 16), 0)
        StrSize=f.read(1)
        bin=f.read(int(binascii.b2a_hex(StrSize[::-1]), 16))
        print bin

        # return binascii.a2b_hex(binascii.b2a_hex(bin)) #从bin转到ascii的值
        return bin


def print_DexString_id(self):
    with open(filepath,'rb') as f:
        print  "\n[+]" + " #DexString "
        f.seek(int(binascii.b2a_hex(self.string_ids_off[::-1]), 16), 0)
        for i in range(0,int(binascii.b2a_hex(self.string_ids_size[::-1]),16)):
            #print "\n[+]" + " #DexString " + str(i)
            u4_StringDataOff=f.read(4);
            #print "u4_StringDataOff : " + hex(int(binascii.b2a_hex(u4_StringDataOff[::-1]), 16))
            #python解析MUTF-8的类型
            #print init_DexString_id(u4_StringDataOff).strip()
            self.DexStringIdList.append(init_DexString_id(u4_StringDataOff))

def print_DexType_id(self):
    with open(filepath,'rb') as f:
        print  "\n[+]" + " #DexTypeid "
        f.seek(int(binascii.b2a_hex(self.type_ids_off[::-1]), 16), 0)
        for i in range(0,int(binascii.b2a_hex(self.type_ids_size[::-1]),16)):
            #print "\n[+]" + " #DexTypeid " + str(i)
            u4_descriptoridx=f.read(4);
            #print  int(binascii.b2a_hex(u4_descriptoridx[::-1]), 16)
            print "u4_descriptoridx" +str(i)+" : " +self.DexStringIdList[int(binascii.b2a_hex(u4_descriptoridx[::-1]), 16)]
            #python解析MUTF-8的类型
            self.DexTypeIdList.append(self.DexStringIdList[int(binascii.b2a_hex(u4_descriptoridx[::-1]), 16)])

#初始化字符串 为MUTF-8
def init_DexTypeList(self,x):
    dexTypeList=[]
    with open(filepath, 'rb') as f:
        #print int(binascii.b2a_hex(x[::-1]), 16) 位移
        f.seek(int(binascii.b2a_hex(x[::-1]), 16), 0)
        StrSize=f.read(4)
        print "DexTypeItemList_Size :"+str(int(binascii.b2a_hex(StrSize[::-1]),16))
        for i in range(0,int(binascii.b2a_hex(StrSize[::-1]),16)):
            item = f.read(2)
            print "DexTypeItem  _"+  self.DexTypeIdList[int(binascii.b2a_hex(item[::-1]),16)]
            dexTypeList.append(self.DexTypeIdList[int(binascii.b2a_hex(item[::-1]),16)])
        return dexTypeList
        #return binascii.a2b_hex(binascii.b2a_hex(f.read(int(binascii.b2a_hex(StrSize[::-1]),16)))) #从bin转到ascii的值

def print_DexProto_id(self):
    with open(filepath,'rb') as f:
        print  "\n[+]" + " #DexPtotoId "
        f.seek(int(binascii.b2a_hex(self.proto_ids_off[::-1]), 16), 0)
        for i in range(0,int(binascii.b2a_hex(self.proto_ids_size[::-1]),16)):
            dexProtoIdObj=DexProtoIdObj()
            #print "\n[+]" + " #DexTypeid " + str(i)
            unit_shorty_id=f.read(4)
            print "#unit_shorty_id " + str(i) + "  " + self.DexStringIdList[int(binascii.b2a_hex(unit_shorty_id[::-1]), 16)]
            dexProtoIdObj.shortyIdx=self.DexStringIdList[int(binascii.b2a_hex(unit_shorty_id[::-1]), 16)]
            unit_return_type_id=f.read(4)
            print "u4_descriptoridx" + str(i) + " : " + self.DexStringIdList[int(binascii.b2a_hex(unit_return_type_id[::-1]), 16)]
            dexProtoIdObj.returnTypeIdx=self.DexStringIdList[int(binascii.b2a_hex(unit_return_type_id[::-1]), 16)]
            unit_parameters_off=f.read(4)
            print "u4_descriptoridx" + str(i) + " : " + str(int(binascii.b2a_hex(unit_parameters_off[::-1]), 16))
            dexProtoIdObj.parametersOff = int(binascii.b2a_hex(unit_parameters_off[::-1]), 16)
            if int(binascii.b2a_hex(unit_parameters_off[::-1]),16)==0:
                dexProtoIdObj.dexTypeList=None
            else:
                dexProtoIdObj.dexTypeList =init_DexTypeList(self,unit_parameters_off)
            print "\n"
            self.DexProtoidList.append(dexProtoIdObj)

def print_KDextypeFieldIdItem(self):
    with open(filepath,'rb') as f:
        print  "\n[+]" + " #KDextypeFieldIdItem "
        f.seek(int(binascii.b2a_hex(self.field_ids_off[::-1]), 16), 0)
        for i in range(0,int(binascii.b2a_hex(self.field_ids_size[::-1]),16)):
            DexFieldID=KDexTypeFieldItem()
            #print "\n[+]" + " #DexTypeid " + str(i)
            classIdx=f.read(2);
            print self.DexTypeIdList[int(binascii.b2a_hex(classIdx[::-1]),16)]
            DexFieldID.classIdx=self.DexTypeIdList[int(binascii.b2a_hex(classIdx[::-1]),16)]
            typeIdx=f.read(2);
            print self.DexTypeIdList[int(binascii.b2a_hex(typeIdx[::-1]),16)]
            DexFieldID.typeIdx=self.DexTypeIdList[int(binascii.b2a_hex(typeIdx[::-1]),16)]
            nameIdx=f.read(4);
            print self.DexStringIdList[int(binascii.b2a_hex(nameIdx[::-1]), 16)]+"\n"
            DexFieldID.nameIdx=self.DexStringIdList[int(binascii.b2a_hex(nameIdx[::-1]), 16)]
            self.KDexTypeFieldItemList.append(DexFieldID)
            #python解析MUTF-8的类型

def print_KDextypeMethodIdItem(self):
    with open(filepath,'rb') as f:
        print  "\n[+]" + " #KDextypeMethodIdItem "
        f.seek(int(binascii.b2a_hex(self.method_ids_off[::-1]), 16), 0)
        for i in range(0,int(binascii.b2a_hex(self.method_ids_size[::-1]),16)):
            DexFieldID=KDexTypeMethodItem()
            #print "\n[+]" + " #DexTypeid " + str(i)
            classIdx=f.read(2);
            print self.DexTypeIdList[int(binascii.b2a_hex(classIdx[::-1]),16)]
            DexFieldID.classIdx=self.DexTypeIdList[int(binascii.b2a_hex(classIdx[::-1]),16)]
            protoIdx=f.read(2);
            print getattr(self.DexProtoidList[int(binascii.b2a_hex(protoIdx[::-1]),16)],'shortyIdx')#[dexProtoIdObj1,dexProtoIdObj2....]获取类中的元素的数据
            DexFieldID.typeIdx=getattr(self.DexProtoidList[int(binascii.b2a_hex(protoIdx[::-1]),16)],'shortyIdx')
            nameIdx=f.read(4);
            print self.DexStringIdList[int(binascii.b2a_hex(nameIdx[::-1]), 16)]+"\n"
            DexFieldID.nameIdx=self.DexStringIdList[int(binascii.b2a_hex(nameIdx[::-1]), 16)]
            self.KDexTypeFieldItemList.append(DexFieldID)
            #python解析MUTF-8的类型


def readUnsignedLeb128(hex_value):
    byte_counts = len(hex_value) / 2

    # 找出第一个不是0的byte位置
    index = 0
    for i in range(byte_counts):
        v1 = int(hex_value[i * 2:i * 2 + 2], 16)
        if v1 > 0:
            index = i
            break

    hex_value = hex_value[index * 2:]
    byte_counts = len(hex_value) / 2

    result = 0
    for i in range(byte_counts):
        cur = int(hex_value[i * 2:i * 2 + 2], 16)
        if cur > 0x7f:
            result = result | ((cur & 0x7f) << (7 * i))
        else:
            result = result | ((cur & 0x7f) << (7 * i))
            break
    return result


def init_DexCode(self,off):
    dexcode=DexCode()
    with open(filepath, 'rb') as f:
        print  "                   [+]" + " # DexCode "
        f.seek(off, 0)
        registersSize=f.read(2)
        print "                    [+]registersSize     " + binascii.b2a_hex(registersSize)
        insSize = f.read(2)
        print "                    [+]insSize           " + binascii.b2a_hex(insSize)
        outsSize = f.read(2)
        print "                    [+]insSoutsSize      " + binascii.b2a_hex(outsSize)
        triesSize = f.read(2)
        print "                    [+]triesSize         " + binascii.b2a_hex(triesSize)
        debugInfoOff = f.read(4)
        print "                    [+]debugInfoOff      " + binascii.b2a_hex(debugInfoOff)
        insnsSize = f.read(4)
        print "                    [+]insnsSize         " + binascii.b2a_hex(insnsSize)
        for i in range(int(binascii.b2a_hex(insnsSize[::-1]),16)):
            insns = f.read(2)
            print "                              [+]insns ["+str(i)+"]       " + binascii.b2a_hex(insns)


def inti_DexClassData(self,off):
    dexclassdatahandler = DexClassDataHandler()
    with open(filepath, 'rb') as f:
        print  "\n[+]" + " # DexClassData "
        f.seek(off, 0)
        staticfieldsSize=f.read(1)
        print "staticfieldsSize     " + binascii.b2a_hex(staticfieldsSize)
        instancefieldsSize=f.read(1)
        print "instancefieldsSize   "+binascii.b2a_hex(instancefieldsSize)
        directmethodsSize=f.read(1)
        print "directmethodsSize    "+binascii.b2a_hex(directmethodsSize)
        virtualmethodsSize=f.read(1)
        print "virtualmethodsSize   "+binascii.b2a_hex(virtualmethodsSize)
        """
                   struct DexField{
                       u4 fieldIdx;
                       u4 accessFlags;
                   }
                   """
        for i in range(int(binascii.b2a_hex(staticfieldsSize),16)):
            array = []
            length = 0
            for j in range(2):
                cur_bytes_hex = binascii.b2a_hex(f.read(1))
                length += 1
                cur_bytes = int(cur_bytes_hex, 16)
                value = cur_bytes_hex
                while cur_bytes > 0x7f:
                    cur_bytes_hex = binascii.b2a_hex(f.read(1))
                    length += 1
                    cur_bytes = int(cur_bytes_hex, 16)
                    value += cur_bytes_hex
                array.append(value)
            dexField = DexField()
            dexField.fieldIdx = readUnsignedLeb128(array[0])
            dexField.accessFlags =readUnsignedLeb128(array[1])
            print "###  staticfields   " +str(i)
            print "     [+] fliedIdx:  " +hex(dexField.fieldIdx)
            print "     [+] accessFlags" +hex(dexField.accessFlags)
        for i in range(int(binascii.b2a_hex(instancefieldsSize),16)):
            array = []
            length = 0
            for j in range(2):
                cur_bytes_hex = binascii.b2a_hex(f.read(1))
                length += 1
                cur_bytes = int(cur_bytes_hex, 16)
                value = cur_bytes_hex
                while cur_bytes > 0x7f:
                    cur_bytes_hex = binascii.b2a_hex(f.read(1))
                    length += 1
                    cur_bytes = int(cur_bytes_hex, 16)
                    value += cur_bytes_hex
                array.append(value)
            dexField = DexField()
            dexField.fieldIdx = readUnsignedLeb128(array[0])
            dexField.accessFlags =readUnsignedLeb128(array[1])
            print "###  instancefields   " + str(i)
            print "     [+] fliedIdx:  " +hex(dexField.fieldIdx)
            print "     [+] accessFlags" +hex(dexField.accessFlags)
        for i in range(int(binascii.b2a_hex(directmethodsSize),16)):
            array = []
            length = 0
            for j in range(3):
                cur_bytes_hex = binascii.b2a_hex(f.read(1))
                length += 1
                cur_bytes = int(cur_bytes_hex, 16)
                value = cur_bytes_hex
                while cur_bytes > 0x7f:
                    cur_bytes_hex = binascii.b2a_hex(f.read(1))
                    length += 1
                    cur_bytes = int(cur_bytes_hex, 16)
                    value += cur_bytes_hex
                array.append(value)
            print "###  directmethods   " +str(i)
            methodIdx = readUnsignedLeb128(array[0])
            accessFlags = readUnsignedLeb128(array[1])#解析accessFlags遇到很大的困难
            codeOff = readUnsignedLeb128(array[2])
            print "     [+] MethodIdx:  " + hex(methodIdx)
            print "     [+] accessFlags " + hex(accessFlags)
            print "     [+] codeOff     " + hex(codeOff)
            init_DexCode(self,codeOff)
        for i in range(int(binascii.b2a_hex(virtualmethodsSize),16)):
            array = []
            length = 0
            for j in range(3):
                cur_bytes_hex = binascii.b2a_hex(f.read(1))
                length += 1
                cur_bytes = int(cur_bytes_hex, 16)
                value = cur_bytes_hex
                while cur_bytes > 0x7f:
                    cur_bytes_hex = binascii.b2a_hex(f.read(1))
                    length += 1
                    cur_bytes = int(cur_bytes_hex, 16)
                    value += cur_bytes_hex
                array.append(value)
            print "###  virtualmethods   " +str(i)
            methodIdx = readUnsignedLeb128(array[0])
            accessFlags = readUnsignedLeb128(array[1])#解析accessFlags遇到很大的困难
            codeOff = readUnsignedLeb128(array[2])
            print "     [+] MethodIdx:  " + hex(methodIdx)
            print "     [+] accessFlags " + hex(accessFlags)
            print "     [+] codeOff     " + hex(codeOff)
        print "----------------------------------------"
    return dexclassdatahandler


def print_KDextypeClassDefItem(self):
    with open(filepath,'rb') as f:
        f.seek(int(binascii.b2a_hex(self.class_defs_off[::-1]), 16), 0)
        for i in range(0,int(binascii.b2a_hex(self.class_defs_size[::-1]),16)):
            print  "\n[+]" + " #KDextypeClassDefItem "  +str(i)
            dexclassdef=DexClassDef()
            classIdx = f.read(4);
            print "classIdx  :     "+self.DexTypeIdList[int(binascii.b2a_hex(classIdx[::-1]), 16)]
            dexclassdef.classIdx= self.DexTypeIdList[int(binascii.b2a_hex(classIdx[::-1]), 16)]
            accessflag = f.read(4);
            print "accessflag:     " + binascii.b2a_hex(accessflag[::-1])
            dexclassdef.accessflag= int(binascii.b2a_hex(accessflag[::-1]), 16)
            superclassIdx = f.read(4);
            print "superclassIdx:  " + self.DexTypeIdList[int(binascii.b2a_hex(superclassIdx[::-1]),16)]
            dexclassdef.superclassIdx= self.DexTypeIdList[int(binascii.b2a_hex(superclassIdx[::-1]), 16)]
            interfacesOff = f.read(4);
            print "interfacesOff:  " + str(getattr(self.DexProtoidList[int(binascii.b2a_hex(interfacesOff[::-1]),16)],'parametersOff'))
            dexclassdef.interfacesOff= int(binascii.b2a_hex(interfacesOff[::-1]), 16)
            sourceFileIdx = f.read(4);
            print "sourceFileIdx:  " + self.DexStringIdList[int(binascii.b2a_hex(sourceFileIdx[::-1]),16)]
            dexclassdef.sourceFileIdx= self.DexStringIdList[int(binascii.b2a_hex(sourceFileIdx[::-1]),16)]
            annotationsOff = f.read(4);
            print "annotationsOff:  " + str(int(binascii.b2a_hex(annotationsOff[::-1]),16))
            dexclassdef.annotationsOff= int(binascii.b2a_hex(sourceFileIdx[::-1]),16)
            classDataOff = f.read(4);
            print "classDataOff:  " + str(int(binascii.b2a_hex(classDataOff[::-1]),16))
            staticValuesOff = f.read(4);
            print "staticValuesOff:  " + str(int(binascii.b2a_hex(staticValuesOff[::-1]),16))
            dexclassdef.staticValuesOff=int(binascii.b2a_hex(staticValuesOff[::-1]),16)
            dexclassdef.classDataOff=inti_DexClassData(self, int(binascii.b2a_hex(classDataOff[::-1]), 16))
            self.KDexTypeCalssDefItemList.append(dexclassdef)

def main():
    Dex=DexHeader()#定义dexheader
    init_header(Dex)#初始化dexheader
    print_header(Dex)#打印header
    print_DexMapList(Dex)
    print_DexString_id(Dex)
    print_DexType_id(Dex)
    print_DexProto_id(Dex)
    print_KDextypeFieldIdItem(Dex)
    print_KDextypeMethodIdItem(Dex)
    print_KDextypeClassDefItem(Dex)

    # #-------华丽的分割线------------------------#
    # #计算checksum和signatrue
    # str='Hello, Android!'
    # print str.encode('hex')


if __name__ == '__main__':
    main()
        
