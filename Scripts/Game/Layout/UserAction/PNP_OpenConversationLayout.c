//! Container for the open conversation dialogue user action
class PNP_ConversationLayoutAttributes : ScriptedUserAction
{
	[Attribute(defvalue: "{A27941092D1EA761}UI/Layouts/conversation.layout")] // setup the created layout here
	protected ResourceName m_sLayout;
	
	[Attribute(params: "conf", desc: "Conversation file for this AI agent")]
	protected ResourceName m_sConversation;
	
	protected PNP_ConversationLayoutUI m_wDisplay;
	
	protected ref PNP_DialogueLoader m_dialogueLoader;
}
//! Handles the user action's behaviour
class PNP_ConversationLayoutUserAction : PNP_ConversationLayoutAttributes
{
	//------------------------------------------------------------------------------------------------
	override bool HasLocalEffectOnlyScript()
	{
		return true;
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
		
		if (!m_dialogueLoader)
		{	
			m_dialogueLoader = new PNP_DialogueLoader();
			ref PNP_NPCComponent npcComponent = PNP_NPCComponent.Cast(pOwnerEntity.FindComponent(PNP_NPCComponent));
			m_dialogueLoader.LoadConversation(LoadConfig(), npcComponent);

			npcComponent.SetDialogueLoader(m_dialogueLoader);
		}
				
		// Create the display and hydrate the widgets
		m_wDisplay = PNP_ConversationLayoutUI.Cast(GetGame().GetMenuManager().OpenDialog(ChimeraMenuPreset.PNP_ConversationLayout, DialogPriority.INFORMATIVE, 0, true));
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
		if (!PNP_NPCComponent.Cast(GetOwner().FindComponent(PNP_NPCComponent)))
			return false;
		
		if (m_wDisplay) return false;
	
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Loads conversation from conf file
	//! \return a container of the conversation
	protected PNP_ConversationConf LoadConfig()
	{
		if (m_sConversation.IsEmpty())
		{
			Print("Conversation file resource is undefined!", LogLevel.ERROR);
			return null;
		}
			
		Resource resource = Resource.Load(m_sConversation);
		if (!resource.IsValid())
			return null;

		return PNP_ConversationConf.Cast(BaseContainerTools.CreateInstanceFromContainer(resource.GetResource().ToBaseContainer()));
	}
}