#!/usr/bin/python

from bokeh.plotting import figure, output_file, show
from bokeh.palettes import inferno# select a palette
import itertools# itertools handles the cycling 
from bokeh.models import HoverTool


# output_file("/tmp/line.html")
# TODO
# Preserve absoulte time in btt output

with open("combined_bp.log") as f:
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
