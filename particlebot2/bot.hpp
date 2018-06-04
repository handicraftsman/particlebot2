#pragma once

#include <memory>

#include <particledi.hpp>

namespace pb2 {

  class bot_private;

  class bot : public particledi::dependency {
  public:
    bot(particledi::dm_ptr dm);
    virtual ~bot();

  private:
    std::shared_ptr<bot_private> priv;
  };

}