//! Container for the open conversation dialogue user action
class PAP_ConversationLayoutAttributes : ScriptedUserAction
{
	[Attribute(defvalue: "{A27941092D1EA761}UI/Layouts/conversation.layout")] // setup the created layout here
	protected ResourceName m_sLayout;
	
	[Attribute(params: "conf", defvalue: "{267D14ACD9EFFEE5}Configs/Dialogue/sample_dialogue.conf", desc: "Conversation file for this AI agent")]
	protected ResourceName m_sConversation;
	
	protected PAP_ConversationLayoutUI m_wDisplay;
	
	protected ref PAP_DialogueLoader m_dialogueLoader;
}
//! Handles the user action's behaviour
class PAP_ConversationLayoutUserAction : PAP_ConversationLayoutAttributes
{
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.Init(pOwnerEntity, pManagerComponent);
		
		m_dialogueLoader = new PAP_DialogueLoader();
		m_dialogueLoader.LoadConversation(LoadConfig());
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		// Cancel if dialogue already exists
		if (m_wDisplay)
		{
			Print("Dialogue with " + pOwnerEntity.GetName() + " is already in use", LogLevel.WARNING);
			return;
		}

		// Get the npc to look at you
		PAP_NPCComponent npcComponent = PAP_NPCComponent.Cast(pOwnerEntity.FindComponent(PAP_NPCComponent));
		if (npcComponent)
		{
			// Doesn't work :(
			// npcComponent.StartRotation(pUserEntity.GetAngles());
		}
		else
		{
			Print("NPC " + pOwnerEntity.GetName() + " doesn't have a PAP_NPCComponent", LogLevel.ERROR);
			return;
		}
		
		// Create the display and hydrate the widgets
		m_wDisplay = PAP_ConversationLayoutUI.Cast(GetGame().GetMenuManager().OpenDialog(ChimeraMenuPreset.PAP_ConversationLayout, DialogPriority.INFORMATIVE, 0, true));
		CharacterIdentityComponent playerCharacterIdentity = CharacterIdentityComponent.Cast(pUserEntity.FindComponent(CharacterIdentityComponent));
		m_wDisplay.ResumeDialogue(m_dialogueLoader, playerCharacterIdentity.GetIdentity(), pOwnerEntity);
	}

	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		CharacterIdentityComponent characterIdentityComponent = CharacterIdentityComponent.Cast(GetOwner().FindComponent(CharacterIdentityComponent));
		if (!characterIdentityComponent) return false;
		Identity identity = characterIdentityComponent.GetIdentity();
		if (!identity) return false;
		
		if (!m_wDisplay)
			outName = "Talk to " + identity.GetFullName();

		return true;
	}
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		if (m_wDisplay) return false;
		SCR_ChimeraCharacter owner = SCR_ChimeraCharacter.Cast(GetOwner());
		SCR_ChimeraCharacter player = SCR_ChimeraCharacter.Cast(user);
		
		// If unfriendly then false
		if (!owner.GetFaction().IsFactionFriendly(player.GetFaction())) return false;
		// If player then false
		if (owner.GetCharacterController().IsPlayerControlled()) return false;
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Loads conversation from conf file
	//! \return a container of the conversation
	protected PAP_ConversationConf LoadConfig()
	{
		if (m_sConversation.IsEmpty())
		{
			Print("Conversation file resource is undefined!", LogLevel.ERROR);
			return null;
		}
			
		Resource resource = Resource.Load(m_sConversation);
		if (!resource.IsValid())
			return null;

		return PAP_ConversationConf.Cast(BaseContainerTools.CreateInstanceFromContainer(resource.GetResource().ToBaseContainer()));
	}
}