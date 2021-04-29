#include "Script/SingleScriptParser.hpp"

#include "Script/Caster.hpp"
#include "Script/CodingStyle.hpp"
#include "Script/DeclaredObjectDefinition.hpp"
#include "Script/Function.hpp"
#include "Script/ScriptLoader.hpp"
#include "StringUtils.hpp"

static const std::string &PARAMETER_MODIFIER{ "&!" };

namespace Nectere
{
	namespace Script
	{
		SingleScriptParser::SingleScriptParser(const std::string &content, Script *script, ScriptLoader *loader) : m_Script(script), m_Loader(loader)
		{
			size_t strSize = content.size() + 1;
			char *buffer = new char[strSize];
			size_t bufferIdx = 0;
			bool inComment = false;
			size_t nbQuoteMarkInStr = 0;
			for (char c : content)
			{
				if (inComment)
				{
					if (c == '#')
						inComment = false;
				}
				else
				{
					if (c == '"')
						++nbQuoteMarkInStr;
					if (c == '#' && (nbQuoteMarkInStr % 2) == 0)
						inComment = true;
					else if ((nbQuoteMarkInStr % 2) != 0 || c > 31)
					{
						if (bufferIdx < strSize)
							buffer[bufferIdx] = c;
						++bufferIdx;
					}
				}
			}
			if (bufferIdx < strSize)
				buffer[bufferIdx] = '\0';
			m_Content = buffer;
			delete[](buffer);
		}

		bool SingleScriptParser::TreatTags(const std::string &line, size_t &nbChars, std::vector<Tag> &tags)
		{
			size_t lineSize = line.size();
			bool inTagParameter = false;
			bool foundTagEnd = false;
			std::string currentTag;
			std::string currentParameter;
			std::unordered_set<std::string> currentTagParameter;
			for (nbChars = 1; nbChars != lineSize && !foundTagEnd; ++nbChars)
			{
				char c = line[nbChars];
				if (inTagParameter)
				{
					switch (c)
					{
					case ')':
					{
						inTagParameter = false;
						break;
					}
					case ',':
					{
						StringUtils::Trim(currentParameter);
						if (currentParameter.size() != 0)
						{
							currentTagParameter.insert(currentParameter);
							currentParameter = "";
						}
						break;
					}
					default:
						currentParameter += c;
					}
				}
				else
				{
					switch (c)
					{
					case ']':
					{
						foundTagEnd = true;
						break;
					}
					case ',':
					{
						StringUtils::Trim(currentTag);
						if (currentTag.size() != 0)
						{
							tags.emplace_back(Tag{ currentTag, currentTagParameter });
							currentTag = "";
							currentTagParameter.clear();
						}
						break;
					}
					case '(':
					{
						inTagParameter = true;
						break;
					}
					default:
						currentTag += c;
					}
				}
			}
			return foundTagEnd;
		}

		bool SingleScriptParser::Parse(const std::string &scriptPath)
		{
			std::vector<std::string> fragments;
			int check = StringUtils::SmartSplit(fragments, m_Content, "}", true);
			switch (check)
			{
			case 1:
			{
				LOG_SCRIPT_PARSING(LogType::Error, "Invalid script \"", m_Script->GetName(), "\": Closing } doesn't match any {");
				return false;
			}
			case 2:
			{
				LOG_SCRIPT_PARSING(LogType::Error, "Invalid script \"", m_Script->GetName(), "\": Non closing {");
				return false;
			}
			case 3:
			{
				LOG_SCRIPT_PARSING(LogType::Error, "Invalid script \"", m_Script->GetName(), "\": Non closing \"");
				return false;
			}
			}
			for (const auto &fragment : fragments)
			{
				std::vector<std::string> declarations;
				StringUtils::SmartSplit(declarations, fragment, ";", false);
				std::vector<Tag> tags;
				for (const auto &declaration : declarations)
				{
					std::string line = declaration;
					while (StringUtils::StartWith(line, "["))
					{
						size_t tagSize = 1;
						if (!TreatTags(line, tagSize, tags))
							return false;
						line = line.substr(tagSize);
					}
					if (StringUtils::StartWith(line, "declare"))
					{
						if (!ParseDeclaredObject(line, tags))
							return false;
						tags.clear();
					}
					else if (StringUtils::StartWith(line, "import"))
					{
						if (!ParseImport(line, scriptPath, tags))
							return false;
					}
					else if (StringUtils::StartWith(line, "cast"))
					{
						if (!ParseCast(line.substr(4), tags, false))
							return false;
						tags.clear();
					}
					else if (StringUtils::StartWith(line, "!cast"))
					{
						if (!ParseCast(line.substr(5), tags, true))
							return false;
						tags.clear();
					}
					else
					{
						size_t openBracketPos = line.find('{');
						if (openBracketPos == std::string::npos || line.find('=') < openBracketPos)
						{
							if (!ParseGlobal(line, tags))
								return false;
							tags.clear();
						}
						else
						{
							if (!ParseFunction(line, tags))
								return false;
							tags.clear();
						}
					}
				}
			}
			return true;
		}

		bool SingleScriptParser::ParseGlobal(const std::string &global, const std::vector<Tag> &tags)
		{
			std::string globalTypeAndName;
			std::string globalValue = "";
			size_t equalOperatorPos = global.find('=');
			if (equalOperatorPos != std::string::npos)
			{
				globalTypeAndName = global.substr(0, equalOperatorPos);
				globalValue = global.substr(equalOperatorPos + 1);
			}
			else
				globalTypeAndName = global;
			StringUtils::Trim(globalTypeAndName);
			StringUtils::Trim(globalValue);
			std::vector<std::string> globalInfo = StringUtils::Split(globalTypeAndName, " ", true, false);
			if (globalInfo.size() == 2)
			{
				bool isPrivate = false;
				std::string globalType = globalInfo[0];
				std::string globalName = globalInfo[1];
				if (globalName.size() == 1 && globalName[0] == '!')
				{
					LOG_SCRIPT_PARSING(LogType::Error, "In ", m_Script->GetName(), ": Invalid global \"", globalTypeAndName, "\": No name to global");
					return false;
				}
				if (globalName[0] == '!')
				{
					isPrivate = true;
					globalName = globalName.substr(1);
				}
				if (!CodingStyle::CheckName(CodingStyle::NameType::Global, globalName))
					return false;
				if (m_RegisteredGlobalToCompile.find(globalName) != m_RegisteredGlobalToCompile.end())
				{
					LOG_SCRIPT_PARSING(LogType::Error, "In ", m_Script->GetName(), ": Global ", globalName, " already exist");
					return false;
				}
				m_RegisteredGlobalToCompile.insert(globalName);
				if (globalValue.size() == 0)
				{
					m_Script->AddGlobalToCompile(Script::GlobalDefinition{ globalName, globalType, globalValue, isPrivate, tags });
					LOG_SCRIPT_PARSING(LogType::Verbose, globalType, ' ', globalName, " added without default value but not compiled");
					return true;
				}
				else
				{
					std::vector<std::string> tmp;
					StringUtils::SmartSplit(tmp, globalValue, ",", false);
					m_Script->AddGlobalToCompile(Script::GlobalDefinition{ globalName, globalType, globalValue, isPrivate, tags });
					LOG_SCRIPT_PARSING(LogType::Verbose, globalType, ' ', globalName, " added with default value: ", globalValue, " but not compiled");
					return true;
				}
			}
			LOG_SCRIPT_PARSING(LogType::Error, "In ", m_Script->GetName(), ": Invalid global \"", globalTypeAndName, "\": No type or name");
			return false;
		}

		bool SingleScriptParser::ParseImport(const std::string &import, const std::string &scriptPath, const std::vector<Tag> &tags)
		{
			bool isPrivate = false;
			std::string importName = import.substr(6);
			StringUtils::Trim(importName);
			size_t importNameSize = importName.size();
			if (importNameSize == 0 || (importNameSize == 1 && importName[0] == '!'))
			{
				LOG_SCRIPT_PARSING(LogType::Error, "In ", m_Script->GetName(), ": Invalid import \"", import, "\": No import");
				return false;
			}
			if (importName[0] == '!')
			{
				isPrivate = true;
				importName = importName.substr(1);
			}
			if (m_RegisteredImportToCompile.find(importName) != m_RegisteredImportToCompile.end())
			{
				LOG_SCRIPT_PARSING(LogType::Error, "In ", m_Script->GetName(), ": ", importName, " already imported");
				return false;
			}
			m_RegisteredImportToCompile.insert(importName);
			Script *importToAdd = m_Loader->Load(scriptPath + importName, false, m_Script->GetNative());
			if (!importToAdd)
				return false;
			importToAdd->AddTags(tags);
			if (isPrivate)
			{
				LOG_SCRIPT_PARSING(LogType::Verbose, "Private import ", importName, " added");
				m_Script->AddPrivateImport(importToAdd);
			}
			else
			{
				LOG_SCRIPT_PARSING(LogType::Verbose, "Public import ", importName, " added");
				m_Script->AddPublicImport(importToAdd);
			}
			return true;
		}

		DeclaredObjectDefinition *SingleScriptParser::ParseDeclaredObjectDefinition(const std::string &definition)
		{
			size_t openingBracketPos = definition.find('{');
			if (openingBracketPos == std::string::npos || definition[definition.size() - 1] != '}')
			{
				LOG_SCRIPT_PARSING(LogType::Error, "Invalid object definition \"", definition, "\": No object definition");
				return nullptr;
			}
			std::string typeName = definition.substr(0, openingBracketPos);
			StringUtils::Trim(typeName);
			if (typeName.size() == 0)
			{
				LOG_SCRIPT_PARSING(LogType::Error, "Invalid object definition \"", definition, "\": No object name");
				return nullptr;
			}

			if (!CodingStyle::CheckName(CodingStyle::NameType::DeclaredObject, typeName))
				return nullptr;
			std::string typeDefinition = definition.substr(openingBracketPos);
			size_t valueSize = typeDefinition.size();
			if (valueSize >= 3)
			{
				DeclaredObjectDefinition *newDeclaredObjectDefiniton = new DeclaredObjectDefinition();
				if (!newDeclaredObjectDefiniton)
				{
					LOG_SCRIPT_PARSING(LogType::Error, "Cannot create new object definition for ", typeName, ": Out of memory");
					return nullptr;
				}
				newDeclaredObjectDefiniton->m_TypeName = typeName;
				std::vector<std::string> subValues;
				StringUtils::SmartSplit(subValues, typeDefinition.substr(1, valueSize - 2), ",", false);
				if (subValues.size() == 0)
				{
					LOG_SCRIPT_PARSING(LogType::Error, "Empty object definition");
					delete(newDeclaredObjectDefiniton);
					return nullptr;
				}
				LOG_SCRIPT_PARSING(LogType::Verbose, "Creating object definition for ", newDeclaredObjectDefiniton->m_TypeName);
				for (const auto &line : subValues)
				{
					std::vector<Tag> tags;
					std::string subValue = line;
					while (StringUtils::StartWith(subValue, "["))
					{
						size_t tagSize = 1;
						if (!TreatTags(subValue, tagSize, tags))
							return false;
						subValue = subValue.substr(tagSize);
					}
					std::string subValueTypeAndName;
					std::string subValueDefaultValue = "";
					size_t equalOperatorPos = subValue.find('=');
					if (equalOperatorPos != std::string::npos)
					{
						subValueTypeAndName = subValue.substr(0, equalOperatorPos);
						subValueDefaultValue = subValue.substr(equalOperatorPos + 1);
					}
					else
						subValueTypeAndName = subValue;
					StringUtils::Trim(subValueTypeAndName);
					StringUtils::Trim(subValueDefaultValue);
					std::vector<std::string> subValueInfo = StringUtils::Split(subValueTypeAndName, " ", true, false);
					if (subValueInfo.size() == 2)
					{
						std::string subValueTypeName = subValueInfo[0];
						std::string subValueName = subValueInfo[1];
						if (!newDeclaredObjectDefiniton->AddVariable(subValueName, VariableType(subValueTypeName), subValueDefaultValue, tags))
						{
							delete(newDeclaredObjectDefiniton);
							return nullptr;
						}
						if (subValueDefaultValue.size() == 0)
							LOG_SCRIPT_PARSING(LogType::Verbose, "    Adding ", subValueTypeName, ' ', subValueName, " to object definition of ", newDeclaredObjectDefiniton->m_TypeName, " without default value");
						else
							LOG_SCRIPT_PARSING(LogType::Verbose, "    Adding ", subValueTypeName, ' ', subValueName, " to object definition of ", newDeclaredObjectDefiniton->m_TypeName, " with default value: ", subValueDefaultValue);
					}
				}
				return newDeclaredObjectDefiniton;
			}
			else
			{
				LOG_SCRIPT_PARSING(LogType::Error, "Invalid object definition \"", definition, "\": No definition inside {}");
				return nullptr;
			}
		}

		bool SingleScriptParser::ParseDeclaredObject(const std::string &declaration, const std::vector<Tag> &tags)
		{
			bool isPrivate = false;
			std::string typeDefinition = declaration.substr(7);
			StringUtils::Trim(typeDefinition);
			size_t typeDefinitionSize = typeDefinition.size();
			if (typeDefinitionSize == 0 || (typeDefinitionSize == 1 && typeDefinition[0] == '!'))
			{
				LOG_SCRIPT_PARSING(LogType::Error, "In ", m_Script->GetName(), ": Invalid declaration \"", declaration, "\": No declaration");
				return false;
			}
			if (typeDefinition[0] == '!')
			{
				isPrivate = true;
				typeDefinition = typeDefinition.substr(1);
			}
			if (m_RegisteredDefinitionToCompile.find(typeDefinition) != m_RegisteredDefinitionToCompile.end())
				return false;
			DeclaredObjectDefinition *declaredObjectDefinition = ParseDeclaredObjectDefinition(typeDefinition);
			if (!declaredObjectDefinition)
				return false;
			declaredObjectDefinition->AddTags(tags);
			m_RegisteredDefinitionToCompile.insert(typeDefinition);
			if (isPrivate)
			{
				LOG_SCRIPT_PARSING(LogType::Verbose, "Private object of type ", declaredObjectDefinition->m_TypeName, " defined but not compiled yet");
				m_Script->AddPrivateDeclaredObjectDefinition(declaredObjectDefinition);
			}
			else
			{
				LOG_SCRIPT_PARSING(LogType::Verbose, "Public object of type ", declaredObjectDefinition->m_TypeName, " defined but not compiled yet");
				m_Script->AddPublicDeclaredObjectDefinition(declaredObjectDefinition);
			}
			return true;
		}

		bool SingleScriptParser::ParseSignature(const std::string &signatureStr, bool &isPrivate, Callable *function)
		{
			std::string tmp = signatureStr;
			std::string signatureArgument;
			std::string signatureName;
			std::string signatureType;
			size_t parenthesesPos = tmp.find('(');
			if (parenthesesPos != std::string::npos)
			{
				size_t closingParenthesesPos = tmp.find(')');
				if (closingParenthesesPos == std::string::npos)
				{
					LOG_SCRIPT_PARSING(LogType::Error, "Invalid function signature \"", tmp, "\": Missing )");
					return false;
				}
				signatureArgument = tmp.substr(parenthesesPos + 1, (closingParenthesesPos - (parenthesesPos + 1)));
				tmp = tmp.substr(0, parenthesesPos);
			}
			std::vector<std::string> signatureInfo = StringUtils::Split(tmp, " ", true, false);
			switch (signatureInfo.size())
			{
			case 1:
			{
				signatureName = signatureInfo[0];
				break;
			}
			case 2:
			{
				signatureType = signatureInfo[0];
				signatureName = signatureInfo[1];
				break;
			}
			default:
			{
				LOG_SCRIPT_PARSING(LogType::Error, "Invalid function signature \"", tmp, "\": Function can only have one return type");
				return false;
			}
			}
			size_t signatureNameSize = signatureName.size();
			if (signatureNameSize == 0 || (signatureNameSize == 1 && signatureName[0] == '!'))
			{
				LOG_SCRIPT_PARSING(LogType::Error, "Invalid function signature \"", tmp, "\": Missing function name");
				return false;
			}
			if (signatureName[0] == '!')
			{
				signatureName = signatureName.substr(1);
				isPrivate = true;
			}
			if (!CodingStyle::CheckName(CodingStyle::NameType::Function, signatureName))
				return false;
			if (signatureType.size() != 0)
				function->SetReturnType(VariableType(signatureType));
			function->SetName(signatureName);
			LOG_SCRIPT_PARSING(LogType::Verbose, "Adding function ", signatureName, " with return type ", signatureType);
			std::vector<std::string> signatureArgumentList = StringUtils::Split(signatureArgument, ",", true, false);
			for (const auto &argument : signatureArgumentList)
			{
				std::vector argumentInfo = StringUtils::Split(argument, " ", true);
				size_t argumentInfoSize = argumentInfo.size();
				if ((argumentInfoSize == 4 && argumentInfo[2] != "?") || (argumentInfoSize != 2 && argumentInfoSize != 4))
				{
					LOG_SCRIPT_PARSING(LogType::Error, "Invalid function signature \"", tmp, "\": Format error in parameter \"", argument, '"');
					return false;
				}
				Parameter::Type argumentParameterType = Parameter::Type::Copy;
				bool argumentIsConst = false;
				std::string argumentType = argumentInfo[0];
				std::string argumentName = argumentInfo[1];
				VariableType argumentVariableType(argumentType);
				char modifier = '\0';
				if (PARAMETER_MODIFIER.find(argumentName[0]) != std::string::npos)
				{
					modifier = argumentName[0];
					argumentName = argumentName.substr(1);
				}
				if (argumentName.size() == 0)
				{
					LOG_SCRIPT_PARSING(LogType::Error, "Invalid function signature \"", tmp, "\": Parameter must have a name");
					return false;
				}
				switch (modifier)
				{
				case '&':
				{
					argumentParameterType = Parameter::Type::Mutable;
					break;
				}
				default:
					break;
				}
				function->AddArgument(argumentName, argumentVariableType, argumentParameterType);
				if (argumentInfoSize == 4)
					function->AddDefaultValue(argumentName, argumentInfo[3]);
				switch (argumentParameterType)
				{
				case Parameter::Type::Mutable:
				{
					LOG_SCRIPT_PARSING(LogType::Verbose, "    Adding mutable ", argumentType, ' ', argumentName, " to parameters list");
					break;
				}
				case Parameter::Type::Const:
				{
					LOG_SCRIPT_PARSING(LogType::Verbose, "    Adding const ", argumentType, ' ', argumentName, " to parameters list");
					break;
				}
				case Parameter::Type::Copy:
				{
					LOG_SCRIPT_PARSING(LogType::Verbose, "    Adding non-mutable and non-const ", argumentType, ' ', argumentName, " to parameters list");
					break;
				}
				default:
					break;
				}
			}
			return true;
		}

		bool SingleScriptParser::ParseFunction(const std::string &content, const std::vector<Tag> &tags)
		{
			size_t contentSize = content.size();
			size_t openBracketPos = content.find('{');
			std::string signatureStr = content.substr(0, openBracketPos);
			std::string body = content.substr(openBracketPos + 1, (contentSize - 2 - signatureStr.size()));
			StringUtils::Trim(signatureStr);
			StringUtils::Trim(body);
			if (signatureStr.size() == 0)
			{
				LOG_SCRIPT_PARSING(LogType::Error, "In ", m_Script->GetName(), ": Invalid function \"", content, "\": No function signature");
				return false;
			}
			Function *newFunction = new Function(m_Script);
			if (!newFunction)
			{
				LOG_SCRIPT_PARSING(LogType::Error, "Cannot create new function: Out of memory");
				return false;
			}
			newFunction->AddTags(tags);
			bool isPrivate = false;
			if (!ParseSignature(signatureStr, isPrivate, newFunction) || !newFunction->AddBody(body))
			{
				delete(newFunction);
				return false;
			}
			std::string functionName = newFunction->GetName();
			if (m_RegisteredFunctionToCompile.find(functionName) != m_RegisteredFunctionToCompile.end())
			{
				LOG_SCRIPT_PARSING(LogType::Error, "In ", m_Script->GetName(), ": Function ", functionName, " already exist");
				delete(newFunction);
				return false;
			}
			if (isPrivate)
			{
				LOG_SCRIPT_PARSING(LogType::Verbose, "Private function ", functionName, " defined but not compiled yet");
				m_Script->AddPrivateFunction(newFunction);
			}
			else
			{
				LOG_SCRIPT_PARSING(LogType::Verbose, "Public function ", functionName, " defined but not compiled yet");
				m_Script->AddPublicFunction(newFunction);
			}
			m_RegisteredFunctionToCompile.insert(functionName);
			return true;
		}

		bool SingleScriptParser::ParseCast(const std::string &content, const std::vector<Tag> &tags, bool isPrivate)
		{
			size_t contentSize = content.size();
			size_t openBracketPos = content.find('{');
			std::string signatureStr = content.substr(0, openBracketPos);
			std::string castDefinition = content.substr(openBracketPos + 1, (contentSize - 2 - signatureStr.size()));
			StringUtils::Trim(signatureStr);
			StringUtils::Trim(castDefinition);
			if (signatureStr.size() == 0)
			{
				LOG_SCRIPT_PARSING(LogType::Error, "In ", m_Script->GetName(), ": Invalid cast \"", content, "\": No cast signature");
				return false;
			}
			std::vector<std::string> signatureInfo = StringUtils::Split(signatureStr, " ", true, false);
			if (signatureInfo.size() != 3)
			{
				LOG_SCRIPT_PARSING(LogType::Error, "In ", m_Script->GetName(), ": Invalid cast \"", content, "\": Invalid cast signature");
				return false;
			}
			bool isBiderictional = signatureInfo[1] == "<->";
			if (!isBiderictional && signatureInfo[1] != "->")
			{
				LOG_SCRIPT_PARSING(LogType::Error, "In ", m_Script->GetName(), ": Invalid cast \"", content, "\": Invalid cast signature");
				return false;
			}
			Caster *caster = new Caster(signatureInfo[0], signatureInfo[2], isBiderictional);
			if (!caster)
			{
				LOG_SCRIPT_PARSING(LogType::Error, "Cannot create new caster: Out of memory");
				return false;
			}
			auto aToBIt = m_RegisteredCasterToCompile.find(caster->m_TypeA);
			if (aToBIt == m_RegisteredCasterToCompile.end())
			{
				m_RegisteredCasterToCompile[caster->m_TypeA] = std::unordered_set<VariableType, VariableTypeHasher>();
				aToBIt = m_RegisteredCasterToCompile.find(caster->m_TypeA);
			}
			if ((*aToBIt).second.find(caster->m_TypeB) != (*aToBIt).second.end())
			{
				LOG_SCRIPT_PARSING(LogType::Error, "Cannot create new caster: Caster ", signatureInfo[0], "->", signatureInfo[1], " already exist");
				delete(caster);
				return false;
			}
			(*aToBIt).second.insert(caster->m_TypeB);
			if (caster->m_IsBidirectional)
			{
				auto bToAIt = m_RegisteredCasterToCompile.find(caster->m_TypeB);
				if (bToAIt == m_RegisteredCasterToCompile.end())
				{
					m_RegisteredCasterToCompile[caster->m_TypeB] = std::unordered_set<VariableType, VariableTypeHasher>();
					bToAIt = m_RegisteredCasterToCompile.find(caster->m_TypeB);
				}
				if ((*aToBIt).second.find(caster->m_TypeA) != (*aToBIt).second.end())
				{
					LOG_SCRIPT_PARSING(LogType::Error, "Cannot create new caster: Caster ", signatureInfo[1], "->", signatureInfo[0], " already exist");
					delete(caster);
					return false;
				}
				(*aToBIt).second.insert(caster->m_TypeA);
			}
			caster->AddTags(tags);
			std::vector<std::string> casterDefinitions = StringUtils::Split(castDefinition, ";", true, false);
			for (const std::string &casterDefinition : casterDefinitions)
			{
				std::vector<std::string> casterInfo = StringUtils::Split(casterDefinition, ":", true, false);
				if (casterInfo.size() != 2)
				{
					LOG_SCRIPT_PARSING(LogType::Error, "Cannot create new caster", signatureStr, ": Caster misformated definition ", casterDefinition);
					delete(caster);
					return false;
				}
				caster->AddCastDefinition(casterInfo[0], casterInfo[1]);
			}
			if (isPrivate)
			{
				LOG_SCRIPT_PARSING(LogType::Verbose, "Private caster ", signatureStr, " defined but not compiled yet");
				m_Script->AddPrivateCaster(caster);
			}
			else
			{
				LOG_SCRIPT_PARSING(LogType::Verbose, "Public caster ", signatureStr, " defined but not compiled yet");
				m_Script->AddPublicCaster(caster);
			}
			return true;
		}

		bool SingleScriptParser::ParsePublicCast(const std::string &content, const std::vector<Tag> &tags)
		{
			return ParseCast(content.substr(4), tags, false);
		}

		bool SingleScriptParser::ParsePrivateCast(const std::string &content, const std::vector<Tag> &tags)
		{
			return ParseCast(content.substr(5), tags, true);
		}
	}
}