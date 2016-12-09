{application, emq_sn, [
	{description, "MQTT-SN Gateway"},
	{vsn, "0.2"},
	{id, "v2.0-dirty"},
	{modules, ['emq_sn','emq_sn_app','emq_sn_gateway','emq_sn_gateway_sup','emq_sn_message','emq_sn_registry','emq_sn_sup']},
	{registered, [emq_sn_sup]},
	{applications, [kernel,stdlib,esockd]},
	{mod, {emq_sn_app, []}}
]}.