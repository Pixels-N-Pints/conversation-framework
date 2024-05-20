//! Container for a conversation
class PAP_DialogueLoader
{
	// Member vars
	//! Checkpoint of the conversation
	protected int m_iCheckpoint;
	//! Dialogue entries defined in a configuration file and rebuilt as a map object for ease of scripting
	protected ref map<int, ref PAP_DialogueOptionJson> m_mDialogueById;
	
	//------------------------------------------------------------------------------------------------
	//! Checkpoint setter
	void SetCheckpoint(int checkpoint)
	{
		m_iCheckpoint = checkpoint;	
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
	ref PAP_DialogueOptionJson GetCurrentOption()
	{
		if (!m_mDialogueById || m_mDialogueById.IsEmpty())
		{
			Print("Dialogue entries not available!", LogLevel.ERROR);
			return null;
		}
		
		ref PAP_DialogueOptionJson option = m_mDialogueById.Get(m_iCheckpoint);
		return option;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Checks the configuration object is valid then calls to build the conversation map
	//! \param conversationConf Configuration object containing the conversation
	void LoadConversation(PAP_ConversationConf conversationConf)
	{
		if (!conversationConf)
		{
			Print("No dialogue configuration file supplied!", LogLevel.ERROR);
			return;
		}
		BuildConversationFromConf(conversationConf);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Builds the conversation map
	//! \param conversationConf Configuration object containing the conversation
	protected void BuildConversationFromConf(notnull PAP_ConversationConf conversationConf)
	{
		m_mDialogueById = new map<int, ref PAP_DialogueOptionJson>();
		array<ref PAP_DialogueEntryConf> options = conversationConf.GetOptions();
		if (!options || options.IsEmpty())
		{
			Print("No entries found in the dialogue file!", LogLevel.ERROR);
			return;
		}
		
		foreach (PAP_DialogueEntryConf entry: options)
		{
			ref PAP_DialogueOptionJson jEntry = new PAP_DialogueOptionJson();
			jEntry.id = entry.GetId();
			jEntry.message = entry.GetMessage();
			jEntry.next = entry.GetNextId();
			jEntry.delay = entry.GetDelay();
			jEntry.entityName = entry.GetEntityName();
			if (entry.GetOptions() && !entry.GetOptions().IsEmpty())
			{
				jEntry.options = {};
				foreach (PAP_DialogueEntryOptionConf entryOption : entry.GetOptions())
				{
					PAP_DialogueOptionJson jEntryOption = new PAP_DialogueOptionJson();
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
	void ~PAP_DialogueLoader()
	{
		delete m_mDialogueById;
	}
}