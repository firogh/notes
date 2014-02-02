#
# Copyright (C) 2013 Hiwifi.tw .
#

include $(TOPDIR)/rules.mk

PKG_NAME:=wystream
PKG_SOURCE_VERSION=4
PKG_VERSION:=0.0.$(PKG_SOURCE_VERSION)
PKG_RELEASE:=20140115-1

#PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
#PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
#PKG_SOURCE_PROTO:=svn
#PKG_SOURCE_URL:=http://trac/svn/p2sp/Accelerator
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SUBMENU:=Accelerator/Uploader
  SECTION:=net
  CATEGORY:=Turbo Wireless
  TITLE:=$(PKG_NAME)
  DEPENDS:=+libpthread +libstdcpp +protobuf +zlib
  URL:=http://www.turboer.com/
  MAINTAINER:=Qin Wei <wei.qin@hiwifi.tw>
endef

define Package/$(PKG_NAME)/description
	$(PKG_NAME) is a multiple task uploader for WeiYun TCP Stream skydrive, developed by Qin Wei <wei.qin@hiwifi.tw>
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

#define Build/Compile
#	$(MAKE) -C $(PKG_BUILD_DIR) $(MAKE_OPTS)
#endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/local/wystream/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/wystream $(1)/usr/local/wystream/bin
	$(INSTALL_DIR) $(1)/usr/local/wystream/etc
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/etc/Config.template $(1)/usr/local/wystream/etc
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/wystream.init $(1)/etc/init.d/wystream
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
