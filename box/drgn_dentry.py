from drgn import Object, container_of
import drgn
import sys

# Helper function to read a string from a 'd_name' structure
count = 0;
def read_dentry_name(dentry):
    try:
        # Get the length of the name
        name_len = int(dentry.d_name.len)
        # Get the pointer to the name
        name = dentry.d_name.name
        # Read the name as a string, up to the specified length
        #name = name_ptr.read_cstring(name_len)
        return name
    except Exception as e:
        return f"<error reading name: {str(e)}>"

def traverse_dentry(dentry, depth=0):
    """
    Recursively traverse the dentry structure.

    Args:
        dentry (drgn.Object): The current dentry to traverse.
        depth (int): Current depth level for printing purposes.
    """
    global count
    try:
        # Read the current dentry name
        name = read_dentry_name(dentry)

        flags = dentry.d_flags
        # Print the current dentry information with indentation based on depth
        #print(f"{' ' * (depth * 2)}Dentry Name: {name}, Flags: {flags}  {Address: hex(dentry.value_())}")
        count = count + 1
        print("D %d %s %lx\n"%(count, name, flags))


        # Access the list of children (d_subdirs)
        subdirs = dentry.d_subdirs

        # Iterate over each child in the d_subdirs linked list (assuming list_head type)
        child_entry = subdirs.next

#        print("c %lx %lx %lx\n"%(child_entry, subdirs.address_of_(),child_entry -9))
        while child_entry != subdirs.address_of_():
            # Get the dentry pointer from the list_head entry using container_of
            #child_dentry = container_of(child_entry, dentry, "d_child")
            child_dentry = child_entry - 9
            child_dentry = Object(prog, "struct dentry", address=child_dentry)
            flags = child_dentry.d_flags
            #print("c %lx %s"%(flags, read_dentry_name(child_dentry)))

            if not (flags &0x00200000):
                child_entry = child_dentry.d_child.next
                continue
            # Recursively traverse the child dentry
            traverse_dentry(child_dentry, depth + 1)

            # Move to the next child in the list
            child_entry = child_dentry.d_child.next

    except Exception as e:
        print(f"{' ' * (depth * 2)}Error reading dentry: {str(e)}")

if __name__ == "__main__":
    # Ensure drgn is running with a valid crash dump or live system
    if len(sys.argv) != 2:
        print("Usage: drgn -p <pid> <script.py> <root_dentry_address>")
        #sys.exit(1)

    # Parse the root dentry address from the command line argument
    #root_dentry_addr = int(sys.argv[1], 16)
    root_dentry_addr = 0xffff96179b98f6c0

    # Get the dentry object at the specified address
    root_dentry = Object(prog, "struct dentry", address=root_dentry_addr)

    # Start traversing from the root dentry
    traverse_dentry(root_dentry)

