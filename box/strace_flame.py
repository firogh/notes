#!/usr/bin/python3

#data_file = "/mounts/ziu/SFSC00325817/SFSC00335322/firo/firo.log"
data_file = "/home/firo/Downloads/dl360s-79_20240609_vcpu_threads.lst.txt"

def main():
    with open(data_file) as f:
        data = f.read()
    # backtrace : time count
    data = data.split('\n')
    strace_data = []
    stack_traces = {}
    symbols = ""
    count = 0 
    last_count = 0 
    for line in data:
        if line.startswith(" > /usr"):
            line = line.split()
            symbols = line[1] + ";" + symbols
        elif len(line) < 5:
            continue
        elif line.startswith("b"):
            if symbols in stack_traces:
                stack_traces[symbols] += 1
            else:
                stack_traces[symbols] = 1 
            strace_data.append(stack_traces)

    
            symbols = ""
            count = 0 
    
            line = line.split()
            # time = line[-1] 
            # count = 1
            #if time.startswith("<"):
            #    count = float(time[1:-1])
        else:
            continue

    for k,v in stack_traces.items():
        if k == "":
            continue
        print("%s %d"%(k,v))
        # print("%s %d"%(k,v *1000000))

if __name__ == "__main__":
    main()
