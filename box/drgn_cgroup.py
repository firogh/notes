from drgn import Object, container_of
import drgn
import sys

# Helper function to read a string from a 'd_name' structure
count = 0
count2 = 0
def read_dentry_name(dentry):
    try:
        name = ""
        # Get the length of the name
        # Read the name as a string, up to the specified length
        #name = name_ptr.read_cstring(name_len)
        return name
    except Exception as e:
        return f"<error reading name: {str(e)}>"

def traverse_cgroup(cgroup, depth=0):
    global count
    global count2
    try:
        # Print the current dentry information with indentation based on depth
        #print(f"{' ' * (depth * 2)}Dentry Name: {name}, Flags: {flags}  {Address: hex(dentry.value_())}")
        count = count + 1

        # Access the list of children (d_subdirs)
        children = cgroup.self.children
        entry = children.next;

        if cgroup.self.flags == 2:
            count2 = count2 + 1
        print("Cgroup: %d %d %lx %lx %d %lx %lx %lx %d\n"%(count, depth, 
            children.address_of_(), entry, 
            cgroup.level, cgroup.self.refcnt.count.counter
&0xFFFFFFFFFFFFFFFF, cgroup.self.online_cnt.counter&0xFFFFFFFFFFFFFFFF,
cgroup.self.flags, count2))

                 

#        print("c %lx %lx %lx\n"%(child_entry, subdirs.address_of_(),child_entry -9))
        while entry != children.address_of_():

            # Get the dentry pointer from the list_head entry using container_of
            #child_dentry = container_of(child_entry, dentry, "d_child")
            #child_dentry = child_entry - 9
            addr = entry.value_() -  0x48
            # print("addr: %lx %lx %lx \n"%( entry, entry.value_(), addr))
            c = Object(prog, "struct cgroup", address=addr)
            # Recursively traverse the child dentry
            traverse_cgroup(c, depth + 1)

            # Move to the next child in the list
            entry = entry.next

    except Exception as e:
        print(f"{' ' * (depth * 2)}Error reading dentry: {str(e)}")

if __name__ == "__main__":
    # Ensure drgn is running with a valid crash dump or live system
    if len(sys.argv) != 2:
        print("Usage: drgn -p <pid> <script.py> <root_dentry_address>")
        #sys.exit(1)

    #root_dentry_addr = int(sys.argv[1], 16)
    root_cgroup_addr = 0xffff96014fc0c010

    # Get the dentry object at the specified address
    root_cgroup = Object(prog, "struct cgroup", address=0xffff96014fc0c010)

    # Start traversing from the root dentry
    traverse_cgroup(root_cgroup)
