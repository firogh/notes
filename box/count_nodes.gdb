define count_nodes
	set $head = (struct list_head *)$arg0
	set $next = $head->next

	set $count = 0
	while $head != $next
		set $count = $count+ 1
		set $next = $next->next
	end
	print $count
end
	
