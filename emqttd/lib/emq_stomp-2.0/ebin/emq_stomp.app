{application, emq_stomp, [
	{description, "Stomp Protocol Plugin"},
	{vsn, "2.0"},
	{id, "v2.0-dirty"},
	{modules, ['emq_stomp','emq_stomp_app','emq_stomp_client','emq_stomp_frame','emq_stomp_heartbeat','emq_stomp_proto','emq_stomp_sup','emq_stomp_transaction']},
	{registered, [emq_stomp_sup]},
	{applications, [kernel,stdlib]},
	{mod, {emq_stomp_app, []}}
]}.