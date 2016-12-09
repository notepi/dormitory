{application, emq_auth_redis, [
	{description, "Authentication/ACL with Redis"},
	{vsn, "2.0"},
	{id, "v2.0-dirty"},
	{modules, ['emq_acl_redis','emq_auth_redis','emq_auth_redis_app','emq_auth_redis_cli','emq_auth_redis_sup']},
	{registered, [emq_auth_redis_sup]},
	{applications, [kernel,stdlib,eredis,ecpool]},
	{mod, {emq_auth_redis_app, []}}
]}.