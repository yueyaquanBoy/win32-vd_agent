win32-vd_agent
==============

spice agent for win32

通过在虚拟机里面安装vdagent（Spice Agent）程序来自动适应屏幕分辨率的功能，这个vdagent是运行在虚拟机里面的，而分辨率的信息来自spice client，这中间隔着spice server，qemu等模块，中间的过程还是比较复杂的。通过分析这个流程，有助于帮助我们理解更多KVM-QEMU虚拟化的机制。
