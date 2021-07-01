#include "Configuration.hpp"

namespace Nectere
{
	namespace Parameters
	{
		class VerboseParameter final : public Configuration::ATypedParameter<bool>
		{
		private:
			bool DefaultValue() const override;
			bool Parse() const override;
			bool Parse(const std::string &value) const override;
			std::string GetDescription() const override;

		public:
			VerboseParameter();
		};
	}
}