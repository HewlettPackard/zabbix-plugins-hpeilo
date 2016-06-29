AUTOMAKE_OPTIONS=foreign
externalscriptszabbixdir=$(prefix)/zabbix/externalscripts

AM_CPPFLAGS = -I$(top_srcdir)/src/include

AM_CFLAGS =

externalscriptszabbix_PROGRAMS = hpeilo_snmp_engine

hpeilo_snmp_engine_SOURCES = hpilo_engine.c hpilo_snmp.c


install-data-hook:
	modify_conf=0; \
	if test -f "/etc/zabbix/zabbix_server.conf" ; then \
		zabbixserver_conf=/etc/zabbix/zabbix_server.conf; \
	else \
		if test -f "/etc/zabbix/zabbix-server.conf" ; then \
			zabbixserver_conf=/etc/zabbix/zabbix-server.conf; \
		fi; \
	fi; \
	if test "x$$zabbixserver_conf" != "x" ; then \
		cp "$$zabbixserver_conf" "$$zabbixserver_conf".pluginsave; \
		ESDir=`grep "ExternalScripts=" $$zabbixserver_conf | grep -v "^#"| awk -F= '{print $$2}'`; \
		if test "x$$ESDir" = "x" ; then \
			echo 'ExternalScripts=$(externalscriptszabbixdir)' >> $$zabbixserver_conf; \
			modify_conf=1; \
		fi; \
		if test "x$$ESDir" != "x" && test "$$ESDir" != "$(externalscriptszabbixdir)" ; then \
			rm -f "$$ESDir"/hpeilo_snmp_engine; \
			ln -s $(externalscriptszabbixdir)/hpeilo_snmp_engine "$$ESDir"/hpeilo_snmp_engine; \
		fi; \
		if test "x`grep "^Timeout=" $$zabbixserver_conf`" = "x" ; then \
			echo "Timeout=20" >> $$zabbixserver_conf; \
			modify_conf=1; \
		else \
			timeout=`grep "^Timeout=" $$zabbixserver_conf| awk -F= '{print $$2}'`; \
			if test $$timeout -lt 20 ; then \
				sed -i 's/^Timeout=.*/Timeout=20/g' $$zabbixserver_conf; \
				modify_conf=1; \
			fi; \
		fi; \
		if test $$modify_conf -eq "1" ; then \
			echo "warning: "$$zabbixserver_conf" saved as "$$zabbixserver_conf".pluginsave"; \
			echo "Restart zabbix-server"; \
			service zabbix-server restart; \
		else \
			rm -f $$zabbixserver_conf.pluginsave; \
		fi; \
	fi;
uninstall-hook:
	if test -f "/etc/zabbix/zabbix_server.conf" ; then \
		zabbixserver_conf=/etc/zabbix/zabbix_server.conf; \
	else \
		if test -f "/etc/zabbix/zabbix-server.conf" ; then \
			zabbixserver_conf=/etc/zabbix/zabbix-server.conf; \
		fi; \
	fi; \
	if test -f "$$zabbixserver_conf".pluginsave ; then \
		rm -f $$zabbixserver_conf; \
		mv $$zabbixserver_conf.pluginsave $$zabbixserver_conf; \
	fi;
