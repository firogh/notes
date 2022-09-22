#!/usr/bin/python

from bokeh.plotting import figure,output_file,  show
from bokeh.palettes import inferno# select a palette
from bokeh.palettes import Dark2_5 as palette
import itertools# itertools handles the cycling 
from bokeh.models import HoverTool
import argparse

output_file("/tmp/b.html")

# TODO
# Preserve absoulte time in btt output

# TODO
# Horizontally Stacked Bars
# https://notebook.community/dennisobrien/bokeh/examples/howto/Categorical%20Data


# TODO
# Btt Q & C activity 
# Step lines
# https://docs.bokeh.org/en/2.4.1/docs/user_guide/plotting.html
# https://docs.bokeh.org/en/2.4.2/docs/reference/models/glyphs/step.html
# https://stackoverflow.com/questions/45475780/how-do-i-add-hover-highlighting-to-bokeh-step-chart


def read_sar_data_file(data_file, extra_legend_idx=0):
    with open(data_file) as f:
        data = f.read()

    data = data.split('\n')

    if not data[0].startswith("Linux"):
        print("Data corruption %s: %s\n"%(data_file, data[0]))
        exit(1);

    data_line = 0
    sar_data = {} 
    if extra_legend_idx == 0:
        sar_data["extra"] = 0
    else
        sar_data["extra"] = 1

    line_no = -1;
    for row in data:
        line_no++
        if data[line_no].startswith("Linux"):
            # First line.
            continue
        elif len(data[line_no] < 5:     # Testing empty line; for robust, don't use len(data[line_no] == 0. 
            # Empty line
            continue
        elif "LINUX RESTART" in len(data[line_no]):
            continue
        else
            # Title
            fields = data[line_no].split()

            sar_data['x'] = []
            s_idx = 1
            sar_data["title"] = {}
            sdt= sar_data["title"] 
            sar_data["metrics"] = {}
            sar_data["extra_legend"] = {}
            sdm = sar_data["metrics"]
            for f in fields[1:]:
                sdt[s_idx]=f
                sdm[s_idx] = []
                s_idx++
            sar_data["last_idx"] = s_idx - 1

            data_line = 1
            break

# sar_data = {
#       'x': x_vals
#       'title': titles
#       'metrics': counters under each titles
    for row in data[line_no + 1:]:
        if data_line == 1:
            if data[line_no].startswith("A"):
                data_line = 0
                continue
            row = row.split()
            sar_data['x'].append(row[0])
            m_idx=1
            if extra_legend_idx == 0:
                for f in row[1:]:
                    sdm[m_idx].append(f)
                    m_idx++
            else                                                
                sde = sar_data["extra_legend"]                  # 09:20:11          DEV       tps     rkB/s
                legend = row[1]                                 # 09:30:11          sda      5.97      9.34
                if not legend in sde:                           # sda is extra_legend-^
                    sde[legend]= {}
                    sdel = sde[legend]
                    sdel["name"] = legend
                    sdel["metrics"] = {}
                else
                    sdel = sde[legend]

                sdelm = sdel["metrics"]
                for f in row[2:]:
                    sdelm[m_idx] = []
                    sdelm[m_idx].append(f)
                    m_idx++
        elif len(data[line_no] < 5:     # Testing empty line; for robust, don't use len(data[line_no] == 0. 
            # Empty line
            data_line = 0
            continue
        elif row.startswith("A") # Short for Average.
                data_line = 0
                continue
        elif "LINUX RESTART" in len(data[line_no]):
            line_no++                   # skip repeted titles
            data_line = 1
            continue
        line_no++
                
    return sar_data 

def render_sar_data(sar_data):
    # p = figure(width=400, height=400)
    p = figure(sizing_mode="stretch_width", title="Btt Q & C activity")

    numLines=20
    colors = itertools.cycle(inferno(numLines))# create a color iterator 

    sdt = sar_date["title"]
    if sar_data["extra"] == 1:
        for sde in sar_data["extra_legend"]:
            for sdel in sde:
                legend = sdel["name"]
                sdelm = sdel["metrics"]
                m_idx = 2
                for i in sdelm[2:]:
                    legend += sdt[m_idx]
                    c = next(colors)
                    renderer = p.line(sar_data['x'], sdelm[m_idx], color=c, legend_label=legend, line_width=2)
                    #renderer = p.line(x, y, color=c, line_width=2)

                    # https://stackoverflow.com/questions/49282078/multiple-hovertools-for-different-lines-bokeh
                    p.add_tools(HoverTool(renderers=[renderer], tooltips=[('Name', legend)]))
    else
        m_idx = 1
        for sdm in sar_data["metrics"]:
            legend = sdt[m_idx]
            c = next(colors)
            renderer = p.line(sar_data['x'], sdm[m_idx], color=c, legend_label=legend, line_width=2)
            #renderer = p.line(x, y, color=c, line_width=2)

            # https://stackoverflow.com/questions/49282078/multiple-hovertools-for-different-lines-bokeh
            p.add_tools(HoverTool(renderers=[renderer], tooltips=[('Name', legend)]))

    show(p)

def render_sar_d_data(data)
    render_sar_dara(data)

def read_sar_d_data_file(data_file):
    return read_sar_data_file(data_file)

def read_btt_q_c_activity_file(data_file):
    return

def btt_Q_C_activity(data_file):
    with open(data_file) as f:
        data = f.read()

    data = data.split('\n')

    # p = figure(width=400, height=400)
    p = figure(sizing_mode="stretch_width", title="Btt Q & C activity")

    numLines=20
    colors = itertools.cycle(inferno(numLines))# create a color iterator 

    x = []
    y = []
    newline = 0 
    for row in data:
        if newline == 0:
            if row.startswith("#  "):
                newline = 1 
                x = []
                y = []
                legend = row 
            continue
        elif len(row) != 0:
            x_val,y_val = row.split()
            x.append(float(x_val))
            y.append(float(y_val))
        else:
            newline = 0
            c = next(colors)
            renderer = p.line(x, y, color=c, legend_label=legend, line_width=2)
            #renderer = p.line(x, y, color=c, line_width=2)

            # https://stackoverflow.com/questions/49282078/multiple-hovertools-for-different-lines-bokeh
            p.add_tools(HoverTool(renderers=[renderer], tooltips=[('Name', legend)]))

    show(p)

    return

def read_data_from_file(data_file):
    with open(data_file) as f:
        data = f.read()

    data = data.split('\n')

    x = []
    y = []
    for row in data:
        if len(row) == 0:
            continue
        X,Y = row.split()

    #    if float(Y) < 0.5:         # TODO: filter
    #        continue
        x.append(float(X))
        y.append(float(Y))

    return x,y

def btt_q2c_latency(data_file):
    return read_data_from_file(data_file)

def param_init():
    parser = argparse.ArgumentParser(description='L3 plot tool')
    parser.add_argument('-c', nargs=1, metavar='cvs', help='Specify the CVS file.')
    parser.add_argument('-p', nargs=1, metavar='path', help='file to be plotted.')
    parser.add_argument('-a', nargs=1, metavar='path', help='file to be plotted.')
    parser.add_argument('-q', nargs=1, metavar='path', help='file to be plotted.')
    parser.add_argument('-z', nargs=1, metavar='path', help='file to be plotted.')
    parser.add_argument('-l', nargs=1, metavar='path', help='file to be plotted.')
    parser.add_argument('-b', nargs=1, metavar='bio_data', help='btt dump file to be plotted.')
    parser.add_argument('-s', nargs=1, metavar='sow.', help='Specify the SOW list file.')
    
    return parser.parse_args();

def render_line(plot, x, y, c,legend):
    p = plot
    renderer = p.line(x, y, color=c, alpha=1, hover_line_color=c, hover_line_alpha=0.7, legend_label=legend, line_width=2)
    #renderer = p.line(x, y, color=c, line_width=2)

    # https://stackoverflow.com/questions/49282078/multiple-hovertools-for-different-lines-bokeh
    p.add_tools(HoverTool(renderers=[renderer], tooltips=legend))


def main():

    args = param_init()

    # prepare some data
    x = [1, 2, 3, 4, 5]
    y = [4, 5, 5, 7, 2]

    if args.a:
        print("Btt Q & C activity file is %s" %(args.a[0]))
        x,y = btt_Q_C_activity(args.a[0])
        return;

    # create a plot
    p = figure(
        title="L3 Plot",
        sizing_mode="stretch_width",
        #max_width=500,
        #height=250,
        tools=[HoverTool(line_policy='nearest')],
        tooltips="@x,@y",
    )

    numLines=50
    colors = itertools.cycle(palette)# create a color iterator 

    if args.b:
        print("Data file is %s" %(args.b[0]))
        x,y = read_data_from_file(args.b[0])
        render_line(p, x, y, next(colors), "data")

    if args.q:
        print("q2c data file is %s" %(args.q[0]))
        x,y = read_data_from_file(args.q[0])
        render_line(p, x, y, next(colors), "q2c")

    if args.z:
        print("q2d data file is %s" %(args.z[0]))
        x,y = read_data_from_file(args.z[0])
        render_line(p, x, y, next(colors), "q2d")

    if args.l:
        print("d2c data file is %s" %(args.z[0]))
        x,y = read_data_from_file(args.z[0])
        render_line(p, x, y, next(colors), "d2c")


    # change the fill colors
    p.background_fill_color = (204, 255, 255)
    p.border_fill_color = (102, 204, 255)
    p.outline_line_color = (0, 0, 255)

    # show the results
    show(p)

# Test blktrace data: blktrace -w 10 -d /dev/sda -o iotrace
def test_main():
    # prepare some data
    x = [1, 2, 3, 4, 5]
    y = [4, 5, 5, 7, 2]

    # create a plot
    p = figure(
        title="I/O example",
        sizing_mode="stretch_width",
        max_width=500,
        height=250,
        tools=[HoverTool(line_policy='nearest')],
        tooltips="x:@x y:@y",
    )

    # add a renderer
    p.line(x, y, line_color="green", line_width=2)

    # change the fill colors
    p.background_fill_color = (204, 255, 255)
    p.border_fill_color = (102, 204, 255)
    p.outline_line_color = (0, 0, 255)

    # show the results
    show(p)

if __name__ == "__main__":
    main()
