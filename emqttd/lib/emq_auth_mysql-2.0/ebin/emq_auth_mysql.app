{application, emq_auth_mysql, [
	{description, "Authentication/ACL with MySQL"},
	{vsn, "2.0"},
	{id, "v2.0-dirty"},
	{modules, ['emq_acl_mysql','emq_auth_mysql','emq_auth_mysql_app','emq_auth_mysql_cli','emq_auth_mysql_sup']},
	{registered, [emq_auth_mysql_sup]},
	{applications, [kernel,stdlib,mysql,ecpool]},
	{mod, {emq_auth_mysql_app, []}}
]}.