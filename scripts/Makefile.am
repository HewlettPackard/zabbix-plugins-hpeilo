AUTOMAKE_OPTIONS=foreign

zabbixdir=${prefix}/zabbix

zabbix_SCRIPTS=common_function hpeilo_zabbix_api.py hpeilo_zabbix_config zabbix_hpeilo_cfg_generator

uninstall:
	cd $(zabbixdir); \
	rm -f hpeilo_zabbix_api.log common_function hpeilo_zabbix_api.py hpeilo_zabbix_config zabbix_hpeilo_cfg_generator
