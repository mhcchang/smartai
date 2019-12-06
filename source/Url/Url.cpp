#include "boost/format.hpp"
#include "Url/Url.h"

namespace framework
{
	namespace wrapper
	{
		URL::URL()
		{}

		URL::~URL(void)
		{}

		void URL::setProtocol(const URLType type /* = URLType::URL_TYPE_NONE */)
		{
			const char* protocol{ std::strstr(url.c_str(), "://") };

			if (!protocol)
			{
				if (URLType::URL_TYPE_LIVESTREAM == type)
				{
					url = "livestream://" + url;
				}
				else if (URLType::URL_TYPE_PLAYBACK == type)
				{
					url = "playback://" + url;
				}
				else if (URLType::URL_TYPE_RTSP == type)
				{
					url = "rtsp://" + url;
				}
			}
		}

		const URLType URL::getProtocol(void) const
		{
			URLType type{ URLType::URL_TYPE_NONE };
			std::size_t beginpos{ url.find_first_of(':', 0) };
			const std::string protocol{ url.substr(0, beginpos) };

			if (!protocol.compare("livestream"))
			{
				type = URLType::URL_TYPE_LIVESTREAM;
			}
			else if (!protocol.compare("playback"))
			{
				type = URLType::URL_TYPE_PLAYBACK;
			}
			else if (!protocol.compare("rtsp"))
			{
				type = URLType::URL_TYPE_RTSP;
			}

			return type;
		}

		void URL::setAddressPort(const std::string& address, const unsigned short port /* = 0 */)
		{
			if (!address.empty() && 0 < port)
			{
				const std::string domain{ (boost::format("@%s:%d") % address % port).str() };
				std::size_t urllen{ url.length() };
				std::size_t beginpos{ url.find_last_of('@', urllen) }, endpos{ url.find_last_of('?', urllen) };

				if (std::string::npos == beginpos)
				{
					if (std::string::npos == endpos)
					{
						url.append(domain);
					} 
					else
					{
						const std::string previous{ url.substr(0, endpos) };
						const std::string next{ url.substr(endpos, urllen) };
						std::string tempUrl{ (boost::format("%s%s%s") % previous % domain % next).str() };
						url.swap(tempUrl);
					}
				}
			}
		}

		void URL::getAddressPort(std::string& address, unsigned short& port)
		{
			if (!url.empty())
			{
				std::size_t beginpos{ url.find_first_of('@', 0) }, endpos{ url.find_first_of('?', 0) };
				const std::string domain{ url.substr(beginpos + 1, endpos - beginpos - 1) };

				std::size_t splitpos{ domain.find_first_of(':', 0) };
				address = domain.substr(0, splitpos);
				port = atoi(domain.substr(splitpos + 1, domain.length()).c_str());
			}
		}

		void URL::addAuthentication(const std::string& name, const std::string& password)
		{
			if (!name.empty() && !password.empty())
			{
				std::size_t urllen{ url.length() };
				std::size_t beginpos{ url.find_last_of('@', urllen) }, endpos{ url.find_last_of('/', urllen) };
				std::string previous, next, tempUrl;

				if (std::string::npos != beginpos)
				{
					previous = url.substr(0, beginpos);
					next = url.substr(beginpos, urllen);
					tempUrl = (boost::format("%s%s:%s%s") % previous % name % password % next).str();
				}
				else
				{
					previous = url.substr(0, endpos + 1);
					next = url.substr(endpos + 1, urllen);
					tempUrl = (boost::format("%s%s:%s%s") % previous % name % password % next).str();
				}

				url.swap(tempUrl);
			}
		}

		void URL::getAuthentication(std::string& name, std::string& password)
		{
			if (!url.empty())
			{
				std::size_t beginpos{ url.find_first_of(':', 0) }, endpos{ url.find_first_of('@', 0) };
				const std::string domain{ url.substr(beginpos + 3, endpos - beginpos - 3) };

				std::size_t splitpos{ domain.find_first_of(':', 0) };
				name = domain.substr(0, splitpos);
				password = domain.substr(splitpos + 1, domain.length());
			}
		}

		void URL::addParameter(const std::string& key, const std::string& value)
		{
			std::size_t urllen{ url.length() };
			std::size_t beginpos{ url.find_last_of('?', urllen) };
			std::string parameter;

			if (std::string::npos == beginpos)
			{
				parameter = (boost::format("?%s=%s") % key % value).str();
			} 
			else
			{
				parameter = (boost::format("&%s=%s") % key % value).str();
			}

			url.append(parameter);
		}

		const std::string URL::getParameter(const std::string& key)
		{
			std::string value;

			if (!url.empty() && !key.empty())
			{
				const char* parameter = std::strstr(url.c_str(), key.c_str());

				if (parameter)
				{
					value = parameter;
				}
			}

			return std::string();
		}
	}//namespace wrapper
}//namespace framework
