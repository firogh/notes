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



def read_btt_q_c_activity_file(data_file):
    return

def btt_Q_C_activity(data_file):
    with open("data_file") as f:
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
    renderer = p.line(x, y, color=c, alpha=0.5, hover_line_color="firebrick", hover_line_alpha=1.0, legend_label=legend, line_width=1)
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
