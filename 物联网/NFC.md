# 概述
首先说明几个概念:  
>* **RFID**: 无线射频识别即射频识别技术（Radio Frequency Identification，RFID），是自动识别技术的一种，通过无线射频方式进行非接触双向数据通信，利用无线射频方式对记录媒体（电子标签或射频卡）进行读写，从而达到识别目标和数据交换的目的，其被认为是21世纪最具发展潜力的信息技术之一。 

>* **NFC**： NFC=Near Field Communication, 中文术语是近距离无线通信, 是一种采用13.56MHz频带的近距离无线通信技术，允许电子设备之间进行非接触式点对点式数据传输与交换(在10cm内)。这个技术由RFID技术演变而来，并向下兼容RFID，从这个概念来说：NFC属于RFID，NFC是RFID技术里的一个类别。

---
    
# RFID卡类型
## 卡类型
关于卡的类型, 最常见的是**IC卡**, **ID卡**   
* IC卡: `IC卡`是一大类,其可以根据不同的分类标准继续细分。
>   * 根据卡中所镶嵌的集成电路芯片的不同可以分成两大类，分别是 **存储器卡** 和 **CPU卡(智能卡)** .   
>>**存储器卡** 采用存储器芯片作为卡芯，只有“硬件”组成，包括**数据存储器**和**安全逻辑控制**等；**智能卡** 采用 **微处理器芯片** 作为卡芯，由硬件和软件共同组成，属于卡上单片机系统。 

>   * 按卡上数据的读写方法来分类，有 **接触型IC卡** 和 **非接触型IC卡** 两种。
  >> 当前使用广泛的是 **接触型IC卡** ，其表面可以看到一个方型镀金接口，共有八个或六个镀金触点，用于与读写器接触，通过电流信号完成读写。读写操作（称为刷卡）时须将IC卡插入读写器，读写完毕，卡片自动弹出，或人为抽出。接触式IC卡刷卡相对慢，但可靠性高，多用于存储信息量大，读写操作复杂的场合。          
  
  >>**非接触型IC卡** 具有接触式IC卡同样的芯片技术和特性，最大的区别在于卡上设有射频信号或红外线收发器，在一定距离内即可收发读写器的信号，因而和读写设备之间无机械接触。在前述IC卡的电路基础上带有射频收发及相关电路的非接触IC卡被称作“射频卡”或“RF卡”。 这种IC卡常用于身份验证，电子门禁等场合。卡上记录信息简单，读写要求不高，卡型变化也较大，可以作成徽章等形式。
  因此，不但可以存储大量信息，具有极强的保密性能，并且抗干扰、无磨损、寿命长。因此在广泛的领域中得到应用

* ID卡: 只存储了`ID`号，设备识别ID号，没有算法可言，容易复制，安全性低；

---
## IC和ID卡图片
### IC卡存储卡
![IC卡](https://i.loli.net/2020/05/19/HTj7JvE9m4CxYzy.png)

![IC卡](https://i.loli.net/2020/05/19/lFtdpNhzuqxEZYv.png)

### IC智能卡(CPU卡)
![image](https://i.loli.net/2020/05/19/k1JXFCTiUmWne5c.png)

### ID卡
![image](https://i.loli.net/2020/05/19/YdufHqeCo6l91bS.png)

---

## NFC标签
**NFC标签** 是一种基于IC存储卡上(采用`13.56MHz RFID`技术), 通过约定的存储格式和访问接口,       实现对NFC设备的标准化存取的一种设备。目前其支持 `ISO14443-A`, `ISO14443-B` 以及飞利浦的`MIFARE`标准。
故常称之为 `NFC-A`,` NFC-B` 以及 `NFC-F`类型, 其主要是表面链路层的编码方式。

目前 **NFC标签** 主要有如下几种类型:
> * 第1类标签：简称`Type 1 Tag` 此类型基于ISO14443A标准。此类标签具有可读、重新写入的能力，用户可将其配置为只读。存储能力为96字节，用来存网址URL或其他小量数据富富有余。然而，内存可被扩充到2k字节。此类NFC标签的通信速度为106 kbit/s。此类标签简洁，故成本效益较好，适用于许多NFC应用

> * 第2类标签：此类标签也是基于ISO14443A，具有可读、重新写入的能力，用户可将其配置为只读。其基本内存大小为48字节，但可被扩充到2k字节。通信速度也是106 kbit/s。   

> * 第3类标签：此类标签基于Sony FeliCa体系。目前具有2k字节内存容量，数据通讯速度为212 kbit/s。故此类标签较为适合较复杂的应用，尽管成本较高。

> * 第4类标签：此类标签被定义为与ISO14443A、B标准兼容。制造时被预先设定为可读/可重写、或者只读。内存容量可达32k字节，通信速度介于106 kbit/s和424 kbit/s之间。

具体的NFC标签 协议标准, 参考   
* *<NFC Forum Type 1 Tag Operation Specification 2.0.pdf>*
* *<ISO14443 1-4(2008).pdf>*


# 标准和协议
目前市面上使用的NFC卡主要是基于`ISO14443`协议标准, 因此主要有`TypeA`以及`TypeB` 型卡， `TypeA`和`TypeB`的区别在于链路层的编码方案不同。具体的后面再讲, 如下是·ISO14443`协议文档的分布。
>* *Part 1: Physical characteristics* 主要的物理特性  比如卡、天线等的尺寸要求
>* *Part 2: Radio frequency power and signal interface* 主要是调制频率, 信号调制比例等接口特性描述
>* *Part 3: Initialization and anticollision* 主要是卡的冲撞检测 卡通讯等描述
>* *Part 4 Transmission protocol* 主要是对新卡的一些协议支持 比如支持RATS以及PPS自动频率匹配


---
### `PCD ---> PICC` 方向进行数据传输时
`TypeA`编码方案采用的是 **米勒编码**， `TypeB` 采用的是 **不归零编码**   
如下
![image](https://i.loli.net/2020/05/19/JcdB4TgLRrO3PCu.png)

### `PICC ---> PCD` 方向进行数据传输时
![image](https://i.loli.net/2020/05/19/PfJQNshnMYOBiGW.png)
![image](https://i.loli.net/2020/05/19/phrfbdq6PcHxDyj.png)

> **Note:** 从上图可知, 不同类型的卡片是无法相互读取, 比如一个支持 `ISO14443-A`类型的读卡器是无法读取一张`TypeB` 类型的卡的，这个是物理层决定的。

---

`ISO14443-3`协议数据交互帧主要有三种: 
* *short frames*
![image](https://i.loli.net/2020/05/19/XDSeUsVAOF16Bft.png)

* *standard frames*
![image](https://i.loli.net/2020/05/19/UJA3vrcHSikyf5q.png)
![image](https://i.loli.net/2020/05/19/d9vLorWBR6IgY4x.png)

* *bit oriented anticollision frame*
![image](https://i.loli.net/2020/05/19/ANbi4KFtsDd9VvU.png)

上述的三种帧的功能各不相同
>* **短帧**: 主要用于设备通讯初期的初始化工作(因为接触初期数据非常不稳定, 所以数据量通讯较少, 确保通讯时长短, 增加寻卡的成功率)
>* **标准帧**: 用于数据的交换， 主要是寻卡和选卡完成后的操作
>* **冲突帧**：主要是用于当在感应区出现两个及以上PICC设备时冲突检测。

---
#### 短帧的编码
![image](https://i.loli.net/2020/05/19/HYjeGSMpmAd9n1V.png)   
如上图:   
> `REQA`：用于搜索感应区未睡眠的TypeA类型卡, 及请求卡数据(卡存在需要回应`ATQA`)。   
> `WUPA`: 用于搜索感应区所有TypeA卡   
其他均不常用

**ATQA响应编码格式**:
![image](https://i.loli.net/2020/05/19/KneQCcg92z53tUq.png)
![image](https://i.loli.net/2020/05/19/NStWjuHRGli5qvX.png)

### 设备的初始化流程
![image](https://i.loli.net/2020/05/19/qCWxIdwDRZnuJj6.png)

主要分为如下几个步骤:  
>**1、卡检测**: 主要发生`REQA`和`WAUPA`类短帧命令, 如果接受到设备的`ATQA`响应, 那么可以进入步骤二来进行冲突检测。否则继续步骤1  

>**2、冲突检测**: 冲突检测有三种情况, 对于不同卡情况不一样, 对于M1类型卡, 及通常的`MIFARE MINI` `MIFARE 1K` `MIFARE 4K`等加密卡, 其ID为4byte, 冲撞检测为一层, 但是对于`MIFARE Ultralight/Ultralight C`类型, 其ID为7byte，冲突检测为两层, 最该为3层,ID为10Byte。

>**3、选卡**: 发送特定命令选中感应区中的特定卡, 选中卡片后, 对于一些非加密卡, 这个时候就可以通过标准帧进行数据的读取, 但是如果是读取M1卡此类的具有硬件加密单元的，还需要进行密钥的验证。


### 冲突检测
![image](https://i.loli.net/2020/05/19/ipC2yZOntaj37Hw.png)
上图为`PCD`进行冲突检测的步骤。可描述如下  
> **1、一阶冲突检测** : 及图中的1、2、3, 这个阶段发送冲突帧<编码为0X93,0X20>   
> **2、检测产生冲突点**: 这个过程主要是读卡器硬件给出   
> **3、循环冲突检测**: 通过对冲突点递增0X20，重新发生冲突帧进行检测<编码为0X95/0X97, NVB, 卡ID>   
> **4、选卡**: 通过传输<编码为0X3/0X95/0X97, NVB, ID>进行选卡, 并返回卡类型`SAK`


那如何确定是几级冲突呢？ 其定义如下,其和设备的ID长度密切相关。
![image](https://i.loli.net/2020/05/19/xD7TnXRGiPcwaFY.png)
![image](https://i.loli.net/2020/05/19/iXC7ghUracpHVxs.png)
![image](https://i.loli.net/2020/05/19/8WHy5mFXGnfrdTl.png)


**此时我们就完成了设备的选择, 可以确定设备的类型**
![image](https://i.loli.net/2020/05/19/9RgGWUMoFSni7fL.png)
但是就单单从上图无法确定是什么类型的卡片，这是由于本协议`iso14443-3`协议较老。下面是我从其他地方摘取的类型代码。
```
sak &= 0x7F;
switch (sak) {
	case 0x04:	return PICC_TYPE_NOT_COMPLETE;	// UID not complete
	case 0x09:	return PICC_TYPE_MIFARE_MINI;
	case 0x08:	return PICC_TYPE_MIFARE_1K;
	case 0x18:	return PICC_TYPE_MIFARE_4K;
	case 0x00:	return PICC_TYPE_MIFARE_UL;
	case 0x10:
	case 0x11:	return PICC_TYPE_MIFARE_PLUS;
	case 0x01:	return PICC_TYPE_TNP3XXX;
	case 0x20:	return PICC_TYPE_ISO_14443_4;
	case 0x40:	return PICC_TYPE_ISO_18092;
	default:	return PICC_TYPE_UNKNOWN;
}
```

> 对于M1加密卡类型,此时不能进行数据的读取, 还需要验证密钥来保证安全。   

![image](https://i.loli.net/2020/05/19/x3aeZ29NDKs4tAL.png)

如图, 可以看到`Select Card`后还需要进行密钥的认证。才能进行读写块。如下为M1卡操作的常用命令
定义   
```
// Commands sent to the PICC.
enum PICC_Command {
    // The commands used by the PCD to manage communication with several PICCs (ISO 14443-3, Type A, section 6.4)
    PICC_CMD_REQA			= 0x26,		// REQuest command, Type A. Invites PICCs in state IDLE to go to READY and prepare for anticollision or selection. 7 bit frame.
    PICC_CMD_WUPA			= 0x52,		// Wake-UP command, Type A. Invites PICCs in state IDLE and HALT to go to READY(*) and prepare for anticollision or selection. 7 bit frame.
    PICC_CMD_CT				= 0x88,		// Cascade Tag. Not really a command, but used during anti collision.
    PICC_CMD_SEL_CL1		= 0x93,		// Anti collision/Select, Cascade Level 1
    PICC_CMD_SEL_CL2		= 0x95,		// Anti collision/Select, Cascade Level 2
    PICC_CMD_SEL_CL3		= 0x97,		// Anti collision/Select, Cascade Level 3
    PICC_CMD_HLTA			= 0x50,		// HaLT command, Type A. Instructs an ACTIVE PICC to go to state HALT.
    PICC_CMD_RATS           = 0xE0,     // Request command for Answer To Reset.
    // The commands used for MIFARE Classic (from http://www.mouser.com/ds/2/302/MF1S503x-89574.pdf, Section 9)
    // Use PCD_MFAuthent to authenticate access to a sector, then use these commands to read/write/modify the blocks on the sector.
    // The read/write commands can also be used for MIFARE Ultralight.
    PICC_CMD_MF_AUTH_KEY_A	= 0x60,		// Perform authentication with Key A
    PICC_CMD_MF_AUTH_KEY_B	= 0x61,		// Perform authentication with Key B
    PICC_CMD_MF_READ		= 0x30,		// Reads one 16 byte block from the authenticated sector of the PICC. Also used for MIFARE Ultralight.
    PICC_CMD_MF_WRITE		= 0xA0,		// Writes one 16 byte block to the authenticated sector of the PICC. Called "COMPATIBILITY WRITE" for MIFARE Ultralight.
    PICC_CMD_MF_DECREMENT	= 0xC0,		// Decrements the contents of a block and stores the result in the internal data register.
    PICC_CMD_MF_INCREMENT	= 0xC1,		// Increments the contents of a block and stores the result in the internal data register.
    PICC_CMD_MF_RESTORE		= 0xC2,		// Reads the contents of a block into the internal data register.
    PICC_CMD_MF_TRANSFER	= 0xB0,		// Writes the contents of the internal data register to a block.
    // The commands used for MIFARE Ultralight (from http://www.nxp.com/documents/data_sheet/MF0ICU1.pdf, Section 8.6)
    // The PICC_CMD_MF_READ and PICC_CMD_MF_WRITE can also be used for MIFARE Ultralight.
    PICC_CMD_UL_WRITE		= 0xA2		// Writes one 4 byte page to the PICC.
};
```

### ISO14443-4协议类型卡
`ISO14443-4`是对`ISO14443-3`的扩展, 以让读卡器和接收器能够自动的进行参数的协调，匹配，减少人为干预的步骤。

![image](https://i.loli.net/2020/05/19/Uh8E4TQODs1BzFK.png)

如上图, 其和`ISO14443-3`协议的流程主要区别在与完成选卡后`ISO14443-4`多了一个发送RATS请求的过程,这个过程用于请求设备端的ATS响应。其仅仅适用于支持`ISO14443-4`协议的设备。然后得到确认后通过ATS响应来确认设备支持的选项，以最优的设置通过PPS下发完成通讯参数的设置。

如下为**RATS指令编码**

![image](https://i.loli.net/2020/05/19/fnvujKbL4MUk1gQ.png)
![1.png](https://i.loli.net/2020/05/19/rYEQvB5mOZj8h1q.png)

设备端响应的**ATS编码  **
![screenShot.png](https://i.loli.net/2020/05/19/m2IBs3KNklGbPhC.png)   
ATS的部分域描述    
>`TL` 描述后面的数据长度 其包含自身长度, 比如如果为5 表示包含TL T0 TA TB TC   
>`T0` 表明是否支持TA TB TC以及设备端的FSCI长度(FSCI编码可以参照ISO14443-4协议)   
>`TA` 表明设备的接收和发送波特率     

具体内容可以参照`ISO14443-4`协议   

**PPS指令编码**

![screenShot.png](https://i.loli.net/2020/05/19/6FNVgdqQZhiGy3U.png)
![screenShot.png](https://i.loli.net/2020/05/19/4vfsldIVaCuDYb2.png)
![screenShot.png](https://i.loli.net/2020/05/19/QwX9j6bJYE4s8fD.png)

**PPS响应**


![screenShot.png](https://i.loli.net/2020/05/19/7q4TEyoROHWx3tz.png)
---
# IC存储卡的内存布局
## 加密M1卡内存布局
![image](https://i.loli.net/2020/05/19/zdsf2hEYA5ncKxS.png)
每一个块大小为16Byte
对于其权限控制,如下  
![image](https://i.loli.net/2020/05/19/TAcXEBaUN139InQ.png)
![image](https://i.loli.net/2020/05/19/gOxFLpD2EI67lnA.png)
![image](https://i.loli.net/2020/05/19/Rt1muh9VgUe43Ld.png)
![image](https://i.loli.net/2020/05/19/XoKRcfHNDTVeFPg.png)



## IC存储卡类型
![image](https://i.loli.net/2020/05/19/s3DfES8H5XA1Jl6.png)
可以看到 其相当于内存的访问, 及一片EEPROM空间。

## NFC Type2 Tag内存布局
![screenShot.png](https://i.loli.net/2020/05/19/G2ROKx5cpWhzsNy.png)
