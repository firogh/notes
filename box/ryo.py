#!/usr/bin/python

from bokeh.plotting import figure,output_file,  show
from bokeh.palettes import inferno# select a palette
from bokeh.palettes import Dark2_5 as palette
from bokeh.models import DatetimeTickFormatter
from bokeh.models import FixedTicker

from bokeh.models import ColumnDataSource, RangeTool


from bokeh.palettes import Category10_10

from bokeh.layouts import column, row

import itertools# itertools handles the cycling 
from bokeh.models import HoverTool
import argparse

import pandas as pd

import numpy as np

import code

from bokeh.models import BoxAnnotation

output_file("/tmp/b.html")

# TODO
# Modify blkparse to geneate combined blktrace binary which could preserve absoulte time in btt output

# TODO
# Horizontally Stacked Bars
# https://notebook.community/dennisobrien/bokeh/examples/howto/Categorical%20Data

# TODO
# Add iotop support

# TODO
# Add task latency support

# TODO
# Improve sar/sysstat output

# TODO
# Scalable x-axis
# https://www.emilkhatib.com/interactive-plots-with-bokeh/

# TODO
# Btt Q & C activity 
# Step lines
# https://docs.bokeh.org/en/2.4.1/docs/user_guide/plotting.html
# https://docs.bokeh.org/en/2.4.2/docs/reference/models/glyphs/step.html
# https://stackoverflow.com/questions/45475780/how-do-i-add-hover-highlighting-to-bokeh-step-chart

sadf_format = 1

def read_sar_data_file(data_file, extra_legend_idx=0):
    with open(data_file) as f:
        data = f.read()

    data = data.split('\n')

    if not data[0].startswith("Linux"):
        print("Data corruption %s: %s\n"%(data_file, data[0]))
        exit(1);

    data_line = 0
    sar_data = {} 
    sar_data["extra_idx"] = extra_legend_idx

    line_no = -1
    for row in data:
        line_no = line_no + 1
        if data[line_no].startswith("Linux"):
            # First line.
            continue
        elif len(data[line_no]) < 5:     # Testing empty line; for robust, don't use len(data[line_no] == 0. 
            # Empty line
            continue
        elif "LINUX RESTART" in data[line_no]:
            continue
        else:
            # Title
            fields = data[line_no].split()

            sar_data['x'] = []
            sar_data["title"] = {}
            sdt= sar_data["title"] 
            sar_data["metrics"] = {}
            sar_data["extra_legend"] = {}
            sdm = sar_data["metrics"]
            s_idx = 1
            for f in fields[1:]:
                sdt[s_idx] = f
                sdm[s_idx] = []
                print((s_idx))
                s_idx = s_idx + 1

            sar_data["last_idx"] = s_idx        # Don't do 's_idx -1', it's convinent for for i in range(1, sar_data["last_idx"])

            data_line = 1
            break

# sar_data = {
#       'x': x_vals
#       'title': titles
#       'metrics': counters for non-extra-legend under each titles
#       'extra_legend': {
#                       'metrics': { counters for extra-legend
#
    x_count = 0
    m_count = 0
    skip_count = 0
    for row in data[line_no + 1:]:
        if data_line == 1:
            if row.startswith("A"):
                data_line = 0
                continue
            if row.startswith("S"):
                data_line = 0
                continue
            row = row.split()
            if len(row) < 5:
                data_line = 0
                continue
            #x_count = x_count + 1
            #print("x========= %d"%x_count)
            #print(row)
            if extra_legend_idx == 0:
                sar_data['x'].append(row[0])
                m_idx=1
                for f in row[1:]:
                    sdm[m_idx].append(f)
                    m_idx = m_idx + 1
            else:
                # 09:20:11          DEV       tps     rkB/s
                # 09:30:11          sda      5.97      9.34
                # sda is extra_legend-^
                legend = row[1]                                 
                sde = sar_data["extra_legend"]                  
                if legend in sde:
                    sdel = sde[legend]
                else:
                    sde[legend]= {}
                    sdel = sde[legend]
                    sdel["metrics"] = {}
                    for idx in range(2, sar_data["last_idx"]):
                        sdel["metrics"][idx] = []
                    sdel["metrics"][0] = []

                counts_dicts = sde[legend]["metrics"]
                m_idx = 2
                for f in row[2:]:
                    counts_dicts[m_idx].append(f)
                    m_idx = m_idx + 1
                counts_dicts[0].append(row[0])
                #m_count = m_count + 1
                #print("m-------------%d %d"%(m_count, len(counts_dicts[2])))
                #print(row)

        elif skip_count > 0:
            skip_count = skip_count -1
        elif len(row) < 5:     # Testing empty line; for robust, don't use len(row) == 0. 
            # Empty line
            data_line = 0
        elif row.startswith("A"):        # Short for Average.
                data_line = 0
        elif row.startswith("S"):        # Short for Summary.
                data_line = 0
        elif "LINUX RESTART" in row:
            count = 2;
        else:
            data_line = 1
                
    return sar_data 

def parse_sar_data_title(title, sar_data):
    fields = title.split()

    sar_data['x'] = []
    sar_data["title"] = {}
    sdt= sar_data["title"] 
    sar_data["metrics"] = {}
    sar_data["extra_legend"] = {}
    sdm = sar_data["metrics"]
    s_idx = 1 
    for f in fields[1:]:
        sdt[s_idx] = f 
        sdm[s_idx] = []
        print((s_idx, f))
        s_idx = s_idx + 1 

    sar_data["last_idx"] = s_idx        # Don't do 's_idx -1', it's convinent for for i in range(1, sar_data["last_idx"])

    return sar_data

def __parse_sar_data(data_lines, sar_data):

    sdm = sar_data["metrics"]

    if sar_data["extra_idx"] == 0:
        for row in data_lines:
            row = row.split()
            sar_data['x'].append(row[0])
            m_idx=1
            for f in row[1:]:
                sdm[m_idx].append(f)
                m_idx = m_idx + 1 
    else:
        for row in data_lines:
            # 09:20:11          DEV       tps     rkB/s
            # 09:30:11          sda      5.97      9.34
            # sda is extra_legend-^
            # or
            # 16:35:46     MBfsfree  MBfsused   %fsused  %ufsused     Ifree     Iused    %Iused FILESYSTEM
            # 16:35:47        21532     28556     57.01     62.15   2373847    902953     27.56 /dev/sda1

            row = row.split()
            sar_data['x'].append(row[0])
            legend = row[sar_data["extra_idx"]]
            sde = sar_data["extra_legend"]    
            if legend in sde:
                sdel = sde[legend]
            else:
                sde[legend]= {}
                sdel = sde[legend]
                sdel["metrics"] = {}
                for idx in range(0, sar_data["last_idx"]):
                    if idx == sar_data["extra_idx"]:
                        continue
                    sdel["metrics"][idx] = []

            sdelm = sde[legend]["metrics"]
            m_idx = 0
            for f in row:
                if m_idx == sar_data["extra_idx"]:
                    m_idx = m_idx + 1
                    continue
                sdelm[m_idx].append(f)
                m_idx = m_idx + 1

    return sar_data

def parse_sar_data(sar_file, extra_legend_idx=0):
    with open(sar_file) as f:
        data = f.read()

    data = data.split('\n')

    if not data[0].startswith("Linux"):
        print("Data corruption at the first line: %s: %s\n"%(sar_file, data[0]))
        exit(1);

#    Linux 5.19.4-200.fc36.x86_64 (snow)     09/05/22        _x86_64_        (4 CPU)
    sys_info = data[0]
    sys_info = sys_info.split()
    kver = sys_info[1]
    host_name = sys_info[2]
    sar_date=sys_info[3]
    host_arch=sys_info[4]
    host_cpus=sys_info[5]+sys_info[6]

    sar_data = {} 
    sar_data["sys_info"]={}
    si = sar_data["sys_info"]
    si["kver"] = kver
    si["host"] = host_name
    si["date"] = sar_date
    si["arch"] = host_arch
    si["cpus"] = host_cpus

    if len(data[1]) != 0:
        print("Data corruption at the 2nd line %s: %s\n"%(data_file, data[0]))
        exit(2)
    if len(data[2]) == 0:
        print("Data corruption at the title line: %s: %s\n"%(data_file, data[0]))
        exit(3)

    sar_data = parse_sar_data_title(data[2], sar_data)

    sar_data["extra_idx"] = extra_legend_idx

    prev_row = ""
    is_data_line = 0
    skip = 0
    data_lines = []
    for row in data[1:]:

        if len(row) < 3:
            is_data_line = 0
            skip = 1                            # skip title
            for line in reversed(data_lines):
                if 'LINUX RESTART' in line:
                    data_lines.pop()
                elif line.startswith('A'):
                    data_lines.pop()
                elif line.startswith('S'):
                    data_lines.pop()
                else:
                    break;
        elif is_data_line == 1:
            data_lines.append(row)
        else:
            if skip == 1:
                skip = 0
                is_data_line = 1
    
    for line in reversed(data_lines):
        if line.startswith('A'):
            data_lines.pop()
        elif line.startswith('S'):
            data_lines.pop()
        else:
            break;

    sar_data = __parse_sar_data(data_lines, sar_data)

    #code.interact(local=locals())
    return sar_data

def parse_sadf_data_title(title, sar_data):
    fields = title.split(';')

    print("title %s"%title)
    sar_data['x'] = []
    sar_data["title"] = {}
    sdt= sar_data["title"] 
    sar_data["metrics"] = {}
    sar_data["extra_legend"] = {}
    sdm = sar_data["metrics"]
    s_idx = 1 
    for f in fields[1:]:
        sdt[s_idx] = f 
        sdm[s_idx] = []
        print((s_idx, f))
        s_idx = s_idx + 1 

    sar_data["last_idx"] = s_idx        # Don't do 's_idx -1', it's convinent for for i in range(1, sar_data["last_idx"])

    return sar_data

def add_row_to_sar_data(row, sar_data):

    sdm = sar_data["metrics"]

    ts_idx = 2
    if sar_data["extra_idx"] == 0:
        sar_data['x'].append(row[ts_idx])
        m_idx=ts_idx
        for f in row[ts_idx:]:
            sdm[m_idx].append(f)
            m_idx = m_idx + 1
    else:
            # 09:20:11          DEV       tps     rkB/s
            # 09:30:11          sda      5.97      9.34
            # sda is extra_legend-^
            # or
            # 16:35:46     MBfsfree  MBfsused   %fsused  %ufsused     Ifree     Iused    %Iused FILESYSTEM
            # 16:35:47        21532     28556     57.01     62.15   2373847    902953     27.56 /dev/sda1

        sar_data['x'].append(row[ts_idx])
#        code.interact(local=locals())
        legend = row[sar_data["extra_idx"]]
        sde = sar_data["extra_legend"]    
        if legend in sde:
            sdel = sde[legend]
        else:
            sde[legend]= {}
            sdel = sde[legend]
            sdel["metrics"] = {}
            for idx in range(0, sar_data["last_idx"]):
                if idx == sar_data["extra_idx"]:
                    continue
                sdel["metrics"][idx] = []

        sdelm = sde[legend]["metrics"]
        m_idx = ts_idx
        for f in row[ts_idx:]:
            if m_idx == sar_data["extra_idx"]:
                m_idx = m_idx + 1
                continue
            sdelm[m_idx].append(f)
            m_idx = m_idx + 1

def add_np_nan_row_to_sar_data(row, sar_data):

    sdm = sar_data["metrics"]

    ts_idx = 2
    if sar_data["extra_idx"] == 0:
        sar_data['x'].append(row[ts_idx])
        m_idx=ts_idx
        for f in row[ts_idx:]:
            sdm[m_idx].append(f)
            m_idx = m_idx + 1
    else:
            # 09:20:11          DEV       tps     rkB/s
            # 09:30:11          sda      5.97      9.34
            # sda is extra_legend-^
            # or
            # 16:35:46     MBfsfree  MBfsused   %fsused  %ufsused     Ifree     Iused    %Iused FILESYSTEM
            # 16:35:47        21532     28556     57.01     62.15   2373847    902953     27.56 /dev/sda1

        sar_data['x'].append(row[ts_idx])
#        code.interact(local=locals())

        legend = row[sar_data["extra_idx"]]
        sde = sar_data["extra_legend"]    
        for legend in sde:
            row[sar_data["extra_idx"]] = legend
            sdelm = sde[legend]["metrics"]
            m_idx = ts_idx
            for f in row[ts_idx:]:
                if m_idx == sar_data["extra_idx"]:
                    m_idx = m_idx + 1
                    continue
                sdelm[m_idx].append(f)
                m_idx = m_idx + 1

def parse_sadf_data(sadf_file, extra_legend_idx=0):
    with open(sadf_file) as f:
        data = f.read()

    data = data.split('\n')

    host_name = data[0].split(';')[0]
    if data[0].startswith("#"):
        host_name = data[1].split(';')[0]

    sar_data = {} 
    sar_data["sys_info"]={}
    si = sar_data["sys_info"]
    si["kver"] = 'unknown'
    si["host"] = host_name
    si["date"] = 'unknown'
    si["arch"] = 'unknown'
    si["cpus"] = 'unknown'

    initialized = 0
    for i in range(0, 10):
        if data[i].startswith('#'):
            sar_data = parse_sadf_data_title(data[i], sar_data)
            initialized = 1
            break

    if initialized == 0:
        print("Corrupt data")
        exit(1)

    sar_data["extra_idx"] = extra_legend_idx
    print(sar_data["extra_idx"])

    data_lines = []
    ts_idx = 2
    tmp_idx = 0
    for row in data:
        if row.startswith('#'):
            continue
        else:
            #code.interact(local=locals())
#            print(row)
            if len(row) == 0:
                continue
            row = row.split(';')
            if row[1] == '-1':
                if len(data_lines) != 0:
                    line = data_lines[-1]
                    ts = int(line[ts_idx]) + 1          # Unix time
                    prev_line = []
                    prev_line.append(host_name)
                    prev_line.append('0')
                    prev_line.append(str(ts))
                    for tmp_idx in range(ts_idx+1, sar_data["last_idx"]):
                        prev_line.append(np.nan)
                    if extra_legend_idx != 0:
                        prev_line[extra_legend_idx] = 'Restart'
                    add_row_to_sar_data(prev_line, sar_data)
                    add_np_nan_row_to_sar_data(prev_line, sar_data)

                ts = row[2]
                next_line = []
                next_line.append(host_name)
                next_line.append('0')
                next_line.append(ts)
                for tmp_idx in range(ts_idx+1, sar_data["last_idx"]):
                    next_line.append(np.nan)
                if extra_legend_idx != 0:
                    next_line[extra_legend_idx] = 'Restart'
                add_row_to_sar_data(next_line, sar_data)
                add_np_nan_row_to_sar_data(next_line, sar_data)
            else:
                add_row_to_sar_data(row, sar_data)

    #code.interact(local=locals())
    return sar_data


def __render_sar_data(sar_data, data_group=[], sar_title="sar"):
    # p = figure(width=400, height=400)
    #p = figure(sizing_mode="stretch_width", title="Btt Q & C activity", x_axis_type='datetime')
    #p = figure(sizing_mode="fixed", title=sar_title, x_axis_type='datetime', x_axis_location="above",tools="xpan",background_fill_color="#efefef",x_range=None)
    #p = figure(sizing_mode="fixed", title=sar_title, x_axis_type='datetime', background_fill_color="#efefef",x_range=None)
    #p = figure(sizing_mode="stretch_both", title=sar_title, x_axis_type='datetime')
    #p = figure(sizing_mode="scale_both", title=sar_title, x_axis_type='datetime')
    #p = figure(sizing_mode="scale_height", title=sar_title, x_axis_type='datetime')
    #p = figure(sizing_mode="stretch_height", title=sar_title, x_axis_type='datetime')
    p = figure(width=400, height=1000, title=sar_title, x_axis_type='datetime')
    #p = figure(sizing_mode="stretch_width", title="Btt Q & C activity")#, x_axis_type='datetime')

#    p.xaxis.formatter.days = ['%Y-%W']
#    p.xaxis.formatter = DatetimeTickFormatter(format="%T")
#    p.xaxis.formatter.hours = ["%H"]
#    p.xaxis.formatter.minutes = ["%M"]
    #p.xaxis.formatter = DatetimeTickFormatter(formats=dict(seconds = ["%H:%M:%S"]))
    #p.xaxis.formatter.seconds = ["%H:%M:%S"]

    #p.xaxis.formatter = DatetimeTickFormatter(hourmin = ['%H:%M'])

    si = sar_data['sys_info']
    p.title.text = sar_title+'-'+ si['host']+'-'+si['kver']+'-'+si['date']

    numLines=20

    # TODO
    # Add rangetool support
    # FIXME
    # I don't know how to convert data from pandas's DataFrame 
    # to RangeID instance
    #select = figure(title="Drag the middle and edges of the selection box to change the range above",
                #height=130, width=800, y_range=p.y_range,
                #x_axis_type="datetime", y_axis_type=None,
                #tools="", background_fill_color="#efefef")
    #df = pd.DataFrame(sar_data['x'])
    #x_date = pd.to_datetime(df[0], format="%H:%M:%S")

    #code.interact(local=locals())
#    p.x_range.start = x_date.min()
#    p.x_range.end = x_date.max()
    #p.x_range = (x_date.min(), x_date.max())

    #range_tool = RangeTool(x_range=p.x_range)
    #range_tool.overlay.fill_color = "navy"
    #range_tool.overlay.fill_alpha = 0.2

    #select.line('date', 'close', source=source)
    #select.ygrid.grid_line_color = None
    #select.add_tools(range_tool)
    #select.toolbar.active_multi = range_tool

      
    colors = Category10_10.__iter__()

    # colors = itertools.cycle(inferno(numLines))# create a color iterator 


# >>> sar_data["extra_legend"]
# {'sda': {'metrics': {1: ['44.35'], 2: ['63.99'], 3: ['175.94'], 4: ['22145.58'], 5: ['504.76'], 6: ['0.06'], 7: ['1.21'], 8: ['3.42']}}, 'zram0': {'metrics': {1: ['0.00'], 2: ['0.01'], 3: ['0.00'], 4: ['0.00'], 5: ['4.00'], 6: ['0.00'], 7: ['1.00'], 8: ['0.00']}}}
    sdt = sar_data["title"]
    # df.index = pd.to_datetime(df.index)

    # sar_data["extra_legend"]['sda']['metrics'][0]
    if sar_data["extra_idx"] != 0:

        # Debug
        #code.interact(local=locals())
#        print("=============================extra")
        for el, sde in sar_data["extra_legend"].items():
            # l is legend
            # sde is sar_data["extra_legend"][l]
            sdem = sde["metrics"]
            if sadf_format != 1:
                df = pd.DataFrame(sdem[0])
                #df.index = pd.to_datetime(df.index, unit='s')
                x_date = pd.to_datetime(df[0], format="%H:%M:%S")
            else:
                df = pd.DataFrame(sdem[2])
                x_date = pd.to_datetime(df[0], unit='s')
                
            # import pandas as pd

            

            # p.xaxis.axis_label = 'time'

            #p.xaxis.formatter = DatetimeTickFormatter(days="%d-%b-%Y %H:%M:%S")
            # https://discourse.bokeh.org/t/x-axis-datetime-display-not-rendering-full-format-for-smaller-range/8207/2
            p.xaxis.formatter = DatetimeTickFormatter(years="%d/%m/%Y %H:%M:%S",
                                          months="%d/%m/%Y %H:%M:%S",
                                          days="%d/%m/%Y %H:%M:%S",
                                          hours="%d/%m/%Y %H:%M:%S",
                                          hourmin="%d/%m/%Y %H:%M:%S",
                                          minutes="%d/%m/%Y %H:%M:%S",
                                          minsec="%d/%m/%Y %H:%M:%S",
                                          seconds="%d/%m/%Y %H:%M:%S",
                                          milliseconds="%d/%m/%Y %H:%M:%S",
                                          microseconds="%d/%m/%Y %H:%M:%S")


            # https://programminghistorian.org/en/lessons/visualizing-with-bokeh
            # BoxAnnotation indicating that system just booted up
            if el == 'Restart':
                for ts in sdem[2]:
                    ts_right =  int(ts) + 10
                    #box_df = pd.DataFrame(ts)
                    #box_left = pd.to_datetime(box_df[0], unit='s')
                    box_left = pd.to_datetime(ts, unit='s')
                    # box_df = pd.DataFrame(ts_right)
                    box_right = pd.to_datetime(ts_right, unit='s')
                
                    #box_left = pd.to_datetime('6-6-1944')
                    #box_right = pd.to_datetime('16-12-1944')

                    box = BoxAnnotation(left=box_left, right=box_right,
                        #line_width=1, line_color='black', #line_dash='dashed',
                        line_width=2, line_color='black', #line_dash='dashed',
                        fill_alpha=0.5, fill_color='black')#orange')

                    p.add_layout(box)
                continue

            #df.index.freq = 'S'
            #df.dropna().head()

            # TODO p.xaxis.formatter = DatetimeTickFormatter(days="%H:%M:%S")

            # FIXME
            # ommit x-axis with missing data
            # https://discourse.bokeh.org/t/solved-datetime-axis-missing-values-skipped-adaptive-formatting/2766/6

            # FIXME
            # Customized X-axis ticker
            # We want to use the date time from text file as the x-axis ticker.
            # https://stackoverflow.com/questions/46916489/bokeh-fixedticker-with-custom-datetime-timestamp-values
            # https://discourse.bokeh.org/t/fixed-tickers-along-a-datetime-axis/1086
            # tick_vals = pd.Series([pd.Timestamp('2017-10-02'), pd.Timestamp('2017-10-05')]).astype(np.int64)

            # FutureWarning: casting datetime64[ns] values to int64 with .astype(...) is deprecated and 
            # will raise in a future version. Use .view(...) instead.
            # TODO tick_vals = pd.Series([pd.Timestamp(x) for x in sdem[0]]).view(np.int64) / 10**6
            # tick_vals = tick_vals
            # TODO tick_vals = tick_vals.astype(float)
            #code.interact(local=locals())
            #p.xaxis.ticker = FixedTicker(ticks=list(tick_vals))

            for idx, counts_list in sdem.items():                 # k is metrics. sdem is sde["metrics"]

                if sadf_format == 1:
                    if idx == 2:
                        continue
                else:
                    if idx ==  0:
                        continue

                if idx == sar_data["extra_idx"]:
                    continue

                if len(data_group) != 0:

                    if sadf_format == 1:
                        tmp_idx = idx - 2

                    if tmp_idx not in data_group:
                        continue
                legend = el + '-' + sdt[idx]
                
                try:
                    c = next(colors)
                except StopIteration:
                    colors = Category10_10.__iter__()
                    c = next(colors)
                    
            
                #renderer = p.line(sar_data['x'], counts_list, color=c, legend_label=legend, line_width=2)
                renderer = p.line(x_date, counts_list, color=c, legend_label=legend, line_width=2)
                #renderer = p.line(sdem[0], counts_list, color=c, legend_label=legend, line_width=2)
                #renderer = p.line(sdem[0], counts_list, color=c, legend_label=legend, line_width=2)
                #renderer = p.line(x, y, color=c, line_width=2)

                # https://stackoverflow.com/questions/49282078/multiple-hovertools-for-different-lines-bokeh
                p.add_tools(HoverTool(renderers=[renderer], tooltips=[('', legend)]))
                #p.xaxis.formatter = DatetimeTickFormatter(days="%H:%M:%S")

    else:
        #print("no =============================extra")
        df = pd.DataFrame(sar_data['x'])
        #df.index = pd.to_datetime(df.index, unit='s')
        x_date = pd.to_datetime(df[0], format="%H:%M:%S")
 
        #code.interact(local=locals())
        for idx, counts_list in sar_data["metrics"].items():
        #    print("%d %s"%(idx, counts_list))

            if len(data_group) != 0:
                if idx not in data_group:
                    continue

            legend = sdt[idx]
            # c = next(colors)

            try:
                c = next(colors)
            except StopIteration:
                colors = Category10_10.__iter__()
                c = next(colors)
                
            renderer = p.line(x_date, counts_list, color=c, legend_label=legend, line_width=2)
            p.add_tools(HoverTool(renderers=[renderer], tooltips=[('', legend)]))

    return p
    #return row(p, select)

def render_sar_data(data, title, grps=[[]]):
    figures = []
    for grp in grps:
        figure = __render_sar_data(data, grp, title)

        p = figure

        p.plot_height=300
        p.plot_width=1500

        figures.append(figure)
    show(column(figures))
    #show(row(figures))
    return

def read_sar_d_data_file(data_file):
    return read_sar_data_file(data_file)

parse_handler = read_sar_data_file
parse_handler = parse_sadf_data
def ____sar(data_file, title, grps, extra=0):

    if parse_handler == parse_sadf_data:
        if extra != 0:
            extra = extra + 2

    data = parse_handler(data_file, extra)

    render_sar_data(data, title, grps)

    return

def __sar_parse(data_file, title, grps, extra_idx=0):
    sar_data = parse_sar_data(data_file, extra_idx)
    render_sar_data(sar_data, title, grps)

    return
def __sar(data_file, title, grps, extra=0):

    ____sar(data_file, title, grps, extra)
    return

def sar_b(data_file):
    title = "sar-b"
    grps = [[1,2,3,4],[5,6,7]]
    __sar(data_file, title, grps, 0)

def sar_B(data_file):
    title = "sar-B"
    grps = [[1,2],[3,4],[5,6,7,8],[9]]
    __sar(data_file, title, grps, 0)


def sar_d(data_file):

    title = "sar-d"
    grps = [[2], [3,4,5], [6],[7],[8],[9]]
    __sar(data_file, title, grps, 1)

    return
def __sar_parse(data_file, title, grps, extra_idx=0):
    sar_data = parse_sar_data(data_file, extra_idx)
    render_sar_data(sar_data, title, grps)

    return

def sar_F(data_file):

    title = "sar-F"
    grps = [[1, 2], [3,4,7], [5,6]]
    __sar_parse(data_file, title, grps, 8)

    return

def sar_W(data_file):

    title = "sar-W"
    grps = [[1,2]]
    __sar(data_file, title, grps)

    return
def sar_u(data_file):

    title = "sar-u"
    grps = [[]]
    __sar(data_file, title, grps, 1)

    return

def sar_w(data_file):

    title = "sar-w"
    grps = [[1,2]]
    __sar(data_file, title, grps)

    return


def sar_v(data_file):

    title = "sar-v"
    grps = [[1,2, 3,4]]
    __sar(data_file, title, grps)

    return
def sar_nsoft(data_file):

    title = "sar-n-soft"
    grps=[[]]
    __sar(data_file, title, grps)

    return

def sar_ntcp(data_file):

    title = "sar-n-"
    grps=[[]]
    __sar(data_file, title, grps)

    return


def sar_nudp(data_file):

    title = "sar-n-"
    grps=[[]]
    __sar(data_file, title, grps)

    return


def sar_netcp(data_file):

    title = "sar-n-"
    grps=[[]]
    __sar(data_file, title, grps)

    return


def sar_nsock(data_file):

    title = "sar-n-"
    grps=[[]]
    __sar(data_file, title, grps)

    return


def sar_nnfsd(data_file):

    title = "sar-n-"
    grps=[[]]
    __sar(data_file, title, grps)

    return


def sar_nnfs(data_file):

    title = "sar-n-"
    grps=[[]]
    __sar(data_file, title, grps)

    return


def sar_nip(data_file):

    title = "sar-n-"
    grps=[[]]
    __sar(data_file, title, grps)

    return


def sar_neip(data_file):

    title = "sar-n-"
    grps=[[]]
    __sar(data_file, title, grps)

    return


def sar_nicmp(data_file):

    title = "sar-n-"
    grps=[[]]
    __sar(data_file, title, grps)

    return


def sar_neicmp(data_file):

    title = "sar-n-"
    grps=[[]]
    __sar(data_file, title, grps)

    return


def sar_nfc(data_file):

    title = "sar-n-"
    grps=[[]]
    __sar(data_file, title, grps)

    return


def sar_nedev(data_file):

    title = "sar-n-"
    grps=[[]]
    __sar(data_file, title, grps)

    return


def sar_ndev(data_file):

    title = "sar-n-"
    grps=[[]]
    __sar(data_file, title, grps)

    return


def sar_qio(data_file):

    title = "sar-q-io"
    grps=[[],[1,2,3,4],[5,6,7,8] ]
    __sar(data_file, title, grps)

    return

def sar_qmem(data_file):

    title = "sar-q-mem"
    grps=[[]]
    __sar(data_file, title, grps)

    return

def sar_qcpu(data_file):

    title = "sar-q-cpu"
    grps=[[]]
    __sar(data_file, title, grps)

    return


def sar_qload(data_file):

    title = "sar-q-load"
    grps=[[],[1,3,4,5,6]]
    __sar(data_file, title, grps)

    return


def sar_r(data_file):

    title = "sar-r"
    grps = [[1,2, 3,5, 6,7,9,10,11], [4,8], [1,2,3],[5,6,7,11],[9,10]]
    __sar(data_file, title, grps)

    return

def sar_H(data_file):

    title = "sar-H"
    grps = [[1, 2,4,5], [3]]
    __sar(data_file, title, grps)

    return

def sar_S(data_file):

    title = "sar-S"
    grps = [[1,2,4], [3,5]]
    __sar(data_file, title, grps)

    return


def sar_I(data_file):

    title = "sar-H"
    grps = [[2]]
    __sar(data_file, title, grps, 1)

    return


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
            p.add_tools(HoverTool(renderers=[renderer], tooltips=[('', legend)]))

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

def render_line(plot, x, y, c,legend):
    p = plot
    renderer = p.line(x, y, color=c, alpha=1, hover_line_color=c, hover_line_alpha=0.7, legend_label=legend, line_width=2)
    #renderer = p.line(x, y, color=c, line_width=2)

    # https://stackoverflow.com/questions/49282078/multiple-hovertools-for-different-lines-bokeh
    p.add_tools(HoverTool(renderers=[renderer], tooltips=legend))

def param_init():
    parser = argparse.ArgumentParser(description='L3 plot tool')
    parser.add_argument('-c', nargs=1, metavar='cvs', help='Specify the CVS file.')
    parser.add_argument('-p', nargs=1, metavar='path', help='file to be plotted.')
    parser.add_argument('-a', nargs=1, metavar='path', help='file to be plotted.')
    parser.add_argument('-q', nargs=1, metavar='q2c', help='btt file to be plotted.')
    parser.add_argument('-z', nargs=1, metavar='q2d', help='btt file to be plotted.')
    parser.add_argument('-l', nargs=1, metavar='c2d', help='btt file to be plotted.')
    parser.add_argument('-i', nargs=1, metavar='bio_data', help='btt dump file to be plotted.')
    parser.add_argument('-s', nargs=1, metavar='sow.', help='Specify the SOW list file.')
    parser.add_argument('-d', nargs=1, metavar='sar-d', help='sar data file to be plotted.')
    parser.add_argument('-b', nargs=1, metavar='sar-b', help='sar data file to be plotted.')
    parser.add_argument('-B', nargs=1, metavar='sar-B', help='sar data file to be plotted.')
    parser.add_argument('-P', nargs=1, metavar='sar test data', help='sar data file to be plotted.')
    parser.add_argument('-F', nargs=1, metavar='sar-F', help='sar data file to be plotted.')
    parser.add_argument('-H', nargs=1, metavar='sar-H', help='sar data file to be plotted.')
    parser.add_argument('-I', nargs=1, metavar='sar-I', help='sar data file to be plotted.')
    parser.add_argument('-S', nargs=1, metavar='sar-S', help='sar data file to be plotted.')
    parser.add_argument('-v', nargs=1, metavar='sar-v', help='sar data file to be plotted.')
    parser.add_argument('-w', nargs=1, metavar='sar-w', help='sar data file to be plotted.')
    parser.add_argument('-W', nargs=1, metavar='sar-W', help='sar data file to be plotted.')
    parser.add_argument('-u', nargs=1, metavar='sar-u', help='sar data file to be plotted.')
    parser.add_argument('-r', nargs=1, metavar='sar-r', help='sar data file to be plotted.')

    parser.add_argument('--qload', nargs=1, metavar='sar-q-LOAD', help='sar data file to be plotted.')
    parser.add_argument('--qio', nargs=1, metavar='sar-q-IO', help='sar data file to be plotted.')
    parser.add_argument('--qcpu', nargs=1, metavar='sar-q-CPU', help='sar data file to be plotted.')
    parser.add_argument('--qmem', nargs=1, metavar='sar-q-MEM', help='sar data file to be plotted.')

    parser.add_argument('--nsoft', nargs=1, metavar='sar-n-SOFT', help='sar data file to be plotted.')
    parser.add_argument('--ntcp', nargs=1, metavar='sar-n-TCP', help='sar data file to be plotted.')
    parser.add_argument('--nudp', nargs=1, metavar='sar-n-UDP', help='sar data file to be plotted.')
    parser.add_argument('--netcp', nargs=1, metavar='sar-n-ETCP', help='sar data file to be plotted.')
    parser.add_argument('--nsock', nargs=1, metavar='sar-n-SOCK', help='sar data file to be plotted.')
    parser.add_argument('--nnfsd', nargs=1, metavar='sar-n-NFSD', help='sar data file to be plotted.')
    parser.add_argument('--nnfs', nargs=1, metavar='sar-n-NFS', help='sar data file to be plotted.')
    parser.add_argument('--nip', nargs=1, metavar='sar-n-IP', help='sar data file to be plotted.')
    parser.add_argument('--neip', nargs=1, metavar='sar-n-EIP', help='sar data file to be plotted.')
    parser.add_argument('--neicmp', nargs=1, metavar='sar-n-EICMP', help='sar data file to be plotted.')
    parser.add_argument('--nicmp', nargs=1, metavar='sar-n-ICMP', help='sar data file to be plotted.')
    parser.add_argument('--nfc', nargs=1, metavar='sar-n-FC', help='sar data file to be plotted.')
    parser.add_argument('--nedev', nargs=1, metavar='sar-n-EDEV', help='sar data file to be plotted.')
    parser.add_argument('--ndev', nargs=1, metavar='sar-n-DEV', help='sar data file to be plotted.')
    
    return parser.parse_args();

def main():

    args = param_init()

    # prepare some data
    x = [1, 2, 3, 4, 5]
    y = [4, 5, 5, 7, 2]

    if args.a:
        print("Btt Q & C activity file is %s" %(args.a[0]))
        btt_Q_C_activity(args.a[0])
        return

    if args.P:
        print("Test file is %s" %(args.P[0]))
        parse_sar_data(args.P[0], 8)
        return

    if args.F:
        print("Sar -F file is %s" %(args.F[0]))
        sar_F(args.F[0])
        return

    option = args.nsoft
    if option:
        print("Sar -n  file is %s" %(option[0]))
        sar_nsoft(option[0])
        return


    option = args.ntcp
    if option:
        print("Sar -n  file is %s" %(option[0]))
        sar_ntcp(option[0])
        return


    option = args.nudp
    if option:
        print("Sar -n  file is %s" %(option[0]))
        sar_nudp(option[0])
        return


    option = args.netcp
    if option:
        print("Sar -n  file is %s" %(option[0]))
        sar_netcp(option[0])
        return


    option = args.nsock
    if option:
        print("Sar -n  file is %s" %(option[0]))
        sar_nsock(option[0])
        return


    option = args.nnfsd
    if option:
        print("Sar -n  file is %s" %(option[0]))
        sar_nnfsd(option[0])
        return


    option = args.nnfs
    if option:
        print("Sar -n  file is %s" %(option[0]))
        sar_nnfs(option[0])
        return


    option = args.nip
    if option:
        print("Sar -n  file is %s" %(option[0]))
        sar_nip(option[0])
        return


    option = args.neip
    if option:
        print("Sar -n  file is %s" %(option[0]))
        sar_n(option[0])
        return


    option = args.nicmp
    if option:
        print("Sar -n  file is %s" %(option[0]))
        sar_nicmp(option[0])
        return


    option = args.neicmp
    if option:
        print("Sar -n  file is %s" %(option[0]))
        sar_neicmp(option[0])
        return


    option = args.ndev
    if option:
        print("Sar -n  file is %s" %(option[0]))
        sar_ndev(option[0])
        return


    option = args.nedev
    if option:
        print("Sar -n  file is %s" %(option[0]))
        sar_nedev(option[0])
        return


    option = args.nfc
    if option:
        print("Sar -n  file is %s" %(option[0]))
        sar_nfc(option[0])
        return

    option = args.qio
    if option:
        print("Sar -q IO file is %s" %(option[0]))
        sar_qio(option[0])
        return


    option = args.qcpu
    if option:
        print("Sar -q CPU file is %s" %(option[0]))
        sar_qcpu(option[0])
        return

    option = args.qmem
    if option:
        print("Sar -q MEM file is %s" %(option[0]))
        sar_qmem(option[0])
        return

    option = args.qload
    if option:
        print("Sar -q LOAD file is %s" %(option[0]))
        sar_qload(option[0])
        return

    option = args.r
    if option:
        print("Sar -r file is %s" %(option[0]))
        sar_r(option[0])
        return

    option = args.u
    if option:
        print("Sar -u file is %s" %(option[0]))
        sar_u(option[0])
        return

    option = args.S
    if option:
        print("Sar -S file is %s" %(option[0]))
        sar_S(option[0])
        return

    option = args.W
    if option:
        print("Sar -w file is %s" %(option[0]))
        sar_W(option[0])
        return


    option = args.w
    if option:
        print("Sar -w file is %s" %(option[0]))
        sar_w(option[0])
        return


    option = args.v
    if option:
        print("Sar -v file is %s" %(option[0]))
        sar_v(option[0])
        return



    option = args.H
    if option:
        print("Sar -H file is %s" %(option[0]))
        sar_H(option[0])
        return

    option = args.I
    if option:
        print("Sar -I file is %s" %(option[0]))
        sar_I(option[0])
        return

    if args.d:
        print("Sar -d file is %s" %(args.d[0]))
        sar_d(args.d[0])
        return

    if args.b:
        print("Sar -b file is %s" %(args.b[0]))
        sar_b(args.b[0])
        return

    if args.B:
        print("Sar -b file is %s" %(args.B[0]))
        sar_B(args.B[0])
        return

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

    if args.i:
        print("Data file is %s" %(args.i[0]))
        x,y = read_data_from_file(args.i[0])
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
