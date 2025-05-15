#pragma once
#include"const.h"
struct SectionInfo {
    SectionInfo() {}
    ~SectionInfo() {
        _section_datas.clear();
    }
    SectionInfo(const SectionInfo& src) {
        _section_datas = src._section_datas;
    }
    SectionInfo& operator = (const SectionInfo& src) {
        if (&src == this) {
            return *this;
        }
        this->_section_datas = src._section_datas;
        return *this;
    }
    std::map<std::string, std::string> _section_datas;
    std::string  operator[](const std::string& key) {
        if (_section_datas.find(key) == _section_datas.end()) {
            return "";
        }
        return _section_datas[key];
    }
};
class ConfigMgr
{
public:
    ~ConfigMgr() {
        _config_map.clear();
    }
    SectionInfo operator[](const std::string& section) {
        if (_config_map.find(section) == _config_map.end()) {
            return SectionInfo();
        }
        return _config_map[section];
    }
    static ConfigMgr& Info()
    {
        static ConfigMgr cfg_mgr;
        return cfg_mgr;
    }
    ConfigMgr& operator=(const ConfigMgr& src) {
        if (&src == this) {
            return *this;
        }
        this->_config_map = src._config_map;
    };
    ConfigMgr(const ConfigMgr& src) {
        this->_config_map = src._config_map;
    }
  
private:
    ConfigMgr();
    // ´æ´¢sectionºÍkey-value¶ÔµÄmap  
    std::map<std::string, SectionInfo> _config_map;
};

