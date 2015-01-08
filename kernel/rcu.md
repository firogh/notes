#History
brlock

#What is rcu
#type-variants-sd-al (extension)
=type
a synchronization mechanism 
=other implement
* rw-lock similitudes 
	read-side critical sections that can execute in parallel

* difference

=advantages 
Advantages of RCU include performance, deadlock immunity, and realtime latency.
=limitation
latencies


#parts-variants-sd-al (intension)
=parts
* perspective
	if any statement in a given RCU read-side critical section precedes a grace period, then all statements in that RCU read-side critical section must complete before that grace period ends.
	state flow: 
	Removal  list_del_rcu, rcu_assignment
	Grace period, synchronize_rcu(some like call_rcu)
	Recalmmation, kfree

* read-side primitives
	In RCU's case, the things waited on are called "RCU read-side critical sections".
	does not explicitly block or sleep
	quiesecent state
	grace period
	rcu_tasklet
	rcu_data,rcu_bh_data

	__rcu sparse will warn you if you access that pointer without the services of one of the variants of rcu_dereference().

	rcu_read_lock/unlock

	rcu_dereference
	rcu_assign_pointer

* rcu_update
	these old versions must be reclaimed after all pre-existing readers complete
	call_rcu
	synchronize_rcu, spin_lock for syncing updaters.

=variants
* Classic RCU
	http://www.rdrop.com/users/paulmck/RCU/whatisRCU.html
* Preemtible RCU
	priority inheritance(priority inversion)
	non-blocking in non-preemt kernel

* SRCU
* RCU-tiny
* Tree RCU
* RCU-task
	


#How to use?
=好不好用

=a replacement of rw-lock

=a Restricted Reference-Counting Mechanism 
* 32 个赞

=a Bulk Reference-Counting Mechanism


=a Poor Man's Garbage Collector

=a Way of Providing Existence Guarantees

=a Way of Waiting for Things to Finish


#Classic RCU
#parts-variants-sd-al
=Perspective
	* Classic RCU requires that read-side critical sections obey the same rules obeyed by the critical sections of pure spinlocks: blocking or sleeping of any sort is strictly prohibited.
	* cpumask

#Preemtible RCU
#parts-variants-sd-al
=Parts	
*Perspetctive
	block only in priority inheritance non-blocking in non-preemt kernel
*softirq
	two stages grace period
	wait queues
	rcu_process_callbacks
	rcu_flipctr: a per-CPU two-element
		[a]:tracks critical sections that started before the current gp stage.
		[b]:tracks new rcu read-side critical sections, namely those starting during the current gp stage.
	rcu_ctrlblk.completed & 0x1
	rcu_flip_flag: synchronize the start of each grace-period stage: once a given CPU has responded to its rcu_flip_flag, it must refrain from incrementing the old grace-period stage. 
	rcu_mb_flag: rcu_flipped  force each CPU to execute a memory barrier at the end of each grace-period stage.
				rcu_mb_needed by the CPU that detects that the sum of the old counters is zero
				 rcu_mb_done only by the corresponding CPU, and even then only after executing a memory barrier.
#Tree RCU
#parts-variants-sd-al
=parts 
*Downside of classical RCU in update
	the update-side primitives which determine when pre-existing read-side critical sections have finished, were designed with only a few tens of CPUs in mind.
	 Their scalability is limited by a global lock that must be acquired by each CPU at least once during each grace period.


#FAQ
=Limitations of RCU-rt?
=In contrast to SRCU, preemptible RCU only permits blocking within primitives that are both subject to prikority inheritance and non-blocking in a non-CONFIG_PREEMPT kernel.
=No need of mb in preempt read-side?
=irq in rcu_sched_qs
=However, note that RCU read-side critical sections that begin after the beginning of a given grace period can and will extend beyond the end of that grace period.?
=RCU read-side critical sections must be permitted in NMI handlers as well as irq handlers?
=classic RCU will happily awaken each and every sleeping CPU at least once per grace period in some cases?

=rcu_deference
