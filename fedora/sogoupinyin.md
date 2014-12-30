


#Remove ibus

#Install fcitx 
yum install fcitx fcitx-configtool

#Install sogou 
ar vx sogou-xxx.deb
tar -Jxvf data.tar.xz -C /
sudo cp /usr/lib/x86_64-linux-gnu/fcitx/fcitx-sogoupinyin.so  /usr/lib64/fcitx/fcitx-sogoupinyin.so


#Chose fcitx
=For fedora 18+
gsettings set org.gnome.settings-daemon.plugins.keyboard active false
imsettings-switch FCITX

#Run fcitx
fcitx
sogou-qimpanel

