{application, emq_mod_subscription, [
	{description, "Subscription Management Module"},
	{vsn, "2.0"},
	{id, "v2.0-dirty"},
	{modules, ['emq_mod_subscription','emq_mod_subscription_app']},
	{registered, [emq_mod_subscription_sup]},
	{applications, [kernel,stdlib]},
	{mod, {emq_mod_subscription_app, []}}
]}.