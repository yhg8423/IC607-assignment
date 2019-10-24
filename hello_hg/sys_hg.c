#include <linux/kernel.h>

asmlinkage long sys_hg(void) {
	printk("[Hyeonggeun] Hyeonggeun systemcall!\n");
	return 0;
}

