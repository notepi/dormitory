{application, emq_mod_retainer, [
	{description, "Retainer Module"},
	{vsn, "2.0"},
	{id, "v2.0-dirty"},
	{modules, ['emq_mod_retainer','emq_mod_retainer_app','emq_mod_retainer_sup']},
	{registered, [emq_mod_retainer_sup]},
	{applications, [kernel,stdlib]},
	{mod, {emq_mod_retainer_app, []}}
]}.