

#parts
=the periodic scheduler scheduler_tick: mark need resched
* vrtime
* TIF_NEED_RESCHED

=try_to_wake_up and wake_up_new_task: mark TIF_NEED_RESCHED

=the main scheduler schedule: initiate a resched

#what is the PREEMPT_ACTIVE



