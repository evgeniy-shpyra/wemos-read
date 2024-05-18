#ifndef STORAGE_H
#define STORAGE_H
#include <EEPROM.h>
#define STR_ADDR 0

struct SettingsStructure
{
  char name[50];
  char hubIp[16];
  char wifiSsid[32];
  char wifiPassword[32];
};

class Storage
{

public:
  Storage()
  {
  }

  SettingsStructure getSettings()
  {
    SettingsStructure data;
    EEPROM.begin(sizeof(SettingsStructure));
    EEPROM.get(STR_ADDR, data);
    EEPROM.end();

    return data;
  }

  void updateSettings(const SettingsStructure &data)
  {
    EEPROM.begin(sizeof(SettingsStructure));
    EEPROM.put(STR_ADDR, data);
    EEPROM.commit();
    EEPROM.end();
  }

  void deleteSettings()
  {
    SettingsStructure blankData;
    memset(&blankData, 0, sizeof(SettingsStructure));
    this->updateSettings(blankData);
  }

  bool settingsExist()
  {
    SettingsStructure savedSettings;
    savedSettings = this->getSettings();
    SettingsStructure defaultSettings;
    memset(&defaultSettings, 0, sizeof(SettingsStructure));

    return memcmp(&savedSettings, &defaultSettings, sizeof(SettingsStructure)) != 0;
  }
};

#endif