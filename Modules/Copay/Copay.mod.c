#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xf922ce58, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x4d9e4938, __VMLINUX_SYMBOL_STR(kthread_stop) },
	{ 0x22da4565, __VMLINUX_SYMBOL_STR(wake_up_process) },
	{ 0xccd30a83, __VMLINUX_SYMBOL_STR(kthread_create_on_node) },
	{ 0x50eedeb8, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x7d11c268, __VMLINUX_SYMBOL_STR(jiffies) },
	{ 0x320c1ca6, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0x45560a52, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0xb1ea840, __VMLINUX_SYMBOL_STR(init_task) },
	{ 0xb4390f9a, __VMLINUX_SYMBOL_STR(mcount) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "7725F3675A0319633FAE98D");