define print_rqs
	set $req = (struct request**)$arg0
	#print $req
	set $idx = 0
	while $idx < 256
		set $rq=$req[$idx]
		set $hctx=$rq->mq_hctx
		if $hctx > 0
			print /d $idx
			print $rq
		#	print $hctx
		end
		set $idx=$idx+1
	end
end

define count_nodes
	set $head = (struct list_head *)$arg0
	set $next = $head->next

	set $count = 0
	while $head != $next
		set $idx = $idx + 1
		set $next = $next->next
	end
	print $next
end
	
