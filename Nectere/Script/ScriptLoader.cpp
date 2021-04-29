#include "Script/ScriptLoader.hpp"

#include <fstream>
#include <filesystem>
#include <sys/stat.h>
#include "Script/SingleScriptParser.hpp"
#include "StringUtils.hpp"

namespace Nectere
{
	namespace Script
	{
		Script *ScriptLoader::Load(const std::string &path, bool reload, Native *native)
		{
			std::filesystem::path fsPath(path);
			std::string fileName = fsPath.filename().string();
			std::string relativePath = path.substr(0, path.size() - fileName.size());
			std::filesystem::path fsAbsolutePath = std::filesystem::absolute(fsPath);
			std::string absoluteName = fsAbsolutePath.string();
			std::string absolutePath = absoluteName.substr(0, absoluteName.size() - fileName.size());
			Script *scriptToLoad = nullptr;
			if (!reload)
			{
				auto loadedScriptIt = m_LoadedScript.find(absoluteName);
				if (loadedScriptIt != m_LoadedScript.end())
					return (*loadedScriptIt).second;
				scriptToLoad = new Script(native, fileName);
				if (!scriptToLoad)
				{
					LOG_SCRIPT_PARSING(LogType::Error, "Cannot create new script: Out of memory");
					m_LoadedScript[absoluteName] = nullptr;
					return nullptr;
				}
				m_LoadedScript[absoluteName] = scriptToLoad;
			}
			else
			{
				auto loadedScriptIt = m_LoadedScript.find(absoluteName);
				if (loadedScriptIt != m_LoadedScript.end())
				{
					scriptToLoad = (*loadedScriptIt).second;
					if (!scriptToLoad)
					{
						LOG_SCRIPT_PARSING(LogType::Verbose, "Cannot reload ", fileName, " try loading it instead");
						return Load(path, false, native);
					}
				}
				else
					return nullptr;
				scriptToLoad->Clear();
			}
			struct stat buf;
			if (stat(path.c_str(), &buf) != 0)
			{
				LOG_SCRIPT_PARSING(LogType::Error, "Cannot open ", path, ": No such file");
				if (!reload)
				{
					delete(scriptToLoad);
					m_LoadedScript[absoluteName] = nullptr;
				}
				return nullptr;
			}
			std::ifstream fileStream(path.c_str());
			std::string content((std::istreambuf_iterator<char>(fileStream)), std::istreambuf_iterator<char>());
			SingleScriptParser parser(content, scriptToLoad, this);
			if (parser.Parse(relativePath))
				return scriptToLoad;
			if (!reload)
			{
				delete(scriptToLoad);
				m_LoadedScript[absoluteName] = nullptr;
			}
			return nullptr;
		}

		Script *ScriptLoader::Load(const std::string &path, Native *native)
		{
			Script *loadedScript = Load(path, false, native);
			if (loadedScript)
				loadedScript->Compile();
			return loadedScript;
		}

		Script *ScriptLoader::Get(const std::string &path)
		{
			std::string absoluteName = std::filesystem::absolute(std::filesystem::path(path)).string();
			auto loadedScriptIt = m_LoadedScript.find(absoluteName);
			if (loadedScriptIt != m_LoadedScript.end())
				return (*loadedScriptIt).second;
			return nullptr;
		}

		bool ScriptLoader::Reload(const std::string &path, Native *native)
		{
			Script *loadedScript = Load(path, false, native);
			if (loadedScript)
				loadedScript->Compile();
			return (loadedScript != nullptr);
		}

		ScriptLoader::~ScriptLoader()
		{
			for (const auto &pair : m_LoadedScript)
			{
				if (pair.second)
					delete(pair.second);
			}
		}
	}
}