//! Allows embedding of expressions in template strings
enum PAP_TemplateString
{
	//! Name of the identity of the entity
	IDENTITY_NAME,
	//! Alias of the identity of the entity
	IDENTITY_ALIAS,
	//! Surname of the identity of the entity
	IDENTITY_SURNAME,
	//! Fullname of the identity of the entity
	IDENTITY_FULLNAME,
	NPC_IDENTITY_NAME,
	NPC_IDENTITY_ALIAS,
	NPC_IDENTITY_SURNAME,
	NPC_IDENTITY_FULLNAME,
}

class PAP_StringInterpolation
{
	static const string INTERPOLATE_CHAR_START = "{";
	static const string INTERPOLATE_CHAR_END = "}";

	//------------------------------------------------------------------------------------------------
	// TODO: Find a better way to do this. It's very cumbersome and hard to extend.
	//! Substitutes expressions in squirly braces with properties of player or npc identities
	//! \param templateString A message the npc or player says
	//! \param identity Identity of the player
	//! \param npcIdentity Identity of the npc	
	//! \return The template string with all expressions substituted
	static string InterpolateString(string templateString, Identity identity, Identity npcIdentity)
	{
		if (SCR_StringHelper.IsEmptyOrWhiteSpace(templateString))
		{
			Print("Template string not supplied!", LogLevel.ERROR);
			return string.Empty;
		}
		
		if (!identity)
		{
			Print("Player identity not supplied!", LogLevel.ERROR);
			return string.Empty;
		}
			
		if (!npcIdentity)
		{
			Print("NPC identity not supplied!", LogLevel.ERROR);
			return string.Empty;
		}
		
		
		
		// Break the template string down into tokens
		array<string> tokens = {};
		templateString.Split(" ", tokens, true);
		
		if (!tokens || tokens.IsEmpty()) return "";
		
		
		array<string> outputTokens = {};
		
		foreach (string token : tokens)
		{
			
			if (token.IndexOf(INTERPOLATE_CHAR_START) != -1 && token.IndexOf(INTERPOLATE_CHAR_END) != -1)
			{
				int expressionStart = token.IndexOf(INTERPOLATE_CHAR_START);
				int expressionEnd = token.IndexOf(INTERPOLATE_CHAR_END);
				
				int expressionVariableStart = expressionStart + 1;
				int expressionVariableEnd = expressionEnd;
				int length = expressionVariableEnd - expressionVariableStart;
				string expressionVariable = token.Substring(expressionVariableStart, length);
				if (SCR_StringHelper.IsEmptyOrWhiteSpace(expressionVariable)) continue;
				string substitute = token.Substring(0, expressionStart) + 
												SubsituteExpression(expressionVariable, identity, npcIdentity) + 
												token.Substring(expressionEnd + 1, token.Length() - expressionEnd - 1); 
				outputTokens.Insert(substitute);
			}
			else
			{		
				outputTokens.Insert(token);
			}
		}
		return SCR_StringHelper.Join(" ", outputTokens);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Substitutes the expression
	//! \param expression Expression to subsitute
	//! \param identity Identity of the player
	//! \param npcIdentity Identity of the npc	
	//! \return The substituted expression
	protected static string SubsituteExpression(string expression, Identity identity, Identity npcIdentity)
	{
		string result;
		switch (typename.StringToEnum(PAP_TemplateString, expression))
		{
			case PAP_TemplateString.IDENTITY_NAME:
				result = identity.GetName();
				break;
			case PAP_TemplateString.IDENTITY_ALIAS:
				result = identity.GetAlias();
				break;
			case PAP_TemplateString.IDENTITY_SURNAME:
				result = identity.GetSurname();
				break;
			case PAP_TemplateString.IDENTITY_FULLNAME:
				result = identity.GetFullName();
				break;
			case PAP_TemplateString.NPC_IDENTITY_NAME:
				result = npcIdentity.GetName();
				break;
			case PAP_TemplateString.NPC_IDENTITY_ALIAS:
				result = npcIdentity.GetAlias();
				break;
			case PAP_TemplateString.NPC_IDENTITY_SURNAME:
				result = npcIdentity.GetSurname();
				break;
			case PAP_TemplateString.NPC_IDENTITY_FULLNAME:
				result = npcIdentity.GetFullName();
				break;
		}
		return result;
	}
}