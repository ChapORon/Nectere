#include "Parameters/LogInFileParameter.hpp"

namespace Nectere
{
	namespace Parameters
	{
		class LogPathParameter final : public Configuration::ATypedParameter<std::string>
		{
		private:
			std::shared_ptr<LogInFileParameter> m_LogInFile;

		private:
			std::string DefaultValue() const override;
			std::string Parse(const std::string &) const override;
			std::string GetDescription() const override;

		public:
			LogPathParameter(const std::shared_ptr<LogInFileParameter> &);
		};
	}
}