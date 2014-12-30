

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


