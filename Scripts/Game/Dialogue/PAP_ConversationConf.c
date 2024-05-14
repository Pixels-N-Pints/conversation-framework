[BaseContainerProps(configRoot: true)]
class PAP_ConversationConf
{
	[Attribute(desc: "Dialogue tree")]
	protected ref array<ref PAP_DialogueEntryConf> options;
	
	ref array<ref PAP_DialogueEntryConf> GetOptions()
	{
		return options;
	}
}


[BaseContainerProps(configRoot: true)]
class PAP_DialogueEntryOptionConf
{
	[Attribute(desc: "Message displayed above the options if any.")]
	protected string m_sMessage;
	[Attribute(defvalue: "-1", desc: "Numerical identifier of the next dialogue entry if no options are applicable.")]
	protected int m_iNextId;
	[Attribute(defvalue: "0", desc: "Delay in seconds the message persists before moving onto the next message. Should be 0 if it's an option.")]
	protected int m_iDelay;
	
	string GetMessage()
	{
		return m_sMessage;
	}
	
	int GetNextId()
	{
		return m_iNextId;
	}
	
	int GetDelay()
	{
		return m_iDelay;
	}
	
}

[BaseContainerProps(configRoot: true)]
class PAP_DialogueEntryConf : PAP_DialogueEntryOptionConf
{
	[Attribute(defvalue: "0", desc: "Numerical identifier of the dialogue entry.")]
	protected int m_iId;
	[Attribute(desc: "Options for the player to select from.")]
	protected ref array<ref PAP_DialogueEntryOptionConf> m_Options;
	
	int GetId()
	{
		return m_iId;
	}
	
	ref array<ref PAP_DialogueEntryOptionConf> GetOptions()
	{
		return m_Options;
	}
	
	
}