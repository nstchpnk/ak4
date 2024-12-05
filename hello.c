// SPDX-License-Identifier: GPL-2.0

/*
 * Copyright (c) 2017, GlobalLogic Ukraine LLC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the GlobalLogic.
 * 4. Neither the name of the GlobalLogic nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY GLOBALLOGIC UKRAINE LLC ``AS IS`` AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL GLOBALLOGIC UKRAINE LLC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/moduleparam.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/ktime.h>

MODULE_AUTHOR("Serhii Popovych <serhii.popovych@globallogic.com>");
MODULE_DESCRIPTION("Hello, world in Linux Kernel Training");
MODULE_LICENSE("Dual BSD/GPL");

static uint print_count = 1;
module_param(print_count, uint, 0444);
MODULE_PARM_DESC(print_count, "Number of times to print 'Hello, world!'");

struct event_data {
	struct list_head list;
	ktime_t time;
};

static LIST_HEAD(event_list);

static int __init hello_init(void)
{
	int i;

	if (print_count == 0 || (print_count >= 5 && print_count <= 10))
		pr_warn("Warning: print_count is 0 or between 5 and 10.\n");

	if (print_count > 10) {
		pr_err("Error: print_count is greater than 10.\n");
		return -EINVAL;
	}

	for (i = 0; i < print_count; i++) {
		struct event_data *event;

		event = kmalloc(sizeof(*event), GFP_KERNEL);
		if (!event)
			return -ENOMEM;

		event->time = ktime_get();
		list_add_tail(&event->list, &event_list);
		pr_info("Hello, world!\n");
	}

	return 0;
}

static void __exit hello_exit(void)
{
	struct event_data *event, *tmp;

	list_for_each_entry_safe(event, tmp, &event_list, list) {
		pr_info("Event time: %lld ns\n", ktime_to_ns(event->time));
		list_del(&event->list);
		kfree(event);
	}

	pr_info("Goodbye, world!\n");
}

module_init(hello_init);
module_exit(hello_exit);

