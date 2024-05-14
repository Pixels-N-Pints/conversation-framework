class PAP_DialogueLoader
{
	protected int m_iCheckpoint;
	protected ref map<int, ref PAP_DialogueOptionJson> m_mDialogueById;

	void SetCheckpoint(int checkpoint)
	{
		m_iCheckpoint = checkpoint;	
	}
	
	int GetCheckpoint()
	{
		return m_iCheckpoint;
	}

	
	PAP_DialogueOptionJson GetCurrentOption()
	{
		if (!m_mDialogueById || m_mDialogueById.IsEmpty())
		{
			Print("Dialogue entries not available!", LogLevel.ERROR);
			return null;
		}
		
		ref PAP_DialogueOptionJson option = m_mDialogueById.Get(m_iCheckpoint);
		return option;
	}
	
	void LoadConversation(PAP_ConversationConf conversationConf)
	{
		if (!conversationConf)
		{
			Print("No dialogue configuration file supplied!", LogLevel.ERROR);
			return;
		}
		BuildConversationFromConf(conversationConf);
	}
	
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
			if (entry.GetOptions() && !entry.GetOptions().IsEmpty())
			{
				jEntry.options = {};
				foreach (PAP_DialogueEntryOptionConf entryOption : entry.GetOptions())
				{
					PAP_DialogueOptionJson jEntryOption = new PAP_DialogueOptionJson();
					jEntryOption.message = entryOption.GetMessage();
					jEntryOption.next = entryOption.GetNextId();
					jEntry.options.Insert(jEntryOption);
				}
			}
			m_mDialogueById.Insert(jEntry.id, jEntry);
		}
		// Reset checkpoint
		m_iCheckpoint = m_mDialogueById.GetKey(0);
	}

	
	void ~PAP_DialogueLoader()
	{
		delete m_mDialogueById;
	}
}