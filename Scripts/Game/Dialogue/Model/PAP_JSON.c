class PAP_DialogueOptionJson : JsonApiStruct
{
	// JSON vars
	int id = 0;
	string message;
	int delay = 3;
	// Default option
	int next = -1;
	ref array<ref PAP_DialogueOptionJson> options;
	// Member vars
	// ...
	
	
	void PAP_DialogueOptionJson()
	{		
		RegV("id");
		RegV("message");
		RegV("next");
		RegV("options");
	}
	
	void ~PAP_DialogueOptionJson()
	{
		if (options)
		{
			delete options;
		}

	}
		
}

class PAP_ConversationJson : JsonApiStruct
{
	// JSON vars
	array<PAP_DialogueOptionJson> dialogue;
	
	void PAP_ConversationJson()
	{
		RegV("dialogue");
	}
}