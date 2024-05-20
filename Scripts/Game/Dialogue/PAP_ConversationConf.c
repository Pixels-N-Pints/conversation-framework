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
	[Attribute(desc: "Message displayed above the options if any. You can interpolate expressions (eg. {IDENTITY_NAME}) to include the identity of the player or npc in the message. See PAP_StringTemplating for more details.")]
	protected string m_sMessage;
	[Attribute(defvalue: "-1", desc: "Numerical identifier of the next dialogue entry if no options are applicable. -1 if options are present or 0 if closing dialogue option.")]
	protected int m_iNextId;
	[Attribute(desc: "Name of a entity in the world editor to spawn. Activation type needs to be set to ON_TRIGGER_ACTIVATION.")]
	protected string m_sEntityName;
	
	string GetMessage()
	{
		return m_sMessage;
	}
	
	int GetNextId()
	{
		return m_iNextId;
	}
	
	string GetEntityName()
	{
		return m_sEntityName;
	}
	
}

[BaseContainerProps(configRoot: true)]
class PAP_DialogueEntryConf : PAP_DialogueEntryOptionConf
{
	[Attribute(defvalue: "0", desc: "Numerical identifier of the dialogue entry.")]
	protected int m_iId;
	[Attribute(desc: "Options for the player to select from.")]
	protected ref array<ref PAP_DialogueEntryOptionConf> m_Options;
	[Attribute(defvalue: "4", desc: "Delay in seconds the message persists before moving onto the next message.")]
	protected int m_iDelay;
	
	int GetId()
	{
		return m_iId;
	}
	
	int GetDelay()
	{
		return m_iDelay;
	}
	
	ref array<ref PAP_DialogueEntryOptionConf> GetOptions()
	{
		return m_Options;
	}
	
	
}