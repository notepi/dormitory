{application, emq_plugin_template, [
	{description, "EMQ Plugin Template"},
	{vsn, "2.0"},
	{id, "v2.0-rc.2-dirty"},
	{modules, ['emq_acl_demo','emq_auth_demo','emq_cli_demo','emq_plugin_template','emq_plugin_template_app','emq_plugin_template_sup']},
	{registered, [emq_plugin_template_sup]},
	{applications, [kernel,stdlib]},
	{mod, {emq_plugin_template_app, []}}
]}.