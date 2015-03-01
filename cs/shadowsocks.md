---
tags: cs
title: shadowsocks
date: 2015-02-27 15:46:14
category: cs
---

#redir
am_ss_redir_OBJECTS = ss_redir-utils.$(OBJEXT) \
    ss_redir-jconf.$(OBJEXT) \
    ss_redir-json.$(OBJEXT) \
    ss_redir-encrypt.$(OBJEXT) \
    ss_redir-udprelay.$(OBJEXT) \
    ss_redir-cache.$(OBJEXT) \
    ss_redir-hwf_ss_auth.$(OBJEXT) \
    ss_redir-redir.$(OBJEXT)

=concept
struct server: is the abstruction of the site you really want to access.
{
destaddr: youtube IP
}

struct remote: proxy searver, hiwifi server machine

chrome -> server -> remote -> google
chrome <- server <- remote <- google

