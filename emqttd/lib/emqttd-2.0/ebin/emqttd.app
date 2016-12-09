{application, emqttd, [
	{description, "Erlang MQTT Broker"},
	{vsn, "2.0"},
	{id, "v2.0-dirty"},
	{modules, ['emqttd','emqttd_access_control','emqttd_access_rule','emqttd_acl_internal','emqttd_acl_mod','emqttd_alarm','emqttd_app','emqttd_auth_mod','emqttd_base62','emqttd_boot','emqttd_bridge','emqttd_bridge_sup','emqttd_bridge_sup_sup','emqttd_broker','emqttd_cli','emqttd_client','emqttd_cluster','emqttd_cm','emqttd_cm_sup','emqttd_ctl','emqttd_gen_mod','emqttd_guid','emqttd_hook','emqttd_http','emqttd_keepalive','emqttd_message','emqttd_metrics','emqttd_mnesia','emqttd_mod_sup','emqttd_mqueue','emqttd_net','emqttd_node','emqttd_opts','emqttd_packet','emqttd_parser','emqttd_plugins','emqttd_pmon','emqttd_pool_sup','emqttd_pooler','emqttd_protocol','emqttd_pubsub','emqttd_pubsub_sup','emqttd_router','emqttd_serializer','emqttd_server','emqttd_session','emqttd_session_sup','emqttd_sm','emqttd_sm_helper','emqttd_sm_sup','emqttd_stats','emqttd_sup','emqttd_sysmon','emqttd_sysmon_sup','emqttd_time','emqttd_topic','emqttd_trace','emqttd_trace_sup','emqttd_trie','emqttd_vm','emqttd_ws','emqttd_ws_client','emqttd_ws_client_sup','gen_server2','lager_emqtt_backend','priority_queue']},
	{registered, [emqttd_sup]},
	{applications, [kernel,stdlib,gproc,lager,gen_logger,esockd,mochiweb]},
	{mod, {emqttd_app, []}}
]}.