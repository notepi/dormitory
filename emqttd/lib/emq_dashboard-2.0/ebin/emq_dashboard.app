{application, emq_dashboard, [
	{description, "Dashboard"},
	{vsn, "2.0"},
	{id, "v2.0-dirty"},
	{modules, ['emq_auth_dashboard','emq_dashboard','emq_dashboard_admin','emq_dashboard_alarm','emq_dashboard_app','emq_dashboard_cli','emq_dashboard_client','emq_dashboard_metric','emq_dashboard_overview','emq_dashboard_plugin','emq_dashboard_route','emq_dashboard_session','emq_dashboard_subscription','emq_dashboard_sup','emq_dashboard_topic','emq_dashboard_user','emq_meter_access','emq_meter_define']},
	{registered, [emq_dashboard_sup]},
	{applications, [kernel,stdlib,mnesia]},
	{mod, {emq_dashboard_app, []}}
]}.