# -*- coding: utf-8 -*-
import struct
import binascii
import hashlib
import zlib

class DexFile:
    def __init__(self, filepath):
        self.dex = open(filepath, "rb")
        self.strings = []
        self.types = []
        self.protos = []
        self.fields = []
        self.method = []
        self.map_type = {0x0: "header_item",
                         0x1: "string_id_item",
                         0x2: "type_id_item",
                         0x3: "proto_id_item",
                         0x4: "field_id_item",
                         0x5: "method_id_item",
                         0x6: "class_def_item",
                         0x1000: "map_list",
                         0x1001: "type_list",
                         0x1002: "annotation_set_ref_list",
                         0x1003: "annotation_set_item",
                         0x2000: "class_data_item",
                         0x2001: "code_item",
                         0x2002: "string_data_item",
                         0x2003: "debug_info_item",
                         0x2004: "annotation_item",
                         0x2005: "encoded_array_item",
                         0x2006: "annotations_directory_item"}

    # 这里定义一个读取字符串长度的函数
    def DecUnsignedLEB128(self, file):
        result = struct.unpack("i", file.read(4))[0]  # 读取4字节中的第一个字节
        result = result & 0x000000ff
        file.seek(-3, 1)  # 倒退回前面的第三个字节  # 不能直接从1字节强转为4字节，所以先取4字节，再清空3字节
        if (result > 0x7f):
            next = struct.unpack("i", file.read(4))[0]
            next = next & 0x000000ff  # 第一位是个位
            file.seek(-3, 1)
            result = (result & 0x7f) | (next & 0x7f) << 7
            if (next > 0x7f):
                next = struct.unpack("i", file.read(4))[0]
                next = next & 0x000000ff  # 加入十位
                file.seek(-3, 1)
                result = result | (next & 0x7f) << 14
                if (next > 0x7f):
                    next = struct.unpack("i", file.read(4))[0]
                    next = next & 0x000000ff
                    file.seek(-3, 1)
                    result = result | (next & 0x7f) << 21
                    if (next > 0x7f):
                        next = struct.unpack("i", file.read(4))[0]
                        next = next & 0x000000ff
                        file.seek(-3, 1)
                        result = result | next << 28
                        # print "result:", result
        return result

    def ReadDexHeader(self):
        print "----------------read Header-----------------------------"
        self.magic = struct.unpack("4s", self.dex.read(4))[0]  #4s表示4字节长的字符串  返回的是一个数组  所以需要用[0]取第一个
        print "magic code:", self.magic,  # 加逗号去掉最后的自动换行
        if self.magic != "dex\n":
            print "the file is not a dex file"
            self.CloseDexFile()
            exit(-1)
            return

        self.version = struct.unpack("4s", self.dex.read(4))[0]
        print "version:", self.version

        # 用adler32算法对这个字段之后的所有数据做的检验码，来验证这个dex文件是否损坏
        self.checksum = struct.unpack("i", self.dex.read(4))[0]        #  i	 int	integer	4
        print "checksum:", self.checksum

        # dex文件的签名值（是对这个字段之后的所有数据做的签名，用来唯一的标识这个dex文件）
        self.sig = struct.unpack("20s", self.dex.read(20))[0]       #20s表示20字节长的字符串  返回的是一个数组  所以需要用[0]取第一个
        # 将字符串以16进制打印出时用 binascii库
        print "signature:", str(binascii.b2a_hex(self.sig))

        self.filesize = struct.unpack("I", self.dex.read(4))[0]   # I	unsigned int	integer or long	4
        print "filesize:", self.filesize

        self.headersize = struct.unpack("I", self.dex.read(4))[0]
        print "headersize:", self.headersize

        # 默认情况下是以小顶端的方式存 小顶端时读出0x12345678,大顶端时读出0x78563412
        self.endiantag = struct.unpack("I", self.dex.read(4))[0]
        print "endiantag:", hex(self.endiantag)

        self.link_size = struct.unpack("I", self.dex.read(4))[0]
        print "link_size:", self.link_size

        self.link_off = struct.unpack("I", self.dex.read(4))[0]
        print "link_off:", self.link_off, "(", hex(self.link_off), ")"

        self.map_off = struct.unpack("I", self.dex.read(4))[0]
        print "map_off:", self.map_off, "(", hex(self.map_off), ")"

        self.string_num = struct.unpack("I", self.dex.read(4))[0]
        print "string_num:", self.string_num

        self.string_table_off = struct.unpack("I", self.dex.read(4))[0]
        print "string_table_off:", self.string_table_off, "(", hex(self.string_table_off), ")"

        self.type_num = struct.unpack("I", self.dex.read(4))[0]
        print "type_num:", self.type_num

        self.type_table_off = struct.unpack("I", self.dex.read(4))[0]
        print "type_table_off:", self.type_table_off, "(", hex(self.type_table_off), ")"

        self.proto_num = struct.unpack("I", self.dex.read(4))[0]
        print "proto_num:", self.proto_num

        self.proto_off = struct.unpack("I", self.dex.read(4))[0]
        print "proto_off:", self.proto_off, "(", hex(self.proto_off), ")"

        self.field_num = struct.unpack("I", self.dex.read(4))[0]
        print "field_num:", self.field_num

        self.field_off = struct.unpack("I", self.dex.read(4))[0]
        print "field_off:", self.field_off, "(", hex(self.field_off), ")"

        self.method_num = struct.unpack("I", self.dex.read(4))[0]
        print "method_num:", self.method_num

        self.method_off = struct.unpack("I", self.dex.read(4))[0]
        print "method_off:", self.method_off, "(", hex(self.method_off), ")"

        self.class_def_size = struct.unpack("I", self.dex.read(4))[0]
        print "class_def_size:", self.class_def_size

        self.class_def_off = struct.unpack("I", self.dex.read(4))[0]
        print "class_def_off:", self.class_def_off, "(", hex(self.class_def_off), ")"

        self.data_size = struct.unpack("I", self.dex.read(4))[0]
        print "data_size:", self.data_size

        self.data_off = struct.unpack("I", self.dex.read(4))[0]
        print "data_off:", self.data_off, "(", hex(self.data_off), ")"

    def CalSignature(self):
        # 4 + 4 + 4 + 20
        self.dex.seek(32)
        sigdata = self.dex.read()
        sha1 = hashlib.sha1()
        sha1.update(sigdata)
        print "signature:", sha1.hexdigest()

    def CalChecksum(self):
        self.dex.seek(12)
        checkdata = self.dex.read()
        checksum = zlib.adler32(checkdata)
        print "checksum:", checksum
    # 为了更方便的寻找到各个段
    def ReadMap(self):
        print "----------------read map-----------------------------"
        self.dex.seek(self.map_off)
        num = struct.unpack("I", self.dex.read(4))[0]
        print "has  map: " , num
        #设置一个循环，然后遍历打印出map的样子
        count=0
        while (count < num):
            Type, unused, size, offset = struct.unpack("HHII", self.dex.read(12))# H	unsigned short	integer	2       2  2  4  4
            print self.map_type[Type], " ", size, " ", hex(offset)
            count += 1

    def ReadStrings(self):
        print "----------------read String-----------------------------"
        self.dex.seek(0x38, 0)  # string table的偏移
        tmp = self.dex.read(8)
        string_count, string_table_off = struct.unpack("II", tmp)  # "II"是分别读取的意思
        print ("size:", string_count, " off:", string_table_off)
        #再从table_off开始偏移
        self.dex.seek(string_table_off, 0)
        DexStrOffList = []    #定义一个数组
        # 设置一个循环，然后遍历打印出String的样子
        count=0
        while (count < string_count):
            DexStrOffList.append(struct.unpack("i", self.dex.read(4))[0])  # unpack返回一个tuple 取第0个元素
            count += 1
        nonullcount = 0
        for stroff in DexStrOffList:
            self.dex.seek(stroff, 0)
            strlen = self.DecUnsignedLEB128(self.dex)
            if (strlen == 0):
                continue
            input = self.dex.read(strlen)
            self.part=struct.unpack(str(strlen) + "s", input)[0]
            self.strings.append(self.part)  # 解析不定长的字符串
            print hex(strlen),self.part
            nonullcount += 1
        outputfile = open("mystring.txt", "w")
        count = 0
        print ("string:", string_count)
        for i in self.strings:
            outputfile.write('%s\n' % i)  # 将元组中的元素写入文件
        outputfile.close()

    # type table的内容全都指向string table
    def ReadType(self):
        print "----------------read Type-----------------------------"
        DexTypeOffList = []
        count = 0
        self.dex.seek(self.type_table_off)
        while (count < self.type_num):
            DexTypeOffList.append(struct.unpack("i", self.dex.read(4))[0])
            count += 1

        print "Type Index:", DexTypeOffList
        # 要先读取string
        # type table中存的是string table的索引(从0开始)
        for i in DexTypeOffList:
            self.types.append(self.strings[i])
            print hex(i),self.strings[i]

    def ReadProto(self):
        print "----------------read Proto-----------------------------"
        count = 0
        while (count < self.proto_num):
            self.dex.seek(self.proto_off + count * 12)
            # 每个item有12个字节，共3个元素，分别表示原型简写，返回类型索引，参数
            shorty_idx, return_type_idx, parameters_off = struct.unpack("III", self.dex.read(12))
            self.protos.append(self.strings[shorty_idx])
            # shorty_idx是在strings表中的索引
            # return_type_idx是在types表中的索引
            print count, "proto:", self.strings[shorty_idx], " | ", self.types[return_type_idx], " | ",
            count += 1
            # 为0时表示没有参数
            if parameters_off == 0:
                print "\n",
                continue
            self.dex.seek(parameters_off)

            # 参数字段指向一个偏移，偏移处第一个元素是uint，表示参数个数，
            # 后面跟着各参数的索引(在types表中的索引)
            parnum = struct.unpack("I", self.dex.read(4))[0]
            i = 0
            while (i < parnum):
                # 每个索引为2字节，ushort类型
                idx = struct.unpack("H", self.dex.read(2))[0]
                # print idx,
                print self.types[idx],
                i += 1
            print "\n",

    # 变量
    def ReadFields(self):
        print "----------------read Fields-----------------------------"
        count = 0
        self.dex.seek(self.field_off)
        print "Fields:", self.field_num
        while (count < self.field_num):
            class_idx, type_idx, name_idx = struct.unpack("HHI", self.dex.read(8))
            print self.types[class_idx], " | ", self.types[type_idx], " | ", self.strings[name_idx]
            self.fields.append((self.types[class_idx], self.types[type_idx], self.strings[name_idx]))
            count += 1

    # 函数
    def ReadMethod(self):
        print "----------------read Method-----------------------------"
        count = 0
        self.dex.seek(self.method_off)
        print "Method:", self.method_num
        while (count < self.method_num):
            class_idx, proto_idx, name_idx = struct.unpack("HHI", self.dex.read(8))
            print self.types[class_idx], " | ", self.protos[proto_idx], " | ", self.strings[name_idx]
            self.method.append((self.types[class_idx], self.protos[proto_idx], self.strings[name_idx]))
            count += 1

    def ReadClass(self):
        count = 0

        print "----------------read Class-----------------------------"
        print "Class:", self.class_def_size
        while (count < self.class_def_size):
            self.dex.seek(self.class_def_off + count * 32)

            class_idx, \
            access_flags, \
            superclass_idx, \
            interfaces_off, \
            source_file_idx, \
            annotations_off, \
            class_data_off, \
            static_values_off = struct.unpack("IIIIIIII", self.dex.read(32))
            print "Class", count, self.types[class_idx]

            count += 1

            # class_data_off为0表示此类没有类数据（是接口）
            if class_data_off == 0:
                continue
            # 获取类数据，获取该类定义的方法和字段
            self.dex.seek(class_data_off)
            static_fields_size = self.DecUnsignedLEB128(self.dex)
            instance_fields_size = self.DecUnsignedLEB128(self.dex)
            direct_methods_size = self.DecUnsignedLEB128(self.dex)
            virtual_methods_size = self.DecUnsignedLEB128(self.dex)

            print "static_fields_size:", static_fields_size
            field_idx = 0
            while (static_fields_size > 0):
                field_idx_diff = self.DecUnsignedLEB128(self.dex)

                print field_idx, field_idx_diff, self.fields[field_idx + field_idx_diff]
                field_idx += field_idx_diff

                access_flags = self.DecUnsignedLEB128(self.dex)
                static_fields_size -= 1
                # do sth

            field_idx = 0
            print "instance_fields_size", instance_fields_size
            while (instance_fields_size > 0):
                field_idx_diff = self.DecUnsignedLEB128(self.dex)

                print self.fields[field_idx + field_idx_diff]
                field_idx += field_idx_diff

                access_flags = self.DecUnsignedLEB128(self.dex)
                instance_fields_size -= 1
                # do sth

            method_idx = 0
            print "direct_methods_size", direct_methods_size
            while (direct_methods_size > 0):
                method_idx_diff = self.DecUnsignedLEB128(self.dex)

                # 第一次循环时method_idx_diff直接指向method table中对应的项
                # 从第二次开始method_idx_diff就是与前一次的偏移值，方法在这个列表
                # 中一定是递增的
                print self.method[method_idx + method_idx_diff]
                method_idx += method_idx_diff

                access_flags = self.DecUnsignedLEB128(self.dex)
                code_off = self.DecUnsignedLEB128(self.dex)
                # print "method_idx_diff",method_idx_diff
                direct_methods_size -= 1

            method_idx = 0
            print "virtual_methods_size", virtual_methods_size
            while (virtual_methods_size > 0):
                method_idx_diff = self.DecUnsignedLEB128(self.dex)

                print self.method[method_idx + method_idx_diff]
                method_idx += method_idx_diff

                access_flags = self.DecUnsignedLEB128(self.dex)
                code_off = self.DecUnsignedLEB128(self.dex)
                virtual_methods_size -= 1

    def CloseDexFile(self):
        self.dex.close()

if __name__ == "__main__":
    df = DexFile("Hello.dex")
    df.ReadDexHeader()#读取头部分
    df.CalSignature()#读取签名
    df.CalChecksum()#读取校验字节
    df.ReadMap()#读取map表  先找到mapdataoff,再找读  以 HHII的结构
    df.ReadStrings()
    print "len :", len(df.strings), df.strings
    df.ReadType()
    df.ReadProto()
    df.ReadFields()
    df.ReadMethod()
    df.ReadClass()
    df.CloseDexFile()