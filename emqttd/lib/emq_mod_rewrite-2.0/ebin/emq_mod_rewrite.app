{application, emq_mod_rewrite, [
	{description, "Rewrite Module"},
	{vsn, "2.0"},
	{id, "v2.0-dirty"},
	{modules, ['emq_mod_rewrite','emq_mod_rewrite_app']},
	{registered, [emq_mod_rewrite_sup]},
	{applications, [kernel,stdlib]},
	{mod, {emq_mod_rewrite_app, []}}
]}.