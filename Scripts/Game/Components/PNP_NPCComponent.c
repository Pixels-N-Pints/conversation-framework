class PNP_NPCComponentClass : ScriptComponentClass
{

}

class PNP_NPCComponent : ScriptComponent
{
	[RplProp(onRplName : "OnCheckpointUpdate")]
	private int m_iCheckpoint;
	
	private PNP_DialogueLoader m_dialogueLoader;
	
	void SetDialogueLoader(PNP_DialogueLoader dialogueLoader)
	{
		m_dialogueLoader = dialogueLoader;
	}
	
	void SetCheckpoint(int checkpoint)
	{
		m_iCheckpoint = checkpoint;
		Replication.BumpMe();
	}
	
	void OnCheckpointUpdate()
	{
		m_dialogueLoader.SetCheckpoint(m_iCheckpoint, false);
	}
}