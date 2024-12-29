#pragma once

#include <string>
#include <vector>
#include <algorithm>

enum class XmlTokenType
{
    COMMENT,
    INSTRUCTION,
    NODE_START,
    NODE_END,
    NODE_FULL,
    CONTENT
};

struct XmlToken
{
    XmlTokenType type;
    std::string tag;
    std::string data;
};

class XmlLexer
{
public:
    XmlLexer(const std::string& raw) : m_raw(raw), m_i(0) {}

    std::vector<XmlToken> generateTokens()
    {
        m_tokens.clear();
        m_i = 0;

        bool isContent = false;
        size_t contentStart = 0;
        size_t contentEnd = 0;
        while(m_i < m_raw.size()) {

            if (m_raw[m_i] == '<') {                
                // content end
                if (isContent) {
                    isContent = false;

                    contentEnd = m_i;

                    m_tokens.emplace_back(
                        XmlTokenType::CONTENT,
                        "",
                        m_raw.substr(contentStart, contentEnd-contentStart)
                    );
                    
                    auto& content = m_tokens.back().data;
                    content.erase(std::remove(content.begin(), content.end(), '\n'), content.end());
                    content.erase(std::remove(content.begin(), content.end(), '\r'), content.end());
                }

                m_i++;

                // comment
                if (m_raw[m_i] == '!' && m_raw[m_i+1] == '-' && m_raw[m_i+2] == '-') {
                    m_i += 3;
                    generateCommentXmlToken();
                }
                // instruction
                if (m_raw[m_i] == '?') {
                    m_i++;
                    generateInstructionXmlToken();
                }
                // end of node
                else if (m_raw[m_i] == '/') {
                    m_i++;
                    generateNodeEndXmlToken();
                }
                // start of node
                else {
                    generateNodeStartXmlToken();
                }

            }
            else {
                              
                // content start
                if (!isContent && m_raw[m_i] != '<') {                    
                    if (m_raw[m_i] != ' ' && m_raw[m_i] != '\n' && m_raw[m_i] != '\r' && m_raw[m_i] != '\t') {
                        isContent = true;
                        contentStart = m_i;
                    }
                }
              
                m_i++;

            }
        }

        return m_tokens;
    }

private:
    void generateCommentXmlToken()
    {
        const size_t start = m_i;
        while((m_raw[m_i] != '-' || m_raw[m_i+1] != '-' || m_raw[m_i+2] != '>') && m_i < m_raw.size()) { 
            m_i++; 
        }
        const size_t end = m_i;
        m_i += 3;

        m_tokens.emplace_back(
            XmlTokenType::COMMENT,
            "",
            m_raw.substr(start, end-start)
        );
    }

    void generateInstructionXmlToken()
    {
        // tag (target)
        const size_t tagStart = m_i;
        while((m_raw[m_i] != '?' || m_raw[m_i+1] != '>') && m_raw[m_i] != ' ' && m_i < m_raw.size()) { 
            m_i++; 
        }
        const size_t tagEnd = m_i;
        
        m_tokens.emplace_back(
            XmlTokenType::INSTRUCTION,
            m_raw.substr(tagStart, tagEnd-tagStart),
            ""
        );
    
        // no content
        if (m_raw[m_i] != ' ') {
            m_i += 2;
            return;
        }
        m_i++;

        // content
        const size_t contentStart = m_i;              
        while((m_raw[m_i] != '?' || m_raw[m_i+1] != '>') && m_i < m_raw.size()) { 
            m_i++; 
        }
        const size_t contentEnd = m_i;
        m_i += 2;

        m_tokens.back().data = m_raw.substr(contentStart, contentEnd-contentStart);
    }

    void generateNodeStartXmlToken()
    {
        // tag
        const size_t tagStart = m_i;
        while(m_raw[m_i] != '>' &&  m_raw[m_i] != ' ' && m_i < m_raw.size()) { 
            m_i++; 
        }
        size_t tagEnd = m_i;
        m_i++;

        m_tokens.emplace_back(
            XmlTokenType::NODE_START,
            m_raw.substr(tagStart, tagEnd-tagStart),
            ""
        );

        if (m_raw[m_i-2] == '/') {
            m_tokens.back().type = XmlTokenType::NODE_FULL;
            tagEnd--;
            m_tokens.back().tag = m_raw.substr(tagStart, tagEnd-tagStart);
        }

        // no content
        if (m_raw[m_i-1] != ' ') {
            if (m_raw[m_i-2] == '/')
                m_tokens.back().type = XmlTokenType::NODE_FULL;
            return;
        }

        // content
        const size_t contentStart = m_i;              
        while(m_raw[m_i] != '>' && m_i < m_raw.size()) { 
            m_i++; 
        }
        size_t contentEnd = m_i;
        m_i++;

        if (m_raw[m_i-2] == '/') {
            m_tokens.back().type = XmlTokenType::NODE_FULL;
            contentEnd--;
        }
        m_tokens.back().data = m_raw.substr(contentStart, contentEnd-contentStart);
    }

    void generateNodeEndXmlToken()
    {
        const size_t start = m_i;
        while(m_raw[m_i] != '>' && m_i < m_raw.size()) { 
            m_i++; 
        }
        const size_t end = m_i;
        m_i++;

        m_tokens.emplace_back(
            XmlTokenType::NODE_END,
            m_raw.substr(start, end-start),
            ""
        );
    }

    std::string m_raw;
    size_t m_i;
    std::vector<XmlToken> m_tokens;

};