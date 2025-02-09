class PNP_NPCComponentClass : ScriptComponentClass
{

}

class PNP_NPCComponent : ScriptComponent
{
	// Checkpoint in the coversation
	[RplProp()]
	private int m_iCheckpoint = 1;
	
	// In use by another player
	[RplProp()]
	private bool m_bInUse;
	
	void SetCheckpoint(int checkpoint)
	{
		m_iCheckpoint = checkpoint;
	}
	
	int GetCheckpoint()
	{
		return m_iCheckpoint;
	}
	
	void SetInUse(bool inUse)
	{
		m_bInUse = inUse;
		Replication.BumpMe();
	}
	
	bool InUse()
	{
		return m_bInUse == true;
	}
}