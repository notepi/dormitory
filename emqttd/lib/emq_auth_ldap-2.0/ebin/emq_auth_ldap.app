{application, emq_auth_ldap, [
	{description, "Authentication/ACL with LDAP"},
	{vsn, "2.0"},
	{id, "v2.0-dirty"},
	{modules, ['emq_auth_ldap','emq_auth_ldap_app']},
	{registered, [emq_auth_ldap_sup]},
	{applications, [kernel,stdlib,eldap]},
	{mod, {emq_auth_ldap_app, []}}
]}.