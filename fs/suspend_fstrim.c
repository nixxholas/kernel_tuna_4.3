#include <linux/module.h>
#include <linux/init.h>
#include <linux/kmod.h>
#include <linux/earlysuspend.h>
#include <linux/fs.h>

static void fstrim_early_suspend(struct early_suspend *handler)
{
	static unsigned long lasttrimjiffies;

	if (jiffies - lasttrimjiffies >= 900 * HZ) {
		char *argv[] = { "/sbin/fstrim", "/cache", NULL };
		static char *envp[] = {
        		"HOME=/",
        		"TERM=linux",
        		"PATH=/sbin", NULL };

		call_usermodehelper( argv[0], argv, envp, UMH_WAIT_EXEC );

		lasttrimjiffies = jiffies;
	}
}

static struct early_suspend suspend_fstrim = {
	.suspend = fstrim_early_suspend,
};

void __init suspend_fstrim_init(void)
{
	register_early_suspend(&suspend_fstrim);
}
