//! Dialogue behaviour for the conversation layout
class PNP_ConversationLayoutUI : MenuBase
{
	// Member vars
	private PNP_DialogueLoader m_DialogueLoader;
	private ref array<ref PNP_DialogueOptionJson> m_mOptions;
	private Identity m_PlayerIdentity;
	private Identity m_NpcIdentity;
	private PNP_NPCComponent m_NpcComponent;
	private ref SCR_ScenarioFrameworkActionBase m_EntityFactory;
	
	// Constants
	private const string MESSAGE = "Message";
	private const string BUTTON = "Button";
	private const string TEXT = "Text";
	private const int MAX_OPTIONS = 3;
	
	//------------------------------------------------------------------------------------------------
	override void OnMenuInit()
	{
		super.OnMenuInit();
		m_EntityFactory = new SCR_ScenarioFrameworkActionBase();
	}
	
	// Overrides for behaviour 
	//------------------------------------------------------------------------------------------------
	override void OnMenuOpen()
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
	override void OnMenuClose()
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
		m_NpcComponent.SetInUse(false);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Loads the conversation from the checkpoint
	//! \param dialogueLoader Container for conversation
	//! \param playerIdentity Identity of the player
	//! \param npc The npc
	void ResumeDialogue(inout PNP_DialogueLoader dialogueLoader, Identity playerIdentity, IEntity npc)
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
		
		m_DialogueLoader = dialogueLoader;
		m_PlayerIdentity = playerIdentity;
		CharacterIdentityComponent npcIdentityComponent = CharacterIdentityComponent.Cast(npc.FindComponent(CharacterIdentityComponent));
		m_NpcIdentity = npcIdentityComponent.GetIdentity();
		m_NpcComponent = PNP_NPCComponent.Cast(npc.FindComponent(PNP_NPCComponent));		
		m_NpcComponent.SetInUse(true);
		
		ref PNP_DialogueOptionJson option = m_DialogueLoader.GetCurrentOption();
		HandleOption(option);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Updates the message and handles options based on the supplied dialogue entry
	//! If the player is not required to answer the next entry will be displayed after the current entry's delay has expired
	//! \param entry The current entry to be displayed
	private void HandleOption(PNP_DialogueOptionJson entry)
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
		// Spawn any entity
		SpawnEntity(entry.entityName);
		// Handle next entry
		HandleNext(entry.next, entry.delay);
		
	}
	
	//------------------------------------------------------------------------------------------------
	//! Displays the main message
	//! \param message The message to be displayed as a string
	private void UpdateMessage(string message)
	{
		TextWidget messageWidget = TextWidget.Cast(GetWidgetByName(MESSAGE));
		if (!messageWidget) return;
		
		messageWidget.SetText(PNP_StringInterpolation.InterpolateString(message, m_PlayerIdentity, m_NpcIdentity));
	}
	
	//------------------------------------------------------------------------------------------------
	//! Updates the option buttons. Disables and hides buttons not required.
	//! \param options The options the player can pick from
	private void UpdateOptions(array<ref PNP_DialogueOptionJson> options)
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
				option.SetText(PNP_StringInterpolation.InterpolateString(m_mOptions[i].message, m_PlayerIdentity, m_NpcIdentity));
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
	//! Spawns the entity associated with the dialogue
	//! \param entityName Name of an entity in the world
	private void SpawnEntity(string entityName)
	{
		if (SCR_StringHelper.IsEmptyOrWhiteSpace(entityName)) return;
		m_EntityFactory.SpawnObjects({entityName}, SCR_ScenarioFrameworkEActivationType.ON_TRIGGER_ACTIVATION);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Handles the next entry if there are no options
	//! \param next Id of the next dialogue entry
	//! \param delay Delay in seconds the current message should persist
	private void HandleNext(int next, int delay)
	{
		if (next != -1 && next != 0)
		{
			m_NpcComponent.SetCheckpoint(next);
			GetGame().GetCallqueue().CallLater(HandleOption, delay * 1000, false, m_DialogueLoader.GetCurrentOption());
		}
		else if (next == 0)
		{	
			GetGame().GetCallqueue().CallLater(Close, delay * 1000, false);
		}
	}
	
	
	//------------------------------------------------------------------------------------------------
	//! Finds the widget in the dialogue by it's ID
	//! \param widgetName The ID of the widget
	//! \return the widget if found
	private Widget GetWidgetByName(string widgetName)
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
	private void HandleFirstOption()
	{
		HandleEntryOption(m_mOptions.Get(0));
	}
	
	//------------------------------------------------------------------------------------------------
	//! Callback for click event of second option
	private void HandleSecondOption()
	{
		HandleEntryOption(m_mOptions.Get(1));
	}
	
	//------------------------------------------------------------------------------------------------
	//! Callback for click event of third option
	private void HandleThirdOption()
	{
		HandleEntryOption(m_mOptions.Get(2));
	}
	
	//------------------------------------------------------------------------------------------------
	//! Updates the checkpoint and displays the next entry
	private void HandleEntryOption(PNP_DialogueOptionJson entryOption)
	{
		m_NpcComponent.SetCheckpoint(entryOption.next);
		GetGame().GetCallqueue().CallLater(HandleOption, 500, false, m_DialogueLoader.GetCurrentOption());
	}

	//------------------------------------------------------------------------------------------------
	//! Destructor
	void ~PNP_ConversationLayoutUI()
	{
		delete m_EntityFactory;
	}
	
}