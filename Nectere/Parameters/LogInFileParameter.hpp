#include "Configuration.hpp"

namespace Nectere
{
	namespace Parameters
	{
		class LogInFileParameter final : public Configuration::ATypedParameter<bool>
		{
		private:
			bool DefaultValue() const override;
			bool Parse() const override;
			std::string GetDescription() const override;

		public:
			LogInFileParameter();
		};
	}
}