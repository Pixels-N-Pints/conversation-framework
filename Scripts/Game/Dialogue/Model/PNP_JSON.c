class PNP_DialogueOptionJson : JsonApiStruct
{
	// JSON vars
	int id = 0;
	string message;
	int delay = 3;
	// Default option
	int next = -1;
	ref array<ref PNP_DialogueOptionJson> options;
	string entityName;
	// Member vars
	// ...
	
	
	void PNP_DialogueOptionJson()
	{		
		RegV("id");
		RegV("message");
		RegV("next");
		RegV("options");
	}
	
	void ~PNP_DialogueOptionJson()
	{
		if (options)
		{
			delete options;
		}

	}
		
}

class PNP_ConversationJson : JsonApiStruct
{
	// JSON vars
	array<PNP_DialogueOptionJson> dialogue;
	
	void PNP_ConversationJson()
	{
		RegV("dialogue");
	}
}