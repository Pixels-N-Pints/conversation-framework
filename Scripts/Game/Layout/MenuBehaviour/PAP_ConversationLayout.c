//! Dialogue behaviour for the conversation layout
class PAP_ConversationLayoutUI : MenuBase
{
	// Member vars
	protected ref PAP_DialogueLoader m_cDialogueLoader;
	protected ref array<ref PAP_DialogueOptionJson> m_mOptions;
	protected bool m_bItallic = false;
	protected Identity m_PlayerIdentity;
	protected IEntity m_Npc;
	protected Identity m_NpcIdentity;
	
	// Constants
	protected const string MESSAGE = "Message";
	protected const string BUTTON = "Button";
	protected const string TEXT = "Text";
	protected const int MAX_OPTIONS = 3;
	
	//------------------------------------------------------------------------------------------------
	//! Loads the conversation from the checkpoint
	//! \param dialogueLoader Container for conversation
	//! \param playerIdentity Identity of the player
	//! \param npc The npc
	void ResumeDialogue(inout PAP_DialogueLoader dialogueLoader, Identity playerIdentity, IEntity npc)
	{
		if (!dialogueLoader)
		{	
			Print("Dialogue loader not supplied!", LogLevel.ERROR);
			return;
		}
		if (!playerIdentity)
		{
			Print("No player identity supplied!", LogLevel.ERROR);
			return;
		}
		if (!npc)
		{
			Print("No npc supplied!", LogLevel.ERROR);
			return;
		}
		
		m_cDialogueLoader = dialogueLoader;
		m_PlayerIdentity = playerIdentity;
		m_Npc = npc;
		CharacterIdentityComponent npcIdentityComponent = CharacterIdentityComponent.Cast(m_Npc.FindComponent(CharacterIdentityComponent));
		m_NpcIdentity = npcIdentityComponent.GetIdentity();
		
		ref PAP_DialogueOptionJson option = m_cDialogueLoader.GetCurrentOption();
		HandleOption(option);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Updates the message and handles options based on the supplied dialogue entry
	//! If the player is not required to answer the next entry will be displayed after the current entry's delay has expired
	//! \param entry The current entry to be displayed
	protected void HandleOption(PAP_DialogueOptionJson entry)
	{
		if (!entry)
		{
			Print("Dialogue entry not supplied!", LogLevel.WARNING);
			return;
		}
		// Display message
		UpdateMessage(entry.message);	
		// Display options
		UpdateOptions(entry.options);
		if (entry.next != -1)
		{
			m_cDialogueLoader.SetCheckpoint(entry.next);
			GetGame().GetCallqueue().CallLater(HandleOption, entry.delay * 1000, false, m_cDialogueLoader.GetCurrentOption());
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Displays the main message
	//! \param message The message to be displayed as a string
	protected void UpdateMessage(string message)
	{
		TextWidget messageWidget = TextWidget.Cast(GetWidgetByName(MESSAGE));
		if (!messageWidget) return;
		
		messageWidget.SetItalic(m_bItallic);
		messageWidget.SetText(PAP_StringInterpolation.InterpolateString(message, m_PlayerIdentity, m_NpcIdentity));
	}
	
	//------------------------------------------------------------------------------------------------
	//! Updates the option buttons. Disables and hides buttons not required.
	//! \param options The options the player can pick from
	protected void UpdateOptions(array<ref PAP_DialogueOptionJson> options)
	{
		m_mOptions = options;
		for(int i = 0; i < MAX_OPTIONS; i++)
		{	
			ButtonWidget button = ButtonWidget.Cast(GetWidgetByName(BUTTON + i));
			if (!button)
			{
				Print("Button widget of button " + i + " could not be found", LogLevel.WARNING);
				continue;
			}
			button.SetVisible(false);
			button.SetEnabled(false);
			
			if (m_mOptions && m_mOptions.Count() > i)
			{
				TextWidget option = TextWidget.Cast(button.GetChildren());
				if (!option)
				{
					Print("Text widget of button " + i + " could not be found", LogLevel.WARNING);
					continue;
				}
				// Update option text
				option.SetText(PAP_StringInterpolation.InterpolateString(m_mOptions[i].message, m_PlayerIdentity, m_NpcIdentity));
				// Reenable button
				button.SetVisible(true);
				button.SetEnabled(true);
				// Add event handler
				SCR_ButtonBaseComponent buttonComponent = SCR_ButtonBaseComponent.GetButtonBase(BUTTON + i, GetRootWidget());
				if (!buttonComponent)
				{
					Print("Button component of " + BUTTON + i + " could not be found", LogLevel.WARNING);
					continue;
				}
				
				buttonComponent.m_OnClicked.Clear();
				switch (i)
				{
					case 0:
						buttonComponent.m_OnClicked.Insert(HandleFirstOption);
						break;
					case 1:
						buttonComponent.m_OnClicked.Insert(HandleSecondOption);
						break;
					case 2:
						buttonComponent.m_OnClicked.Insert(HandleThirdOption);
						break;
					default:
						Print("Attempted to add event handler to undefined button " + BUTTON + i, LogLevel.WARNING);
				}
				
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Finds the widget in the dialogue by it's ID
	//! \param widgetName The ID of the widget
	//! \return the widget if found
	protected Widget GetWidgetByName(string widgetName)
	{
		if (!widgetName || widgetName == string.Empty) return null;
		
		Widget rootWidget = GetRootWidget();
		if (!rootWidget)
		{
			Print("Root widget could not be found", LogLevel.WARNING);
			return null;
		}

		Widget widget = Widget.Cast(rootWidget.FindWidget(widgetName));
		
		if (!widget)
		{
			Print("Widget with name " + widgetName + " could not be found", LogLevel.WARNING);
			return null;
		}
		
		return widget;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Callback for click event of first option
	protected void HandleFirstOption()
	{
		HandleEntryOption(m_mOptions.Get(0));
	}
	
	//------------------------------------------------------------------------------------------------
	//! Callback for click event of second option
	protected void HandleSecondOption()
	{
		HandleEntryOption(m_mOptions.Get(1));
	}
	
	//------------------------------------------------------------------------------------------------
	//! Callback for click event of third option
	protected void HandleThirdOption()
	{
		HandleEntryOption(m_mOptions.Get(2));
	}
	
	//------------------------------------------------------------------------------------------------
	//! Updates the checkpoint and displays the next entry
	protected void HandleEntryOption(PAP_DialogueOptionJson entryOption)
	{
		// m_bItallic = true;
		m_cDialogueLoader.SetCheckpoint(entryOption.next);
		HandleOption(m_cDialogueLoader.GetCurrentOption());
		// m_bItallic = false;
	}
	
	// Overrides for behaviour 
	//------------------------------------------------------------------------------------------------
	protected override void OnMenuOpen()
	{
		Print("OnMenuOpen: menu/dialog opened!", LogLevel.NORMAL);

		Widget rootWidget = GetRootWidget();
		if (!rootWidget)
		{
			Print("Error in Layout Tutorial layout creation", LogLevel.ERROR);
			return;
		}
		

		InputManager inputManager = GetGame().GetInputManager();
		if (inputManager)
		{
			// this is for the menu/dialog to close when pressing ESC
			// an alternative is to have a button with the SCR_NavigationButtonComponent component
			// and its Action Name field set to MenuBack - this would activate the button on ESC press
			inputManager.AddActionListener("MenuOpen", EActionTrigger.DOWN, Close);
			inputManager.AddActionListener("MenuBack", EActionTrigger.DOWN, Close);
#ifdef WORKBENCH // in Workbench, F10 is used because ESC closes the preview
			inputManager.AddActionListener("MenuOpenWB", EActionTrigger.DOWN, Close);
			inputManager.AddActionListener("MenuBackWB", EActionTrigger.DOWN, Close);
#endif
		}
	}

	//------------------------------------------------------------------------------------------------
	protected override void OnMenuClose()
	{
		// here we clean action listeners added above as the good practice wants it
		InputManager inputManager = GetGame().GetInputManager();
		if (inputManager)
		{
			inputManager.RemoveActionListener("MenuOpen", EActionTrigger.DOWN, Close);
			inputManager.RemoveActionListener("MenuBack", EActionTrigger.DOWN, Close);
#ifdef WORKBENCH
			inputManager.RemoveActionListener("MenuOpenWB", EActionTrigger.DOWN, Close);
			inputManager.RemoveActionListener("MenuBackWB", EActionTrigger.DOWN, Close);
#endif
		}
		
		// End npc rotation
		PAP_NPCComponent npcComponent = PAP_NPCComponent.Cast(m_Npc.FindComponent(PAP_NPCComponent));
		npcComponent.StopRotation();
	}

	//------------------------------------------------------------------------------------------------
	//! Destructor
	void ~PAP_ConversationLayoutUI()
	{
		delete m_mOptions;
	}
	
}