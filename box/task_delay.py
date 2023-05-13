#!/usr/bin/python
from __future__ import print_function
import os
import socket
import struct

import errno
import os
import pprint
import pwd 
import stat
import struct
import sys 
import time

import argparse

import code
import pickle

from datetime import datetime

from math import pi

from bokeh.plotting import figure,output_file, show
from bokeh.palettes import Category10_10

from bokeh.models import HoverTool

from bokeh.models import DatetimeTickFormatter

import pandas as pd

import numpy as np

output_file_name="/tmp/bokeh_task_delays_ram0.html"


CTRL_CMD_UNSPEC = 0 
CTRL_CMD_NEWFAMILY = 1 
CTRL_CMD_DELFAMILY = 2 
CTRL_CMD_GETFAMILY = 3 
CTRL_CMD_NEWOPS = 4 
CTRL_CMD_DELOPS = 5 
CTRL_CMD_GETOPS = 6 

CTRL_ATTR_UNSPEC = 0 
CTRL_ATTR_FAMILY_ID = 1 
CTRL_ATTR_FAMILY_NAME = 2 
CTRL_ATTR_VERSION = 3 
CTRL_ATTR_HDRSIZE = 4 
CTRL_ATTR_MAXATTR = 5 
CTRL_ATTR_OPS = 6 


class Attr:
    def __init__(self, attr_type, data, *values):
        self.type = attr_type
        if len(values):
            self.data = struct.pack(data, *values)
        else:
            self.data = data

    def _dump(self):
        hdr = struct.pack("HH", len(self.data) + 4, self.type)
        length = len(self.data)
        pad = ((length + 4 - 1) & ~3) - length
        return hdr + self.data + b'\0' * pad

    def __repr__(self):
        return '<Attr type %d, data "%s">' % (self.type, repr(self.data))

    def u16(self):
        return struct.unpack('H', self.data)[0]

    def s16(self):
        return struct.unpack('h', self.data)[0]

    def u32(self):
        return struct.unpack('I', self.data)[0]

    def s32(self):
        return struct.unpack('i', self.data)[0]

    def str(self):
        return self.data

    def nulstr(self):
        return self.data.split('\0')[0]
    def nested(self):
        return parse_attributes(self.data)


class StrAttr(Attr):
    def __init__(self, attr_type, data):
        Attr.__init__(self, attr_type, "%ds" % len(data), data.encode('utf-8'))


class NulStrAttr(Attr):
    def __init__(self, attr_type, data):
        Attr.__init__(self,
                      attr_type, "%dsB" % len(data), data.encode('utf-8'), 0)

class U32Attr(Attr):
    def __init__(self, attr_type, val):
        Attr.__init__(self, attr_type, "I", val)


class U8Attr(Attr):
    def __init__(self, attr_type, val):
        Attr.__init__(self, attr_type, "B", val)

class Nested(Attr):
    def __init__(self, attr_type, attrs):
        self.attrs = attrs
        self.type = attr_type

    def _dump(self):
        contents = []
        for attr in self.attrs:
            contents.append(attr._dump())
        contents = ''.join(contents)
        length = len(contents)
        hdr = struct.pack("HH", length + 4, self.type)
        return hdr + contents

class NetlinkMessage:
    def __init__(self, msg_type, flags=0, seq=-1, payload=None):
        self.type = msg_type
        self.flags = flags
        self.seq = seq
        self.pid = -1
        payload = payload or []
        if isinstance(payload, list):
            contents = []
            for attr in payload:
                contents.append(attr._dump())
            self.payload = b''.join(contents)
        else:
            self.payload = payload

    def send(self, conn):
        if self.seq == -1: 
            self.seq = conn.sequence()

        self.pid = conn.pid
        length = len(self.payload)

        hdr = struct.pack("IHHII", length + 4 * 4, self.type,
                          self.flags, self.seq, self.pid)
        conn.send(hdr + self.payload)

#struct genlmsghdr {
#      __u8    cmd;            /* Command, as defined by the Family */
#      __u8    version;        /* Irrelevant, set to 1 */
#      __u16   reserved;       /* Reserved, set to 0 */
#};
class GeNlMsgHdr:
    def __init__(self, cmd, version=0):
        self.cmd = cmd
        self.version = version

    def _dump(self):
        return struct.pack("BBxx", self.cmd, self.version)

def parse_genlmsg_hdr(data):
    return GeNlMsgHdr(*struct.unpack("BBxx", data))

def parse_attributes(data):
    attrs = {}
    while len(data):
        attr_len, attr_type = struct.unpack("HH", data[:4])
        attrs[attr_type] = Attr(attr_type, data[4:attr_len])
        attr_len = ((attr_len + 4 - 1) & ~3)
        data = data[attr_len:] 
    return attrs

class GeNetlinkMessage(NetlinkMessage):
    def __init__(self, family, cmd, attrs=[], flags=0):
        self.cmd = cmd
        self.attrs = attrs
        self.family = family
        NetlinkMessage.__init__(self, family, flags=flags,
                         payload=[GeNlMsgHdr(self.cmd)] + attrs)

    @staticmethod
    def recv(conn):
        # NetlinkMessage
        msg = conn.recv()
        packet = msg.payload
        hdr = parse_genlmsg_hdr(packet[:4])

        genlmsg = GeNetlinkMessage(msg.type, hdr.cmd, [], msg.flags)
        genlmsg.attrs = parse_attributes(packet[4:])
        genlmsg.version = hdr.version

        return genlmsg

class Netlink:
    def __init__(self, nltype, group_addr=0):
        self.fd = socket.socket(socket.AF_NETLINK, socket.SOCK_RAW, nltype)
        self.fd.setsockopt(socket.SOL_SOCKET, socket.SO_SNDBUF, 65536)
        self.fd.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, 65536)
        self.fd.bind((0,group_addr))
        self.pid, self.groups = self.fd.getsockname()
        self.seq = 0

    def send(self, msg):
        self.fd.send(msg)

    def recv(self):
        bufs=16384
        contents, (nlpid, nlgrps) = self.fd.recvfrom(bufs)

        # https://www.kernel.org/doc/html//next/userspace-api/netlink/intro.html
        # struct nlmsghdr {
        #       __u32   nlmsg_len;      /* Length of message including headers */
        #       __u16   nlmsg_type;     /* Generic Netlink Family (subsystem) ID */
        #       __u16   nlmsg_flags;    /* Flags - request or dump */
        #       __u32   nlmsg_seq;      /* Sequence number */
        #       __u32   nlmsg_pid;      /* Port ID, set to 0 */
        # };
        # struct genlmsghdr {
        #       __u8    cmd;            /* Command, as defined by the Family */
        #       __u8    version;        /* Irrelevant, set to 1 */
        #       __u16   reserved;       /* Reserved, set to 0 */
        # };
        # /* TLV attributes follow... */
        msglen, msg_type, flags, seq, pid = struct.unpack("IHHII", contents[:16])
        msg = NetlinkMessage(msg_type, flags, seq, contents[16:])
        msg.pid = pid
        if msg.type == NLMSG_ERROR:
            errno = -struct.unpack("i", msg.payload[:4])[0]
            if errno != 0:
                err = OSError("Netlink error: %s (%d)" % (os.strerror(errno), errno))
                err.errno = errno
                raise err
        return msg

    def sequence(self):
        self.seq += 1
        return self.seq

# flags
NLM_F_REQUEST = 1
NLM_F_MULTI = 2
NLM_F_ACK = 4
NLM_F_ECHO = 8

# types
NLMSG_NOOP = 1
NLMSG_ERROR = 2
NLMSG_DONE = 3
NLMSG_OVERRUN = 4
NLMSG_MIN_TYPE = 0x10

class DumpableObject(object):
    def __repr__(self):
        return '%s: %s>' % (str(type(self))[:-1], pprint.pformat(self.__dict__))
# Interesting members' offsets
# crash> taskstats -o
# struct taskstats {
#     [0x0] __u16 version;
#     [0x4] __u32 ac_exitcode;
#     [0x8] __u8 ac_flag;
#     [0x9] __u8 ac_nice;
#    [0x10] __u64 cpu_count;
#    [0x18] __u64 cpu_delay_total;
#    [0x20] __u64 blkio_count;
#    [0x28] __u64 blkio_delay_total;
#    [0x30] __u64 swapin_count;
#    [0x38] __u64 swapin_delay_total;
#    [0x40] __u64 cpu_run_real_total;
#    [0x48] __u64 cpu_run_virtual_total;
#    [0x50] char ac_comm[32];
#    [0x70] __u8 ac_sched;
#    [0x71] __u8 ac_pad[3];
#    [0x78] __u32 ac_uid;
#    [0x7c] __u32 ac_gid;
#    [0x80] __u32 ac_pid;
#    [0x84] __u32 ac_ppid;
#    [0x88] __u32 ac_btime;
#    [0x90] __u64 ac_etime;
#    [0x98] __u64 ac_utime;
#    [0xa0] __u64 ac_stime;
#    [0xa8] __u64 ac_minflt;
#    [0xb0] __u64 ac_majflt;
#    [0xb8] __u64 coremem;
#    [0xc0] __u64 virtmem;
#    [0xc8] __u64 hiwater_rss;
#    [0xd0] __u64 hiwater_vm;

# Off-cpu time = Interval - cpu_delay_total - cpu_run_virtual_total

taskstats_offsets = [
#        ('version', 0x0),
        ('freepages_delay_total', 0x140),
        #('freepages_count', 0x138),
        ('blkio_delay_total', 0x28),
        ('cpu_delay_total', 0x18),
       # ('cpu_run_virtual_total', 0x48),
       # ('cpu_run_real_total', 0x40),
        ('swapin_delay_total', 0x38),
        ('thrashing_delay_total', 0x150),
        ('compact_delay_total', 0x168),
        ('swapin_delay_total', 0x38),
        ('ac_utime', 0x98),
        ('ac_stime', 0xa0)
    ]
#    [0xd8] __u64 read_char;
#    [0xe0] __u64 write_char;
#    [0xe8] __u64 read_syscalls;
#    [0xf0] __u64 write_syscalls;
#    [0xf8] __u64 read_bytes;
#   [0x100] __u64 write_bytes;
#   [0x108] __u64 cancelled_write_bytes;
#   [0x110] __u64 nvcsw;
#   [0x118] __u64 nivcsw;
#   [0x120] __u64 ac_utimescaled;
#   [0x128] __u64 ac_stimescaled;
#   [0x130] __u64 cpu_scaled_run_real_total;
#   [0x138] __u64 freepages_count;
#   [0x140] __u64 freepages_delay_total;
#   [0x148] __u64 thrashing_count;
#   [0x150] __u64 thrashing_delay_total;
#   [0x158] __u64 ac_btime64;
#   [0x160] __u64 compact_count;
#   [0x168] __u64 compact_delay_total;
#   [0x170] __u32 ac_tgid;
#   [0x178] __u64 ac_tgetime;
#   [0x180] __u64 ac_exe_dev;
#   [0x188] __u64 ac_exe_inode;
#   [0x190] __u64 wpcopy_count;
#   [0x198] __u64 wpcopy_delay_total;
# }

TASKSTATS_CMD_GET = 1 
TASKSTATS_CMD_ATTR_PID = 1 
TASKSTATS_TYPE_AGGR_PID = 4 
TASKSTATS_TYPE_PID = 1 
TASKSTATS_TYPE_STATS = 3

class Taskstats(DumpableObject):
    def __init__(self, task_stats_buffer):
        sd = self.__dict__
        for name, offset in taskstats_offsets:
            data = task_stats_buffer[offset:offset + 8]
            sd[name] = struct.unpack('Q', data)[0]

GENL_ID_CTRL = NLMSG_MIN_TYPE
NETLINK_GENERIC = 16
# Taskstats
class TaskDelays:
    def __init__(self):
        self.conn = Netlink(NETLINK_GENERIC)
        self.family_id = self.get_family_id('TASKSTATS')

    def get_family_id(self, family):
        a = NulStrAttr(CTRL_ATTR_FAMILY_NAME, family)
        m = GeNetlinkMessage(GENL_ID_CTRL, CTRL_CMD_GETFAMILY,flags=NLM_F_REQUEST, attrs=[a])
        m.send(self.conn)
        m = GeNetlinkMessage.recv(self.conn)
        return m.attrs[CTRL_ATTR_FAMILY_ID].u16()

    def build_genlmsg(self, tid):
        return GeNetlinkMessage(self.family_id, cmd=TASKSTATS_CMD_GET, attrs=[U32Attr(TASKSTATS_CMD_ATTR_PID, tid)], flags=NLM_F_REQUEST)

    def get_taskstats(self, tid):
        self.build_genlmsg(tid).send(self.conn)
        try:                     
            reply = GeNetlinkMessage.recv(self.conn)
        except OSError as e:
            if e.errno == errno.ESRCH:
                # OSError: Netlink error: No such process (3)
                return
            raise
        for attr_type, attr_value in reply.attrs.items():
            if attr_type == TASKSTATS_TYPE_AGGR_PID:
                reply = attr_value.nested()
                break
        else:
            return
        taskstats_data = reply[TASKSTATS_TYPE_STATS].data
        if len(taskstats_data) < 272:
            return
        taskstats_version = struct.unpack('H', taskstats_data[:2])[0]
        return Taskstats(taskstats_data)

def param_init():
    parser = argparse.ArgumentParser(description='L3 delay accounting tool')
    parser.add_argument('-p', nargs=1, metavar='pid', help='Process')
    #parser.add_argument('-t', nargs=1, metavar='tid', help='Thread.')
    parser.add_argument('-T', nargs=1, metavar='monitor interval', help='default, 5 seconds')
    parser.add_argument('-u', nargs=1, metavar='unit', help='default, 1000 i.e. us')
    parser.add_argument('-d', action='store_true', help='output path for dm test')
    #parser.add_argument('-f', nargs=0, metavar='file for parsing', help='File')
    #parser.add_argument('-I', nargs=0, metavar='irq delay', help='Interrupt')
    #parser.add_argument('-i', nargs=0, metavar='soft irq delay', help='Soft Interrupt')
    #parser.add_argument('-e', nargs=0, metavar='exception delay', help='Exception')
    #parser.add_argument('-c', nargs=0, metavar='compact delay', help='Memory I/O')
    #parser.add_argument('-s', nargs=0, metavar='systemcalls delay', help='Syscall')
    #parser.add_argument('-r', nargs=0, metavar='reclaim delay', help='Memory I/O')
    #parser.add_argument('-o', nargs=0, metavar='io thrash delay', help='I/O')
    #parser.add_argument('-O', nargs=0, metavar='off-cpu delay', help='off-cpu')
    #parser.add_argument('-O', nargs=0, metavar='off-cpu delay', help='off-cpu')
    return parser.parse_args();

def show_delays(delays):
    #fruits = ['Apples', 'Pears', 'Nectarines', 'Plums', 'Grapes', 'Strawberries']
    #years = ["2015", "2016", "2017"]
    #colors = ["#c9d9d3", "#718dbf", "#e84d60"]

    #data = {'fruits' : fruits,
    #        '2015'   : [2, 1, 4, 3, 2, 4],
    #        '2016'   : [5, 3, 4, 2, 4, 6],
    #        '2017'   : [3, 2, 4, 4, 5, 3]}

    df = pd.DataFrame(delays['ts'])
    #df.index = pd.to_datetime(df.index, unit='s')
    #x_date = pd.to_datetime(df[0], format="%Y-%d-%m-%H:%M:%S")

    # vbar stack
    # https://docs.bokeh.org/en/latest/docs/gallery/bar_stacked.html
    #p = figure(x_axis_type='datetime', title="Task delays every seconds",
    #p = figure(x_range=delays['ts'], sizing_mode="scale_width",x_axis_type='datetime', title="Task delays",
    #delays['ts'] = x_date
    #p = figure(x_range=delays['ts'], sizing_mode="stretch_width", x_axis_type='datetime', title="Task delays",
    p = figure(x_range=delays['ts'], sizing_mode="stretch_width",  title="Task delays",
               tools="hover, pan, wheel_zoom, box_zoom, box_select,reset, save", tooltips="$name @$name")
               #toolbar_location=None, tools="hover", tooltips="$name @$name")

    data = delays
    delays_type = []
    c_colors = Category10_10.__iter__()
    colors = []
    for k in taskstats_offsets:
        delays_type.append(k[0])
        try: 
            c = next(c_colors)
        except StopIteration:
            c_colors = Category10_10.__iter__()
            c = next(c_colors)
        colors.append(c)

    #p.vbar_stack(delays_type, x='ts', width=0.9, source=data,
    p.vbar_stack(delays_type, x='ts', width=0.9, color=colors, source=data,
    #p.vbar_stack(delays_type, x=x_date, y=data, width=0.9, color=colors, 
                 legend_label=delays_type)

    p.y_range.start = 0
    p.x_range.range_padding = 0.1
    p.xgrid.grid_line_color = None
    p.axis.minor_tick_line_color = None
    p.outline_line_color = None
    p.legend.location = "top_left"
    p.legend.orientation = "horizontal"

    p.xaxis.major_label_orientation = pi/4

    #p.xaxis.formatter = DatetimeTickFormatter(years="%d/%m/%Y %H:%M:%S",
    #                                      months="%d/%m/%Y %H:%M:%S",
    #                                      days="%d/%m/%Y %H:%M:%S",
    #                                      hours="%d/%m/%Y %H:%M:%S",
    #                                      hourmin="%d/%m/%Y %H:%M:%S",
    #                                      minutes="%d/%m/%Y %H:%M:%S",
    #                                      minsec="%d/%m/%Y %H:%M:%S",
    #                                      seconds="%d/%m/%Y %H:%M:%S",
    #                                      milliseconds="%d/%m/%Y %H:%M:%S",
    #                                      microseconds="%d/%m/%Y %H:%M:%S")


    #p.add_tools(HoverTool(renderers=[renderer], tooltips=[('', legend)]))
    #p.add_tools(HoverTool(renderers=[renderer]))#, tooltips=[('', legend)]))

    show(p)

def main():
    args = param_init()

    monitor_interval = 1
    if args.T:
        monitor_interval = args.T[0]

    tid = 0
    print(args.p[0])
    if args.p:
        tid = int(args.p[0])
    else:
        exit(0)

    unit = 1000000
    if args.u:
        unit = int(args.u[0])

    local_output_file_name = output_file_name
    if args.d:
        local_output_file_name="/tmp/bokeh_task_delays_dm0.html"
        #local_output_file_name=args.d[0]

    print("TID %d"%(tid))

    t = TaskDelays()
    raw_data = {}
    delays = {}
    data = {}
    td = []
    delays['ts'] = []
    for k in taskstats_offsets:
        raw_data[k[0]] = []
        delays[k[0]] = []

    while True:
        try:
            time.sleep(monitor_interval)
        except KeyboardInterrupt:
            with open("/tmp/task_delays.txt", "wb") as file:
                pickle.dump(td, file)
            break 
            with open("/tmp/task_delays.txt", "rb") as file:
                td = pickle.load(file)
        tmp = {}
        #ts = datetime.now().strftime("%Y-%d-%m-%H:%M:%S")
        ts = datetime.now().strftime("%d-%m-%H:%M:%S")
        try:
            data = t.get_taskstats(tid).__dict__
        except:
            break

        tmp['data'] = data
        td.append(tmp)
        delays['ts'].append(ts)
        for k,v in data.items():
            if len(raw_data[k]) != 0:
                delta = v - raw_data[k][-1]
                if k == 'ac_utime':
                    delta = delta * 1000
                elif k == 'ac_stime':
                    delta = delta * 1000

                delays[k].append(delta/unit)
            raw_data[k].append(v)

    delays['ts'].pop(0)
    code.interact(local=locals())
    print("show_delays")

    output_file(local_output_file_name)
    show_delays(delays)

if __name__ == "__main__":
    main()
