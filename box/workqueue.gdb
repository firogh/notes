define print_pool
        set $task = (struct task_struct *)$arg0
        # print $task
        set $kthread = (struct kthread*)$task->set_child_tid
        #print $kthread
        set $data = $kthread->data
        # print $data
        set $worker = (struct worker*) $data
        # print $worker
        set $pool = $worker->pool
        print $pool
end

define print_worker
        set $task = (struct task_struct *)$arg0
        # print $task
        set $kthread = (struct kthread*)$task->set_child_tid
        #print $kthread
        set $data = $kthread->data
        # print $data
        set $worker = (struct worker*) $data
        print $worker
end

