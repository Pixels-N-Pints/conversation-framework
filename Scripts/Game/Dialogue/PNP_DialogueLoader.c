//! Container for a conversation
class PNP_DialogueLoader
{
	// Member vars
	//! Checkpoint of the conversation
	protected int m_iCheckpoint;
	//! Dialogue entries defined in a configuration file and rebuilt as a map object for ease of scripting
	protected ref map<int, ref PNP_DialogueOptionJson> m_mDialogueById;
	
	private PNP_NPCComponent m_nPcComponent;
	
	//------------------------------------------------------------------------------------------------
	//! Checkpoint setter
	void SetCheckpoint(int checkpoint, bool shouldReplicate)
	{
		m_iCheckpoint = checkpoint;	
		if (shouldReplicate) 
			m_nPcComponent.SetCheckpoint(checkpoint);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Checkpoint getter
	int GetCheckpoint()
	{
		return m_iCheckpoint;
	}

	//------------------------------------------------------------------------------------------------
	//! Gets the current option based on the value of the checkpoint
	//! \return a dialogue entry
	ref PNP_DialogueOptionJson GetCurrentOption()
	{
		if (!m_mDialogueById || m_mDialogueById.IsEmpty())
		{
			Print("Dialogue entries not available!", LogLevel.ERROR);
			return null;
		}
		
		ref PNP_DialogueOptionJson option = m_mDialogueById.Get(m_iCheckpoint);
		return option;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Checks the configuration object is valid then calls to build the conversation map
	//! \param conversationConf Configuration object containing the conversation
	void LoadConversation(PNP_ConversationConf conversationConf, PNP_NPCComponent npcComponent)
	{
		if (!conversationConf)
		{
			Print("No dialogue configuration file supplied!", LogLevel.ERROR);
			return;
		}
		if (!npcComponent)
		{	
			Print("No npc component to replicate checkpoint", LogLevel.ERROR);
			return;
		}
		
		m_nPcComponent = npcComponent;
		
		BuildConversationFromConf(conversationConf);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Builds the conversation map
	//! \param conversationConf Configuration object containing the conversation
	protected void BuildConversationFromConf(notnull PNP_ConversationConf conversationConf)
	{
		m_mDialogueById = new map<int, ref PNP_DialogueOptionJson>();
		array<ref PNP_DialogueEntryConf> options = conversationConf.GetOptions();
		if (!options || options.IsEmpty())
		{
			Print("No entries found in the dialogue file!", LogLevel.ERROR);
			return;
		}
		
		foreach (PNP_DialogueEntryConf entry: options)
		{
			ref PNP_DialogueOptionJson jEntry = new PNP_DialogueOptionJson();
			jEntry.id = entry.GetId();
			jEntry.message = entry.GetMessage();
			jEntry.next = entry.GetNextId();
			jEntry.delay = entry.GetDelay();
			jEntry.entityName = entry.GetEntityName();
			if (entry.GetOptions() && !entry.GetOptions().IsEmpty())
			{
				jEntry.options = {};
				foreach (PNP_DialogueEntryOptionConf entryOption : entry.GetOptions())
				{
					PNP_DialogueOptionJson jEntryOption = new PNP_DialogueOptionJson();
					jEntryOption.message = entryOption.GetMessage();
					jEntryOption.next = entryOption.GetNextId();
					jEntryOption.entityName = entryOption.GetEntityName();
					jEntry.options.Insert(jEntryOption);
				}
			}
			m_mDialogueById.Insert(jEntry.id, jEntry);
		}
		// Reset checkpoint
		m_iCheckpoint = m_mDialogueById.GetKey(0);
	}

	//------------------------------------------------------------------------------------------------
	//! Destructor
	void ~PNP_DialogueLoader()
	{
		delete m_mDialogueById;
	}
}