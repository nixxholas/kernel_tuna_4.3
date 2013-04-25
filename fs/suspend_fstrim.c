#include <linux/module.h>
#include <linux/init.h>
#include <linux/kmod.h>
#include <linux/earlysuspend.h>
#include <linux/fs.h>

static u64 resumejiffies;

static void fstrim_early_suspend(struct early_suspend *handler)
{
	static u64 awakejiffies;

	awakejiffies += (jiffies - resumejiffies);

	if (awakejiffies >= 360 * HZ) {
		char *argv[] = { "/sbin/fstrim", "/cache", NULL };
		static char *envp[] = {
        		"HOME=/",
        		"TERM=linux",
        		"PATH=/sbin", NULL };

		call_usermodehelper( argv[0], argv, envp, UMH_WAIT_EXEC );

		awakejiffies = 0;
	}
}

static void fstrim_late_resume(struct early_suspend *handler)
{
	resumejiffies = jiffies;
}

static struct early_suspend suspend_fstrim = {
	.suspend = fstrim_early_suspend,
	.resume = fstrim_late_resume,
};

void __init suspend_fstrim_init(void)
{
	register_early_suspend(&suspend_fstrim);
}
