/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef HUB_COMMANDS_FACTORY_H_
#define HUB_COMMANDS_FACTORY_H_

#include <map>
#include <string>
#include <string_view>

#include "common/commands/command.h"
#include "cppclient/api.h"

namespace hub {

namespace cmd {

typedef std::shared_ptr<common::ICommand> (*CreateCommandFn)(void);

class CommandFactory {
 public:
  static std::shared_ptr<common::ICommand> create(
      std::string_view name, std::shared_ptr<cppclient::IotaAPI> api) {
    std::shared_ptr<common::ICommand> cmd;
    try {
      auto creator = get()->_factoryMap.at(name.data());
      cmd = creator();
      if (cmd->needApi()) {
        cmd->setApi(api);
      }
    } catch (const std::out_of_range &e) {
      LOG(ERROR) << name << " is an unknown command\n";
    }

    return cmd;
  }
  CommandFactory(const CommandFactory &) = delete;
  CommandFactory &operator=(const CommandFactory &) = delete;

  ~CommandFactory() { _factoryMap.clear(); }

  static CommandFactory *get() {
    static CommandFactory _instance;
    return &_instance;
  }

  void registerCreator(const std::string name,
                       CreateCommandFn commandCreateFn) {
    _factoryMap.emplace(name, commandCreateFn);
  };

 private:
  CommandFactory(){};
  typedef std::map<std::string, CreateCommandFn> FactoryMap;
  FactoryMap _factoryMap;
};

template <class Cmd>
class CommandFactoryRegistrator {
 public:
  CommandFactoryRegistrator() {
    CommandFactory::get()->registerCreator(Cmd::name(), Cmd::create);
  }
  CommandFactoryRegistrator(const CommandFactory &) = delete;
  CommandFactoryRegistrator &operator=(const CommandFactory &) = delete;
};
}  // namespace cmd
}  // namespace hub

#endif  // HUB_COMMANDS_FACTORY_H_
