#pragma once

#include "xml_util.h"
#include "XmlLexer.h"

#include <string>
#include <vector>
#include <map>
#include <variant>
#include <iostream>
#include <ranges>
#include <string_view>

struct XmlNode
{
    XmlNode* parent;
    std::vector<XmlNode> children;
    std::string tag;
    std::string content;
    std::map<std::string, std::variant<int, float, std::string>> attributes;
};

class XmlParser
{
public:
    XmlParser(const std::vector<XmlToken>& tokens) : m_tokens(tokens), m_i(0) {}

    XmlNode parse()
    {
        m_root = XmlNode{
            .parent = nullptr,
            .children = {},
            .tag = "",
            .content = "",
            .attributes = {}
        };
        m_i = 0;

        if (m_tokens.empty())
            return m_root;

        createNodes(&m_root);
        return m_root;
    }

    static void print(const XmlNode& node, const int level = 0)
    {
        if (level > 0) {
            std::string ident(level-1, '\t');
            std::cout << ident << "tag: " << node.tag;
            if (!node.attributes.empty()) {
                std::cout << ", attr:";
                for (auto&[name, val] : node.attributes) {
                    if (val.index() == 0)
                        std::cout << " " << name << "=" << std::to_string(std::get<int>(val));
                    else if (val.index() == 1)
                        std::cout << " " << name << "=" << std::to_string(std::get<float>(val));
                    else
                        std::cout << " " << name << "=" << std::get<std::string>(val);
                }
            }
            std::cout << ", content: " << node.content << '\n';
        }

        for (auto child : node.children)
            print(child, level+1);
    }

private:
    void createNodes(XmlNode* parent)
    {
        while(m_i < m_tokens.size()) {
            auto token = m_tokens[m_i++];
            
            // add content
            if (token.type == XmlTokenType::CONTENT) {
                parent->content = token.data;
            }
            // add full node
            else if (token.type == XmlTokenType::NODE_FULL) {
                parent->children.push_back(XmlNode{
                    .parent = parent,
                    .children = {},
                    .tag = token.tag,
                    .content = "",
                    .attributes = {}
                });
                parseAttributes(parent->children.back(), token);
            }
            // start new node
            else if (token.type == XmlTokenType::NODE_START) {
                parent->children.push_back(XmlNode{
                    .parent = parent,
                    .children = {},
                    .tag = token.tag,
                    .content = "",
                    .attributes = {}
                });
                parseAttributes(parent->children.back(), token);
                createNodes(&parent->children.back());
            }
            // node end
            else if (token.type == XmlTokenType::NODE_END) {
                return;
            }
        }
    }

    void parseAttributes(XmlNode& node, const XmlToken& token)
    {
        const std::string attr = token.data;

        auto parts = splitString(attr, "= ");
        if (parts.size() % 2 != 0)
            return;

        const auto isNumber = [](const std::string& str) -> bool {
            if (str.empty())
                return false;

            auto begin = str.begin();
            if (str[0] == '-')
                begin++;

            return std::find_if(begin, str.end(), [](const char c) { return !std::isdigit(c); }) == str.end();
        };

        const auto contains = [](const std::string& str, const char c) -> bool {
            return std::find(str.begin(), str.end(), c) != str.end();
        };
        
        for (size_t i = 0; i < parts.size(); i+=2) {
            const std::string name = parts[i];
            std::string val = parts[i+1];

            std::variant<int, float, std::string> var;
            if (isNumber(val)) {
                if (contains(val, '.'))
                    var = stof(val);
                else
                    var = stoi(val);
            }
            else {
                val.erase(std::ranges::remove_if(val, [](const char c) -> bool { 
                    return c == '"';
                }).begin(), val.end());
                var = val;
            }

            node.attributes.emplace(name, var);
        }
    }

    std::vector<XmlToken> m_tokens;
    size_t m_i;
    XmlNode m_root;

};