{application, emq_coap, [
	{description, "CoAP Gateway"},
	{vsn, "0.2"},
	{id, "v2.0-dirty"},
	{modules, ['emq_coap','emq_coap_app','emq_coap_channel','emq_coap_channel_sup','emq_coap_gateway','emq_coap_handler','emq_coap_iana','emq_coap_message','emq_coap_observer','emq_coap_response','emq_coap_server','emq_coap_server_handle','emq_coap_sup']},
	{registered, [emq_coap_sup]},
	{applications, [kernel,stdlib,cbor,lager,esockd,mochiweb]},
	{mod, {emq_coap_app, []}}
]}.