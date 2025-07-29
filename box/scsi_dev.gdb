define print_scsi
        set $gd = (struct gendisk*)$arg0
        print /x $gd 
        set $sd = (struct scsi_disk *)$gd->private_data
        print /x  $sd 
        # print /x  "struct scsi_disk " $sd
        set $sdev = $sd->device 
        print /x $sdev
        #struct scsi_device
        set $host = $sdev->host
        print /x $host
        set $hostdata = $sdev->hostdata
        print /x $hostdata
        set $shostdata = $host->hostdata
        print /x $shostdata

        print "======"
end

define print_scsi_hostdata
	set $gd = (struct gendisk*)$arg0
	#print /x $gd
	set $sd = (struct scsi_disk *)$gd->private_data
	#print /x  $sd
	set $sdev = $sd->device 
	#print /x $sdev
	#struct scsi_device
	set $host = $sdev->host
	#print /x $host
	set $hostdata = $sdev->hostdata
	#print /x $hostdata
	set $shostdata = $host->hostdata
	print /x $shostdata

	print "======"
end
