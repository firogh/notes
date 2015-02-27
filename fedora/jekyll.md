---
tags: fedora
title: jekyll
date: 2015-02-27 15:46:11
category: fedora
---
#install ruby-dev
sudo yum install gcc-c++ gcc ruby-devel.x86_64

#add taobao source
$ gem sources --remove http://rubygems.org/
$ gem sources -a http://ruby.taobao.org/
$ gem sources -l
*** CURRENT SOURCES ***
http://ruby.taobao.org

#install jekyll execjs therubyracer

#Edite below file to break date-tile.md scheme
.gem/ruby/gems/jekyll-2.5.3/lib/jekyll/post.rb

	# Valid post name regex.
	#MATCHER = /^(.+\/)*(\d+-\d+-\d+)-(.*)(\.[^.]+)$/
	MATCHER = /^(.+\/)*(.*)(\.[^.]+)$/

	#MATCHER = /^(.+\/)*(.*)(\.[^.]+)$/
	#m, cats, date, slug, ext = *name.match(MATCHER)
	m, cats, slug, ext = *name.match(MATCHER)
	date = "2014-12-28"

#Write your own jekyll plugin
* Place it in _plugins/ with extension .rb
* Configure your _config.yaml with
safe: false

#Make github pages directly show static html without build!
* Method 1: touch .nojekyll file: 
https://help.github.com/articles/using-jekyll-with-pages/#turning-jekyll-off
* Method 1: move your github pages repository .git/ to subdirectory _site/, then git commit it!
it's means that ripe out all file other then site it's self, 
you need to keep your _config.yaml, _layouts/ and something else in other git repository.

