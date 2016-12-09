{application, emq_mod_presence, [
	{description, "Presence Module"},
	{vsn, "2.0"},
	{id, "v2.0-dirty"},
	{modules, ['emq_mod_presence','emq_mod_presence_app']},
	{registered, [emq_mod_presence_sup]},
	{applications, [kernel,stdlib]},
	{mod, {emq_mod_presence_app, []}}
]}.