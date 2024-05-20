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
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		super.EOnFrame(owner, timeSlice);
		Rotate(owner, timeSlice);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Rotates the NPC such that they come face to face with the player
	//! \param owner The npc
	//! \param timeSlice The slice of time
	protected void Rotate(IEntity owner, float timeSlice)
	{
		m_fElapsedTime += timeSlice;
		
		// Terminate rotation of NPC once complete
		if (m_fElapsedTime > m_fReactionDelay)
		{
			StopRotation();
			return;
		} 
		
		vector newAngles = vector.Lerp(m_vNpcAngles, m_vTargetAngles, m_fElapsedTime / m_fReactionDelay);
		GetOwner().SetAngles(newAngles);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Starts the rotation of the NPC
	//! \param playerPosition Angles of the player talking to this NPC
	void StartRotation(vector playerPosition)
	{
		SCR_AIUtilityComponent utilityComponent = SCR_AIUtilityComponent.Cast(GetOwner().FindComponent(SCR_AIUtilityComponent));
		if (!utilityComponent) return;
		utilityComponent.m_LookAction.LookAt(playerPosition, 1, m_fReactionDelay);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Stops rotation
	void StopRotation()
	{
		// ClearEventMask(GetOwner(), EntityEvent.FRAME);
		// GetOwner().SetFlags(EntityFlags.ACTIVE, true);
	}
}