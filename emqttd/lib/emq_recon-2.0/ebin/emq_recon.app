{application, emq_recon, [
	{description, "Recon Plugin"},
	{vsn, "2.0"},
	{id, "v2.0-dirty"},
	{modules, ['emq_recon_app','emq_recon_cli','emq_recon_gc','emq_recon_sup']},
	{registered, [emq_recon_sup]},
	{applications, [kernel,stdlib,recon]},
	{mod, {emq_recon_app, []}}
]}.