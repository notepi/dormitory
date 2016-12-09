{application, emq_auth_username, [
	{description, "Authentication with Username/Password"},
	{vsn, "2.0"},
	{id, "v2.0-dirty"},
	{modules, ['emq_auth_username','emq_auth_username_app']},
	{registered, [emq_auth_username_sup]},
	{applications, [kernel,stdlib]},
	{mod, {emq_auth_username_app, []}}
]}.