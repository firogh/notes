define print_dma_entry
        set $map = (struct hash_bucket *)0xffffffff90337980
        set $left = (unsigned long) 0x1008c718000
        #set $left = (unsigned long) 0x10174718000 - 1
        set $right = (unsigned long) 0x1008fffffff
        #set $right = (unsigned long) 0x10174718000 + 0x2000

        # [9100, 16384)
        set $idx = 9100
        while $idx < 16384
                set $head = &$map[$idx]
                set $n = $head->list.next
                #print &($head->list)
                #print $n
                while $n != &($head->list)
                        #print $n
                        set $entry = (struct dma_debug_entry *)$n
                        set $n = $n->next
                        set $dma_addr = $entry->dev_addr
                        if ($left <= $dma_addr && $dma_addr <= $right)
                                print $entry
                end
                # print /d $idx
                set $idx=$idx+1
        end
end
