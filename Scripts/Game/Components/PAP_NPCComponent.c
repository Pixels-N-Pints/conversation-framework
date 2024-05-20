[ComponentEditorProps(description: "NPC attributes and behaviour for conversations with player")]
class PAP_NPCComponentClass : ScriptComponentClass
{
}
//! Behaviour for NPC to rotate to face the player when in conversation
class PAP_NPCComponent : ScriptComponent
{
	[Attribute(defvalue: "1", desc: "How long in seconds it takes the NPC to look at the player.")]
	protected float m_fReactionDelay;
	
	private float m_fElapsedTime;
	
	private vector m_vNpcAngles;
	private vector m_vTargetAngles;
	
	//! Rotation on the x axis
	private const vector m_vRotation[3] = { "1 0 0", "0 -1 0", "0 0 -1" };

	
	//------------------------------------------------------------------------------------------------
	//! Rotates the NPC such that they come face to face with the player
	protected void Rotate(float timeSlice)
	{
		m_fElapsedTime += timeSlice;
		
		// Terminate rotation of NPC once complete
		if (m_fElapsedTime > m_fReactionDelay)
		{
			// ClearEventMask(owner, EntityEvent.POSTFIXEDFRAME);
			return;
		} 
		
		vector newAngles = vector.Lerp(m_vNpcAngles, m_vTargetAngles, m_fElapsedTime / m_fReactionDelay);
		GetOwner().SetAngles(newAngles);
		GetGame().GetCallqueue().CallLater(Rotate, 100, false, timeSlice);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Starts the rotation of the NPC
	//! \param playerAngles Angles of the player talking to this NPC
	void StartRotation(vector playerAngles)
	{
		m_fElapsedTime = 0;
		m_vNpcAngles = GetOwner().GetAngles();
		m_vTargetAngles = playerAngles.Multiply3(m_vRotation);
		Rotate(0.1);
		// SetEventMask(GetOwner(), EntityEvent.POSTFIXEDFRAME);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Stops rotation
	void StopRotation()
	{
		// ClearEventMask(GetOwner(), EntityEvent.POSTFIXEDFRAME);
	}
}