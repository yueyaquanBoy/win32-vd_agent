win32-vd\_agent
==============

spice agent for win32
[参考一](http://blog.csdn.net/hbsong75/article/details/9465683)
[参考二](http://blog.csdn.net/hbsong75/article/details/9470719)
[参考三](http://blog.csdn.net/hbsong75/article/details/9570393)

通过在虚拟机里面安装vdagent（Spice Agent）程序来自动适应屏幕分辨率的功能，这个vdagent是运行在虚拟机里面的，而分辨率的信息来自spice client，这中间隔着spice server，qemu等模块，中间的过程还是比较复杂的。通过分析这个流程，有助于帮助我们理解更多KVM-QEMU虚拟化的机制。

GuestOS      QEMU                     ClientOS
   |           |            Client - SpiceClient
vdagent    I/O Rings     - libspice
   |           |            Server
 Driver - VDI Port Device

Spice agent运行在客户机（虚拟机）操作系统中。Spice server和Spice client利用spice agent来执行一些需要在虚拟机里执行的任务，如配置分辨率，另外还有通过剪贴板来拷贝文件等。从上图可以看出，Spice client与server与Spice Agent的通信需要借助一些其他的软件模块，如在客户机里面，Spice Agent需要通过VDIPort Driver与主机上 QEMU的VDIPort Device进行交互，他们的交互通过一种叫做输入/输出的环进行。Spice Client和Server产生的消息被写入到设备的输出环中，由VDI Port Driver读取；而Spice Agent发出的消息则通过VDI Port Driver先写入到VDI Port Device输入环中,被QEMU读入到Spice server的缓冲区中，然后再根据消息决定由Spice Server直接处理，还是被发往Spice Client中。

在技术上KVM-QMEU架构采用了一种叫virtio-serial的技术，这种技术处理主机用户空间和客户机用户空间的数据传输。它主要包含两个部分：1. Qemu中模拟一个叫virtio-pci的设备，这个设备提供给客户机使用；2.客户机上安装一个字符设备驱动访问virtio-pci设备。

在实现上，大致需要以下一些工作：

1.在用libvirt定义虚拟机的时候需要包含添加virtio-serial的控制器设备：

	<controller type='virtio-serial' index='0'>
		 <address  type='pci' domain='0x0000' bus='0x00' slot='0x05' function='0x0'/>
	</controller>

该设备挂载在PCI总线0的第5个槽位上。这个设备就是QEMU中模拟的，提供给客户及访问的virtio-pci设备；


2.在用libvirt定义虚拟机的时候需要包含一个叫“com.redhat.spice.0”的客户机接口，它是”channel”类型，其他的客户机接口类型还有parallel port, serial port, console等，它们从本质上来说都是一种提供给客户机访问的字符设备。

	<channel type='spicevmc'>
	<target type='virtio' name='com.redhat.spice.0'/>
	<address type='virtio-serial' controller='0' bus='0' port='1'/>
	</channel>

Channel还可以分为”spicevmc”，”unix”, “pty”等类型，具体可以参考libvirt的说明文档：http://www.libvirt.org/formatdomain.html

注意上面的”address”，它指定了这个字符设备是挂载到了我们在前面定义的virtio-serial控制器上，这样这两者就关联起来了。
 

3.在虚拟机里面安装vdagent（参考Ubuntu12.10下搭建基于KVM-QEMU的虚拟机环境（十八））。


4.上述工作都做了以后，启动虚拟机，然后打开设备管理器（注意，需要右击设备管理器->查看->显示隐藏设备）

在虚拟机设备管理器里有两个跟这个virtio-serial机制相关的设备：VirtIO-Serial Driver和vportOp1，这两个设备分别代表libvirt定义中定义的virtio-serial控制器设备,槽位号是5。QEMU把它模拟出来，并已经提供给虚拟机里的虚拟串口驱动访问了。 vportOp1表示libvirt里定义里定义的“'com.redhat.spice.0”字符设备，它的位置1与我们定义的port=’1’一致，如果要实验可以改一下port=’2’看看，虚拟机里面的位置也会随之改成2。

vdagent程序就可以通过对这个vportOp1字符设备的读写来与主机上的qemu进程交互了，也就是虚拟机与主机的通信通道已经通了。

