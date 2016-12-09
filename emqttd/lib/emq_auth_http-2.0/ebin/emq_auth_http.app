{application, emq_auth_http, [
	{description, "Authentication/ACL with HTTP API"},
	{vsn, "2.0"},
	{id, "v2.0-rc.2-dirty"},
	{modules, ['emq_acl_http','emq_auth_http','emq_auth_http_app','emq_auth_http_cli']},
	{registered, [emq_auth_http_sup]},
	{applications, [kernel,stdlib]},
	{mod, {emq_auth_http_app, []}}
]}.