---
tags: kernel
title: deferring-work
date: 2015-02-27 15:46:13
category: kernel
---
# Differences between softirq and workqueue

Softirq(tasklet)
init static:	DECLARE_TASKLET(), DECLARE_TASKLET_DISABLED()
init dynamic:	tasklet_init()
schedule:	tasklet_schedule()
exit:		tasklet_kill()
pros and cons:	execute in interrupt context, serialization, tasklet blonged to the cpu which tasklet_schedule() it, oneshot

Workqueue
init static:	DECLARE_WORK()
init dynamic:	kzalloc() INIT_WORK(), kzalloc() PREPARE_WORK()
schedule1:	create_singlethread_workqueue(), create_workqueue()  
		queue_work()
exit1:		flush_workqueue(), destroy_workqueue()
schedule2:	schedule_work()
exit2:		(none)
pros and cons:	execute in process context, workqueue has two type, driver can create workqueue or use system workqueue, oneshot
