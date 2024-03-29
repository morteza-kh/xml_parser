/* in the name of Allah */

#ifndef IFF5C4E15_1DD8_4D3D_9B3B_10A5F4B0BD0D
#define IFF5C4E15_1DD8_4D3D_9B3B_10A5F4B0BD0D

#include <memory>
#include <string>
#include <vector>
#include <map>

#include <boost/regex.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

struct xml_node {
    std::shared_ptr<xml_node>              parent;
    std::string                            name;
    std::map<std::string, std::string>     attributes;
    std::vector<std::shared_ptr<xml_node>> children;
};

auto get_attributes(std::string const &str) -> std::map<std::string, std::string> {
    auto map  = std::map<std::string, std::string> {};
    auto pat  = std::regex { "(\\w+)\\s*=\\s*\"([^\"]+)\"" };
    auto iter = std::sregex_iterator { std::cbegin(str), std::cend(str), pat };
    auto end  = std::sregex_iterator {};

    while (iter != end) {
        map.insert({ (*iter)[1], (*iter)[2] });
        iter++;
    }
    return map;
}

auto peek_node(std::string &content, std::shared_ptr<xml_node> parent) -> std::shared_ptr<xml_node> { //
    auto match = boost::smatch {};

    if (boost::regex_match(content, match, boost::regex { "(<(\\w+)([^\\/>]*)\\/?>).*" })) {
        auto name  = match[2].str();
        auto attrs = get_attributes(match[3].str());
        auto elem  = std::shared_ptr<xml_node> { new xml_node { parent, name, attrs, {} } };
        auto child = std::shared_ptr<xml_node> {};
        auto str   = match[1].str();

        content = content.substr(str.size());

        if (!boost::algorithm::ends_with(str, "/>")) {
            while ((child = peek_node(content, elem)).get() != nullptr) {
                elem->children.push_back(child);
            }
            if (!boost::regex_match(content, match, boost::regex { (boost::format("(<\\/%1%>).*") % name).str() })) {
                throw std::runtime_error { "exception on: " + content };
            } else {
                content = content.substr(match[1].length());
            }
        }
        return elem;
    }
    return {};
}

auto xml_parse(std::string content) -> std::shared_ptr<xml_node> {
    /* remove any white spaces from xml string */
    content = boost::regex_replace(content, boost::regex { "[\\s\\r\\n]*(<\\/?(\\w+)([^\\/>]+)\\/?>)[\\s\\r\\n]*" }, "$01");

    return peek_node(content, {});
}


namespace xml {
	auto parse(std::string const &string) -> std::shared_ptr<xml_node> 
	{
		return xml_parser(string);
	}
}

#endif /* !IFF5C4E15_1DD8_4D3D_9B3B_10A5F4B0BD0D */
