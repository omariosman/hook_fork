#include <linux/build-salt.h>
#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x7ba62dba, "module_layout" },
	{ 0xdb7305a1, "__stack_chk_fail" },
	{ 0xd4b3235, "pv_cpu_ops" },
	{ 0x20c55ae0, "sscanf" },
	{ 0x9166fada, "strncpy" },
	{ 0x1e6d26a8, "strstr" },
	{ 0x76563f1d, "vfs_read" },
	{ 0x37a0cba, "kfree" },
	{ 0xd1b70d02, "filp_close" },
	{ 0xd5db35e0, "filp_open" },
	{ 0x1ce1ab25, "kmem_cache_alloc_trace" },
	{ 0x9547fcad, "current_task" },
	{ 0x3a8870f8, "kmalloc_caches" },
	{ 0x7c32d0f0, "printk" },
	{ 0x2ea2c95c, "__x86_indirect_thunk_rax" },
	{ 0xbdfb6dbb, "__fentry__" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

